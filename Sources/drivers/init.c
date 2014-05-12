#include "init.h"
#include "dspi.h"
#include "mcg.h" /* Freescale code for pll_init */
#include "derivative.h" /* Headers */

#include "config.h" /* Configuration for choosing peripherals to enable/disable */

/* Function outputs OSCERCLK (should be 50Mhz) to PHY */
static void enable_phyclk(void) {
	/* Enable OSCERCLK0 */
	OSC_CR |= OSC_CR_ERCLKEN_MASK;	/* 1 = enable */
	/* Set CLKOUT to OSCERCLK0 */
	SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(6); /* 6 = OSCERCLK0 */
	/* Set high drive strength */
	PORTC_PCR3 |= PORT_PCR_DSE_MASK;  /* 1 = high speed */
	/* Set fast slew rate */
	PORTC_PCR3 &= ~PORT_PCR_SRE_MASK; /* 0 = fast speed */
	/* Enable pin */
	PORTC_PCR3 &= ~PORT_PCR_MUX_MASK; /* Clear */
	PORTC_PCR3 |= PORT_PCR_MUX(5); /* Set to 5=CLKOUT */	
}

void init_hw(void) {
	/* Enable port clocks */
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
				   | SIM_SCGC5_PORTB_MASK 
				   | SIM_SCGC5_PORTC_MASK 
				   | SIM_SCGC5_PORTD_MASK 
				   | SIM_SCGC5_PORTE_MASK );
	
	/* Set system dividers BEFORE setting clocks */
    SIM_CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)		/* Core at 100Mhz */
                    | SIM_CLKDIV1_OUTDIV2(1)		/* Bus at 50Mhz */
                    | SIM_CLKDIV1_OUTDIV3(1)		/* Flexbus at 50Mhz */
                    | SIM_CLKDIV1_OUTDIV4(3) );		/* Flash at 25Mhz */
	
	/* Setup clocks */
	pll_init(
			50000000,		/* 50MHz external oscillator */
			LOW_POWER,		/* No meaning as external osc */
			CANNED_OSC,		/* External reference clock */
			25,				/* Clock freq = ((50/25)*50)/2 = 100MHz */
			50,
			MCGOUT			/* use as mcgclock */
			);
	
	/* Output clock to PHY */
	enable_phyclk();
	
	#ifdef CONFIG_ADC_SUPPORT
	/* Enable DSPI */
	dspi_init();
	#endif
	
}

