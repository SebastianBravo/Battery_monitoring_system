/* 
 * File:   timer.h
 * Author: USUARIO
 *
 * Created on 12 de agosto de 2021, 09:07 AM
 */

#ifndef TIMER0_H
#define	TIMER0_H

#include <xc.h>
#include <stdint.h>

// Variables
uint16_t init_count;
double clk_freq;
int ps;
float freq;

// Funciones
void initial_count(float T);
void timer0_config(void);
void timer0_reset(void);
uint16_t multiplo(float *Ts, float *des_Ts);

//Macros

#endif	/* TIMER_H */