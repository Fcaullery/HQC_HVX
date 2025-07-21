//
// Created by fcaullery on 21/07/25.
//

#ifndef KYBER_NTT_H
#define KYBER_NTT_H
#include <stdint.h>

#define KYBER_N 256
#define KYBER_Q 3329
#define MONT -1044 // 2^16 mod q
#define QINV -3327 // q^-1 mod 2^16

void ntt(int16_t r[256]);

#endif //KYBER_NTT_H
