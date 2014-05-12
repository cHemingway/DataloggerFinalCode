#include "derivative.h"
#include "dspi_common.h"

/* definitions */

#define CORE_SRAM_ADDR 0x20000000 

/* prototypes */

//initialise pin muxing for all three DSPI modules
void dspi_init(void);

//send and receive data, no fifo enabled, input args:  DSPI0, DSPI1, or DSPI2
uint16_t dspi_no_fifo_3(uint32_t master);

/* globals */
volatile struct	SPI_MemMap *ptrDSPI[NUM_DSPI] = {SPI0_BASE_PTR, SPI1_BASE_PTR, SPI2_BASE_PTR};


/********************************************************************/
/* Initialise all ports for DSPI */
/********************************************************************/
void dspi_init(void) 
{
  /* clock gate */
  SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK|SIM_SCGC6_SPI1_MASK;
  
  PORTB_PCR3 &= ~PORT_PCR_MUX_MASK;
  PORTB_PCR3 |= PORT_PCR_MUX(1); //SPI1_PCS0   //PTB3 --- we use ADC_CNV (PTB3), GPIO - NOT PCS!
  GPIOB_PDDR |= ~0x08;           //enable PTB3 as input 
  
  PORTE_PCR0 &= ~PORT_PCR_MUX_MASK;
  PORTE_PCR0 |= PORT_PCR_MUX(2); //SPI1_PCS0   //PTE0 --- leave this enabled even though not physically used to avoid problems
  
  PORTE_PCR2 &= ~PORT_PCR_MUX_MASK;
  PORTE_PCR2 |= PORT_PCR_MUX(2); //SPI1_SCK    //PTE2
  
  PORTE_PCR3 &= ~PORT_PCR_MUX_MASK;
  PORTE_PCR3 |= PORT_PCR_MUX(2); //SPI1_SIN    //PTE3
  
}

uint16_t dspi_no_fifo_3(uint32_t master)
{
	//master mode, flush rx fifo, flush tx fifo, halt transfer
	ptrDSPI[master]->MCR = 0x80000C01;
	
	/********************************************************************************/
	
	//master mode, shift incoming data into shift register, ADC inactive state is high, halt transfer
	ptrDSPI[master]->MCR = 0x81010001;
	
	//frame size is 16bits, divide fsys by 1024 => fnew ~ 10kHz??
	ptrDSPI[master]->CTAR[0] = 0x78000001;
 
	/* Send Data */
	
	//Assert PCS bit 16, put (0x0000) into master pushr, needed for shift regs. errors otherwise
	ptrDSPI[master]->PUSHR = 0x00010000;

	// Clear HALT bit
	ptrDSPI[master]->MCR &= 0xFFFFFFFE;
	
	//wait while
	while((ptrDSPI[master]->SR & SPI_SR_TCF) == 0){;}  
	ptrDSPI[master]->SR = ptrDSPI[master]->SR | SPI_SR_TCF;
		
	/* halt */
	// set HALT bit to stop transfer
	ptrDSPI[master]->MCR |= 1;
	
	/* read data */
	return ptrDSPI[master]->POPR;
}
