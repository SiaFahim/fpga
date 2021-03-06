/*
 * Empty C++ Application
 */

#include "xgpio.h"
#include "xil_printf.h"

XGpio Gpio;

/*
 * Simple delay function
 */
void delay(int time) {
	for (int i = 0; i < time * 50000; i++) {
	}
}

int data = 64;

u8 oled_byte(u8 in) {
	return ((in & 8) << 3) | ((in & 4) << 2) | ((in & 2) << 1) | (in & 1);
}

void vdd(bool value) {
	if (value) {
		data &= ~0x01;
	} else {
		data |= 0x01;
	}
	XGpio_DiscreteWrite(&Gpio, 1, data);
}

void vbat(bool value) {
	if (value) {
		data &= ~0x02;
	} else {
		data |= 0x02;
	}
	XGpio_DiscreteWrite(&Gpio, 1, data);
}

void reset(bool value) {
	if (value) {
		data |= 0x04;
	} else {
		data &= ~0x04;
	}
	XGpio_DiscreteWrite(&Gpio, 1, data);
}

void sclk(bool value) {
	if (value) {
		data |= 0x08;
	} else {
		data &= ~0x08;
	}
	XGpio_DiscreteWrite(&Gpio, 1, data);
}

void sdata(bool value) {
	if (value) {
		data |= 0x10;
	} else {
		data &= ~0x10;
	}
	XGpio_DiscreteWrite(&Gpio, 1, data);
}

void scmd(bool value) {
	if (value) {
		data |= 0x20;
	} else {
		data &= ~0x20;
	}
	XGpio_DiscreteWrite(&Gpio, 1, data);
}

void spi_delay() {
	for (int i = 0; i < 0; i++) {
	}
}

void spi_write(u8 data, bool cmd) {
	scmd(cmd);
	sclk(false);
	for (int i = 7; i >= 0; i--) {
		sdata((data >> i) & 1);
		sclk(true);
		spi_delay();
		sclk(false);
	}
}

int main() {

	/*
	 * Initialize the GPIO driver
	 */
	int status = XGpio_Initialize(&Gpio, 0);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the direction of all port to be outputs
	 */
	XGpio_SetDataDirection(&Gpio, 1, 0);

	print("Power Up Sequence Start\n");
	xil_printf("Hex: %x", oled_byte(0xF));
	vdd(true);
	delay(5);
	reset(false);
	delay(5);
	reset(true);
	delay(5);
	vbat(true);
	print("Power Up Sequence Done\n");

	spi_write(0xAE, 0); // Display Off
	spi_write(0x8D, 0); // Pump 1
	spi_write(0x14, 0);
	spi_write(0xD9, 0); // Precharge
	spi_write(0xF1, 0);
	spi_write(0x81, 0); // Contrast
	spi_write(0xFF, 0);
	spi_write(0xA4, 0); // Display from RAM
	spi_write(0xAF, 0); // Display On

	delay(5);

	spi_write(0x20, 0); // Vertical Addressing
	spi_write(0x01, 0);

	spi_write(0xFF, 1);

	u8 temp = 0;
	while (1) {
		for (int i = 0; i < 64 * 16; i++) {
			spi_write(oled_byte(temp), 1);
		}
		//xil_printf("Hex: %x", oled_byte(temp));
		temp++;
		delay(1000);
	}

	return 0;
}

