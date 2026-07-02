#ifndef TIMER_H
#define TIMER_H

void timer_init(void);
void handle_timer_irq(void);
unsigned long get_system_timer(void);

#endif
