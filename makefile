qemu_cortex_m3:
	rm -rf build/qemu_cortex_m3/ejs \
	&& west build -b qemu_cortex_m3 bsp/qemu_cortex_m3/ejs -d build/qemu_cortex_m3/ejs

bearpi_stm32l431:
	rm -rf build/stm32/bearpi_stm32l431 \
	&& west build -b bearpi_stm32l431 bsp/ejs/stm32/bearpi_stm32l431 -d build/stm32/bearpi_stm32l431

.PHONY: arm_cortex_m3 bearpi_stm32l431
