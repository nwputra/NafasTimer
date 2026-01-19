#define	DELAY		10
#define	LEDPORT		PORTB
#define LEDDDR		DDRB
#define	LED		3
#define	BUZZ		2

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>


// This special variable(s) assignment which gets retained after Reset
volatile uint8_t wdtcnt  __attribute__ ((section (".noinit"))); 
volatile uint8_t beepcnt __attribute__ ((section (".noinit"))); 


ISR(WDT_OVERFLOW_vect)
{
	uint8_t		i;

	LEDPORT ^= (1<<LED);
	_delay_ms( 25 );
	LEDPORT |= (1<<LED);

	if( --wdtcnt == 0 )
	{
		// Re-arm timer
		wdtcnt = DELAY;

		for( i=beepcnt; i>0; i-- )
		{
			LEDPORT |= (1<<BUZZ);
			_delay_ms( 100 );
			LEDPORT ^= (1<<BUZZ);
			_delay_ms( 100 );
		}

		// Toggle Beep Count ( 1 or 2 )
		if( ++beepcnt > 2 ) beepcnt = 1;
	}
}

 
int main(void) 
{

	// Set up Port pin LED mode to output
	LEDDDR |= ((1<<LED) | (1<<BUZZ));

	if( MCUSR & _BV(WDRF) )
	{
		// Recovery from Watchdog Timer
		MCUSR ^= (1<<WDRF);

		// Not implemented, must not reach here
	}
	else if( (MCUSR & _BV(PORF)) || (MCUSR & _BV(EXTRF)) )
	{
		// Recovery from Power-on or RESET pin

		// Clear Flag
		MCUSR = 0x00;

		// Init the State, 5 sec before starting first beep
		wdtcnt  = 5;
		beepcnt = 1;
	}



	cli();							// Start timed equence
	wdt_reset();						//
	WDTCSR	|= (1<<WDCE) | (1<<WDE);			//
	WDTCSR	=  (1<<WDIE) | (1<<WDP2) | (1<<WDP1);		// Set new prescaler(time-out) value = 128K cycles (~1s)
	sei();							// Enable global interrupts
 
	// Use the Power Down sleep mode and let Watchdog timer wakes up
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// Waiting for interrupt...
	sleep_mode();

	for (;;) 
	{
		// Nothing
	}
}
