#include "cic.h"
#include <stddef.h>
#include <string.h>

int cic_decimate_init_q32(cic_decimate_instance_q32 *S, uint16_t M, uint8_t N, uint8_t R, uint32_t G, uint32_t blockSize)
{
	if(S==NULL) { /* Check for null pointers */
		return -1;
	}

	if (R > CIC_MAX_R) { /* R is above limit */
		return -1;
	}

	if (N > CIC_MAX_N) { /* M is above limit */
		return -1;
	}

	/* Clear Arrays, we can't just use sizeof as we have a pointer */
	memset(S->accum, 0, sizeof S->accum[0] * CIC_MAX_N);
	memset(S->prev_accum, 0, sizeof S->prev_accum[0] * CIC_MAX_N);
	memset(S->combs, 0, sizeof S->combs[0][0] * (CIC_MAX_N+1) * (CIC_MAX_R+1));

	S->M = M;
	S->N = N;
	S->R = R;
	S->G = G;
	S->nSample = 0;
	return 0; /* Success */
}

void cic_decimate_q32(cic_decimate_instance_q32 *S, q32_t *pSrc, q32_t *pDst, uint32_t blockSize)
{
	int i = 0, j = 0, k = 0;
	uint16_t M = 	S->M; 			/*Decimation Factor */
	uint8_t	 R = 	S->R;			/*Differential delay scale in comb */
	uint32_t G = 	S->G;			/* DC Gain */
	uint8_t	 nStages = 	S->N;		/*Number of stages */
	int nSample = S->nSample;


	/* Downsample each element one by one */
	j = 0; /* Output Index */
	for (i=0; i<blockSize; i++) {
		int n;

		/*INTEGRATE*/
		S->accum[0] = S->prev_accum[0] + pSrc[i]; /* 1st Stage */
		for (n=1; n<nStages; n++) {	/* Remaining Stages */
			/* Output =  Last Output Value + Output of Previous Stage */
			S->accum[n] = S->prev_accum[n] + S->accum[n-1];
		}
		/* Copy back to delay */
		memcpy(S->prev_accum, S->accum, sizeof S->prev_accum[0] * CIC_MAX_N);

		if (nSample==M-1) { /*If we are at a decimate block*/

			/* COMB */
			/* First Value - Store in _Input_ */
			S->combs[R][0] = S->accum[nStages-1];

			/* Next Values */
			for (n=1;n<=nStages;n++) {
				S->combs[R][n] = S->combs[R][n-1] - S->combs[0][n-1];
			}

			/* Output */
			pDst[j++] = S->combs[R][nStages] / G;
			nSample = 0;

			/* Copy back delays */
            for (n=0;n<=nStages+1;n++) {
                for (k=R;k>0;k--) {
					S->combs[k-1][n] = S->combs[k][n];
				}
			}

		}
		else {
			nSample += 1;
		}

	}
	S->nSample = nSample;
}
