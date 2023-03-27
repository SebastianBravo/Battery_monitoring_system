/* 
 * File:   fir.h
 * Author: USUARIO
 *
 * Created on 24 de septiembre de 2021, 06:44 PM
 */

#ifndef FIR_H
#define	FIR_H

#include <xc.h>
#include <stdint.h>

//Filtros

typedef struct{
    uint16_t k;
    uint16_t x_index;
    int16_t *pointer_coef;
    uint16_t *pointer_x;
} filtro_fir;

// Variables (crear un x por cada filtro que se vaya a usar)
long out_filtro;

// Funciones
void inicializar_filtro_fir(filtro_fir *filtro);
long filtrar_fir_indices(uint16_t in_filtro, int16_t *coeficientes, uint16_t orden_filtro, uint16_t *x, filtro_fir *filtro);
long filtrar_fir_apuntadores(uint16_t in_filtro, int16_t *coeficientes, uint16_t orden_filtro, uint16_t *x, filtro_fir *filtro);

#endif	/* FIR_H */

