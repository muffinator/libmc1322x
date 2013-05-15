#include <mc1322x.h>
#include <board.h>
#include <stdio.h>
#include <string.h>

#include "tests.h"
#include "config.h"
#include "sleep2.h"
#include "stuffing.h"

#define debug 0
#define compare 0x3b1
#define tsleep 20000

volatile char rupt = 0;
volatile char s = 0;
volatile uint16_t data[256];
volatile uint8_t idx = 0;
volatile uint8_t xmit = 0;
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
			CRM->WU_CNTLbits.TIMER_WU_EN = 1;
			CRM->WU_CNTLbits.RTC_WU_EN = 0;
			CRM->WU_CNTLbits.AUTO_ADC = 0;
			CRM->RTC_TIMEOUT = 1000;
			CRM->WU_TIMEOUT = tsleep;
			
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

			CRM->VREG_CNTLbits.VREG_1P8V_EN = 1; 
			CRM->VREG_CNTLbits.VREG_1P5V_EN = 3;
			while(CRM->STATUSbits.VREG_1P8V_RDY == 0) { continue; }
			while(CRM->STATUSbits.VREG_1P5V_RDY == 0) { continue; }
			maca_on();
			set_adc(ADC1);
			set_adc(ADC0);
			set_adc(ADC3);
			gpio_set(KBI5);
			idx=0;
			xmit=0;
			memset((uint16_t *)data,0,256);
			ADC->COMP_1 = (compare|0x9000); //waiting for vbat to drop now
		}
	ADC->IRQ = 0x1000;
	}
	if(((ADC->IRQ)&0x4000) == 0x4000)
	{
		ADC->IRQ = 0x4000;
	}
	if(((ADC->IRQ)&0x2000) == 0x2000)
	{
		ADC->IRQ = 0x2000;
	}
	if(((ADC->IRQ)&0x8000) == 0x8000)
	{
		uint16_t temp = ADC->FIFO_READ;
		if((temp&0xf000)==0x3000)
		{	
			if(idx<=15)
			{
				data[idx] = temp;
				idx++;
			}else{
				if(xmit!=2)
				{
					gpio_set(KBI7);
					xmit = 1;
				}
			}
		}
		ADC->IRQ = 0x8000;
	}
}

#define PAYLOAD_LEN 125

uint32_t x;
void main(void)
{
//	volatile packet_t *p;
	board_init();

#if debug
	uart_init(UART1, 115200);
#endif
	set_adc(ADC1);
	set_adc(ADC0);
	set_adc(ADC3);

	adc_init();
	ADC->COMP_1 = (0x9000|compare); //waiting for vbat to drop now
	ADC->SR_1_HIGH = 0;
	ADC->SR_1_LOW = 65;
	ADC->SEQ_1 =	0x800a;	//only channel 1 is enabled
	ADC->CONTROL |= 0xf003;
	ADC->FIFO_CONTROL = 0x7;
	enable_irq(ADC);	
	s=0;

	trim_xtal();
	vreg_init();

	maca_init();
	set_channel(0);
	set_power(0x12);
	#define DELAY 3000000
//	volatile uint32_t i;
	volatile packet_t *p;
	set_out(KBI3);
	set_out(KBI7);
	set_out(KBI5);
	memset((uint16_t *)(data),0,256);
	idx=0;
	xmit=0;
	while(1)
	{
		#if debug
		putstr("woke up \n");
//		for(i=0; i<DELAY; i++) { continue; }
		#endif
		if(xmit==1)
		{
		gpio_reset(KBI7);
//		for(i=0; i<DELAY; i++) { continue; }
//		for(i=0; i<DELAY; i++) { continue; }
			check_maca();
			free_all_packets();
			while((p=rx_packet())) {
				if(p) free_packet(p);
			}
			p = get_free_packet();
			if(p) {
				fill_packet(p, (uint8_t *)data, 31);
				tx_packet(p);
			}	
			xmit=2; //set to two, so we don't transmit again.
//			idx=0;	//just using this for testing
//			memset((uint16_t *)(data),0,256);//just using this for testing
		gpio_reset(KBI3);
		}
		if(rupt==1)
		{
			#if debug
			putstr("adc interrupt! \r\n");
			#endif		
		rupt=0;
		}
	}
}

