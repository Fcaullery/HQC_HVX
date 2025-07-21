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
#include "kyber_ntt_i.h"

#include <stdio.h>

#include <stdint.h>

#include <string.h>

static int16_t montgomery_reduce(int32_t a)
{
  int16_t t;

  t = (int16_t)a*QINV;
  t = (a - (int32_t)t*KYBER_Q) >> 16;
  return t;
}

static int16_t fqmul(int16_t a, int16_t b) {
  return montgomery_reduce((int32_t)a*b);
}


const int16_t zetas[128] = {
  -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
   -171,   622,  1577,   182,   962, -1202, -1474,  1468,
    573, -1325,   264,   383,  -829,  1458, -1602,  -130,
   -681,  1017,   732,   608, -1542,   411,  -205, -1571,
   1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
    516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
   -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
   -398,   961, -1508,  -725,   448, -1065,   677, -1275,
  -1103,   430,   555,   843, -1251,   871,  1550,   105,
    422,   587,   177,  -235,  -291,  -460,  1574,  1653,
   -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
  -1590,   644,  -872,   349,   418,   329,  -156,   -75,
    817,  1097,   603,   610,  1322, -1285, -1465,   384,
  -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
  -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
   -108,  -308,   996,   991,   958, -1460,  1522,  1628
};

void ntt_c(int16_t r[256]) {
  unsigned int len, start, j, k;
  int16_t t, zeta;

  k = 1;
  for(len = 128; len >= 2; len >>= 1) {
    putchar(10);
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas[k++];
      printf("len %d, k %d zeta %d\n", len, k, zeta);
      for(j = start; j < start + len; j++) {
        t = fqmul(zeta, r[j + len]);
        r[j + len] = r[j] - t;
        r[j] = r[j] + t;
      }
    }
  }
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


    unsigned long long test[16*5] __attribute__((aligned(128))) = {0};



    int offset = 0;
    for (i = 0; i < 25; i++) {
		test[i + offset] = i + 1;
        if(i % 5 == 4) offset+=11;
    }

    for (i = 0; i < 16*5; i++) {
      	if(test[i]){
        	printf("%016llx ", test[i]);
    	}
        if(i % 16 == 15) printf("\n");
	}

    RESET_PMU();
    start_time = READ_PCYCLES();
    keccak_24(test);
    total_cycles = READ_PCYCLES() - start_time;
    printf("\n");

    for (i = 0; i < 16*5; i++) {
      	if(test[i]){
        	printf("%016llx ", test[i]);
    	}
        if(i % 16 == 15) printf("\n");
	}
	printf("AppReported (HVX%db-mode): - keccak: %llu cycles\n", VLEN, total_cycles);
    uint32_t test_weights[W] = {0};
    for (i = 0; i < W; i++) {
    	test_weights[i] = (17 + 1023 * i) % N;
    }
	uint32_t poly_in[N_HVX * 32] __attribute__((aligned(128))) = {0};
	uint32_t poly_out[N_HVX * 32] __attribute__((aligned(128)))= {0};

    for(int i = 0; i < N_HVX * 32; i++){
      poly_in[i] = i;
    }

    RESET_PMU();
	start_time = READ_PCYCLES();
	poly_mul_w(poly_in, poly_out, test_weights);
    total_cycles = READ_PCYCLES() - start_time;
    printf("\n");

    for (i = 0; i < N_HVX * 32; i++) {
      	printf("%03lu ", poly_in[i]);
        if(i % 32 == 31) printf("\n");
	}
    printf("\n");    printf("\n");
    printf("\n");    printf("\n");

    for (i = 0; i < N_HVX * 32; i++) {
      	printf("%03lu ", poly_out[i]);
        if(i % 32 == 31) printf("\n");
	}
    printf("\n");
    printf("AppReported (HVX%db-mode): - Poly_mul: %llu cycles\n", VLEN, total_cycles);


    int16_t ntt_test[3392] __attribute__((aligned(128))) = {0};
    int16_t ntt_test_tv[KYBER_Q] = {0};

    for (i = 0; i < KYBER_Q; i++) {
      ntt_test[i] = i;
      ntt_test_tv[i] = fqmul(ntt_test[i], zetas[1]);
//      printf("%d*%d=%d\n", ntt_test[i], zetas[0], ntt_test_tv[i]);
    }
    for (i = 0; i < 64; i++) {
      printf("%d ", ntt_test_tv[i]);
      if(i % 64 == 63) printf("\n\n");
    }
//    ntt_c(ntt_test);

    RESET_PMU();

    start_time = READ_PCYCLES();
    ntt(ntt_test);
    total_cycles = READ_PCYCLES() - start_time;

    DUMP_PMU();

    for (i = 0; i < 64; i++) {
      printf("%d ", ntt_test[i]);
      if(i % 64 == 63) printf("\n\n");
    }
    printf("AppReported (HVX%db-mode): - NTT: %llu cycles\n", VLEN, total_cycles);

    return 0;
}
