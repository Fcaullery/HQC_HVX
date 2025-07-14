/***************************************************************************
* Copyright (c) Date: Mon Nov 24 16:26:02 CST 2008 QUALCOMM INCORPORATED
* All Rights Reserved
* Modified by QUALCOMM INCORPORATED on Mon Nov 24 16:26:03 CST 2008
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__hexagon__)
#include "hexagon_standalone.h"
#include "subsys.h"
#endif
#include "io.h"
#include "hvx.cfg.h"
#include "keccak.h"
#include "polynomial_mul_i.h"



signed char mask[12] =
{
    1, -4, 7, 0,
    2, -5, 8, 0,
    3, -6, 9, 0
};


#include <stdio.h>

#include <stdint.h>

#include <string.h>

// Keccak-f[1600] parameters

#define NUM_ROUNDS 24

typedef uint64_t lane_t;

// 5×5 state array

static uint64_t state[5][5];

// Expected intermediate states for a zero input:

// – After θ, ρ, π, χ: still all zeros

// – After ι (round 0): only A[0][0] ^= RC[0] = 0x1

static const lane_t theta_expected[5][5] = { {0} };

static const lane_t rho_expected  [5][5] = { {0} };

static const lane_t pi_expected   [5][5] = { {0} };

static const lane_t chi_expected  [5][5] = { {0} };

static const lane_t iota_expected [5][5] = {

    { 0x0000000000000001ULL, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0 },

    { 0, 0, 0, 0, 0 }

};

static void print_state(const char *label, lane_t A[5][5]) {

    printf("%s\n", label);

    for(int y = 0; y < 5; y++) {

        for(int x = 0; x < 5; x++)

            printf("%016llu", A[x][y]);

        putchar('\n');

    }

}

// θ step

static void theta(lane_t A[5][5]) {

    lane_t C[5], D[5];

    for(int x=0; x<5; x++)

        C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];

    for(int x=0; x<5; x++)

        D[x] = C[(x+4)%5] ^ ((C[(x+1)%5] << 1) | (C[(x+1)%5] >> 63));

    for(int x=0; x<5; x++)

      for(int y=0; y<5; y++)

        A[x][y] ^= D[x];

}

// ρ step

static void rho(lane_t A[5][5]) {

    const int R[5][5] = {

      {  0, 36,  3, 41, 18},

      {  1, 44, 10, 45,  2},

      { 62,  6, 43, 15, 61},

      { 28, 55, 25, 21, 56},

      { 27, 20, 39,  8, 14}

    };

    for(int x=0; x<5; x++)

      for(int y=0; y<5; y++)

        A[x][y] = (A[x][y] << R[x][y]) | (A[x][y] >> (64 - R[x][y]));

}

// π step

static void pi(lane_t A[5][5]) {

    lane_t T[5][5];

    memcpy(T, A, sizeof(T));

    for(int x=0; x<5; x++)

      for(int y=0; y<5; y++)

        A[y][(2*x+3*y)%5] = T[x][y];

}

// χ step

static void chi(lane_t A[5][5]) {

    lane_t T[5];

    for(int y=0; y<5; y++) {

        for(int x=0; x<5; x++)

            T[x] = A[x][y];

        for(int x=0; x<5; x++)

            A[x][y] = T[x] ^ ((~T[(x+1)%5]) & T[(x+2)%5]);

    }

}

// ι step

static void iota(lane_t A[5][5], int roundIdx) {

    static const lane_t RC[NUM_ROUNDS] = {

      0x0000000000000001ULL, 0x0000000000008082ULL,

      0x800000000000808aULL, 0x8000000080008000ULL,

      0x000000000000808bULL, 0x0000000080000001ULL,

      0x8000000080008081ULL, 0x8000000000008009ULL,

      0x000000000000008aULL, 0x0000000000000088ULL,

      0x0000000080008009ULL, 0x000000008000000aULL,

      0x000000008000808bULL, 0x800000000000008bULL,

      0x8000000000008089ULL, 0x8000000000008003ULL,

      0x8000000000008002ULL, 0x8000000000000080ULL,

      0x000000000000800aULL, 0x800000008000000aULL,

      0x8000000080008081ULL, 0x8000000000008080ULL,

      0x0000000080000001ULL, 0x8000000080008008ULL

    };

    A[0][0] ^= RC[roundIdx];

}

// compare two states

static int cmp_state(const lane_t A[5][5], const lane_t B[5][5]) {

    for(int x=0; x<5; x++)

      for(int y=0; y<5; y++)

        if (A[x][y] != B[x][y]) return 0;

    return 1;

}


int main(int argc, char* argv[])
{
    int i;
    int width, height, stride;
    FH fp;

    long long start_time, total_cycles;


#if defined(__hexagon__)
    subsys_enable();
    SIM_ACQUIRE_HVX;
#if LOG2VLEN == 7
    SIM_SET_HVX_DOUBLE_MODE;
#endif
#endif
    /* -----------------------------------------------------*/
    /*  Call fuction                                        */
    /* -----------------------------------------------------*/

//
//    uint8_t in[(N + 7) / 8]__attribute__((aligned(128))) = {0};
//
//    for(int i = 0; i < 128; i++){
//        in[i] = i + 1;
//    }
//
//    uint8_t out[(N + 7) / 8] __attribute__((aligned(128))) = {0};
//    unsigned long bits_set[W_RE];
//    poly_mul_w(in, out, bits_set);
//    for (i = 0; i < (N + 7) / 8; i++) {
//        printf("%02X ", in[i]);
//    }
//    printf("\n");
//    printf("\n");
//    for (i = 0; i < (N + 7) / 8; i++) {
//        printf("%02X ", out[i]);
//    }
//    printf("\n");
//    printf("\n");
//



    unsigned long long test[16*5] __attribute__((aligned(128))) = {0};
//    for (i = 0; i < 16*5; i++) {
//        test[i] = i+1;
//    }

    int offset = 0;
    for (i = 0; i < 25; i++) {
		test[i + offset] = i + 1;
        if(i % 5 == 4) offset+=11;
    }

    for (i = 0; i < 16*5; i++) {
        printf("%02lld ", test[i]);
        if(i % 16 == 15) printf("\n");
    }

    printf("\n\n");
    RESET_PMU();
    start_time = READ_PCYCLES();
    keccak_24(test);
    total_cycles = READ_PCYCLES() - start_time;

    for (i = 0; i < 16*5; i++) {
        printf("%02lld ", test[i]);
        if(i % 16 == 15) printf("\n");
    }

    // printf("\n\n");
    //
    // unsigned long long sline000[16*5] __attribute__((aligned(128))) = {0};
    // for (i = 0; i < 16*5; i++) {
    //     sline000[i] = 0xFF;
    // }
    // for (i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         sline000[i * 16 + j] = 1 << (i*4 +j);
    //     }
    // }
    //
    // sline000[2] = 0x3llu<<62;
    // for (i = 0; i < 16*5; i++) {
    //     printf("%08llx ", sline000[i]);
    //     if (i % 16 == 15) {
    //         printf("\n");
    //     }
    // }
    //
    // keccak_24(sline000);
    //
    //
    // printf("\n\n");
    // for (i = 0; i < 16*5; i++) {
    //     printf("%08llx ", sline000[i]);
    //     if (i % 16 == 15) {
    //         printf("\n");
    //     }
    // }
    //
    //
    // conv3x3( input, stride, width, height, mask, 4, output );


    DUMP_PMU();

    printf("AppReported (HVX%db-mode): - keccak: %llu cycles\n", VLEN, total_cycles);


    return 0;
}
