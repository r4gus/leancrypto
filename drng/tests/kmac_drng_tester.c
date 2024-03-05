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

#include "alignment.h"
#include "build_bug_on.h"
#include "compare.h"
#include "lc_kmac256_drng.h"
#include "visibility.h"

#define LC_KMAC_DRNG_SEED_CUSTOMIZATION_STRING "KMAC-DRNG seed"
#define LC_KMAC_DRNG_CTX_CUSTOMIZATION_STRING "KMAC-DRNG generate"

static int kmac_drng_selftest(struct lc_rng_ctx *kmac_ctx)
{
	struct lc_kmac256_drng_state *state = kmac_ctx->rng_state;

	uint8_t seed[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	};
	static const uint8_t exp1[] = {
		0xbc, 0x70, 0xc5, 0xd6, 0xfe, 0xc4, 0x28, 0x23, 0xab, 0x57,
		0x92, 0x5e, 0xb7, 0xd5, 0x95, 0xce, 0x2d, 0x98, 0x3a, 0x47,
		0x71, 0x2f, 0x6d, 0x4f, 0x82, 0x29, 0xe8, 0x5c, 0x11, 0x08,
		0x48, 0x32, 0xfb, 0xcc, 0x30, 0x6c, 0xa1, 0x76, 0x45, 0x18,
		0x7c, 0x05, 0xc3, 0x73, 0x20, 0x28, 0xf2, 0x88, 0x7e, 0xe8,
		0x60, 0x3c, 0xf9, 0xe8, 0x84, 0xa6, 0x11, 0x1d, 0xa3, 0x92,
		0xe1, 0x8a, 0x98, 0xc1, 0xfb, 0x31, 0xf1, 0xfc, 0x0a, 0x36,
		0xab, 0x94, 0xa0, 0x39, 0xb6, 0x3a, 0xb3, 0xe4, 0x7d, 0xe3,
		0x28, 0xb2, 0xd1, 0x10, 0xb8, 0x08, 0x6d, 0xc7, 0xdd, 0xea,
		0x10, 0x3a, 0xe3, 0x41, 0x2c, 0x83, 0xfb, 0x3f, 0xc1, 0x32,
		0xfc, 0xa1, 0xdb, 0xcb, 0x2e, 0xb6, 0x10, 0x9d, 0x17, 0xf3,
		0xfc, 0x30, 0x70, 0x23, 0x67, 0x62, 0x1d, 0xc3, 0x4e, 0x63,
		0x8b, 0xc3, 0x26, 0xa2, 0x24, 0x70, 0x90, 0x0e, 0x99, 0x1a,
		0x93, 0xe3, 0x09, 0x1e, 0xa0, 0xe7, 0x4a, 0x6a, 0xb8, 0x79,
		0x97, 0x9a, 0xbb, 0x0d, 0xd0, 0xbc, 0xcc, 0x7a, 0x7b, 0xae,
		0x98, 0x1f, 0x51, 0x38, 0x41, 0x9a, 0xf1, 0x2f, 0x88, 0x90,
		0x53, 0x99, 0x58, 0xb1, 0xf9, 0x1c, 0xd3, 0x24, 0x11, 0x0e,
		0x10, 0xf3, 0xfc, 0x2d, 0x59, 0x83, 0x59, 0xee, 0x93, 0xea,
		0x98, 0xf8, 0xb7, 0x0e, 0x10, 0x42, 0x8b, 0x42, 0x5c, 0xf9,
		0x41, 0xdf, 0x3f, 0xe5, 0xf1, 0xe7, 0xb1, 0x70, 0x9d, 0x4d,
		0xbb, 0x4d, 0x69, 0x99, 0x34, 0xae, 0x8d, 0x3b, 0xdc, 0xeb,
		0xe9, 0xc4, 0xfc, 0x7a, 0x79, 0x1f, 0x62, 0x4b, 0x6f, 0x96,
		0xb6, 0x0c, 0xb8, 0x76, 0xe3, 0x94, 0x84, 0xeb, 0x7a, 0xe6,
		0x3b, 0x71, 0x40, 0xc9, 0xd0, 0xf6, 0xa3, 0x75, 0x4b, 0xb6,
		0x4e, 0x92, 0xd0, 0xa1, 0x29, 0x56, 0x8f, 0x0c, 0x80, 0xf7,
		0x8b, 0xba, 0x87, 0x7f, 0x08, 0xfc, 0xa5, 0x0f, 0x4a, 0x0a,
		0x84, 0x2b, 0xc5, 0x0d, 0x2f, 0x47, 0x82, 0x42, 0xb6, 0xf1,
		0x18, 0x82, 0xdb, 0x7d, 0x50, 0x29, 0x47, 0x4f, 0x54, 0xc8,
		0xbd, 0xa3, 0xd3, 0xcb, 0x2b, 0x30, 0x0f, 0x8b, 0xc4, 0x46,
		0xfd, 0xf6, 0xe7, 0xb0, 0x39, 0xee, 0x9b, 0x14, 0xf8, 0xca,
		0x80, 0x35, 0x19, 0xca, 0xe5, 0x3c
	};
	uint8_t act1[sizeof(exp1)] __align(sizeof(uint32_t));
	uint8_t compare1[LC_KMAC256_DRNG_MAX_CHUNK];
	uint8_t encode;
	int ret;
	LC_KMAC_CTX_ON_STACK(kmac_compare, lc_cshake256);

	BUILD_BUG_ON(sizeof(exp1) < sizeof(compare1));

	lc_rng_seed(kmac_ctx, seed, sizeof(seed), NULL, 0);
	lc_rng_generate(kmac_ctx, NULL, 0, act1, sizeof(act1));
	ret = lc_compare(act1, exp1, sizeof(act1), "KMAC DRNG");
	lc_rng_zero(kmac_ctx);

	/* Verfy the generation operation with one KMAC call */
	/* Prepare the key */
	lc_rng_seed(kmac_ctx, seed, sizeof(seed), NULL, 0);
	lc_kmac_init(kmac_compare, NULL, 0,
		     (uint8_t *)LC_KMAC_DRNG_SEED_CUSTOMIZATION_STRING,
		     sizeof(LC_KMAC_DRNG_SEED_CUSTOMIZATION_STRING) - 1);
	lc_kmac_update(kmac_compare, seed, sizeof(seed));
	encode = 0;
	lc_kmac_update(kmac_compare, &encode, sizeof(encode));
	/* Generate data with one KMAC call */
	lc_kmac_final_xof(kmac_compare, compare1, LC_KMAC256_DRNG_KEYSIZE);
	ret += lc_compare(compare1, state->key, LC_KMAC256_DRNG_KEYSIZE,
			  "KMAC DRNG verification");

	/* Verify the generate operation */
	/* Use the already generated state from above */
	lc_kmac_init(kmac_compare, compare1, LC_KMAC256_DRNG_KEYSIZE,
		     (uint8_t *)LC_KMAC_DRNG_CTX_CUSTOMIZATION_STRING,
		     sizeof(LC_KMAC_DRNG_CTX_CUSTOMIZATION_STRING) - 1);
	encode = 2 * 85;
	lc_kmac_update(kmac_compare, &encode, sizeof(encode));
	lc_kmac_final_xof(kmac_compare, compare1, sizeof(compare1));
	ret += lc_compare(compare1 + LC_KMAC256_DRNG_KEYSIZE, exp1,
			  LC_KMAC256_DRNG_MAX_CHUNK - LC_KMAC256_DRNG_KEYSIZE,
			  "KMAC DRNG generate verification");

	lc_rng_zero(kmac_ctx);
	lc_kmac_zero(kmac_compare);

	return ret;
}

static int kmac_test(void)
{
	struct lc_rng_ctx *kmac_ctx_heap;
	int ret;
	LC_KMAC256_DRNG_CTX_ON_STACK(kmac_ctx);

	ret = kmac_drng_selftest(kmac_ctx);

	if (lc_kmac256_drng_alloc(&kmac_ctx_heap))
		return 1;

	ret += kmac_drng_selftest(kmac_ctx_heap);

	lc_rng_zero_free(kmac_ctx_heap);
	return ret;
}

LC_TEST_FUNC(int, main, int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	return kmac_test();
}
