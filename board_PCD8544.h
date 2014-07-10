/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/PCD8544/board_PCD8544.h
 * @brief   GDISP Graphic Driver subsystem board interface for the PCD8544 display.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.
#define PCD8544_SPI_PORT	GPIOA
#define PCD8544_SPI_NSS		4
#define PCD8544_SPI_SCK		5
#define PCD8544_SPI_MISO	6
#define PCD8544_SPI_MOSI	7

#define PCD8544_PIN_PORT	GPIOA
#define PCD8544_PIN_RST		3
#define PCD8544_PIN_DC		2

#define SET_RST				palSetPad(PCD8544_PIN_PORT, PCD8544_PIN_RST);
#define CLR_RST				palClearPad(PCD8544_PIN_PORT, PCD8544_PIN_RST);

/*
 * Maximum speed SPI configuration (18MHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig hs_spicfg = {
	NULL,
	PCD8544_SPI_PORT,
	PCD8544_SPI_NSS,
	0
};

#if GFX_USE_OS_CHIBIOS
	static int32_t thdPriority = 0;
#endif

static inline void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state) {
		CLR_RST;
	} else {
		SET_RST;
	}
}

static inline void init_board(GDisplay *g) {
	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
        /*
         * SPI1 I/O pins setup.
         */
	    palSetPadMode(PCD8544_PIN_PORT, PCD8544_PIN_RST,  PAL_MODE_OUTPUT_PUSHPULL);            /* RESET */
	    setpin_reset(g, TRUE);
	    palSetPadMode(PCD8544_PIN_PORT, PCD8544_PIN_DC,   PAL_MODE_OUTPUT_PUSHPULL);            /*  D/C  */

        palSetPadMode(PCD8544_SPI_PORT, PCD8544_SPI_SCK,  PAL_MODE_STM32_ALTERNATE_PUSHPULL);   /* SCK. */
        palSetPadMode(PCD8544_SPI_PORT, PCD8544_SPI_MISO, PAL_MODE_STM32_ALTERNATE_PUSHPULL);   /* MISO.*/
        palSetPadMode(PCD8544_SPI_PORT, PCD8544_SPI_MOSI, PAL_MODE_STM32_ALTERNATE_PUSHPULL);   /* MOSI.*/
        palSetPadMode(PCD8544_SPI_PORT, PCD8544_SPI_NSS,  PAL_MODE_OUTPUT_PUSHPULL);            /* NSS  */
        palSetPad(PCD8544_SPI_PORT,     PCD8544_SPI_NSS);

		spiInit();
		break;
	}
}

static inline void acquire_bus(GDisplay *g) {
	(void) g;
	#if GFX_USE_OS_CHIBIOS
		thdPriority = (int32_t)chThdGetPriority();
		chThdSetPriority(HIGHPRIO);
	#endif
	spiAcquireBus(&SPID1);
}

static inline void release_bus(GDisplay *g) {
	(void) g;
	#if GFX_USE_OS_CHIBIOS
		chThdSetPriority(thdPriority);
	#endif
	spiReleaseBus(&SPID1);
}

static inline void write_cmd(GDisplay *g, uint8_t index) {
	(void)	g;

	palClearPad(PCD8544_PIN_PORT, PCD8544_PIN_DC);

	spiStart(&SPID1, &hs_spicfg);
	spiSelect(&SPID1);
	spiSend(&SPID1, 1, &index);
	spiUnselect(&SPID1);
	spiStop(&SPID1);
}

static inline void write_data(GDisplay *g, uint8_t * data, uint16_t length) {
	(void) g;

	palSetPad(PCD8544_PIN_PORT, PCD8544_PIN_DC);

	spiStart(&SPID1, &hs_spicfg);
	spiSelect(&SPID1);
	spiSend(&SPID1, length, &data);
	spiUnselect(&SPID1);
	spiStop(&SPID1);
}

static inline void post_init_board(GDisplay *g) {
    (void) g;
}

static inline void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

#endif /* _GDISP_LLD_BOARD_H */
