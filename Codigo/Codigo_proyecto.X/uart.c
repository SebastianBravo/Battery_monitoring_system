#include <xc.h>
#include <stdint.h>
#include "uart.h"

double clk_freq = 64000000;
uint8_t len_buffertx = 100;
uint8_t buffertx[100];
uint16_t buffertx_pointer = 0;
uint16_t bufferrx_pointer = 0;

void digit_separator(uint16_t num, uint8_t *digits){
       
    int i = 3;
    uint16_t n = num;
    
    while(n != 0){
        digits[i--] = n%10;
        n = n/10;
    }
    for(i; i>= 0; i--){
        digits[i] = 0;
    }
} 

void array_2_ascii(uint8_t *digits, uint8_t *digits_ascii){
    for(uint8_t i = 0; i<4; i++){
        digits_ascii[i] = digits[i] + '0';
    }
}

void bcd_2_hexascii(uint16_t number, uint8_t *digits_ascii, uint8_t desfase){
    uint8_t dig0 = 0x0F & (uint8_t)(number);
    uint8_t dig1 = 0x0F & (uint8_t)(number>>4);
    uint8_t dig2 = 0x0F & (uint8_t)(number>>8);
    
    
    digits_ascii[desfase] = '0';
    digits_ascii[desfase+1] = (dig2>9) ? (dig2 + 55) : (dig2 + '0');
    digits_ascii[desfase+2] = (dig1>9) ? (dig1 + 55) : (dig1 + '0');
    digits_ascii[desfase+3] = (dig0>9) ? (dig0 + 55) : (dig0 + '0');
}

//void bcd_2_hexascii(uint16_t number, uint8_t *digits_ascii, uint8_t desfase){
//    for(uint8_t i = 0; i<4; i++){
//        uint8_t digit = 0x0F & (uint8_t)(number>>((3-i)*4));
//        if(digit < 10){
//            digits_ascii[i+desfase] = digit + '0';
//        }else{
//            digits_ascii[i+desfase] = digit + 55;
//        }       
//    }    
//}

void hexascii_2_bcd(uint16_t *number, uint8_t *digits_ascii){
    *number = 0;
    for(uint8_t i = 0; i<3; i++){
        uint8_t x;
        if(digits_ascii[i] < 58){
            x = digits_ascii[i] - '0';
        }else{
            x = digits_ascii[i] - 55;
        }
        *number |= x<<((2-i)*4);
    }    
}

void uart_config(unsigned long des_baud_rate, uint8_t interrupt_ena){
    INTCON0bits.GIE = 0;                // Desabilitar interrupciones
    
    // Configurar Baud rate
    uint16_t baud_rate = clk_freq/(des_baud_rate*4) - 1;
    U1BRGH = baud_rate>>8;
    U1BRGL = baud_rate;

    U1CON0bits.BRGS = 1;                // Alto baud rate
    U1CON0bits.MODE = 0b0000;           // Asynchronous 8-bit UART mode
    U1RXPPS = 0b10111;                  // RC7 pin RX
    RC6PPS = 0b010011;                  // RC6 pin TX
    U1CON1bits.ON = 1;                  // Habilita el puerto serial

    PIE3bits.U1TXIE = 1;                // Configurar interrupcion
    PIE3bits.U1RXIE = 1;                // Configurar interrupcion
    U1CON0bits.RXEN = 1;                // Habilitar la recepcion
    
    INTCON0bits.GIE = 1;
    
}

void uart_tx(uint8_t data){
    U1TXB = data;
}

uint8_t uart_rx(void){
    return U1RXB;
}

void send_array(uint8_t *array, uint8_t len_array){
    for(int i = 0; i<len_array; i++){
        buffertx[i] = array[i];
    }
    
    U1CON0bits.TXEN = 1;                 // Habilitar Transmision
}


void send_buffer(uint8_t bytes_2_sent, uint8_t *array){
    
    if(buffertx_pointer < bytes_2_sent){
        uart_tx(array[buffertx_pointer++]);
    }
    if((buffertx_pointer == bytes_2_sent) && (U1ERRIRbits.TXMTIF)){
        U1CON0bits.TXEN = 0;
        buffertx_pointer = 0;        
    }   
}

void read_buffer(uint8_t *data_rx,uint8_t len_data_rx, uint8_t *rx_done){
    if(bufferrx_pointer < len_data_rx){
        data_rx[bufferrx_pointer] = uart_rx();
        bufferrx_pointer ++;
    }else{
        bufferrx_pointer = 0;
        *rx_done = 1;
    }
}

void start_tx(void){
    U1CON0bits.TXEN = 1;
}

