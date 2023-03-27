#include <xc.h>
#include <stdint.h>
#include "timer0.h"

double clk_freq = 64000000;
int ps = 1;
float freq;

void initial_count(float T){
    init_count = (65590 - (T*freq));
}

void timer0_config(void){
    
    INTCON0bits.GIE = 0;        // Desabilitar interrupciones
    
    // Timer0 Control Register
    T0CON1bits.CS = 0b010;      // Clock source: Fosc/4
    T0CON1bits.ASYNC = 0;       // No input sync   
    T0CON1bits.CKPS = 0b0000;   // Prescale: x16
    T0CON0bits.OUTPS = 0b0000;  // Postscale: x1
    T0CON0bits.MD16 = 1;        // TMR0 is a 16-bit timer
    T0CON0bits.EN = 1;          //  Enables Timer1
    
    freq = clk_freq/(4*ps);
    
    // Ajustar conteo para un periodo deseado
    initial_count(0.000104166);
    TMR0H = init_count>>8;               
    TMR0L = init_count;
       
    PIR3bits.TMR0IF = 0;        // Inicializar bandera de overflow en 0
    PIE3bits.TMR0IE = 1;        // Habilitar interrupcion overflow
    INTCON0bits.GIE = 1;        // Habilitar interrupciones
}

void timer0_reset(void){
    PIR3bits.TMR0IF = 0; //Poner bandera de overflow en 0    
    //Ajustar conteo para un periodo deseado
    TMR0H = init_count>>8;               
    TMR0L = init_count;
}

uint16_t multiplo(float *Ts, float *des_Ts){
    return (*des_Ts)/(*Ts);
}