//
// Created by fcaullery on 21/07/25.
//

#include "kyber_ntt_i.h"
#include "hexagon_types.h"
#include "hvx.cfg.h"

const int16_t zetas_vec[128] = {
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


static HVX_Vector fqmul_vec(HVX_Vector a, HVX_Vector b){

  HVX_VectorPair pair = Q6_Ww_vmpy_VhVh(a, b);
  HVX_Vector t_even = Q6_Vw_vmpyi_VwRh(Q6_V_hi_W(pair), 0xF301F301); // int16 QINV=-3327 duplicated into 32b
  HVX_Vector t_odd = Q6_Vw_vmpyi_VwRh(Q6_V_lo_W(pair), 0xF301F301); // int16 QINV=-3327 duplicated into 32b
  HVX_Vector tmp = Q6_Vh_vshuffe_VhVh(t_even, t_odd);

  HVX_VectorPair pair_1 = Q6_Ww_vmpy_VhRh(tmp, 0x0d010d01); //Kyber_Q duplicated
  pair = Q6_Ww_vsub_WwWw(pair, pair_1);

  t_even = Q6_Vw_vasr_VwR(Q6_V_hi_W(pair), 16);
  t_odd = Q6_Vw_vasr_VwR(Q6_V_lo_W(pair), 16);

  return Q6_Vh_vshuffe_VhVh(t_even, t_odd);
}

void ntt(int16_t r[256]){

  HVX_Vector *r_vec = (HVX_Vector *)r;
  HVX_Vector r0 = r_vec[0];
  HVX_Vector r1 = r_vec[1];
  HVX_Vector r2 = r_vec[2];
  HVX_Vector r3 = r_vec[3];

  HVX_Vector zetas = Q6_Vh_vsplat_R(zetas_vec[1]);
  r_vec[0] = fqmul_vec(r0, zetas);

  return;
}