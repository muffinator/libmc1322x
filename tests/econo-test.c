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

#include "tests.h"
#include "config.h"
#include "econo-test.h"

#define LED LED_RED

/* 802.15.4 PSDU is 127 MAX */
/* 2 bytes are the FCS */
/* therefore 125 is the max payload length */
#define PAYLOAD_LEN 16
#define DELAY 100000

volatile uint8_t complete=1;

void fill_packet(volatile packet_t *p) {
	static volatile uint8_t count=0;
	volatile uint8_t i;
	p->length = PAYLOAD_LEN;
	p->offset = 0;
	for(i=0; i<PAYLOAD_LEN; i++) {
		p->data[i] = count++;
	}

	/* acks get treated differently, even in promiscuous mode */
	/* setting the third bit makes sure that we never send an ack */
        /* or any valid 802.15.4-2006 packet */
	p->data[0] |= (1 << 3); 
}

void main(void) {
	volatile uint32_t i;
	volatile packet_t *p;
    volatile int okay1=0;
    volatile int okay2=0;
    volatile int okay3=0;
    volatile int count1=0;
    volatile int count2=0;
    volatile int count3=0;
	
	/* trim the reference osc. to 24MHz */
	trim_xtal();

	setupIO();
	uart_init(UART1, 115200);

	vreg_init();

	maca_init();

	set_channel(0); /* channel 11 */
//	set_power(0x0f); /* 0xf = -1dbm, see 3-22 */
//	set_power(0x11); /* 0x11 = 3dbm, see 3-22 */
	set_power(0x12); /* 0x12 is the highest, not documented */

        /* sets up tx_on, should be a board specific item */
//        *GPIO_FUNC_SEL2 = (0x01 << ((44-16*2)*2));
//	gpio_pad_dir_set( 1ULL << 44 );
	printf("\x1B[2J");
	printf("\x1B[H");
	print_welcome("econo-test \n");

	while(1){    		
		/* call check_maca() periodically --- this works around */
		/* a few lockup conditions */
		check_maca();

		while((p = rx_packet())) {
			if(p) free_packet(p);
		}

		p = get_free_packet();
		if(p) {
			fill_packet(p);
			
			printf("\x1B[2;0H");
			printf("rftest-tx --- ");
			print_packet(p);

			tx_packet(p);
			
			for(i=0; i<DELAY; i++) { continue; }
		}
	
		setGPIO();
		printf("\x1B[5;0H");
		printGPIO();
        okay1 = (readg(VREF1L)&&readg(ADC1)&&readg(ADC3)&&readg(ADC5)&&readg(U2RTS)&&readg(U2RX)&&readg(U1CTS)&&readg(SCL));
        okay2 = (readg(TMR3)&&readg(TMR1)&&readg(SCK)&&readg(MISO)&&readg(BTCK)&&readg(SSIRX)&&readg(KBI7)&&readg(KBI2)&&readg(ANT1)&&readg(RXON));
        okay3=(readg(TDI));
        if ((count1!=2)&&(okay1==1)) count1=1;
        if ((okay2==1)&&(count2!=2)) count2=1;
        if ((okay3==1)&&(count3!=2)) count3=1;
		resetGPIO();
		printf("\x1B[6;0H");
		printGPIO();
        okay1 = (readg(VREF1L)||readg(ADC1)||readg(ADC3)||readg(ADC5)||readg(U2RTS)||readg(U2RX)||readg(U1CTS)||readg(SCL));
        okay2 = (readg(TMR3)||readg(TMR1)||readg(SCK)||readg(MISO)||readg(BTCK)||readg(SSIRX)||readg(KBI7)||readg(KBI2)||readg(ANT1)||readg(RXON));
        okay3=(readg(TDI));
        if ((okay1==0)&&(count1==1)) count1=2;
        if ((okay2==0)&&(count2==1)) count2=2;
        if ((okay3==0)&&(count3==1)) count3=2;
		printf("\x1B[7;0H");
        if (count1==2) printf("     1: good");
        if (count2==2) printf("     2: good");
        if (count3==2) printf("     3: good");
	}	

}
