#include "derivative.h"

//initialise pin muxing for all three DSPI modules
void dspi_init(void);

//send and receive data, no fifo enabled, input args:  DSPI0, DSPI1, or DSPI2

/* Initialise ports for DSPI */
void dspi_init(void) {
	/* clock gate */
	SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK | SIM_SCGC6_SPI1_MASK;

	PORTB_PCR3 &= ~PORT_PCR_MUX_MASK;
	PORTB_PCR3 |= PORT_PCR_MUX(1); //SPI1_PCS0   //PTB3 --- we use ADC_CNV (PTB3), GPIO - NOT PCS!
	GPIOB_PDDR |= ~0x08;           //enable PTB3 as input 

	PORTE_PCR0 &= ~PORT_PCR_MUX_MASK;
	PORTE_PCR0 |= PORT_PCR_MUX(2); //SPI1_PCS0   //PTE0 --- leave enabled even though not physically used to avoid problems

	PORTE_PCR2 &= ~PORT_PCR_MUX_MASK;
	PORTE_PCR2 |= PORT_PCR_MUX(2); //SPI1_SCK    //PTE2

	PORTE_PCR3 &= ~PORT_PCR_MUX_MASK;
	PORTE_PCR3 |= PORT_PCR_MUX(2); //SPI1_SIN    //PTE3
}

inline uint16_t dspi_no_fifo_3(uint32_t master) {
	
	//master mode, flush rx fifo, flush tx fifo, halt transfer
	SPI1_MCR = 0x80000C01;

	//master mode, shift incoming data into shift register, ADC inactive state is high, halt transfer
	SPI1_MCR = 0x81010001;

	//frame size is 16bits, divide fsys by 1024 => fnew ~ 10kHz??
	SPI1_CTAR0 = 0xfa000000;

	//Assert PCS bit 16, put data into master pushr (0x0000), needed for shift regs. errors otherwise.
	SPI1_PUSHR = 0x00010000;

	// Clear halt bit - start transfer
	SPI1_MCR &= 0xFFFFFFFE;

	//wait while all bits are clocked in
	while ((SPI1_SR & SPI_SR_TCF_MASK) == 0) {
		;
	}
	SPI1_SR = SPI1_SR | SPI_SR_TCF_MASK;

	// set halt bit - stop transfer
	SPI1_MCR |= 1;

	//read data from POPR
	return SPI1_POPR ;
}  
