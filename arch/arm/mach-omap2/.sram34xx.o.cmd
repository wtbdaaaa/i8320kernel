cmd_arch/arm/mach-omap2/sram34xx.o := arm-none-eabi-gcc -Wp,-MD,arch/arm/mach-omap2/.sram34xx.o.d  -nostdinc -isystem /opt/arm-2010q1/bin/../lib/gcc/arm-none-eabi/4.4.1/include -I/home/wtbdaaaa/i8320kernel/arch/arm/include -Iinclude  -include include/generated/autoconf.h -I/home/wtbdaaaa/i8320kernel/samsung/rfs_fsr/include -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2     -Wa,-march=armv7-a  -c -o arch/arm/mach-omap2/sram34xx.o arch/arm/mach-omap2/sram34xx.S

deps_arch/arm/mach-omap2/sram34xx.o := \
  arch/arm/mach-omap2/sram34xx.S \
    $(wildcard include/config/omap3/sdrc/ac/timing.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/linkage.h \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/hwcap.h \
  arch/arm/mach-omap2/include/mach/hardware.h \
  arch/arm/plat-omap/include/plat/hardware.h \
    $(wildcard include/config/reg/base.h) \
    $(wildcard include/config/arch/omap1.h) \
    $(wildcard include/config/mach/samsung/nowplus.h) \
  /home/wtbdaaaa/i8320kernel/arch/arm/include/asm/sizes.h \
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
  arch/arm/mach-omap2/include/mach/io.h \
  arch/arm/plat-omap/include/plat/io.h \
    $(wildcard include/config/arch/omap2420.h) \
    $(wildcard include/config/arch/omap2430.h) \
    $(wildcard include/config/arch/omap3.h) \
    $(wildcard include/config/arch/omap4.h) \
  arch/arm/mach-omap2/sdrc.h \
  arch/arm/plat-omap/include/plat/sdrc.h \
    $(wildcard include/config/arch/omap2.h) \
  arch/arm/mach-omap2/cm.h \
  arch/arm/mach-omap2/prcm-common.h \
  arch/arm/mach-omap2/cm44xx.h \

arch/arm/mach-omap2/sram34xx.o: $(deps_arch/arm/mach-omap2/sram34xx.o)

$(deps_arch/arm/mach-omap2/sram34xx.o):
