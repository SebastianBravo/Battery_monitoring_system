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

uint8_t n_canales = 3;
uint8_t canal_actual = 0;
uint8_t lectura_done = 0;
uint8_t rx_done = 0;

// Variables para conversiones
//uint16_t lecturas[3];
uint32_t lecturas[3];
uint32_t lecturas_filtradas[3];
uint16_t lecturas_promediadas[3];
uint8_t lecturas_ascii[13] = {0,0,0,0,0xA,0,0,0,0,0,0,0,0xA};

uint8_t sample = 0;

// Variables para recepcion de datos
uint8_t command[3];
uint16_t command_bcd;
uint8_t command[3];
uint8_t command_length = 3;

uint8_t prueba = 0;

// Variables temporizacion
float Ts = 0.000104;
float des_Ts = 0.005;
float des_Ts2 = 1/9600;

uint16_t count = 0;
uint16_t k = 0;
uint16_t j = 0;
uint16_t k2 = 0;
uint16_t signal[200]={4095,4093,4090,4085,4078,4069,4058,4045,4030,4013,3994,3973,3951,3926,3900,3871,3841,3809,3776,3740,3703,3665,3625,3583,3540,3495,3449,3401,3352,3302,3250,3198,3144,3089,3033,2977,2919,2860,2801,2741,2680,2618,2556,2494,2431,2367,2304,2240,2176,2111,2047,1983,1918,1854,1790,1727,1663,1600,1538,1476,1414,1353,1293,1234,1175,1117,1061,1005,950,896,844,792,742,693,645,599,554,511,469,429,391,354,318,285,253,223,194,168,143,121,100,81,64,49,36,25,16,9,4,1,0,1,4,9,16,25,36,49,64,81,100,121,143,168,194,223,253,285,318,354,391,429,469,511,554,599,645,693,742,792,844,896,950,1005,1061,1117,1175,1234,1293,1353,1414,1476,1538,1600,1663,1727,1790,1854,1918,1983,2047,2111,2176,2240,2304,2367,2431,2494,2556,2618,2680,2741,2801,2860,2919,2977,3033,3089,3144,3198,3250,3302,3352,3401,3449,3495,3540,3583,3625,3665,3703,3740,3776,3809,3841,3871,3900,3926,3951,3973,3994,4013,4030,4045,4058,4069,4078,4085,4090,4093};

/****************************** Filtros FIR ***********************************/
// FILTRO FIR1
uint16_t n1 = 3;
int16_t coef1[3] = {
     7905,  16958,   7905
};

filtro_fir filtro_fir_1;
uint16_t x1[3];

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    uart_config(921600, 1);
    timer0_config();
    voltage_ref_config();
    adc_config(1);
    
    k =  multiplo(&Ts, &des_Ts);
    k2 = multiplo(&Ts, &des_Ts2);
    
    while (1)
    {
//        if(rx_done){
//            rx_done = 0;
//            hexascii_2_bcd(&command_bcd, command);
//            k =  multiplo(Ts, command_bcd*0.0001);
//        }
        if((lectura_done==1) && (sample==24)){
            LATDbits.LD0=~LATDbits.LD0;
            lectura_done = 0;
            sample = 0;
//            bcd_2_hexascii(signal[j], lecturas_ascii, 0);
//            j++;
//            if(j>199){
//                j=0;
//            }          
            lecturas_filtradas[0] = (uint16_t)(lecturas[0]>>3);
            lecturas_filtradas[1] = (uint16_t)(lecturas[1]>>3);
            lecturas_filtradas[2] = (uint16_t)(lecturas[2]>>3);
            lecturas[0] = 0;
            lecturas[1] = 0;
            lecturas[2] = 0;
            
            bcd_2_hexascii(lecturas_filtradas[0], lecturas_ascii, 0);
            bcd_2_hexascii(lecturas_filtradas[1], lecturas_ascii, 4);
            bcd_2_hexascii(lecturas_filtradas[2], lecturas_ascii, 8);
            
            start_tx(); 
        }
    }
}

void __interrupt() ISR(void)
{
    // ISR ADC
    if(PIR1bits.ADIF){  
        LATDbits.LD1 = 0;
        lectura_done = read_n_adc(lecturas, &canal_actual, n_canales);
        sample += 1;
    } 
    
//    // ISR RX
//    if(PIR3bits.U1RXIF){
//        read_buffer(command, command_length, &rx_done);
//    }
    
    // ISR TX
    if(PIR3bits.U1TXIF){
        send_buffer(13, lecturas_ascii);  // Envia dato correspondiente de buffer
    }
    //ISR TMR0
    if(PIR3bits.TMR0IF){
        timer0_reset();       
        start_conversion(0);
//        if(count < k-1){
//            count++;
//        }else{
//            count = 0;
//            LATDbits.LD0=~LATDbits.LD0;
//            start_conversion(0);
//        }
    } 
}
/**
 End of File
*/
