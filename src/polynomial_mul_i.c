//
// Created by fcaullery on 11/07/25.
//

#include "polynomial_mul_i.h"
#include "hexagon_types.h"
#include "hvx.cfg.h"

void poly_mul_w(uint8_t *in, uint8_t *out, unsigned long *bits_set){


    HVX_Vector *pvector_in = (HVX_Vector *)in;
    HVX_Vector *pvector_out = (HVX_Vector *)out;

    HVX_Vector vector_in = pvector_in[0];
    vector_in = Q6_V_vror_VR(vector_in, 1);
    vector_in = Q6_Vub_vlsr_VubR(vector_in, 2);

    for(int i = 1; i < W; i++){
        //rotate vector by offset i minus offset i-1
        unsigned int m = bits_set[i];

    }

    *pvector_out = vector_in;

}

