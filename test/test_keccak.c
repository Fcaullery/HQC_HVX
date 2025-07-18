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

#include <stdio.h>

#include <stdint.h>

#include <string.h>


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
	test_weights[0] = 33;
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

    DUMP_PMU();

    printf("AppReported (HVX%db-mode): - keccak: %llu cycles\n", VLEN, total_cycles);


    return 0;
}
