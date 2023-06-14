/* XDRBG with SHAKE256
 *
 * Copyright (C) 2023, Stephan Mueller <smueller@chronox.de>
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
#include "lc_memcmp_secure.h"
#include "lc_xdrbg256.h"
#include "math_helper.h"
#include "visibility.h"

static inline void xdrbg256_shake_final(struct lc_hash_ctx *shake_ctx,
					uint8_t *digest, size_t digest_len)
{
	lc_hash_set_digestsize(shake_ctx, digest_len);
	lc_hash_final(shake_ctx, digest);
}

static void xdrbg256_drng_selftest(int *tested, const char *impl)
{
	uint8_t seed[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	};
	static const uint8_t exp[] = {
		0x21, 0x15, 0x36, 0x5a, 0x2c, 0x86, 0x93, 0x55,
		0x80, 0x81, 0x52, 0xc4, 0xf4, 0xac, 0x13, 0x08,
		0x7e, 0xfe, 0xe6, 0x5b, 0xd7, 0x77, 0x0b, 0x0e,
		0xaa, 0xc2, 0x9c, 0xc7, 0xa7, 0xfd, 0x41, 0xbf,
		0x28, 0x11, 0x4a, 0xe8, 0x90, 0x33, 0x68, 0xe0,
		0xec, 0xcf, 0xbe, 0xc7, 0xab, 0x52, 0x95, 0xa3,
		0x40, 0xa8, 0x99, 0x3f, 0x63, 0xf0, 0x2a, 0xe8,
		0xa7, 0xa8, 0xca, 0x4e, 0xfd, 0xec, 0x82, 0x5a,
		0x44, 0x68, 0x47, 0xfb, 0xd8, 0x36, 0x50, 0xb8,
		0x62, 0xba, 0x8e, 0x9b, 0xaa, 0xe7, 0xbb, 0xd0,
		0xe9, 0xc8, 0x13, 0x67, 0x0c, 0xce, 0x10, 0xc8,
		0x51, 0x8d, 0x89, 0x44, 0xd7, 0xa2, 0xda, 0x2f,
		0x2b, 0x9d, 0xad, 0xa4, 0x30, 0x62, 0xf9, 0x47,
		0xe5, 0xa2, 0xac, 0x29, 0x87, 0xdd, 0x2f, 0xcc,
		0x2a, 0x90, 0x1a, 0x72, 0x4d, 0x21, 0x6f, 0x8e,
		0x95, 0xe2, 0xca, 0x7d, 0x1c, 0x46, 0x36, 0x97,
		0x59, 0x8a, 0x7b, 0x70, 0x02, 0x5d, 0xaa, 0x98,
		0x83, 0xe7, 0xfd, 0x9d, 0x2d, 0x34, 0x96, 0x28,
		0x80, 0xbb, 0x98, 0xb8, 0x96, 0xe7, 0xf2, 0x9d,
		0x16, 0x25, 0xae, 0xe3, 0x4b, 0x8e, 0x03, 0x3a,
		0xae, 0xe5, 0xe9, 0x34, 0xd7, 0xf2, 0x67, 0xca,
		0x15, 0x26, 0xc0, 0x51, 0x15, 0x68, 0x40, 0x1d,
		0x87, 0xc7, 0x12, 0xdd, 0xca, 0x22, 0x2b, 0x80,
		0xd7, 0xf9, 0x25, 0xe8, 0x51, 0x73, 0xfa, 0x42,
		0x5b, 0x5e, 0xb3, 0x6a, 0xb7, 0x66, 0x97, 0x60,
		0xe7, 0x8a, 0x83, 0x18, 0x8a, 0x63, 0x9a, 0x9a,
		0x41, 0x11, 0xa0, 0x8c, 0xe5, 0x8d, 0x24, 0x4b,
		0x42, 0xde, 0x4c, 0x74, 0xda, 0x13, 0xdb, 0x85,
		0xd6, 0x30, 0x01, 0x30, 0xe8, 0x48, 0x28, 0xcb,
		0xbd, 0xb8, 0xa0, 0x14, 0xf9, 0xec, 0x73, 0x2f,
		0xca, 0x6d, 0xf3, 0x84, 0x30, 0x3b, 0xba

	};
	uint8_t act[sizeof(exp)] __align(sizeof(uint32_t));

	LC_SELFTEST_RUN(tested);

	LC_XDRBG256_DRNG_CTX_ON_STACK(shake_ctx);

	lc_rng_seed(shake_ctx, seed, sizeof(seed), NULL, 0);
	lc_rng_generate(shake_ctx, NULL, 0, act, sizeof(act));
	lc_compare_selftest(act, exp, sizeof(exp), impl);
	lc_rng_zero(shake_ctx);
}

/*
 * Fast-key-erasure initialization of the SHAKE context. The caller must
 * securely dispose of the initialized SHAKE context. Additional data
 * can be squeezed from the state using lc_hash_final.
 *
 * This function initializes the SHAKE context that can later be used to squeeze
 * random bits out of the SHAKE context. The initialization happens from the key
 * found in the state. Before any random bits can be created, the first 512
 * output bits that are generated is used to overwrite the key. This implies
 * an automatic backtracking resistance as the next round to generate random
 * numbers uses the already updated key.
 *
 * When this function completes, initialized SHAKE context can now be used
 * to generate random bits.
 */
static void
xdrbg256_drng_fke_init_ctx(struct lc_xdrbg256_drng_state *state,
			   struct lc_hash_ctx *shake_ctx,
			   const uint8_t *addtl_input, size_t addtl_input_len)
{
	lc_hash_init(shake_ctx);

	/* Initialize the SHAKE with V' */
	lc_hash_update(shake_ctx, state->v, LC_XDRBG256_DRNG_KEYSIZE);

	/* Insert the additional data into the SHAKE state. */
	lc_hash_update(shake_ctx, addtl_input, addtl_input_len);

	/* Generate the V to store in the state and overwrite V'. */
	xdrbg256_shake_final(shake_ctx, state->v, LC_XDRBG256_DRNG_KEYSIZE);
}

/*
 * Generating random bits is performed by initializing a transient SHAKE state
 * with the key found in state. The initialization implies that the key in
 * the state variable is already updated before random bits are generated.
 *
 * The random bits are generated by performing a SHAKE final operation. The
 * generation operation is chunked to ensure that the fast-key-erasure updates
 * the key when large quantities of random bits are generated.
 */
static int
lc_xdrbg256_drng_generate(void *_state,
			  const uint8_t *addtl_input, size_t addtl_input_len,
			  uint8_t *out, size_t outlen)
{
	struct lc_xdrbg256_drng_state *state = _state;
	LC_HASH_CTX_ON_STACK(shake_ctx, lc_shake256);

	if (!state)
		return -EINVAL;

	while (outlen) {
		// TODO: is maxout l + |V| or just l?
		size_t todo = min_size(outlen, LC_XDRBG256_DRNG_MAX_CHUNK);

		/* Instantiate SHAKE with V', generate V */
		xdrbg256_drng_fke_init_ctx(state, shake_ctx,
					   addtl_input, addtl_input_len);

		/* Generate the requested amount of output bits */
		xdrbg256_shake_final(shake_ctx, out, todo);

		out += todo;
		outlen -= todo;
	}

	/* V is already in place. */

	/* Clear the SHAKE state which is not needed any more. */
	lc_hash_zero(shake_ctx);

	return 0;
}

/*
 * The DRNG is seeded by initializing a fast-key-erasure SHAKE context and add
 * the key into the SHAKE state. The SHAKE final operation replaces the key in
 * state.
 */
static int
lc_xdrbg256_drng_seed(void *_state,
		      const uint8_t *seed, size_t seedlen,
		      const uint8_t *persbuf, size_t perslen)
{
	static int tested = 0;
	struct lc_xdrbg256_drng_state *state = _state;
	LC_HASH_CTX_ON_STACK(shake_ctx, lc_shake256);

	if (!state)
		return -EINVAL;

	xdrbg256_drng_selftest(&tested, "SHAKE DRNG");

	lc_hash_init(shake_ctx);

	/*
	 * Initialize the SHAKE with V'. During initial seeding V' is a zero
	 * buffer.
	 */
	// TODO: XDRBG specification says: initial seeding shall not use V
	lc_hash_update(shake_ctx, state->v, LC_XDRBG256_DRNG_KEYSIZE);

	/* Insert the seed data into the SHAKE state. */
	lc_hash_update(shake_ctx, seed, seedlen);

	/* Insert the personalization string into the SHAKE state. */
	lc_hash_update(shake_ctx, persbuf, perslen);

	/* Generate the V to store in the state and overwrite V'. */
	xdrbg256_shake_final(shake_ctx, state->v, LC_XDRBG256_DRNG_KEYSIZE);

	/* Clear the SHAKE state which is not needed any more. */
	lc_hash_zero(shake_ctx);

	return 0;
}

static void lc_xdrbg256_drng_zero(void *_state)
{
	struct lc_xdrbg256_drng_state *state = _state;

	if (!state)
		return;

	lc_memset_secure((uint8_t *)state, 0, LC_XDRBG256_DRNG_STATE_SIZE);
}

LC_INTERFACE_FUNCTION(
int, lc_xdrbg256_drng_alloc, struct lc_rng_ctx **state)
{
	struct lc_rng_ctx *out_state = NULL;
	int ret;

	if (!state)
		return -EINVAL;

	ret = lc_alloc_aligned_secure((void *)&out_state,
				      LC_HASH_COMMON_ALIGNMENT,
				      LC_XDRBG256_DRNG_CTX_SIZE);
	if (ret)
		return -ret;

	LC_XDRBG256_RNG_CTX(out_state);

	*state = out_state;

	return 0;
}

static const struct lc_rng _lc_xdrbg256_drng = {
	.generate	= lc_xdrbg256_drng_generate,
	.seed		= lc_xdrbg256_drng_seed,
	.zero		= lc_xdrbg256_drng_zero,
};
LC_INTERFACE_SYMBOL(
	const struct lc_rng *, lc_xdrbg256_drng) = &_lc_xdrbg256_drng;
