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
	/* Setup capture */
	capture_setup(CONFIG_SAMPLES_PER_BUFFER);
	
	/* Start capture isr */
	trigger_isr_start(capture_isr);
	#elif CONFIG_BOARD == CONFIG_BOARD_PWM
	{
		netprot_param *capture_freq, *capture_width;
		netprot_find_object_attr("PWM","FREQ", &capture_freq);
		netprot_find_object_attr("PWM","WIDTH",&capture_width);
		pwm_set(capture_freq->intval, capture_width->intval);
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


