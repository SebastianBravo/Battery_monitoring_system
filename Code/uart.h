/* 
 * File:   usart.h
 * Author: USUARIO
 *
 * Created on 10 de agosto de 2021, 11:01 PM
 */

#ifndef UART_H
#define	UART_H

#include <xc.h>
#include <stdint.h>

// Variables
uint8_t buffertx[];
uint16_t buffertx_pointer;
uint8_t len_buffertx;
uint16_t bufferrx_pointer;
double clk_freq;

// Funciones
void digit_separator(uint16_t num, uint8_t *digits);
void array_2_ascii(uint8_t *digits, uint8_t *digits_ascii);
void uart_config(unsigned long des_baud_rate, uint8_t interrupt_ena);
void uart_tx(uint8_t data);
void send_array(uint8_t *array);
void send_buffer(void);
void read_buffer(uint8_t *data_rx,uint8_t len_data_rx, uint8_t *rx_done);
uint8_t uart_rx(void);
void bcd_2_hexascii(uint16_t number, uint8_t *digits_ascii, uint8_t desfase);
void hexascii_2_bcd(uint16_t *number, uint8_t *digits_ascii);

//Macros

#endif	/* UART_H */
