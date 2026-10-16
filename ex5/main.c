#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00

static const uint32_t INPUT[3] = {TEST_A, TEST_B, TEST_C};
static const uint32_t OUTPUT[3] = {RESPONSE_A, RESPONSE_B, RESPONSE_C};
static volatile bool pending[3] = {false, false, false};

__attribute__((__interrupt__)) static void interrupt_J3(void);

void init(){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);
    
    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

__attribute__((__interrupt__)) static void interrupt_J3(void){ 
	for (int i = 0; i < 3; i++){
		if (gpio_get_pin_interrupt_flag(INPUT[i])){
			gpio_clear_pin_interrupt_flag(INPUT[i]);
			if (gpio_pin_is_low(INPUT[i])){
				pending[i]=true;
			} else {
				pending[i]=false;
			}
		}
	}
}

int main (void){
    init();
	for (int i = 0; i < 3; i++){
		gpio_configure_pin(INPUT[i], GPIO_DIR_INPUT);
		gpio_configure_pin(OUTPUT[i], GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
		gpio_enable_pin_interrupt(INPUT[i],GPIO_PIN_CHANGE);
	}
    
	bool responded[3] = {false, false, false};
	
    while(1){
		for (int i = 0; i < 3; i++){
			if(pending[i] && !responded[i]){
				//if (i == 1) busy_delay_us(100);
				gpio_set_pin_low(OUTPUT[i]);
				responded[i] = true;
			} else if (!pending[i] && responded[i]){
				gpio_set_pin_high(OUTPUT[i]);
				responded[i] = false;
			}
		}
    }
}
