/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F47K42
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "timer0.h"
#include "adc.h"
#include "uart.h"
#include "fir.h"
#include "iir.h"

uint8_t n_canales = 2;
uint8_t canal_actual = 0;
uint8_t lectura_done = 0;
uint8_t rx_done = 0;

// Variables para conversiones
uint16_t lecturas[2];
uint16_t lecturas_filtradas[2];
uint8_t lecturas_ascii[9] = {0,0,0,0,0,0,0,0,0xA};

// Variables para recepcion de datos
uint8_t command[3];
uint16_t command_bcd;
uint8_t command[3];
uint8_t command_length = 3;

float prueba = 0;

// Variables temporizacion
float Ts = 0.0001;
//float des_Ts = 0.00625; //160 Hz
float des_Ts = 0.00555555555555555; //180 Hz
uint16_t count = 1;
uint16_t k = 1;

/****************************** Filtros FIR ***********************************/
// FILTRO FIR1
uint16_t n1 = 8;
int16_t coef1[8] = {
        0,   2496,  10880,  19392,  19392,  10880,   2496,      0
};

// FILTRO FIR2
uint16_t n2 = 12;
int16_t coef2[12] = {
        0,    128,   1632,   5536,  10816,  14656,  14656,  10816,   5536,
     1632,    128,      0
};

// FILTRO FIR3
uint16_t n3 = 18;
int16_t coef3[18] = {
     0,      0,    384,    640,  -1216,  -3584,      0,  12224,  24320,
    24320,  12224,      0,  -3584,  -1216,    640,    384,      0,      0
};

// FILTRO FIR4
uint16_t n4 = 31;
//int16_t coef4[31] = {
//        0,      0,     128,      0,   -256,      0,    768,      0,   -1664,
//        0,   3072,      0,  -6272,      0,  20608,  32768,  20608,      0,
//    -6272,      0,   3072,      0,   -1664,      0,    768,      0,   -256,
//        0,     128,      0,      0
//};
int16_t coef4[31] = {
        0,      0,     64,      0,   -128,      0,    384,      0,   -832,
        0,   1536,      0,  -3136,      0,  10304,  16384,  10304,      0,
    -3136,      0,   1536,      0,   -832,      0,    384,      0,   -128,
        0,     64,      0,      0
};

filtro_fir filtro_fir_1;
uint16_t x1[18];
filtro_fir filtro_fir_2;
uint16_t x2[18];

/******************************************************************************/

/****************************** Filtros IIR ***********************************/

// Filtro IIR1 Rechazabanda

uint16_t n_secciones1 = 2;
float gain1[2] = {0.8837263584, 0.8837263584};
float num1[2][3] = {
    {1,1.015426636,1},
    {1,1.015426636,1}
};
float den1[2][3] = {
    {1,1.091402411,0.7957869172},
    {1,0.6923882365,0.7671743035}
};

// Filtro IIR2 Pasa bajos fc 36 

uint16_t n_secciones2 = 2;
float gain2[2] = {0.2533015125277, 0.18390299443864};
float num2[2][3] = {
    {1,2,1},
    {1,2,1}
};
float den2[2][3] = {
    {1,-0.4531195164,0.4663255811},
    {1,-0.3289756775,0.06458765268}
};

// Filtro IIR3 Pasa altos fc 45 

uint16_t n_secciones3 = 2;
float gain3[2] = {0.3616156876, 0.2598915398};
float num3[2][3] = {
    {1,-2,1},
    {1,-2,1}
};
float den3[2][3] = {
    {1,-3.2117925e-16,0.4464626908},
    {1,-2.308300571e-16,0.03956612945}
};

float w[2][3] = {{0,0,0},{0,0,0}};
/******************************************************************************/


/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    uart_config(256000, 1);
    timer0_config();
    voltage_ref_config();
    adc_config(1);
    SPI1_Initialize();
    
    k =  multiplo(&Ts, &des_Ts);
    LATEbits.LE0 = 1;
    LATBbits.LB4 = 1;
    
    inicializar_filtro_fir(&filtro_fir_1);
    inicializar_filtro_fir(&filtro_fir_2);
    
    while (1)
    {
//        if(rx_done){
//            rx_done = 0;
//            hexascii_2_bcd(&command_bcd, command);
//            k =  multiplo(Ts, command_bcd*0.0001);
//        }
        if(lectura_done){
            lectura_done = 0;
            LATDbits.LD0=1;
            lecturas_filtradas[1] = (uint16_t)filtrar_fir_apuntadores(lecturas[1], coef4, n4, x1, &filtro_fir_1); 
            //lecturas_filtradas[1] = filtrar_iir(lecturas[1], num1, den1, w, gain1, n_secciones1); 
//            prueba = filtrar_seccion_iir((float)lecturas[1], num3[0], den3[0], w[0], gain3[0]); 
//            lecturas_filtradas[1] = (uint16_t)filtrar_seccion_iir(prueba, num3[1], den3[1], w[1], gain3[1])&0x0FFF; 
            LATDbits.LD0=0;
            lecturas_filtradas[0]  = lecturas[0];
            
            SPI_transmit(lecturas_filtradas);
            
            bcd_2_hexascii(lecturas_filtradas[0], lecturas_ascii, 0);
            bcd_2_hexascii(lecturas_filtradas[1], lecturas_ascii, 4);
            send_array(lecturas_ascii);
        }
    }
}

void __interrupt() ISR(void)
{   
//    // ISR RX
//    if(PIR3bits.U1RXIF){
//        read_buffer(command, command_length, &rx_done);
//    }
    //ISR TMR0
    if(PIR3bits.TMR0IF){
        timer0_reset();
        
        if(count < k-1){
            count++;
        }else{
            count = 0;
           // LATDbits.LD0=~LATDbits.LD0;
            start_conversion(0);
        }
    }
    // ISR TX
    if(PIR3bits.U1TXIF){
        send_buffer();          // Envia dato correspondiente de buffer
    }
    // ISR ADC
    if(PIR1bits.ADIF){
        LATDbits.LD1 = 0;
        lectura_done = read_n_adc(lecturas, &canal_actual, n_canales);
    }  
}
/**
 End of File
*/
