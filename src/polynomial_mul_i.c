//
// Created by fcaullery on 11/07/25.
//

#include "polynomial_mul_i.h"
#include "hexagon_types.h"
#include "hvx.cfg.h"
#include <stdio.h>

//assumes that bits_set is sorted
void poly_mul_w(uint32_t *in, uint32_t *out, uint32_t *bits_set){


    HVX_Vector *pvector_in = (HVX_Vector *)in;
    HVX_Vector *pvector_out = (HVX_Vector *)out;
    HVX_Vector tmp, tmp1, zero = Q6_V_vzero();
    HVX_Vector shifted[N_HVX];


    uint32_t vector_offset = bits_set[0] / 1024; // gets the number of full vectors we need to rotate by
    uint32_t word_offset = (bits_set[0] % 1024) / 32; // gets the number of word each vector has to be shifted by
    uint32_t last_offset = bits_set[0] % 32; // gets the number of bits each vector has to be shifted by after word shift
    HVX_VectorPred pred = Q6_Q_vsetq_R(word_offset * 4);
    pred = ~pred;

    for (uint32_t i = 0; i < N_HVX; i++){
        pvector_out[(i + vector_offset) % N_HVX] = Q6_V_vror_VR(pvector_in[i], 128 - word_offset * 4);
    }

    for (uint32_t i = 0; i < N_HVX; i++){
      HVX_Vector hi = Q6_Vw_vasl_VwR(pvector_out[i], last_offset);
      pvector_out[i] = Q6_Vuw_vlsr_VuwR(pvector_out[i], 32 - last_offset);
      pvector_out[i] = Q6_V_vror_VR(pvector_out[i], 124);
      pvector_out[i] |= hi;
    }

    //TODO implement this with swapping should be more efficient
    //Creating a mask
    HVX_Vector mask = Q6_V_vsplat_R(0xFFFFFFFF);
    mask = Q6_Vw_vinsert_VwR(mask, 0xFFFFFFFF >> (32 - last_offset));
    mask = Q6_V_vror_VR(mask, 128 - word_offset * 4);
    //FIXME
    uint32_t word_offset_ceil = ((bits_set[0] % 1024) + 31) / 32;
    pred = Q6_Q_vsetq_R(word_offset_ceil * 4);
    mask = Q6_Vb_condacc_QVbVb(pred, zero, mask);


    tmp = pvector_out[0] & mask;
    pvector_out[0] = (pvector_out[0] & ~mask) | (pvector_out[N_HVX - 1] & mask);
    pvector_out[N_HVX - 1] = (pvector_out[N_HVX - 1] & ~mask) ^ tmp;
    for(uint32_t i = 1; i < N_HVX - 1; i++){
        tmp = pvector_out[N_HVX - i] & mask;
        pvector_out[N_HVX - i] = (pvector_out[N_HVX - i] & ~mask) | (pvector_out[N_HVX - i - 1] & mask);
        pvector_out[N_HVX - i - 1] = (pvector_out[N_HVX - i - 1] & ~mask) ^ tmp;
    }

    for(uint32_t i = 1; i < W; i++){
        vector_offset = bits_set[i] / 1024; // gets the number of full vectors we need to rotate by
        word_offset = ((bits_set[i]) % 1024) / 32; // gets the number of word each vector has to be shifted by
        uint32_t last_offset = ((bits_set[i])) % 32; // gets the number of bits each vector has to be shifted by after word shift
        HVX_VectorPred pred = Q6_Q_vsetq_R(word_offset * 4);
        pred = ~pred;

        for (uint32_t i = 0; i < N_HVX; i++){
            shifted[(i + vector_offset) % N_HVX] = Q6_V_vror_VR(pvector_in[i], 128 - word_offset * 4);
        }

        for (uint32_t i = 0; i < N_HVX; i++){
            HVX_Vector hi = Q6_Vw_vasl_VwR(shifted[i], last_offset);
            shifted[i] = Q6_Vuw_vlsr_VuwR(shifted[i], 32 - last_offset);
            shifted[i] = Q6_V_vror_VR(shifted[i], 124);
            shifted[i] |= hi;
        }

        //TODO implement this with swapping should be more efficient
        //Creating a mask
        HVX_Vector mask = Q6_V_vsplat_R(0xFFFFFFFF);
        mask = Q6_Vw_vinsert_VwR(mask, 0xFFFFFFFF >> (32 - last_offset));
        mask = Q6_V_vror_VR(mask, 128 - word_offset * 4);
        //FIXME
        uint32_t word_offset_ceil = ((bits_set[0] % 1024) + 31) / 32;
        pred = Q6_Q_vsetq_R(word_offset_ceil * 4);
        mask = Q6_Vb_condacc_QVbVb(pred, zero, mask);


        tmp = shifted[0] & mask;
        shifted[0] = (shifted[0] & ~mask) | (shifted[N_HVX - 1] & mask);
        shifted[N_HVX - 1] = (shifted[N_HVX - 1] & ~mask) ^ tmp;
        for(uint32_t i = 1; i < N_HVX - 1; i++){
            tmp = shifted[N_HVX - i] & mask;
            shifted[N_HVX - i] = (shifted[N_HVX - i] & ~mask) | (shifted[N_HVX - i - 1] & mask);
            shifted[N_HVX - i - 1] = (shifted[N_HVX - i - 1] & ~mask) ^ tmp;
        }
        for(uint32_t i = 0; i < N_HVX; i++){
          pvector_out[i] ^= shifted[i];
        }
    }


}
