cmd_arch/arm/mach-omap2/clock3xxx_data.o := arm-none-eabi-gcc -Wp,-MD,arch/arm/mach-omap2/.clock3xxx_data.o.d  -nostdinc -isystem /opt/arm-2010q1/bin/../lib/gcc/arm-none-eabi/4.4.1/include -I/home/wtbdaaaa/i8320kernel/arch/arm/include -Iinclude  -include include/generated/autoconf.h -I/home/wtbdaaaa/i8320kernel/samsung/rfs_fsr/include -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(clock3xxx_data)"  -D"KBUILD_MODNAME=KBUILD_STR(clock3xxx_data)"  -c -o arch/arm/mach-omap2/clock3xxx_data.o arch/arm/mach-omap2/clock3xxx_data.c

deps_arch/arm/mach-omap2/clock3xxx_data.o := \
  arch/arm/mach-omap2/clock3xxx_data.c \
  include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/arm-2010q1/bin/../lib/gcc/arm-none-eabi/4.4.1/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/posix_types.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/32v6k.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/irqflags.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/hwcap.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/outercache.h \
    $(wildcard include/config/outer/cache/sync.h) \
    $(wildcard include/config/outer/cache.h) \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/dynamic_debug.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/div64.h \
  include/linux/clk.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/prefetch.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/processor.h \
    $(wildcard include/config/mmu.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  arch/arm/plat-omap/include/plat/control.h \
    $(wildcard include/config/arch/omap2plus.h) \
  arch/arm/mach-omap2/include/mach/io.h \
  arch/arm/plat-omap/include/plat/io.h \
    $(wildcard include/config/arch/omap2420.h) \
    $(wildcard include/config/arch/omap2430.h) \
    $(wildcard include/config/arch/omap3.h) \
    $(wildcard include/config/arch/omap4.h) \
  arch/arm/mach-omap2/include/mach/hardware.h \
  arch/arm/plat-omap/include/plat/hardware.h \
    $(wildcard include/config/reg/base.h) \
    $(wildcard include/config/arch/omap1.h) \
    $(wildcard include/config/mach/samsung/nowplus.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/sizes.h \
  arch/arm/plat-omap/include/plat/cpu.h \
    $(wildcard include/config/arch/omap730.h) \
    $(wildcard include/config/arch/omap850.h) \
    $(wildcard include/config/arch/omap15xx.h) \
    $(wildcard include/config/arch/omap16xx.h) \
    $(wildcard include/config/arch/omap2.h) \
    $(wildcard include/config/arch/omap3430.h) \
  arch/arm/plat-omap/include/plat/multi.h \
  arch/arm/plat-omap/include/plat/serial.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  arch/arm/plat-omap/include/plat/omap7xx.h \
    $(wildcard include/config/base.h) \
  arch/arm/plat-omap/include/plat/omap1510.h \
  arch/arm/plat-omap/include/plat/omap16xx.h \
  arch/arm/plat-omap/include/plat/omap24xx.h \
  arch/arm/plat-omap/include/plat/omap34xx.h \
  arch/arm/plat-omap/include/plat/omap44xx.h \
  arch/arm/mach-omap2/include/mach/board-nowplus.h \
    $(wildcard include/config/twl4030/gpio.h) \
  arch/arm/mach-omap2/include/mach/nowplus.h \
    $(wildcard include/config/samsung/archer/target/sk.h) \
    $(wildcard include/config/archer/target/eu.h) \
    $(wildcard include/config/archer/target/kt.h) \
  arch/arm/mach-omap2/include/mach/ctrl_module_core_44xx.h \
    $(wildcard include/config/idlemode/shift.h) \
    $(wildcard include/config/idlemode/mask.h) \
  arch/arm/mach-omap2/include/mach/ctrl_module_wkup_44xx.h \
  arch/arm/mach-omap2/include/mach/ctrl_module_pad_core_44xx.h \
  arch/arm/mach-omap2/include/mach/ctrl_module_pad_wkup_44xx.h \
  arch/arm/plat-omap/include/plat/clkdev_omap.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/clkdev.h \
  arch/arm/mach-omap2/clock.h \
    $(wildcard include/config/omap/reset/clocks.h) \
    $(wildcard include/config/cpu/freq.h) \
  arch/arm/plat-omap/include/plat/clock.h \
    $(wildcard include/config/pm/debug.h) \
    $(wildcard include/config/debug/fs.h) \
  arch/arm/mach-omap2/clock3xxx.h \
  arch/arm/mach-omap2/clock34xx.h \
  arch/arm/mach-omap2/clock36xx.h \
  arch/arm/mach-omap2/clock3517.h \
  arch/arm/mach-omap2/cm.h \
  arch/arm/mach-omap2/prcm-common.h \
  arch/arm/mach-omap2/cm44xx.h \
  arch/arm/mach-omap2/cm-regbits-34xx.h \
  arch/arm/mach-omap2/prm.h \
    $(wildcard include/config/offset.h) \
  arch/arm/mach-omap2/prm44xx.h \
  arch/arm/mach-omap2/prm-regbits-34xx.h \

arch/arm/mach-omap2/clock3xxx_data.o: $(deps_arch/arm/mach-omap2/clock3xxx_data.o)

$(deps_arch/arm/mach-omap2/clock3xxx_data.o):
