#ifndef SEVENSEG_H_
#define SEVENSEG_H_

/*Decimal Points*/
#define DP_0	1
#define DP_1	2
#define DP_2	4
#define DP_3	8

void sevenseg_init(void);

void sevenseg_set(const char segs[4], int DP);

#endif /* SEVENSEG_H_ */
