#ifndef TRIGGER_H_
#define TRIGGER_H_

int trigger_isr_start(void (*func)(void));

void trigger_isr_stop(void);

#endif /* TRIGGER_H_ */
