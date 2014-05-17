#include <stdio.h>


extern unsigned long _swtp_addr;//beginning of trace points table
extern unsigned long _swtp_end;//end of trace points table

#define ETM_DIS 0xFFFFF7FF
#define ETM_EN 0x800
#define SVC_OPCODE     0xDF00
#define START_BIT	   0x1//the type of trace point 0x1 Start 0x0 Stop
#define LOCKING_MASK 0x00000001;
#define DIS_PROG_BIT 0xFFFFFBFF
#define EN_PROG_BIT 0x400
#define EN_POWER_DOWN_BIT 0x1
#define DIS_POWER_DOWN_BIT 0xFFFFFFFE


void empty_interrupt_handler() {
	//nothing for now
}

void setupTracepoints(unsigned long *intRetAddr) {
	//ETM
	volatile unsigned long *etmcr = (unsigned long *) 0xE0041000;
	volatile unsigned long *etmlar = (unsigned long *) 0xE0041FB0;
	volatile unsigned long *etmlsr = (unsigned long *) 0xE0041FB4;

	int trace_enabled = 0;
	unsigned long *crt_hash_addr, *hash_start_addr;
	// tracepoints hash table contents
	unsigned long virtAddr, rest, retAddr = 0, opcode;
	unsigned short flags;
	char tracepointFound = 0;

	//disable trace to avoid trace collection inside the handler
	*etmlar = 0xC5ACCE55; //unlock ETM regs
	*etmcr = *etmcr | EN_PROG_BIT; //setting ETM programming bit
	*etmcr = *etmcr & ETM_DIS;
	*etmcr = *etmcr | EN_POWER_DOWN_BIT;
	*etmcr = *etmcr & DIS_PROG_BIT; //reset ETM programming bit

	retAddr = *intRetAddr;
	retAddr -= 2;
	hash_start_addr = &_swtp_addr;
	crt_hash_addr = hash_start_addr;

	while ((*crt_hash_addr) != 0) {
		virtAddr = *crt_hash_addr;
		crt_hash_addr ++;
		rest = (*crt_hash_addr);
		flags = (unsigned short)(rest & 0xFFFF);
		crt_hash_addr ++;
		if ( *(unsigned short*)virtAddr == SVC_OPCODE ) {
			opcode = *crt_hash_addr;

			if (virtAddr == retAddr) {
				// found the tracepoint that was hit
				tracepointFound = 1;
				// write back the original opcode
				*(unsigned short*)virtAddr = opcode;

				if (flags & START_BIT) {
					// enable trace
					trace_enabled = 1;
				} else {
					// disable trace
					trace_enabled = 0;
				}
			}
		} else {
			// some other tracepoint; need to put the svc opcode back
			if ( virtAddr != retAddr) {
				// write back the svc opcode
				*(unsigned short*)virtAddr = SVC_OPCODE;
			}
		}
		crt_hash_addr ++;
	}

	if (tracepointFound) {
		if (trace_enabled) {
			//enable ETM trace
			//update the return address
			*etmlar = 0xC5ACCE55; //unlock ETM regs
			*etmcr = *etmcr | EN_PROG_BIT; //setting ETM programming bit
			*etmcr = *etmcr | ETM_EN;
			//reset power down bit
			*etmcr = *etmcr & DIS_POWER_DOWN_BIT;
			*etmcr = *etmcr & DIS_PROG_BIT; //reset ETM programming bit
			*etmlar &= LOCKING_MASK;
			//set the return address
			*intRetAddr = *intRetAddr - 0x2;

		} else {
			// disable trace to avoid branches/calls
			*etmlar = 0xC5ACCE55; //unlock ETM regs
			*etmcr |= EN_PROG_BIT; //setting ETM programming bit
			*etmcr &= ETM_DIS;
			*etmcr |= EN_POWER_DOWN_BIT;
			*etmcr &= DIS_PROG_BIT; //reset ETM programming bit
			*etmlar &= LOCKING_MASK;
			//set the return address
			*intRetAddr = *intRetAddr - 0x2;
		}
	}
}
