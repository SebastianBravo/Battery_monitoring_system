#include <xc.h>
#include <stdint.h>
#include "adc.h"

void voltage_ref_config(void){
    INTCON0bits.GIE = 0;         // Desabilitar interrupciones
    
    FVRCONbits.FVREN = 1;       // Habilitar referencia de voltaje fija
    FVRCONbits.ADFVR = 0b11;    // Referencia de voltaje de ADC de 4.096V
    FVRCONbits.CDAFVR = 0b11;    // Referencia de voltaje de ADC de 4.096V
    
    INTCON0bits.GIE = 1;         // Habilitar interrupciones   
}

void adc_config(uint8_t interrupt_ena){
    INTCON0bits.GIE = 0;         // Desabilitar interrupciones
    
    ADCON0bits.FM = 1;          // Justificacion a la derecha
    ADCON0bits.CS = 0;          // Clock supplied by FOSC
    ADCON2bits.MD = 0b000;      // Basic Mode
    ADCLKbits.CS = 0b010001;    // Fosc/32
    
    ADREFbits.NREF = 0;         // Vref- VSS
    ADREFbits.PREF = 0b11;      // Vref+ FVR module
    //ADCON1bits.ADPREF = 0b00;   // Vref+ VDD
    
    ADPCHbits.PCH = 0b000000;   // Canal 0
    ADCON0bits.ON = 1;          // Habilitar modulo ADC
    ADCON0bits.GO = 0;          // No empezar conversion 
    
    if(interrupt_ena){
        PIR1bits.ADIF = 0;      // Inicializar bandera en 0
        PIE1bits.ADIE = 1;      // Habilitar intterupciones ADC
    }
    
    INTCON0bits.GIE = 1;         // Habilitar interrupciones
}

void start_conversion(uint8_t channel){
    ADPCHbits.PCH = channel;
    ADCON0bits.GO = 1;    // Iniciar conversion
    LATDbits.LD1 = 1;
    LATDbits.LD1 = 1;
}

uint8_t conversion_done(void){
    return PIR1bits.ADIF;
} 

uint16_t read_adc(void){
    uint16_t result = (uint16_t)((ADRESH<<8) | (ADRESL));
    PIR1bits.ADIF = 0;
    return result;
}

uint8_t read_n_adc(uint16_t *lecturas,uint8_t *canal_actual,uint8_t n_canales){
    lecturas[*canal_actual] = read_adc();
    if(*canal_actual == n_canales-1){
        *canal_actual = 0;
        uint8_t done = 1;
        return done;
    }else{
        *canal_actual += 1;
        start_conversion(*canal_actual);
        return 0;
    }  
}

uint16_t scale_read(uint16_t read){
    //uint16_t result = read*1000/1024;
    uint16_t result = (uint16_t)((float)read*0.99292);
    return result;
}


