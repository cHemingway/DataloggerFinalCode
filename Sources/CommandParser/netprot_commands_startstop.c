#include "fnet.h" /* For Delay */

#include "config.h"

#include "netprot_commands_startstop.h"
#include "netprot_setget_params.h"
#include "trigger.h"
#include "capture.h"
#include "pwm.h"
#include <string.h>
#include "stdio.h"


int netprot_cmd_start(const char *in, char *out, int outlen) {
	/* Choose action based on board type */
	#if CONFIG_BOARD == CONFIG_BOARD_ADC
	netprot_param *capture_delay;
	netprot_find_object_attr("CHANNEL","DELAY",&capture_delay);
	/* Setup capture */
	capture_setup(CONFIG_SAMPLES_PER_BUFFER,capture_delay->intval);
	
	/* Start capture isr */
	trigger_isr_start(capture_isr);
	#elif CONFIG_BOARD == CONFIG_BOARD_PWM
	{
		netprot_param *pwm_freq, *pwm_width, *pwm_delay;
		netprot_find_object_attr("PWM","FREQ", &pwm_freq);
		netprot_find_object_attr("PWM","WIDTH",&pwm_width);
		netprot_find_object_attr("PWM","DELAY",&pwm_delay);
		pwm_set(capture_freq->intval, capture_width->intval);
		/* Delay for specified number of milliseconds */
		fnet_timer_delay(fnet_timer_ms2ticks(capture_delay->intval));
		pwm_start();
	}
	#else
		#error "Unknown board type"
	#endif
	
	/* Don't output anything */
	*out = '\0';
	/* Success */
	return 0;
}

int netprot_cmd_stop(const char *in, char *out, int outlen) {
	char retval[] = "+OK \r\n";
	/* Choose action based on board type */
	#if CONFIG_BOARD == CONFIG_BOARD_ADC
		/* Stop capture isr */
		trigger_isr_stop();
	#elif CONFIG_BOARD == CONFIG_BOARD_PWM
		pwm_stop();
	#else
		#error "Unknown board type"
	#endif
	
	/* Output OK */
	if (outlen < sizeof(retval))
	{
		return -1;
	} else {
		strcpy(out,retval);
		return 0; /* Success */
	}
}


