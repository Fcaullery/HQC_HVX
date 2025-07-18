//
// Created by fcaullery on 11/07/25.
//

#ifndef POLYNOMIAL_MUL_I_H
#define POLYNOMIAL_MUL_I_H


#define N 17669
#define N_HVX 18 //number of HVX vectors necessary to hold a polynomial
#define W 66
#define W_RE 75

#include <stdint.h>

void poly_mul_w(uint32_t *in, uint32_t *out, uint32_t *bits_set);

#endif //POLYNOMIAL_MUL_I_H
