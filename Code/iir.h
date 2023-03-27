/* 
 * File:   iir.h
 * Author: USUARIO
 *
 * Created on 26 de septiembre de 2021, 06:55 PM
 */

#ifndef IIR_H
#define	IIR_H

// Variables (crear un x por cada filtro que se vaya a usar)
float out;

// Funciones
float filtrar_seccion_iir(float in_filtro, float *num, float *den, float *w, float gain);
uint16_t filtrar_iir(uint16_t in_filtro, float *num, float *den, float *w, float *gain, uint16_t n_secciones);

#endif	/* IIR_H */

