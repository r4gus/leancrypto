/*
 * Copyright (C) 2022 - 2024, Stephan Mueller <smueller@chronox.de>
 *
 * License: see LICENSE file in root directory
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
/*
 * This code is derived in parts from the code distribution provided with
 * https://github.com/pq-crystals/kyber
 *
 * That code is released under Public Domain
 * (https://creativecommons.org/share-your-work/public-domain/cc0/).
 */

#include "kyber_consts_avx2.h"
#include "lc_kyber.h"

#define Q LC_KYBER_Q
#define MONT -1044 // 2^16 mod q
#define QINV -3327 // q^-1 mod 2^16
#define V 20159 // floor(2^26/q + 0.5)
#define FHI 1441 // mont^2/128
#define FLO -10079 // qinv*FHI
#define MONTSQHI 1353 // mont^2
#define MONTSQLO 20553 // qinv*MONTSQHI
#define MASK 4095
#define SHIFT 32

const kyber_qdata_t kyber_qdata = { {
#define _16XQ 0
	Q,	  Q,	    Q,	      Q,	Q,	  Q,	    Q,
	Q,	  Q,	    Q,	      Q,	Q,	  Q,	    Q,
	Q,	  Q,

#define _16XQINV 16
	QINV,	  QINV,	    QINV,     QINV,	QINV,	  QINV,	    QINV,
	QINV,	  QINV,	    QINV,     QINV,	QINV,	  QINV,	    QINV,
	QINV,	  QINV,

#define _16XV 32
	V,	  V,	    V,	      V,	V,	  V,	    V,
	V,	  V,	    V,	      V,	V,	  V,	    V,
	V,	  V,

#define _16XFLO 48
	FLO,	  FLO,	    FLO,      FLO,	FLO,	  FLO,	    FLO,
	FLO,	  FLO,	    FLO,      FLO,	FLO,	  FLO,	    FLO,
	FLO,	  FLO,

#define _16XFHI 64
	FHI,	  FHI,	    FHI,      FHI,	FHI,	  FHI,	    FHI,
	FHI,	  FHI,	    FHI,      FHI,	FHI,	  FHI,	    FHI,
	FHI,	  FHI,

#define _16XMONTSQLO 80
	MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO,
	MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO, MONTSQLO,
	MONTSQLO, MONTSQLO,

#define _16XMONTSQHI 96
	MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI,
	MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI, MONTSQHI,
	MONTSQHI, MONTSQHI,

#define _16XMASK 112
	MASK,	  MASK,	    MASK,     MASK,	MASK,	  MASK,	    MASK,
	MASK,	  MASK,	    MASK,     MASK,	MASK,	  MASK,	    MASK,
	MASK,	  MASK,

#define _REVIDXB 128
	3854,	  3340,	    2826,     2312,	1798,	  1284,	    770,
	256,	  3854,	    3340,     2826,	2312,	  1798,	    1284,
	770,	  256,

#define _REVIDXD 144
	7,	  0,	    6,	      0,	5,	  0,	    4,
	0,	  3,	    0,	      2,	0,	  1,	    0,
	0,	  0,

#define _ZETAS_EXP 160
	31498,	  31498,    31498,    31498,	-758,	  -758,	    -758,
	-758,	  5237,	    5237,     5237,	5237,	  1397,	    1397,
	1397,	  1397,	    14745,    14745,	14745,	  14745,    14745,
	14745,	  14745,    14745,    14745,	14745,	  14745,    14745,
	14745,	  14745,    14745,    14745,	-359,	  -359,	    -359,
	-359,	  -359,	    -359,     -359,	-359,	  -359,	    -359,
	-359,	  -359,	    -359,     -359,	-359,	  -359,	    13525,
	13525,	  13525,    13525,    13525,	13525,	  13525,    13525,
	-12402,	  -12402,   -12402,   -12402,	-12402,	  -12402,   -12402,
	-12402,	  1493,	    1493,     1493,	1493,	  1493,	    1493,
	1493,	  1493,	    1422,     1422,	1422,	  1422,	    1422,
	1422,	  1422,	    1422,     -20907,	-20907,	  -20907,   -20907,
	27758,	  27758,    27758,    27758,	-3799,	  -3799,    -3799,
	-3799,	  -15690,   -15690,   -15690,	-15690,	  -171,	    -171,
	-171,	  -171,	    622,      622,	622,	  622,	    1577,
	1577,	  1577,	    1577,     182,	182,	  182,	    182,
	-5827,	  -5827,    17363,    17363,	-26360,	  -26360,   -29057,
	-29057,	  5571,	    5571,     -1102,	-1102,	  21438,    21438,
	-26242,	  -26242,   573,      573,	-1325,	  -1325,    264,
	264,	  383,	    383,      -829,	-829,	  1458,	    1458,
	-1602,	  -1602,    -130,     -130,	-5689,	  -6516,    1496,
	30967,	  -23565,   20179,    20710,	25080,	  -12796,   26616,
	16064,	  -12442,   9134,     -650,	-25986,	  27837,    1223,
	652,	  -552,	    1015,     -1293,	1491,	  -282,	    -1544,
	516,	  -8,	    -320,     -666,	-1618,	  -1162,    126,
	1469,	  -335,	    -11477,   -32227,	20494,	  -27738,   945,
	-14883,	  6182,	    32010,    10631,	29175,	  -28762,   -18486,
	17560,	  -14430,   -5276,    -1103,	555,	  -1251,    1550,
	422,	  177,	    -291,     1574,	-246,	  1159,	    -777,
	-602,	  -1590,    -872,     418,	-156,	  11182,    13387,
	-14233,	  -21655,   13131,    -4587,	23092,	  5493,	    -32502,
	30317,	  -18741,   12639,    20100,	18525,	  19529,    -12619,
	430,	  843,	    871,      105,	587,	  -235,	    -460,
	1653,	  778,	    -147,     1483,	1119,	  644,	    349,
	329,	  -75,	    787,      787,	787,	  787,	    787,
	787,	  787,	    787,      787,	787,	  787,	    787,
	787,	  787,	    787,      787,	-1517,	  -1517,    -1517,
	-1517,	  -1517,    -1517,    -1517,	-1517,	  -1517,    -1517,
	-1517,	  -1517,    -1517,    -1517,	-1517,	  -1517,    28191,
	28191,	  28191,    28191,    28191,	28191,	  28191,    28191,
	-16694,	  -16694,   -16694,   -16694,	-16694,	  -16694,   -16694,
	-16694,	  287,	    287,      287,	287,	  287,	    287,
	287,	  287,	    202,      202,	202,	  202,	    202,
	202,	  202,	    202,      10690,	10690,	  10690,    10690,
	1358,	  1358,	    1358,     1358,	-11202,	  -11202,   -11202,
	-11202,	  31164,    31164,    31164,	31164,	  962,	    962,
	962,	  962,	    -1202,    -1202,	-1202,	  -1202,    -1474,
	-1474,	  -1474,    -1474,    1468,	1468,	  1468,	    1468,
	-28073,	  -28073,   24313,    24313,	-10532,	  -10532,   8800,
	8800,	  18426,    18426,    8859,	8859,	  26675,    26675,
	-16163,	  -16163,   -681,     -681,	1017,	  1017,	    732,
	732,	  608,	    608,      -1542,	-1542,	  411,	    411,
	-205,	  -205,	    -1571,    -1571,	19883,	  -28250,   -15887,
	-8898,	  -28309,   9075,     -30199,	18249,	  13426,    14017,
	-29156,	  -12757,   16832,    4311,	-24155,	  -17915,   -853,
	-90,	  -271,	    830,      107,	-1421,	  -247,	    -951,
	-398,	  961,	    -1508,    -725,	448,	  -1065,    677,
	-1275,	  -31183,   25435,    -7382,	24391,	  -20927,   10946,
	24214,	  16989,    10335,    -7934,	-22502,	  10906,    31636,
	28644,	  23998,    -17422,   817,	603,	  1322,	    -1465,
	-1215,	  1218,	    -874,     -1187,	-1185,	  -1278,    -1510,
	-870,	  -108,	    996,      958,	1522,	  20297,    2146,
	15355,	  -32384,   -6280,    -14903,	-11044,	  14469,    -21498,
	-20198,	  23210,    -17442,   -23860,	-20257,	  7756,	    23132,
	1097,	  610,	    -1285,    384,	-136,	  -1335,    220,
	-1659,	  -1530,    794,      -854,	478,	  -308,	    991,
	-1460,	  1628,

#define _16XSHIFT 624
	SHIFT,	  SHIFT,    SHIFT,    SHIFT,	SHIFT,	  SHIFT,    SHIFT,
	SHIFT,	  SHIFT,    SHIFT,    SHIFT,	SHIFT,	  SHIFT,    SHIFT,
	SHIFT,	  SHIFT } };
