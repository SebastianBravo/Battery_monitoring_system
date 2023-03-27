#include <xc.h>
#include <stdint.h>
#include "iir.h"

float out = 0;

float filtrar_seccion_iir(uint16_t in_filtro, float *num, float *den, float *w, float gain){
    w[0] = den[0]*(gain*((float)in_filtro) - w[1]*den[1] - w[2]*den[2]);
    out = w[0]*num[0] + w[1]*num[1] + w[2]*num[2];
    w[2] = w[1];
    w[1] = w[0];
    return out;
}

uint16_t filtrar_iir(uint16_t in_filtro, float *num, float *den, float *w, float *gain, uint16_t n_secciones){
    uint16_t out_filtro = in_filtro;
    for(int i=0; i<2; i++){
        out_filtro = (uint16_t)filtrar_seccion_iir(out_filtro, &num[i], &den[i], &w[i], gain[i]);
    }
    return out_filtro;
}