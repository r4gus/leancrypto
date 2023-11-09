/*
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

#include "ext_headers.h"

#include "compare.h"
#include "kyber_internal.h"
#include "ret_checkers.h"
#include "small_stack_support.h"
#include "selftest_rng.h"
#include "visibility.h"

static int kyber_kem_double_tester(void)
{
	static const uint8_t kyber_pk_exp[] = {
		0x78, 0x0c, 0x72, 0x5a, 0x25, 0x15, 0x1f, 0x1b,
		0x09, 0xe9, 0xc7, 0x1d, 0x26, 0x97, 0x5d, 0xd6,
		0x52, 0x20, 0xe0, 0x59, 0xca, 0x52, 0x6b, 0x6e,
		0x6f, 0x87, 0x4a, 0xa5, 0xe8, 0xb6, 0x22, 0x97
	};
	static const uint8_t kyber_sk_exp[] = {
		0x24, 0xc5, 0x9d, 0x1c, 0x76, 0x03, 0xe7, 0xb7,
		0x4b, 0xc7, 0xaa, 0x1b, 0xc2, 0xcb, 0x3a, 0x21,
		0x4b, 0x3c, 0xfa, 0xeb, 0xb6, 0x3b, 0xd8, 0x5b,
		0x65, 0x40, 0x84, 0x27, 0xc4, 0x98, 0xba, 0x39
	};
	static const uint8_t x25519_pk_exp[] = {
		0xe5, 0x6f, 0x17, 0x57, 0x67, 0x40, 0xce, 0x2a,
		0x32, 0xfc, 0x51, 0x45, 0x03, 0x01, 0x45, 0xcf,
		0xb9, 0x7e, 0x63, 0xe0, 0xe4, 0x1d, 0x35, 0x42,
		0x74, 0xa0, 0x79, 0xd3, 0xe6, 0xfb, 0x2e, 0x15
	};
	static const uint8_t x25519_sk_exp[] = {
		0x35, 0xb8, 0xcc, 0x87, 0x3c, 0x23, 0xdc, 0x62,
		0xb8, 0xd2, 0x60, 0x16, 0x9a, 0xfa, 0x2f, 0x75,
		0xab, 0x91, 0x6a, 0x58, 0xd9, 0x74, 0x91, 0x88,
		0x35, 0xd2, 0x5e, 0x6a, 0x43, 0x50, 0x85, 0xb2
	};
	static const uint8_t ss_exp[] = {
		0x52, 0xd8, 0xe7, 0xc0, 0xfc, 0x97, 0x25, 0xeb, 0x72, 0x17,
		0xde, 0x01, 0xf2, 0x7f, 0xdf, 0xc7, 0xd9, 0x27, 0xe2, 0xd1,
		0xb4, 0x55, 0xd4, 0x89, 0x65, 0xfa, 0xdf, 0xcc, 0x21, 0xae,
		0x25, 0xf3, 0xcc, 0x56, 0xe8, 0x13, 0x38, 0x90, 0x4f, 0xb4,
		0xfd, 0x75, 0xff, 0x03, 0x86, 0x29, 0x42, 0xe0, 0x1f, 0x58,
		0x09, 0xad, 0x3b, 0x6e, 0xe3, 0xaa, 0xa1, 0x77, 0x11, 0x96,
		0xf2, 0x4c, 0xaf, 0x67, 0xdc, 0x74, 0xba
	};
	struct workspace {
		struct lc_kyber_x25519_pk pk;
		struct lc_kyber_x25519_sk sk;
		struct lc_kyber_x25519_ct ct;
		uint8_t ss1[sizeof(ss_exp)], ss2[sizeof(ss_exp)];
	};
	int ret;
	LC_DECLARE_MEM(ws, struct workspace, sizeof(uint64_t));
	LC_SELFTEST_DRNG_CTX_ON_STACK(selftest_rng);

	CKINT(lc_kyber_x25519_keypair(&ws->pk, &ws->sk, selftest_rng));

	lc_compare(ws->pk.pk.pk, kyber_pk_exp, sizeof(kyber_pk_exp),
		   "Kyber pk keygen\n");
	lc_compare(ws->sk.sk.sk, kyber_sk_exp, sizeof(kyber_sk_exp),
		   "Kyber sk keygen\n");
	lc_compare(ws->pk.pk_x25519.pk, x25519_pk_exp, sizeof(x25519_pk_exp),
		   "X25519 pk keygen\n");
	lc_compare(ws->sk.sk_x25519.sk, x25519_sk_exp, sizeof(x25519_sk_exp),
		   "X25519 sk keygen\n");

	CKINT(lc_kyber_x25519_enc_kdf_internal(&ws->ct, ws->ss1,
					       sizeof(ws->ss1), &ws->pk,
					       selftest_rng));

	CKINT(lc_kyber_x25519_dec_kdf(ws->ss2, sizeof(ws->ss2), &ws->ct,
				      &ws->sk));

	lc_compare(ws->ss1, ss_exp, sizeof(ss_exp),
		   "Kyber double SS generation\n");
	lc_compare(ws->ss1, ws->ss2, sizeof(ws->ss2),
		   "Kyber double SS comparison\n");

out:
	LC_RELEASE_MEM(ws);
	return ret;
}

LC_TEST_FUNC(int, main, int argc, char *argv[])
{
	int ret = 0;

	(void)argc;
	(void)argv;

	ret += kyber_kem_double_tester();

	return ret;
}
