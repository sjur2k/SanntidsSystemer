/*
 * busy_delay.c
 * Modified delay.c
 *
 * Created: 22.06.2012 11:04:24
 *  Author: oyvindne
 */ 

//_____  I N C L U D E S ___________________________________________________

#include "delay.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//! CPU frequency
static unsigned long s_fcpu_hz;

//_____ D E C L A R A T I O N S ____________________________________________

void busy_delay_init(unsigned long fcpu_hz)
{
    s_fcpu_hz = fcpu_hz;
}


void busy_delay_ms(unsigned long delay)
{
    cpu_delay_ms(delay, s_fcpu_hz);
}

void busy_delay_us(unsigned long delay)
{
    cpu_delay_us(delay, s_fcpu_hz);
}