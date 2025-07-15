/* ======================================================================== */
/*  QUALCOMM TECHNOLOGIES, INC.                                             */
/*                                                                          */
/*  HEXAGON HVX Image/Video Processing Library                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (c) 2014 QUALCOMM TECHNOLOGIES Incorporated.          */
/*                           All Rights Reserved.                           */
/*                  QUALCOMM Confidential and Proprietary                   */
/* ======================================================================== */

/*[========================================================================]*/
/*[ FUNCTION                                                               ]*/
/*[     conv3x3                                                            ]*/
/*[                                                                        ]*/
/*[------------------------------------------------------------------------]*/
/*[ DESCRIPTION                                                            ]*/
/*[     This function applies a 3x3 kernel to filter a image.              ]*/
/*[     During the computation, 16bit accumulator is used.                 ]*/
/*[                                                                        ]*/
/*[------------------------------------------------------------------------]*/
/*[ REVISION DATE                                                          ]*/
/*[     AUG-01-2014                                                        ]*/
/*[                                                                        ]*/
/*[========================================================================]*/
#include "hexagon_types.h"
#include "hvx.cfg.h"

/* ======================================================================== */
/*  Intrinsic C version of conv3x3().                                       */
/* ======================================================================== */
void conv3x3Per2Row(
    unsigned char *restrict inp,
    int            stride,
    int            width,
    signed char   *restrict mask,
    int            shift,
    unsigned char *restrict outp
    )
{
    int i;
    HEXAGON_Vect32 __m2m1m0, __m5m4m3, __m8m7m6;
    HEXAGON_Vect32 *mask4 = (HEXAGON_Vect32 *)mask;

    HVX_Vector sline000,sline004,sline064;
    HVX_Vector sline100,sline104,sline164;
    HVX_Vector sline200,sline204,sline264;
    HVX_Vector sline300,sline304,sline364;
    HVX_Vector sSum20L, sSum31L, sSum20H, sSum31H;
    HVX_Vector sOut01, sOut01p, sOut11, sOut11p, sOut00, sOut10;

    HVX_VectorPair dline000,dline100, dline200,dline300;
    HVX_VectorPair dSum020, dSum031, dSum120, dSum131;

    __m2m1m0 = mask4[0];
    __m5m4m3 = mask4[1];
    __m8m7m6 = mask4[2];

    HVX_Vector *iptr0 = (HVX_Vector *)(inp  - 1*stride);
    HVX_Vector *iptr1 = (HVX_Vector *)(inp  + 0*stride);
    HVX_Vector *iptr2 = (HVX_Vector *)(inp  + 1*stride);
    HVX_Vector *iptr3 = (HVX_Vector *)(inp  + 2*stride);
    HVX_Vector *optr0 = (HVX_Vector *)(outp + 0*stride);
    HVX_Vector *optr1 = (HVX_Vector *)(outp + 1*stride);

    sline000 = *iptr0++;
    sline100 = *iptr1++;
    sline200 = *iptr2++;
    sline300 = *iptr3++;

    sOut01p = Q6_V_vzero();
    sOut11p = Q6_V_vzero();

    for ( i=width; i>0; i-=VLEN )
    {
        sline064 = *iptr0++;
        sline164 = *iptr1++;
        sline264 = *iptr2++;
        sline364 = *iptr3++;

        sline004 = Q6_V_valign_VVI(sline064,sline000,4);
        sline104 = Q6_V_valign_VVI(sline164,sline100,4);
        sline204 = Q6_V_valign_VVI(sline264,sline200,4);
        sline304 = Q6_V_valign_VVI(sline364,sline300,4);

        dline000 = Q6_W_vcombine_VV(sline004, sline000);
        dline100 = Q6_W_vcombine_VV(sline104, sline100);
        dline200 = Q6_W_vcombine_VV(sline204, sline200);
        dline300 = Q6_W_vcombine_VV(sline304, sline300);

        dSum020 = Q6_Ww_vrmpy_WubRbI(dline000,__m2m1m0, 0);
        dSum031 = Q6_Ww_vrmpy_WubRbI(dline000,__m2m1m0, 1);

        dSum020 = Q6_Ww_vrmpyacc_WwWubRbI(dSum020, dline100,__m5m4m3, 0);
        dSum031 = Q6_Ww_vrmpyacc_WwWubRbI(dSum031, dline100,__m5m4m3, 1);

        dSum020 = Q6_Ww_vrmpyacc_WwWubRbI(dSum020, dline200,__m8m7m6, 0);
        dSum031 = Q6_Ww_vrmpyacc_WwWubRbI(dSum031, dline200,__m8m7m6, 1);

        sSum31L = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum031),Q6_V_lo_W(dSum031),shift);
        sSum20L = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum020),Q6_V_lo_W(dSum020),shift);
        sOut01 = Q6_Vub_vsat_VhVh(sSum31L,sSum20L);
        sOut00 = Q6_V_vlalign_VVI(sOut01,sOut01p,1);
        *optr0++ = sOut00;

        dSum120 = Q6_Ww_vrmpy_WubRbI(dline100,__m2m1m0, 0);
        dSum131 = Q6_Ww_vrmpy_WubRbI(dline100,__m2m1m0, 1);

        dSum120 = Q6_Ww_vrmpyacc_WwWubRbI(dSum120, dline200,__m5m4m3, 0);
        dSum131 = Q6_Ww_vrmpyacc_WwWubRbI(dSum131, dline200,__m5m4m3, 1);

        dSum120 = Q6_Ww_vrmpyacc_WwWubRbI(dSum120, dline300,__m8m7m6, 0);
        dSum131 = Q6_Ww_vrmpyacc_WwWubRbI(dSum131, dline300,__m8m7m6, 1);

        sSum31H = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum131),Q6_V_lo_W(dSum131),shift);
        sSum20H = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum120),Q6_V_lo_W(dSum120),shift);
        sOut11 = Q6_Vub_vsat_VhVh(sSum31H,sSum20H);
        sOut10 = Q6_V_vlalign_VVI(sOut11,sOut11p,1);
        *optr1++ = sOut10;

        sline000 = sline064;
        sline100 = sline164;
        sline200 = sline264;
        sline300 = sline364;

        sOut01p = sOut01;
        sOut11p = sOut11;
    }
}


/* ======================================================================== */
void conv3x3(
    unsigned char *restrict inp,
    int            stride,
    int            width,
    int            height,
    signed char   *restrict mask,
    int            shift,
    unsigned char *restrict outp
    )
{
    int i;
    unsigned char *input  = inp  + 1*stride;
    unsigned char *output = outp + 1*stride;

    for (i = 1; i< (height-1); i+=2)
    {
       conv3x3Per2Row( input, stride, width, mask, shift, output );

       input += 2*stride;
       output+= 2*stride;
   }
}


// first is rotate 5 elements left by one element
// second is rotate 5 elements right by one element
static const unsigned char rotation_crtl[256] __attribute__((aligned(128))) = {
    0b111000, 0b111000, 0b111000, 0b111000, 0b111000, 0b111000, 0b111000, 0b111000,
    0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000,
    0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000,
    0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000, 0b1000,
    0b100000, 0b100000, 0b100000, 0b100000, 0b100000, 0b100000, 0b100000, 0b100000,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    0b111000,0b111000,0b111000,0b111000,0b111000,0b111000,0b111000,0b111000,
    0b1000,0b1000,0b1000,0b1000,0b1000,0b1000,0b1000,0b1000,
    0b011000,0b011000,0b011000,0b011000,0b011000,0b011000,0b011000,0b011000,
    0b1000,0b1000,0b1000,0b1000,0b1000,0b1000,0b1000,0b1000,
    0b100000,0b100000,0b100000,0b100000,0b100000,0b100000,0b100000,0b100000,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

static const unsigned int rots[64 * 5] __attribute__((aligned(128))) = {
    00,00,01,01,31,31,28,28,
    27,27,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,

    00,00,00,00,31,31,00,00,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    31,31,31,31,6,6,31,31,
    20,20,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    5,5,13,13,0,0,24,24,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    18,18,2,2,31,31,31,31,
    14,14,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    0,0,0,0,30,30,25,25,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    3,3,10,10,31,31,25,25,
    31,31,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    0,0,0,0,12,12,0,0,
    8,8,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    31,31,31,31,15,15,21,21,
    8,8,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    10,10,14,14,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};


static const unsigned char pi_ctrl[256] __attribute__((aligned(128))) = {
    0b0111000, 0b0111000, 0b0111000, 0b0111000, 0b0111000, 0b0111000, 0b0111000, 0b0111000,
    0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000,
    0b010000, 0b010000, 0b010000, 0b010000, 0b010000, 0b010000, 0b010000, 0b010000,
    0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000,
    0b100000, 0b100000, 0b100000, 0b100000, 0b100000, 0b100000, 0b100000, 0b100000,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,

    0b0100000, 0b0100000, 0b0100000, 0b0100000, 0b0100000, 0b0100000, 0b0100000, 0b0100000,
    0b0,0b00,0b0, 0b0,0b00,0b0, 0b0,0b00,0b0, 0b0,0b00,0b0, 0b0,0b00,0b0, 0b0,0b00,0b0, 0b0,0b00,0b0, 0b0,0b00,0b0,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00,0x00,0x0,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00, 0x00
};

static const unsigned long long rc[24*16] __attribute__((aligned(128))) = {
    0x1,                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8082,             0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x800000000000808A, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000080008000, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x000000000000808B, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x0000000080000001, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000080008081, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000000008009, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x000000000000008A, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x0000000000000088, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x0000000080008009, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x000000008000000A, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x000000008000808B, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x800000000000008B, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000000008089, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000000008003, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000000008002, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000000000080, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x000000000000800A, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x800000008000000A, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000080008081, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000000008080, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x0000000080000001, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x8000000080008008, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};






void keccak_24(unsigned long long *state){

    int stride = 16;

    HVX_Vector *plane0 = (HVX_Vector *)(state);
    HVX_Vector *plane1 = (HVX_Vector *)(state + 16);
    HVX_Vector *plane2 = (HVX_Vector *)(state + 32);
    HVX_Vector *plane3 = (HVX_Vector *)(state + 48);
    HVX_Vector *plane4 = (HVX_Vector *)(state + 64);

    HVX_Vector lane0 = *plane0;
    HVX_Vector lane1 = *plane1;
    HVX_Vector lane2 = *plane2;
    HVX_Vector lane3 = *plane3;
    HVX_Vector lane4 = *plane4;
	HVX_Vector *control_p = (HVX_Vector *)(rotation_crtl);
    HVX_Vector shift = *control_p;
    HVX_Vector ctrl;
	HVX_Vector *shifts = (HVX_Vector *)(rots);


//    for(int i = 0; i < 24; i++){
    // Theta
        HVX_Vector C = lane0 ^ lane1 ^ lane2 ^ lane3 ^ lane4;

        //Rotate C left by 1 (C[x+1])
        HVX_Vector D = Q6_V_vdelta_VV(C, shift);

        //Rotate C right by 1 (C[x-1])
        C = Q6_V_vrdelta_VV(C, shift);

        // rot(C[x+1] (=D for now), 1)
        HVX_Vector tmp = Q6_Vw_vasl_VwR(D, 1);
        HVX_Vector tmp1 = Q6_Vuw_vlsr_VuwR(D, 31);
        shift = Q6_V_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, shift);
        D = tmp ^ tmp1;
        D ^= C;

        lane0 ^= D;
        lane1 ^= D;
        lane2 ^= D;
        lane3 ^= D;
        lane4 ^= D;

        //rot by vector

        HVX_Vector shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane0, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane0, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane0 = tmp1 ^ tmp;
        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane0, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane0, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane0 = tmp ^ tmp1;

        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane1, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane1, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane1 = tmp ^ tmp1;
        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane1, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane1, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane1 = tmp ^ tmp1;

        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane2, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane2, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane2 = tmp ^ tmp1;
        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane2, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane2, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane2 = tmp ^ tmp1;

        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane3, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane3, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane3 = tmp ^ tmp1;
        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane3, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane3, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane3 = tmp ^ tmp1;

        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane4, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane4, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane4 = tmp ^ tmp1;

        shifts++;
        shifts_opp = Q6_V_vsplat_R(32) - *shifts;
        tmp = Q6_Vw_vasl_VwVw(lane4, *shifts);
        tmp1 = Q6_Vw_vlsr_VwVw(lane4, shifts_opp);
        ctrl = Q6_Vb_vsplat_R(4);
        tmp1 = Q6_V_vdelta_VV(tmp1, ctrl);
        lane4 = tmp ^ tmp1;



        //Pi
//        control_p = (HVX_Vector *)(pi_ctrl);
//        ctrl = *control_p++;
//        lane0 = Q6_V_vdelta_VV(lane0, ctrl);
//        ctrl = *control_p--;
//        lane0 = Q6_V_vdelta_VV(lane0, ctrl);
//        ctrl = *control_p++;
//        lane1 = Q6_V_vdelta_VV(lane1, ctrl);
//        ctrl = *control_p--;
//        lane1 = Q6_V_vdelta_VV(lane1, ctrl);
//        ctrl = *control_p++;
//        lane2 = Q6_V_vdelta_VV(lane2, ctrl);
//        ctrl = *control_p--;
//        lane2 = Q6_V_vdelta_VV(lane2, ctrl);
//        ctrl = *control_p++;
//        lane3 = Q6_V_vdelta_VV(lane3, ctrl);
//        ctrl = *control_p--;
//        lane3 = Q6_V_vdelta_VV(lane3, ctrl);
//        ctrl = *control_p++;
//        lane4 = Q6_V_vdelta_VV(lane4, ctrl);
//        ctrl = *control_p--;
//        lane4 = Q6_V_vdelta_VV(lane4, ctrl);
//
//        //Qui?
//
//        HVX_Vector tmp0, tmp2, tmp3, tmp4;
//        tmp0 = lane0 ^ (~(lane1) & lane2);
//        tmp1 = lane1 ^ (~(lane2) & lane3);
//        tmp2 = lane2 ^ (~(lane3) & lane4);
//        tmp3 = lane3 ^ (~(lane4) & lane0);
//        tmp4 = lane4 ^ (~(lane0) & lane1);
//
//        lane0 = tmp0;
//        lane1 = tmp1;
//        lane2 = tmp2;
//        lane3 = tmp3;
//        lane4 = tmp4;

        //Transpose
//		HVX_Vector tmp, *control = ((HVX_Vector *) rotation_crtl);
//		HVX_Vector shift = *control;
//
//        lane1 = Q6_V_vrdelta_VV(lane1, shift);
//        lane2 = Q6_V_vrdelta_VV(lane2, shift);
//        lane2 = Q6_V_vrdelta_VV(lane2, shift);
//        lane3 = Q6_V_vdelta_VV(lane3, shift);
//        lane3 = Q6_V_vdelta_VV(lane3, shift);
//        lane4 = Q6_V_vdelta_VV(lane4, shift);
//
//        HVX_VectorPred Q = Q6_Q_vsetq_R(8);
//        HVX_VectorPred Q1 = Q6_Q_vsetq_R(16);
//        HVX_VectorPred Q2 = Q6_Q_vsetq_R(24);
//        HVX_VectorPred Q3 = Q6_Q_vsetq_R(32);
//        HVX_VectorPred Q4 = Q6_Q_vsetq_R(40);
//        HVX_VectorPred swap = Q ^ Q1;
//	    HVX_VectorPair helper = Q6_W_vswap_QVV(swap, lane0, lane1);
//    	lane1 = Q6_V_lo_W(helper);
//    	lane0 = Q6_V_hi_W(helper);
//        swap = Q1 ^ Q2;
//	    helper = Q6_W_vswap_QVV(swap, lane0, lane2);
//    	lane2 = Q6_V_lo_W(helper);
//    	lane0 = Q6_V_hi_W(helper);
//        swap = Q3 ^ Q2;
//	    helper = Q6_W_vswap_QVV(swap, lane0, lane3);
//    	lane3 = Q6_V_lo_W(helper);
//    	lane0 = Q6_V_hi_W(helper);
//        swap = Q3 ^ Q4;
//	    helper = Q6_W_vswap_QVV(swap, lane0, lane4);
//    	lane4 = Q6_V_lo_W(helper);
//    	lane0 = Q6_V_hi_W(helper);
//		helper = Q6_W_vswap_QVV(Q, lane1, lane4);
//    	lane4 = Q6_V_lo_W(helper);
//    	lane1 = Q6_V_hi_W(helper);
//		helper = Q6_W_vswap_QVV(Q, lane2, lane3);
//    	lane3 = Q6_V_lo_W(helper);
//    	lane2 = Q6_V_hi_W(helper);
//        helper = Q6_W_vswap_QVV(Q2^Q3, lane1, lane2);
//    	lane2 = Q6_V_lo_W(helper);
//    	lane1 = Q6_V_hi_W(helper);
//        helper = Q6_W_vswap_QVV(Q^Q1, lane2, lane4);
//    	lane4 = Q6_V_lo_W(helper);
//    	lane2 = Q6_V_hi_W(helper);
//        helper = Q6_W_vswap_QVV(Q4^Q3, lane1, lane3);
//    	lane3 = Q6_V_lo_W(helper);
//    	lane1 = Q6_V_hi_W(helper);
//        helper = Q6_W_vswap_QVV(Q1^Q2, lane3, lane4);
//    	lane4 = Q6_V_lo_W(helper);
//    	lane3 = Q6_V_hi_W(helper);
//
//
//		lane1 = Q6_V_vdelta_VV(lane1, shift);
//		lane2 = Q6_V_vdelta_VV(lane2, shift);
//		lane2 = Q6_V_vdelta_VV(lane2, shift);
//		lane3 = Q6_V_vrdelta_VV(lane3, shift);
//		lane3 = Q6_V_vrdelta_VV(lane3, shift);
//		lane4 = Q6_V_vrdelta_VV(lane4, shift);


//        control_p = (HVX_Vector *)(rc + 16 * i);
//        lane0 ^= *control_p;
//    }


    *plane0 = lane0;
    *plane1 = lane1;
    *plane2 = lane2;
    *plane3 = lane3;
    *plane4 = lane4;
//
//    HVX_Vector *plane1 = (HVX_Vector *)(state + 16);
//    HVX_Vector *plane2 = (HVX_Vector *)(state + 32);
//    HVX_Vector *plane3 = (HVX_Vector *)(state + 48);
//    HVX_Vector *plane4 = (HVX_Vector *)(state + 64);
//
//
//    HVX_Vector lane0 = *plane0;
//    HVX_Vector lane1 = *plane1;
//    HVX_Vector lane2 = *plane2;
//    HVX_Vector lane3 = *plane3;
//    HVX_Vector lane4 = *plane4;
//
//    // Theta
//    HVX_Vector C = lane0 ^ lane1 ^ lane2 ^ lane3 ^ lane4;
//    HVX_Vector D, tmp1;
//    HVX_Vector tmp = Q6_V_vror_VR(C, 120); // shifting right by one element
//    D = Q6_V_vror_VR(C, 32); // shifting left by 4 elements
//    HVX_VectorPred Q = Q6_Q_vsetq_R(8);
//    HVX_VectorPair helper = Q6_W_vswap_QVV(Q, D, tmp);
//    D = Q6_V_lo_W(helper);
//
//    tmp = Q6_V_vror_VR(C, 96);
//    tmp1 = Q6_V_vror_VR(C, 8);
//    Q = Q6_Q_vsetq_R(32);
//    helper = Q6_W_vswap_QVV(Q, tmp1, tmp);
//    tmp1 = Q6_V_lo_W(helper);
//    *plane0 = tmp1;



}




//

