/*
 * busy_delay.h
 * Modifed delay.h
 *
 * Created: 22.06.2012 11:02:13
 *  Author: oyvindne
 */ 


#ifndef BUSY_DELAY_H_
#define BUSY_DELAY_H_


/**
 * \defgroup group_avr32_services_basic_delay TIMING - Delay function
 *
 * Driver for busy-waiting. Supports delaying a number of milliseconds,
 * and works in both standalone and with FreeRTOS.
 *
 * \{
 */

#include "compiler.h"
#ifdef FREERTOS_USED
# include "FreeRTOS.h"
# include "task.h"
#else
# include "cycle_counter.h"
#endif

/*!
 * \brief Initialize the delay driver.
 *
 * \param  fcpu_hz: CPU frequency in Hz.
 */
void busy_delay_init(unsigned long fcpu_hz);


/*!
 * \brief Busy wait during at least the specified delay (in millisecond) before returning.
 *
 * \param  delay:   Number of millisecond to wait.
 */
void busy_delay_ms(unsigned long delay);


/*!
 * \brief Busy wait during at least the specified delay (in microsecond) before returning.
 *
 * \param  delay:   Number of microsecond to wait.
 */
void busy_delay_us(unsigned long delay);

/**
 * \}
 */


#endif /* BUSY_DELAY_H_ */