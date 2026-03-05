#ifndef SYSTICK_H
#define SYSTICK_H


extern volatile uint32_t tick_count;

/* configure systick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_1ms(uint32_t count);
/* delay decrement */
void delay_decrement(void);

/* delay a time in microseconds */
void delay_1us(uint32_t count);

void start_timing_ms(void);
uint32_t stop_timing_ms(void);
void print_timing_ms(uint32_t time);

/* measure execution time in microseconds */
void start_timing_us(void);
uint32_t stop_timing_us(void);
void print_timing_us(uint32_t time);

#endif /* SYSTICK_H */
