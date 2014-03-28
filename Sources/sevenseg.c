#include "sevenseg.h"
#include <stdint.h>
#include "derivative.h"


/* Configuration for data pins, code assumes continuous */
#define DATA_PDDR 	(GPIOD_PDDR)
#define DATA_PDOR 	(GPIOD_PDOR)
#define DATA_SHIFT 	(0)			/* PTD0->PTD7 */
#define DATA_MASK 	(0x00ff)
#define DATA_POL	1			/* 1 = ON */

/* Configuration for segment pins, assume 0-3 in order */
#define SEGS_PDDR 	(GPIOC_PDDR)
#define SEGS_PDOR 	(GPIOC_PDOR)
#define SEGS_SHIFT 	(12)		/* PTC12->PTC15 */
#define SEGS_MASK 	(0xf000)
#define SEGS_POL	1			/* 0 = ON */

char lookup_table[] = {

		0xEB, //0
		0x28, //1
		0xB3, //2
		0xBA, //3
		0x78, //4
		0xDA, //5
		0xDB, //6
		0xA8, //7
		0xFB, //8
		0xF8, //9
		0x43, //L
		0xEB, //O
		0x79, //H
		0x28, //I
		0xC3, //C
		0x04, //DP
};
 
 
//Usage: seg = numtoseg(5) to display "5"
char numtoseg(int num) {
     return lookup_table[num];
}


//FIXME: Make this configurable
static void sevenseg_pinmux(void) {
	/* Enable segments */
	PORTC_PCR(12) |= PORT_PCR_MUX(1);
	PORTC_PCR(13) |= PORT_PCR_MUX(1);
	PORTC_PCR(14) |= PORT_PCR_MUX(1);
	PORTC_PCR(15) |= PORT_PCR_MUX(1);
	/* Enable data */
	PORTD_PCR(0) |= PORT_PCR_MUX(1);
	PORTD_PCR(1) |= PORT_PCR_MUX(1);
	PORTD_PCR(2) |= PORT_PCR_MUX(1);
	PORTD_PCR(3) |= PORT_PCR_MUX(1);
	PORTD_PCR(4) |= PORT_PCR_MUX(1);
	PORTD_PCR(5) |= PORT_PCR_MUX(1);
	PORTD_PCR(6) |= PORT_PCR_MUX(1);
	PORTD_PCR(7) |= PORT_PCR_MUX(1);
}

void sevenseg_init(void) {
	/* Enable pinmux, not configurable for now */
	sevenseg_pinmux();
	/* Enable output for data */
	DATA_PDDR |= DATA_MASK;
	/* Enable output for segments */
	SEGS_PDDR |= SEGS_MASK;
}

void sevenseg_write_segment(int seg, char data) {
	uint32_t data_pdor; /* Temp vars to split long expressions */
	/* We clear/set all bits within the mask that are not to be set/cleared*/
	
	/* CLEAR SEGMENT */
#if SEGS_POL==1
	SEGS_PDOR &= ~(SEGS_MASK);
#else 
	SEGS_PDOR |= (SEGS_MASK);
#endif
	
	/* DATA */
#if DATA_POL==1
	data_pdor = DATA_PDOR;
	data_pdor &= ~(DATA_MASK);
	data_pdor |=  (DATA_MASK & (data<<DATA_SHIFT));
	DATA_PDOR = data_pdor;
#else /* DATA_POL==0 */
	data_pdor = DATA_PDOR
	data_pdor |=  (DATA_MASK);
	data_pdor &= ~(DATA_MASK & (data<<DATA_SHIFT));
	DATA_PDOR = data_pdor;
#endif
	
	/* SET SEGMENT */
#if SEGS_POL==1
	SEGS_PDOR |=  (SEGS_MASK & (1<<(SEGS_SHIFT+seg)));
#else 
	SEGS_PDOR &= ~(SEGS_MASK & (1<<(SEGS_SHIFT+seg));
#endif
	
}
