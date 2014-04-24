#include "trigger.h"
#include "derivative.h"
#include <stddef.h>

/* Data Direction Register */
#define TRIGGER_PDDR 	(GPIOB_PDDR)
/* Data Input Register */
#define TRIGGER_PDIR	(GPIOB_PDIR)
/* Pin Control Register */
#define TRIGGER_PCR		(PORTB_PCR(3))
/* Shift for trigger */
#define TRIGGER_SHIFT	(3)
/* Mask for trigger */
#define TRIGGER_MASK	(0x08)
/* 0 = Falling, 1 = Rising */
#define TRIGGER_EDGE	0

/* Interrupt Configuration */
#define TRIGGER_ISR_IRQ 	 (88)	/* ISR IRQ Number */
#define TRIGGER_ISR_BITREG	 (TRIGGER_ISR_IRQ/32) 	/* Offset for which group of 32 bits ISR is in */	
#define TRIGGER_ISR_PRI		 (0x0)			/* Priority = 0, highest */

/* Global function pointer to ISR callback */
static void (*callback)(void);

void trigger_isr(void) {
	/* Clear interrupt flag */
	TRIGGER_PCR |= PORT_PCR_ISF_MASK;
	/* Call callback */
	(*callback)();	
}

int trigger_isr_start(void (*func)(void)) {
	/* Check callback and set */
	if (func == NULL) {
		return -1;
	}
	callback = func;
	
	/* Clear Pending ISR */
	NVIC_ICPR(TRIGGER_ISR_BITREG) |= 1 << (TRIGGER_ISR_IRQ%32); 
	/* Enable ISR */
	NVIC_ISER(TRIGGER_ISR_BITREG) |= 1 << (TRIGGER_ISR_IRQ%32);
	/* NVIC_IP is 8 bits wide, upper half contains priority so we shift
	 * up by 4. The actual register is 32 bits, but the headers only expose 8.
	 * This is easier than doing the other bit messing.
	 */
	NVIC_IP(TRIGGER_ISR_IRQ) = (TRIGGER_ISR_PRI) << 4;  
	
	/* Set port to GPIO- Needed for interrupt for some reason */
	TRIGGER_PCR &= ~PORT_PCR_MUX_MASK;
	TRIGGER_PCR |= PORT_PCR_MUX(1);
	/* Clear interrupt flag */
	TRIGGER_PCR |= PORT_PCR_ISF_MASK;
	/* Set interrupt to correct edge & enable */
	TRIGGER_PCR &= ~PORT_PCR_IRQC_MASK;
	TRIGGER_PCR	|= ((TRIGGER_EDGE) ? (0b1001) : (0b1010)) << PORT_PCR_IRQC_SHIFT;
	
	/* Success */
	return 0;
}

void trigger_isr_stop(void) {
	/* Unset interrupt */
	TRIGGER_PCR &= ~PORT_PCR_IRQC_MASK;
	/* Clear interrupt flag */
	TRIGGER_PCR |= PORT_PCR_ISF_MASK;
	/* Clear Pending ISR */
	NVIC_ICPR(TRIGGER_ISR_BITREG) |= NVIC_ICPR_CLRPEND(TRIGGER_ISR_IRQ%32); 
	/* Unset ISR */
	NVIC_ISER(TRIGGER_ISR_BITREG) &= ~NVIC_ISER_SETENA(TRIGGER_ISR_IRQ%32);
}

