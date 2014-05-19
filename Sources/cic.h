/*
 * Cascaded Integrator Comb filter implementation in C
 * API heavily inspired by CMSIS-DSP, will use same datatypes if included
 */


#ifndef CIC_H
#define CIC_H

#define CIC_MAX_N	 	(3)
#define CIC_MAX_R 	 	(2)

#include <stdint.h>

#ifndef CMSIS_DSP
typedef int32_t q32_t;
#endif


typedef struct cic_decimate_instance_q32_t {
	uint16_t M;			/* Decimation Factor */
	uint8_t  N; 		/* Order */
	uint8_t	 R;			/* Delay scale of comb filter */
	uint32_t G;			/* DC Gain */
	q32_t	 accum[CIC_MAX_N];					/* Accumulators for COMB */
	q32_t 	 prev_accum[CIC_MAX_N];				/* Previous value of accumulators */
	q32_t 	 combs[CIC_MAX_R+1][CIC_MAX_N+1];	/* Values of comb, including previous */
	int nSample;		/*Current sample number */
} cic_decimate_instance_q32;

int cic_decimate_init_q32(cic_decimate_instance_q32 *S, uint16_t M, uint8_t N, uint8_t R, uint32_t G, uint32_t blockSize);

void cic_decimate_q32(cic_decimate_instance_q32 *S, q32_t *pSrc, q32_t *pDst, uint32_t blockSize);
/* CIC_H */
#endif
