#include <mc1322x.h>
#include <board.h>

#include "tests.h"
#include "config.h"

#define debug 1

volatile char rupt = 0;
volatile char s = 0;
uint32_t last_rtc;


void adc_isr(void)
{
	if((*CRM_WU_CNTL&0x8)==0x8)
	{
		*CRM_SLEEP_CNTL |= 0x1; 
	}
	
	if(((ADC->IRQ)&0x1000) == 0x1000)
	{
	    *CRM_WU_CNTL &= ~(0x8);
		uint16_t trig = ADC->TRIGGERS;
		rupt = ((trig&0x0002)==0x0002);
		if((ADC->COMP_1&0x8000)==0x8000)
		{
			s = 1;
			ADC->COMP_1 = 0x13ff;
		}else{
			s = 0;
			ADC->COMP_1 = 0x91ff;
		}
	}

	ADC->IRQ = 0xf000;
}


void main(void)
{
	board_init();
#if debug
	uart_init(UART1, 115200);
#endif

	GPIO->FUNC_SEL.ADC1 = 1;
	GPIO->PAD_DIR.ADC1 = 0;
	GPIO->PAD_KEEP.ADC1 = 0;
	GPIO->PAD_PU_EN.ADC1 = 0;

	adc_init();
	ADC->COMP_1=	0x13ff;	//use channel 3, compare on 0ff
	ADC->SEQ_1 =	0x2;	//only channel 3 is enabled
	ADC->CONTROL &= ~0xe000;
	enable_irq(ADC);	


	#if USE_32KHZ
		/* turn on the 32kHz crystal */
//	putstr("enabling 32kHz crystal\n\r");
	clear_bit(*CRM_RINGOSC_CNTL,0);
	set_bit(*CRM_XTAL32_CNTL,0);
	set_bit(*CRM_SYS_CNTL,5);
	{
		static volatile uint32_t old;
		old = *CRM_RTC_COUNT;
		putstr("waiting for xtal\n\r");
		while(*CRM_RTC_COUNT == old) { 
			continue; 
		}
		/* RTC has started up */

		set_bit(*CRM_SYS_CNTL,5);
//		putstr("32kHZ xtal started\n\r");

	}
	#endif

	#define DELAY 1000000
//	GPIO->FUNC_SEL.TXON = 3;
//	GPIO->PAD_DIR_SET.TXON=1;
	volatile uint32_t i;
	s=1;
	while(1)
	{
		#if debug
		putstr("woke up \n");
		for(i=0; i<DELAY; i++) { continue; }
		#endif
		if(s==1)
		{
  //  		while((CRM->RTC_COUNT - last_rtc) <= 2) {
    //    		CRM->STATUS = ~0; //clear events
	//		}
			#if debug
			putstr("going to sleep again\n");
			#endif
			for(i=0; i<DELAY; i++) { continue; }
			
			CRM->WU_CNTLbits.TIMER_WU_EN = 0;
			CRM->WU_CNTLbits.RTC_WU_EN = 1;
			CRM->WU_CNTLbits.AUTO_ADC = 1;
			CRM->RTC_TIMEOUT = 1000;
			CRM->WU_TIMEOUT = 0;
			
			maca_off();

			CRM->SLEEP_CNTLbits.DOZE = 0;
			CRM->SLEEP_CNTLbits.RAM_RET = 3;
			CRM->SLEEP_CNTLbits.MCU_RET = 1;
			CRM->SLEEP_CNTLbits.DIG_PAD_EN = 1;
			CRM->SLEEP_CNTLbits.HIB = 1;
			while((*CRM_STATUS & 0x1) == 0) { continue; }
			*CRM_STATUS = 1; 
			/* asleep */
			while((*CRM_STATUS & 0x1) == 0) { continue; }
			*CRM_STATUS = 1; 
			CRM->WU_CNTLbits.TIMER_WU_EN = 1;
			CRM->WU_CNTLbits.RTC_WU_EN = 0;
//			last_rtc = CRM->RTC_COUNT;
		}
//		gpio_set(TXON);	
		//putstr("woke up!");
//		for(i=0; i<DELAY; i++) { continue; }
//		gpio_reset(TXON);

		if(rupt==1)
		{
			#if debug
			putstr("adc interrupt! \r\n");
			#endif		
		rupt=0;
		}
	}
}

