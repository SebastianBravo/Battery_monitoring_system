#include <xc.h>
#include <stdint.h>
#include "fir.h"

long out_filtro = 0;

void inicializar_filtro_fir(filtro_fir *filtro){
    filtro -> k = 0;
    filtro -> x_index = 0;
}

long filtrar_fir_indices(uint16_t in_filtro, int16_t *coeficientes, uint16_t orden_filtro, uint16_t *x, filtro_fir *filtro){
    x[filtro -> k] = in_filtro;
    filtro->x_index = filtro->k;
    out_filtro = 0;
    
    for(int i=0; i<orden_filtro; i++){
       out_filtro += (long)coeficientes[i]*((long)x[filtro->x_index]);
       //out_filtro += (long)coeficientes[i]*(long)4096;
        if(filtro->x_index == 0){
            filtro->x_index = orden_filtro - 1; 
        }
        else{
            (filtro->x_index)--;
        }
    }
    (filtro->k)++;
    (filtro->k) = ((filtro->k) >= orden_filtro) ? 0 : (filtro->k);
  //  filtro->k = (filtro->x_index) + 1;
    return (out_filtro>>16)&0x0FFF;
}

long filtrar_fir_apuntadores(uint16_t in_filtro, int16_t *coeficientes, uint16_t orden_filtro, uint16_t *x, filtro_fir *filtro){
    x[filtro->k] = in_filtro;
    filtro->x_index = filtro->k;
    filtro->pointer_coef = &coeficientes[0];
    filtro->pointer_x = &x[filtro->x_index];
    out_filtro = 0;
    
    for(int i=0; i<orden_filtro; i++){
        out_filtro += (long)(*(filtro->pointer_coef))*(long)(*(filtro->pointer_x));
        if(filtro->x_index == 0){
            filtro->x_index = orden_filtro - 1; 
        }
        else{
            (filtro->x_index)--;
        }
        filtro->pointer_x = &x[filtro->x_index];
        (filtro->pointer_coef)++;
    }
    (filtro->k)++;
    (filtro->k) = ((filtro->k) >= orden_filtro) ? 0 : (filtro->k);
    return out_filtro>>16;
}
