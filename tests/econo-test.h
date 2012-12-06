#ifndef ECONO_TEST_H
#define ECONO_TEST_H

#define readg(x) GPIO->DATA.x
#define setdo(x) GPIO->PAD_DIR_SET.x=1
#define setdi(x) GPIO->PAD_DIR_RESET.x=1

void setupIO(void){
	GPIO->FUNC_SEL0 = 0xffffffff;
    GPIO->FUNC_SEL1 = 0xffffffff;
    GPIO->FUNC_SEL2 = 0xffffffff;
    GPIO->FUNC_SEL3 = 0xffffffff;
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
}

void setGPIO(void){
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
}

void resetGPIO(void){
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
}

void printGPIO(void){
        printf("    1: %d %d %d %d %d %d %d %d 2: %d %d %d %d %d %d %d %d %d %d 3: %d %d \n", readg(VREF1L), readg(ADC1), readg(ADC3), readg(ADC5), readg(U2RTS), readg(U2RX), readg(U1CTS), readg(SCL), readg(TMR3), readg(TMR1), readg(SCK), readg(MISO), readg(BTCK), readg(SSIRX), readg(KBI7), readg(KBI2), readg(ANT1), readg(RXON), readg(VREF2H), readg(TDO));
}


#endif
