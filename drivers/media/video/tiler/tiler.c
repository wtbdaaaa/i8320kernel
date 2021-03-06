/*
 * tiler.c
 *
 * TILER driver support functions for TI OMAP processors.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>            /* struct cdev */
#include <linux/kdev_t.h>          /* MKDEV() */
#include <linux/fs.h>              /* register_chrdev_region() */
#include <linux/device.h>          /* struct class */
#include <linux/platform_device.h> /* platform_device() */
#include <linux/err.h>             /* IS_ERR() */
#include <linux/uaccess.h>         /* copy_to_user */
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/dma-mapping.h>
#include <linux/pagemap.h>         /* page_cache_release() */

#include "tiler.h"
#include "tiler_def.h"
#include "../dmm/dmm.h"

#include "tcm/tcm_sita.h"	/* Algo Specific header */

struct tiler_dev {
	struct cdev cdev;
};

struct platform_driver tiler_driver_ldm = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "tiler",
	},
	.probe = NULL,
	.shutdown = NULL,
	.remove = NULL,
};

struct __buf_info {
	struct list_head list;
	struct tiler_buf_info buf_info;
};

struct mem_info {
	struct list_head list;
	u32 sys_addr;          /* system space (L3) tiler addr */
	u32 num_pg;            /* number of pages in page-list */
	u32 usr;               /* user space address */
	u32 *pg_ptr;           /* list of mapped struct page pointers */
	struct tcm_area area;
	u32 *mem;              /* list of alloced phys addresses */
};

static s32 tiler_major;
static s32 tiler_minor;
static struct tiler_dev *tiler_device;
static struct class *tilerdev_class;
static u32 id;
static struct __buf_info buf_list;
static struct mem_info mem_list;
static struct mutex mtx;
static struct tcm *tcm;

static s32 __adjust_area(enum tiler_fmt fmt, u32 width, u32 height, u16 *x_area,
		u16 *y_area)
{
	u32 x_pagedim = 0, y_pagedim = 0;
	u16 tiled_pages_per_ss_page = 1;

	switch (fmt) {
	case TILFMT_8BIT:
		x_pagedim = DMM_PAGE_DIMM_X_MODE_8;
		y_pagedim = DMM_PAGE_DIMM_Y_MODE_8;
		tiled_pages_per_ss_page = TILER_PAGE / x_pagedim;
		break;
	case TILFMT_16BIT:
		x_pagedim = DMM_PAGE_DIMM_X_MODE_16;
		y_pagedim = DMM_PAGE_DIMM_Y_MODE_16;
		tiled_pages_per_ss_page = TILER_PAGE / x_pagedim / 2;
		break;
	case TILFMT_32BIT:
		x_pagedim = DMM_PAGE_DIMM_X_MODE_32;
		y_pagedim = DMM_PAGE_DIMM_Y_MODE_32;
		tiled_pages_per_ss_page = TILER_PAGE / x_pagedim / 4;
		break;
	case TILFMT_PAGE:
		/* for 1D area keep the height (1), width is in tiler slots */
		*x_area = DIV_ROUND_UP(width, TILER_PAGE);
		*y_area = 1;

		if (*x_area * *y_area > TILER_WIDTH * TILER_HEIGHT)
			return -1;
		return 0;
	default:
		return -1;
	}

	/* adjust and check 2D areas */
	*x_area = DIV_ROUND_UP(width, x_pagedim);
	*y_area = DIV_ROUND_UP(height, y_pagedim);
	*x_area = ALIGN(*x_area, tiled_pages_per_ss_page);

	if (*x_area > TILER_WIDTH || *y_area > TILER_HEIGHT)
		return -1;
	return 0x0;
}

static s32 get_area(u32 sys_addr, u32 *x_area, u32 *y_area)
{
	enum tiler_fmt fmt;

	sys_addr &= TILER_ALIAS_VIEW_CLEAR;
	fmt = TILER_GET_ACC_MODE(sys_addr);

	switch (fmt + 1) {
	case TILFMT_8BIT:
		*x_area = DMM_HOR_X_PAGE_COOR_GET_8(sys_addr);
		*y_area = DMM_HOR_Y_PAGE_COOR_GET_8(sys_addr);
		break;
	case TILFMT_16BIT:
		*x_area = DMM_HOR_X_PAGE_COOR_GET_16(sys_addr);
		*y_area = DMM_HOR_Y_PAGE_COOR_GET_16(sys_addr);
		break;
	case TILFMT_32BIT:
		*x_area = DMM_HOR_X_PAGE_COOR_GET_32(sys_addr);
		*y_area = DMM_HOR_Y_PAGE_COOR_GET_32(sys_addr);
		break;
	case TILFMT_PAGE:
		*x_area = (sys_addr & 0x7FFFFFF) >> 12;
		*y_area = *x_area / 256;
		*x_area &= 255;
		break;
	default:
		return -EFAULT;
	}
	return 0x0;
}

static u32 __get_alias_addr(enum tiler_fmt fmt, u16 x, u16 y)
{
	u32 acc_mode = -1;
	u32 x_shft = -1, y_shft = -1;

	switch (fmt) {
	case TILFMT_8BIT:
		acc_mode = 0; x_shft = 6; y_shft = 20;
		break;
	case TILFMT_16BIT:
		acc_mode = 1; x_shft = 7; y_shft = 20;
		break;
	case TILFMT_32BIT:
		acc_mode = 2; x_shft = 7; y_shft = 20;
		break;
	case TILFMT_PAGE:
		acc_mode = 3; y_shft = 8;
		break;
	default:
		return 0;
		break;
	}

	if (fmt == TILFMT_PAGE)
		return (u32)TIL_ALIAS_ADDR((x | y << y_shft) << 12, acc_mode);
	else
		return (u32)TIL_ALIAS_ADDR(x << x_shft | y << y_shft, acc_mode);
}

static s32 tiler_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct __buf_info *_b = NULL;
	struct tiler_buf_info *b = NULL;
	s32 i = 0, j = 0, k = 0, m = 0, p = 0, bpp = 1;
	struct list_head *pos = NULL;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	mutex_lock(&mtx);
	list_for_each(pos, &buf_list.list) {
		_b = list_entry(pos, struct __buf_info, list);
		if ((vma->vm_pgoff << PAGE_SHIFT) == _b->buf_info.offset)
			break;
	}
	mutex_unlock(&mtx);
	if (!_b)
		return -EFAULT;

	b = &_b->buf_info;

	for (i = 0; i < b->num_blocks; i++) {
		if (b->blocks[i].fmt >= TILFMT_8BIT &&
					b->blocks[i].fmt <= TILFMT_32BIT) {
			/* get line width */
			bpp = (b->blocks[i].fmt == TILFMT_8BIT ? 1 :
			       b->blocks[i].fmt == TILFMT_16BIT ? 2 : 4);
			p = PAGE_ALIGN(b->blocks[i].dim.area.width * bpp);

			for (j = 0; j < b->blocks[i].dim.area.height; j++) {
				/* map each page of the line */
				vma->vm_pgoff =
					(b->blocks[i].ssptr + m) >> PAGE_SHIFT;
				if (remap_pfn_range(vma, vma->vm_start + k,
					(b->blocks[i].ssptr + m) >> PAGE_SHIFT,
					p, vma->vm_page_prot))
					return -EAGAIN;
				k += p;
				if (b->blocks[i].fmt == TILFMT_8BIT)
					m += 64*TILER_WIDTH;
				else
					m += 2*64*TILER_WIDTH;
			}
			m = 0;
		} else if (b->blocks[i].fmt == TILFMT_PAGE) {
			vma->vm_pgoff = (b->blocks[i].ssptr) >> PAGE_SHIFT;
			p = PAGE_ALIGN(b->blocks[i].dim.len);
			if (remap_pfn_range(vma, vma->vm_start + k,
				(b->blocks[i].ssptr) >> PAGE_SHIFT, p,
				vma->vm_page_prot))
				return -EAGAIN;;
			k += p;
		}
	}
	return 0;
}

static s32 tiler_pat_refill(struct tcm_area *area, u32 *ptr)
{
	s32 res = 0;
	s32 size = tcm_sizeof(*area) * sizeof(*ptr);
	u32 *page;
	dma_addr_t page_pa;
	struct pat pat_desc = {0};
	struct tcm_area slice, area_s;

	page = dma_alloc_coherent(NULL, size, &page_pa, GFP_ATOMIC);
	if (!page)
		return -ENOMEM;

	/* send pat descriptor to dmm driver */
	pat_desc.ctrl.dir = 0;
	pat_desc.ctrl.ini = 0;
	pat_desc.ctrl.lut_id = 0;
	pat_desc.ctrl.start = 1;
	pat_desc.ctrl.sync = 0;
	pat_desc.next = NULL;

	/* must be a 16-byte aligned physical address */
	pat_desc.data = page_pa;

	tcm_for_each_slice(slice, *area, area_s) {
		pat_desc.area.x0 = slice.p0.x;
		pat_desc.area.y0 = slice.p0.y;
		pat_desc.area.x1 = slice.p1.x;
		pat_desc.area.y1 = slice.p1.y;

		memcpy(page, ptr, sizeof(*ptr) * tcm_sizeof(slice));
		ptr += tcm_sizeof(slice);

		if (dmm_pat_refill(&pat_desc, MANUAL)) {
			res = -EFAULT;
			break;
		}
	}

	dma_free_coherent(NULL, size, page, page_pa);

	return res;
}

static s32 map_buffer(enum tiler_fmt fmt, u32 width, u32 height, u32 *sys_addr,
								u32 usr_addr)
{
	u16 x_area = 0, y_area = 0;
	u32 i = 0, tmp = -1;
	u8 write = 0;
	struct mem_info *mi = NULL;
	struct page *page = NULL;
	struct task_struct *curr_task = current;
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma = NULL;

	/* we only support mapping a user buffer in page mode */
	if (fmt != TILFMT_PAGE)
		return -EFAULT;

	/* reserve area in tiler container */
	if (__adjust_area(fmt, width, height, &x_area, &y_area))
		return -EFAULT;

	mi = kmalloc(sizeof(*mi), GFP_KERNEL);
	if (!mi)
		return -ENOMEM;
	memset(mi, 0x0, sizeof(*mi));

	if (tcm_reserve_1d(tcm, x_area * y_area, &mi->area)) {
		kfree(mi);
		return -ENOMEM;
	}

	/* formulate system space address */
	*sys_addr = __get_alias_addr(fmt, mi->area.p0.x, mi->area.p0.y);

	/* allocate pages */
	mi->num_pg = tcm_sizeof(mi->area);
	mi->sys_addr = *sys_addr;
	mi->usr = usr_addr;

	mi->mem = kmalloc(mi->num_pg * sizeof(*mi->mem), GFP_KERNEL);
	if (!mi->mem)
		goto free;

	mi->pg_ptr = kmalloc(mi->num_pg * sizeof(*mi->pg_ptr), GFP_KERNEL);
	if (!mi->pg_ptr)
		goto free;

	/*
	 * Important Note: usr_addr is mapped from user
	 * application process to current process - it must lie
	 * completely within the current virtual memory address
	 * space in order to be of use to us here.
	 */
	down_read(&mm->mmap_sem);
	vma = find_vma(mm, mi->usr);

	/*
	 * It is observed that under some circumstances, the user
	 * buffer is spread across several vmas, so loop through
	 * and check if the entire user buffer is covered.
	 */
	while ((vma) && (mi->usr + width > vma->vm_end)) {
		/* jump to the next VMA region */
		vma = find_vma(mm, vma->vm_end + 1);
	}
	if (!vma) {
		printk(KERN_ERR "Failed to get the vma region for "
			"user buffer.\n");
		goto fault;
	}

	if (vma->vm_flags & (VM_WRITE | VM_MAYWRITE))
		write = 1;

	tmp = mi->usr;
	for (i = 0; i < mi->num_pg; i++) {
		if (get_user_pages(curr_task, mm, tmp, 1, write, 1, &page,
									NULL)) {
			if (page_count(page) < 1) {
				printk(KERN_ERR "Bad page count from"
							"get_user_pages()\n");
			}
			mi->pg_ptr[i] = (u32)page;
			mi->mem[i] = page_to_phys(page);
			tmp += PAGE_SIZE;
		} else {
			printk(KERN_ERR "get_user_pages() failed\n");
			goto fault;
		}
	}
	up_read(&mm->mmap_sem);

	mutex_lock(&mtx);
	list_add(&mi->list, &mem_list.list);
	mutex_unlock(&mtx);

	if (tiler_pat_refill(&mi->area, mi->mem))
		goto release;

	/* for safety */
	kfree(mi->mem);
	mi->mem = NULL;
	return 0;

release:
	for (i = 0; i < mi->num_pg; i++) {
		page = (struct page *)mi->pg_ptr[i];
		if (!PageReserved(page))
			SetPageDirty(page);
		page_cache_release(page);
	}
fault:
	up_read(&mm->mmap_sem);
	kfree(mi->mem);
	kfree(mi->pg_ptr);
	kfree(mi);
	return -EFAULT;
free:
	kfree(mi->mem);
	kfree(mi->pg_ptr);
	kfree(mi);
	return -ENOMEM;
}

s32 tiler_free(u32 sys_addr)
{
	u32 i = 0;
	struct list_head *pos = NULL, *q = NULL;
	struct mem_info *mi = NULL;
	struct page *page = NULL;

	mutex_lock(&mtx);
	list_for_each_safe(pos, q, &mem_list.list) {
		mi = list_entry(pos, struct mem_info, list);
		if (mi->sys_addr == sys_addr) {

			if (tcm_free(&mi->area))
				printk(KERN_NOTICE "warning: failed to "
						"unreserve tiler area.\n");
			if (mi->pg_ptr) {
				for (i = 0; i < mi->num_pg; i++) {
					page = (struct page *)mi->pg_ptr[i];
					if (!PageReserved(page))
						SetPageDirty(page);
					page_cache_release(page);
				}
				kfree(mi->pg_ptr);
			} else {
				dmm_free_pages(mi->mem);
			}
			list_del(pos);
			kfree(mi);
		}
	}
	mutex_unlock(&mtx);
	if (!mi)
		return -EFAULT;

	/* for debugging, we can set the PAT entries to DMM_LISA_MAP__0 */
	return 0x0;
}
EXPORT_SYMBOL(tiler_free);

/* :TODO: Currently we do not track enough information from alloc to get back
   the actual width and height of the container, so we must make a guess.  We
   do not even have enough information to get the virtual stride of the buffer,
   which is the real reason for this ioctl */
s32 tiler_find_buf(u32 sys_addr, struct tiler_block_info *blk)
{
	struct tcm_area area;
	struct tcm_pt pt;
	u32 x_area = -1, y_area = -1;
	u16 x_page = 0, y_page = 0;
	s32 mode = -1;

	if (get_area(sys_addr, &x_area, &y_area))
		return -EFAULT;

	memset(&area, 0, sizeof(area));
	pt.x = x_area;
	pt.y = y_area;
	if (tcm_get_parent(tcm, &pt, &area))
		return -EFAULT;
	x_page = area.p1.x - area.p0.x + 1;
	y_page = area.p1.y - area.p0.y + 1;
	blk->ptr = NULL;

	mode = TILER_GET_ACC_MODE(sys_addr);
	blk->fmt = (mode + 1);
	if (blk->fmt == TILFMT_PAGE) {
		blk->dim.len = x_page * y_page * TILER_PAGE;
		if (blk->dim.len == 0)
			goto error;
		blk->stride = 0;
		blk->ssptr = (u32)TIL_ALIAS_ADDR(((area.p0.x |
						(area.p0.y << 8)) << 12), mode);
	} else {
		blk->stride = blk->dim.area.width = x_page * TILER_BLOCK_WIDTH;
		blk->dim.area.height = y_page * TILER_BLOCK_HEIGHT;
		if (blk->dim.area.width == 0 || blk->dim.area.height == 0)
			goto error;
		if (blk->fmt == TILFMT_8BIT) {

			blk->ssptr = (u32)TIL_ALIAS_ADDR(((area.p0.x << 6) |
						(area.p0.y << 20)), mode);
		} else {
			blk->ssptr = (u32)TIL_ALIAS_ADDR(((area.p0.x << 7) |
						(area.p0.y << 20)), mode);

			blk->stride <<= 1;
			blk->dim.area.height >>= 1;
			if (blk->fmt == TILFMT_32BIT)
				blk->dim.area.width >>= 1;
		}
		blk->stride = PAGE_ALIGN(blk->stride);
	}
	return 0;

error:
	blk->fmt = TILFMT_INVALID;
	blk->dim.len = blk->stride = blk->ssptr = 0;
	return -EFAULT;
}

static s32 tiler_ioctl(struct inode *ip, struct file *filp, u32 cmd,
			unsigned long arg)
{
	pgd_t *pgd = NULL;
	pmd_t *pmd = NULL;
	pte_t *ptep = NULL, pte = 0x0;
	s32 r = -1;
	u32 til_addr = 0x0;

	struct __buf_info *_b = NULL;
	struct tiler_buf_info buf_info = {0};
	struct tiler_block_info block_info = {0};
	struct list_head *pos = NULL, *q = NULL;

	switch (cmd) {
	case TILIOC_GBUF:
		if (copy_from_user(&block_info, (void __user *)arg,
					sizeof(block_info)))
			return -EFAULT;

		switch (block_info.fmt) {
		case TILFMT_PAGE:
			r = tiler_alloc(block_info.fmt, block_info.dim.len, 1,
				&til_addr);
			if (r)
				return r;
			break;
		case TILFMT_8BIT:
		case TILFMT_16BIT:
		case TILFMT_32BIT:
			r = tiler_alloc(block_info.fmt,
					block_info.dim.area.width,
					block_info.dim.area.height, &til_addr);
			if (r)
				return r;
			break;
		default:
			return -EINVAL;
		}

		block_info.ssptr = til_addr;
		if (copy_to_user((void __user *)arg, &block_info,
					sizeof(block_info)))
			return -EFAULT;
		break;
	case TILIOC_FBUF:
	case TILIOC_UMBUF:
		if (copy_from_user(&block_info, (void __user *)arg,
					sizeof(block_info)))
			return -EFAULT;

		if (tiler_free(block_info.ssptr))
			return -EFAULT;
		break;
	case TILIOC_GSSP:
		pgd = pgd_offset(current->mm, arg);
		if (!(pgd_none(*pgd) || pgd_bad(*pgd))) {
			pmd = pmd_offset(pgd, arg);
			if (!(pmd_none(*pmd) || pmd_bad(*pmd))) {
				ptep = pte_offset_map(pmd, arg);
				if (ptep) {
					pte = *ptep;
					if (pte_present(pte))
						return (pte & PAGE_MASK) |
							(~PAGE_MASK & arg);
				}
			}
		}
		/* va not in page table */
		return 0x0;
		break;
	case TILIOC_MBUF:
		if (copy_from_user(&block_info, (void __user *)arg,
					sizeof(block_info)))
			return -EFAULT;

		if (!block_info.ptr)
			return -EFAULT;

		if (map_buffer(block_info.fmt, block_info.dim.len, 1,
				&block_info.ssptr, (u32)block_info.ptr))
			return -ENOMEM;

		if (copy_to_user((void __user *)arg, &block_info,
					sizeof(block_info)))
			return -EFAULT;
		break;
	case TILIOC_QBUF:
		if (copy_from_user(&buf_info, (void __user *)arg,
					sizeof(buf_info)))
			return -EFAULT;

		mutex_lock(&mtx);
		list_for_each(pos, &buf_list.list) {
			_b = list_entry(pos, struct __buf_info, list);
			if (buf_info.offset == _b->buf_info.offset) {
				if (copy_to_user((void __user *)arg,
					&_b->buf_info,
					sizeof(_b->buf_info))) {
					mutex_unlock(&mtx);
					return -EFAULT;
				} else {
					mutex_unlock(&mtx);
					return 0;
				}
			}
		}
		mutex_unlock(&mtx);
		return -EFAULT;
		break;
	case TILIOC_RBUF:
		_b = kmalloc(sizeof(*_b), GFP_KERNEL);
		if (!_b)
			return -ENOMEM;
		memset(_b, 0x0, sizeof(*_b));

		if (copy_from_user(&_b->buf_info, (void __user *)arg,
					sizeof(_b->buf_info))) {
			kfree(_b); return -EFAULT;
		}

		_b->buf_info.offset = id;
		id += 0x1000;

		mutex_lock(&mtx);
		list_add(&(_b->list), &buf_list.list);
		mutex_unlock(&mtx);

		if (copy_to_user((void __user *)arg, &_b->buf_info,
					sizeof(_b->buf_info))) {
			kfree(_b); return -EFAULT;
		}
		break;
	case TILIOC_URBUF:
		if (copy_from_user(&buf_info, (void __user *)arg,
					sizeof(buf_info)))
			return -EFAULT;

		mutex_lock(&mtx);
		list_for_each_safe(pos, q, &buf_list.list) {
			_b = list_entry(pos, struct __buf_info, list);
			if (buf_info.offset == _b->buf_info.offset) {
				list_del(pos);
				kfree(_b);
				mutex_unlock(&mtx);
				return 0;
			}
		}
		mutex_unlock(&mtx);
		return -EFAULT;
		break;
	case TILIOC_QUERY_BLK:
		if (copy_from_user(&block_info, (void __user *)arg,
					sizeof(block_info)))
			return -EFAULT;

		if (tiler_find_buf(block_info.ssptr, &block_info))
			return -EFAULT;

		if (copy_to_user((void __user *)arg, &block_info,
			sizeof(block_info)))
			return -EFAULT;
		break;
	default:
		return -EINVAL;
	}
	return 0x0;
}

s32 tiler_alloc(enum tiler_fmt fmt, u32 width, u32 height, u32 *sys_addr)
{
	u16 x_area = 0, y_area = 0, band;

	struct mem_info *mi = NULL;

	/* reserve area in tiler container */
	if (__adjust_area(fmt, width, height, &x_area, &y_area))
		return -EFAULT;

	mi = kmalloc(sizeof(*mi), GFP_KERNEL);
	if (!mi)
		return -ENOMEM;
	memset(mi, 0x0, sizeof(*mi));

	switch (fmt) {
	case TILFMT_8BIT:
	case TILFMT_16BIT:
	case TILFMT_32BIT:
		band = fmt == TILFMT_8BIT ? ALIGN_64 : ALIGN_32;
		if (tcm_reserve_2d(tcm, x_area, y_area, band, &mi->area)) {
			kfree(mi);
			return -ENOMEM;
		}
		break;
	case TILFMT_PAGE:
		if (tcm_reserve_1d(tcm, x_area * y_area, &mi->area)) {
			kfree(mi);
			return -ENOMEM;
		}
		break;
	default:
		kfree(mi);
		return -ENOMEM;
	}

	/* formulate system space address */
	*sys_addr = __get_alias_addr(fmt, mi->area.p0.x, mi->area.p0.y);

	/* allocate pages */
	mi->num_pg = tcm_sizeof(mi->area);

	mi->mem = dmm_get_pages(mi->num_pg);
	if (!mi->mem)
		goto cleanup;

	mi->sys_addr = *sys_addr;

	mutex_lock(&mtx);
	list_add(&(mi->list), &mem_list.list);
	mutex_unlock(&mtx);

	/* program PAT */
	if (0 == tiler_pat_refill(&mi->area, mi->mem))
		return 0x0;

cleanup:
	kfree(mi->mem);
	kfree(mi);
	return -ENOMEM;
}
EXPORT_SYMBOL(tiler_alloc);

static void __exit tiler_exit(void)
{
	struct __buf_info *_b = NULL;
	struct mem_info *mi = NULL;
	struct list_head *pos = NULL, *q = NULL;

	tcm_deinit(tcm);
	/* remove any leftover info structs that haven't been unregistered */
	mutex_lock(&mtx);
	pos = NULL, q = NULL;
	list_for_each_safe(pos, q, &buf_list.list) {
		_b = list_entry(pos, struct __buf_info, list);
		list_del(pos);
		kfree(_b);
	}
	mutex_unlock(&mtx);

	/* free any remaining mem structures */
	mutex_lock(&mtx);
	pos = NULL, q = NULL;
	list_for_each_safe(pos, q, &mem_list.list) {
		mi = list_entry(pos, struct mem_info, list);
		if (!mi->pg_ptr)
			dmm_free_pages(mi->mem);
		list_del(pos);
		kfree(mi);
	}
	mutex_unlock(&mtx);

	mutex_destroy(&mtx);
	platform_driver_unregister(&tiler_driver_ldm);
	cdev_del(&tiler_device->cdev);
	kfree(tiler_device);
	device_destroy(tilerdev_class, MKDEV(tiler_major, tiler_minor));
	class_destroy(tilerdev_class);
}

static s32 tiler_open(struct inode *ip, struct file *filp)
{
	return 0x0;
}

static s32 tiler_release(struct inode *ip, struct file *filp)
{
	return 0x0;
}

static const struct file_operations tiler_fops = {
	.open    = tiler_open,
	.ioctl   = tiler_ioctl,
	.release = tiler_release,
	.mmap    = tiler_mmap,
};

static s32 __init tiler_init(void)
{
	dev_t dev  = 0;
	s32 r = -1;
	struct device *device = NULL;
	struct tcm_pt div_pt;

	if (tiler_major) {
		dev = MKDEV(tiler_major, tiler_minor);
		r = register_chrdev_region(dev, 1, "tiler");
	} else {
		r = alloc_chrdev_region(&dev, tiler_minor, 1, "tiler");
		tiler_major = MAJOR(dev);
	}

	tiler_device = kmalloc(sizeof(*tiler_device), GFP_KERNEL);
	if (!tiler_device) {
		unregister_chrdev_region(dev, 1);
		return -ENOMEM;
	}
	memset(tiler_device, 0x0, sizeof(*tiler_device));

	cdev_init(&tiler_device->cdev, &tiler_fops);
	tiler_device->cdev.owner = THIS_MODULE;
	tiler_device->cdev.ops   = &tiler_fops;

	r = cdev_add(&tiler_device->cdev, dev, 1);
	if (r)
		printk(KERN_ERR "cdev_add():failed\n");

	tilerdev_class = class_create(THIS_MODULE, "tiler");

	if (IS_ERR(tilerdev_class)) {
		printk(KERN_ERR "class_create():failed\n");
		goto EXIT;
	}

	device = device_create(tilerdev_class, NULL, dev, NULL, "tiler");
	if (device == NULL)
		printk(KERN_ERR "device_create() fail\n");

	r = platform_driver_register(&tiler_driver_ldm);

	mutex_init(&mtx);
	INIT_LIST_HEAD(&buf_list.list);
	INIT_LIST_HEAD(&mem_list.list);
	id = 0xda7a000;

	/* Hardcoded for testing */
	div_pt.x = 192;
	div_pt.y = 96;
	tcm = sita_init(256, 128, (void *)&div_pt);
	/*To Do: Error Checking */

EXIT:
	return r;
}

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("davidsin@ti.com");
module_init(tiler_init);
module_exit(tiler_exit);
