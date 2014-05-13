/*
 * pwm.h
 *
 * Functions for setting the FlexTimer PWM channel on the trigger boards.
 * Channel is hardcoded, as we only have one.
 * Called from netprot_commands_startstop.c
 */

#ifndef PWM_H_
#define PWM_H_

/* 
 * NAME: pwm_init
 * DESCRIPTION: sets up the PWM channel and PORT mux, does not start PWM
 */
void pwm_init(void);

/*
 * NAME: pwm_start 
 * DESCRIPTION: starts PWM generation
 */
void pwm_start(void);

/*
 * NAME: pwm_stop
 * DESCRIPTION: stops PWM generation
 */
void pwm_stop(void);

/*
 * NAME: pwm_set
 * DESCRIPTION: sets PWM generation frequency and pulse width
 * May not be 100% accurate, best effort.
 * PARAMS:
 * 		int freq:	The frequency
 * 		int width:	The pulse width in nanoseconds
 */
void pwm_set(int freq, int width);

#endif /* PWM_H_ */
