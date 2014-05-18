#include "pwm.h"
#include "derivative.h"

/* Code assumes module is FTM0, which is probably will be for foreseeable future */
/* Also assumes bus clock = 50MHZ */
#define BUS_CLOCK 		(50000000) 

/* Channel number */
#define PWM_CHANNEL 	(3)

/* Polarity 0= Active High, 1 = Active Low */
#define PWM_POLARITY	(1)

#define PWM_DEFAULT_FREQ 	(100000)
#define PWM_DEFAULT_WIDTH 	(3000)

/* Channel registers used */
#define PWM_CHAN_SC FTM0_CnSC(PWM_CHANNEL)	/* Channel status and control */
#define PWM_CHAN_V	FTM0_CnV(PWM_CHANNEL)	/* Channel value */

/* PORT register used */
#define PORT_PCR_PWM PORTC_PCR4
#define PORT_PCR_PWM_MUX (4)	/* Mode 5 = FTM0_CH3 */


void pwm_init(void) {
	/* Enable FlexTimer module clock gate */
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	/* Disable write protection */
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;

	/* Enable extra flextimer features */
	FTM0_MODE |= FTM_MODE_FTMEN_MASK;
	
	/* Disable quadrature mode */
	FTM0_QDCTRL &=~FTM_QDCTRL_QUADEN_MASK;  
		
	/* Set channel mode to edge aligned PWM, high true */
	PWM_CHAN_SC |= FTM_CnSC_ELSB_MASK;
	PWM_CHAN_SC &= ~FTM_CnSC_ELSA_MASK;
	PWM_CHAN_SC |= FTM_CnSC_MSB_MASK;
	
	/* Set counter to zero */
	FTM0_CNT = 0;
	
	/* Set frequency and pulse width */
	pwm_set(PWM_DEFAULT_FREQ, PWM_DEFAULT_WIDTH);

	/* Set counter initial value to zero */
	FTM0_CNTIN = 0;
	
	/* Set clock divider to 2x */
	FTM0_SC |= FTM_SC_PS(1);
	
	/* Set pin mux to enable pin */
	PORT_PCR_PWM &= ~PORT_PCR_MUX_MASK; /* Clear */
	PORT_PCR_PWM |= PORT_PCR_MUX(PORT_PCR_PWM_MUX);
}

void pwm_start(void) {
	/* Set module count using system clock*/
	FTM0_SC |= FTM_SC_CLKS(1);
	
	/* Load PWM */
	FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK_MASK | FTM_PWMLOAD_CH3SEL_MASK;
}

void pwm_stop(void) {
	/* Stop module clock */
	FTM0_SC &= ~FTM_SC_CLKS(3);
}

void pwm_set(int freq, int width) {
	/* Prescaler is 2, bus clock is set above, -1 as rollover */
	FTM0_MOD = (BUS_CLOCK)/(2*freq)-1;
	
	/* Channel width is in ns, so divide width by period in ns */
	/* Care must be taken here to avoid overflow */
	PWM_CHAN_V = width / (1000000000/(BUS_CLOCK/2));
}
