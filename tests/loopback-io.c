/*
 * Copyright (c) 2010, Mariano Alvira <mar@devl.org> and other contributors
 * to the MC1322x project (http://mc1322x.devl.org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of libmc1322x: see http://mc1322x.devl.org
 * for details. 
 *
 *
 */

#include <mc1322x.h>
#include <board.h>
#include <stdio.h>
#include "config.h"

#define DELAY 40000
#define readg(x) GPIO->DATA.x
#define setdo(x) GPIO->PAD_DIR_SET.x=1
#define setdi(x) GPIO->PAD_DIR_RESET.x=1

void main(void) {
	GPIO->FUNC_SEL0 = 0xffffffff;
	GPIO->FUNC_SEL1 = 0xffffffff;
	GPIO->FUNC_SEL2 = 0xffffffff;
	GPIO->FUNC_SEL3 = 0xffffffff;
	uart_init(UART1,115200);
	GPIO->FUNC_SEL.TDI=3;
	GPIO->FUNC_SEL.ADC7=3;
	GPIO->PAD_PU_EN.VREF1L=1;
	GPIO->PAD_PU_SEL.VREF1L=1;
	GPIO->PAD_KEEP.VREF1L=1;
	setdo(GPIO_40);
	setdo(ADC0);
	setdo(ADC2);
	setdo(ADC4);
	setdo(ADC6);
	setdo(U2CTS);
	setdo(U2TX);
	setdo(SDA);
	setdo(TMR2);
	setdo(TMR0);
	setdo(MOSI);
	setdo(SS);
	setdo(FSYN);
	setdo(SSITX);
	setdo(KBI6);
	setdo(KBI3);
	setdo(TXON);
	setdo(ANT2);
	setdo(VREF2L);
	setdo(TDO);
	setdo(ADC7);
	setdo(EVTI);
//inputs
	setdi(ADC1);
	setdi(ADC3);
	setdi(VREF1L);
	setdi(ADC5);
	setdi(U2RTS);
	setdi(U2RX);
	setdi(U1CTS);
	setdi(SCL);
	setdi(TMR3);
	setdi(TMR1);
	setdi(SCK);
	setdi(MISO);
	setdi(BTCK);
	setdi(SSIRX);
	setdi(KBI7);
	setdi(KBI2);
	setdi(ANT1);
	setdi(RXON);
	setdi(VREF2H);
	setdi(TDI);
	GPIO->PAD_PU_SEL.VREF2H=1;
	GPIO->PAD_KEEP.VREF2H=1;
	GPIO->PAD_PU_EN.U1CTS=0;
	GPIO->PAD_PU_SEL.U1CTS=0;
	GPIO->PAD_PU_SEL.SCL=0;
	volatile uint32_t i;
	volatile int okay1=0;
	volatile int okay2=0;
	volatile int okay3=0;
	volatile int count1=0;
	volatile int count2=0;
	volatile int count3=0;
	while(1) {

		gpio_set(VREF1H);
		gpio_set(ADC0);
		gpio_set(ADC2);
		gpio_set(ADC4);
		gpio_set(ADC6);
		gpio_set(U2CTS);
		gpio_set(U2TX);
		gpio_set(SDA);
		gpio_set(TMR2);
		gpio_set(TMR0);
		gpio_set(MOSI);
		gpio_set(SS);
		gpio_set(FSYN);
		gpio_set(SSITX);
		gpio_set(KBI6);
		gpio_set(KBI3);
		gpio_set(TXON);
		gpio_set(ANT2);
		gpio_set(VREF2L);
		gpio_set(ADC7);
		gpio_set(EVTI);
		printf("	1: %d %d %d %d %d %d %d %d 2: %d %d %d %d %d %d %d %d %d %d 3: %d %d \n", readg(VREF1L), readg(ADC1), readg(ADC3), readg(ADC5), readg(U2RTS), readg(U2RX), readg(U1CTS), readg(SCL), readg(TMR3), readg(TMR1), readg(SCK), readg(MISO), readg(BTCK), readg(SSIRX), readg(KBI7), readg(KBI2), readg(ANT1), readg(RXON), readg(VREF2H), readg(TDO));
		okay1 = (readg(VREF1L)&&readg(ADC1)&&readg(ADC3)&&readg(ADC5)&&readg(U2RTS)&&readg(U2RX)&&readg(U1CTS)&&readg(SCL));
		okay2 = (readg(TMR3)&&readg(TMR1)&&readg(SCK)&&readg(MISO)&&readg(BTCK)&&readg(SSIRX)&&readg(KBI7)&&readg(KBI2)&&readg(ANT1)&&readg(RXON));
		okay3=(readg(TDI));
		if ((count1!=2)&&(okay1==1)) count1=1;
		if ((okay2==1)&&(count2!=2)) count2=1;
		if ((okay3==1)&&(count3!=3)) count3=1;

		for(i=0; i<DELAY; i++) { continue; }
		gpio_reset(VREF1H);
		gpio_reset(ADC0);
		gpio_reset(ADC2);
		gpio_reset(ADC4);
		gpio_reset(ADC6);
		gpio_reset(U2CTS);
		gpio_reset(U2TX);
		gpio_reset(SDA);
		gpio_reset(TMR2);
		gpio_reset(TMR0);
		gpio_reset(MOSI);
		gpio_reset(SS);
		gpio_reset(FSYN);
		gpio_reset(SSITX);
		gpio_reset(KBI6);
		gpio_reset(KBI3);
		gpio_reset(TXON);
		gpio_reset(ANT2);
		gpio_reset(VREF2L);
		gpio_reset(ADC7);
		gpio_reset(EVTI);
		okay1 = (readg(VREF1L)||readg(ADC1)||readg(ADC3)||readg(ADC5)||readg(U2RTS)||readg(U2RX)||readg(U1CTS)||readg(SCL));
		okay2 = (readg(TMR3)||readg(TMR1)||readg(SCK)||readg(MISO)||readg(BTCK)||readg(SSIRX)||readg(KBI7)||readg(KBI2)||readg(ANT1)||readg(RXON));
		okay3=(readg(TDI));
		if ((okay1==0)&&(count1==1)) count1=2;
		if ((okay2==0)&&(count2==1)) count2=2;
		if ((okay3==0)&&(count3==1)) count3=2;
		for(i=0; i<DELAY; i++) { continue; }
		if (count1==2) printf("		1: good");
		if (count2==2) printf("		2: good");
		if (count3==2) printf("		3: good");
		printf("\n");
		//if(count1!=2)count1=0;
		//if(count2!=2)count2=0;
		//if(count3!=3)count3=0;
	};
}
