/* 
 * File:   adc.h
 * Author: estudiante
 *
 * Created on August 11, 2021, 3:49 PM
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h>
#include <stdint.h>

// Variables

// Funciones
void adc_config(uint8_t interrupt_ena);
void voltage_ref_config(void);
void start_conversion(uint8_t channel);
uint8_t conversion_done(void);
uint16_t read_adc(void);
uint16_t scale_read(uint16_t read);
//uint8_t read_n_adc(uint16_t *lecturas,uint8_t *canal_actual,uint8_t n_canales);
uint8_t read_n_adc(uint32_t *lecturas,uint8_t *canal_actual,uint8_t n_canales);

//Macros

#endif	/* ADC_H */

