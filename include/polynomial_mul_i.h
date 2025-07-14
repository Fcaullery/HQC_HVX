//
// Created by fcaullery on 11/07/25.
//

#ifndef POLYNOMIAL_MUL_I_H
#define POLYNOMIAL_MUL_I_H


#define N 17669
#define W 66
#define W_RE 75

#include <stdint.h>

void poly_mul_w(uint8_t *in, uint8_t *out, unsigned long *bits_set);

#endif //POLYNOMIAL_MUL_I_H
