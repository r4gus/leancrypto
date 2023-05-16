/*
 * Copyright (C) 2022 - 2023, Stephan Mueller <smueller@chronox.de>
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

#include "kyber_internal.h"
#include "kyber_kdf.h"

#include "lc_memset_secure.h"
#include "lc_kyber.h"
#include "ret_checkers.h"
#include "visibility.h"

int lc_kex_uake_responder_init_internal(struct lc_kyber_pk *pk_e_r,
					struct lc_kyber_ct *ct_e_r,
					struct lc_kyber_ss *tk,
					struct lc_kyber_sk *sk_e,
					const struct lc_kyber_pk *pk_i,
					struct lc_rng_ctx *rng_ctx)
{
	int ret;

	CKINT(lc_kyber_keypair(pk_e_r, sk_e, rng_ctx));
	CKINT(lc_kyber_enc_internal(ct_e_r, tk->ss, LC_KYBER_SSBYTES, pk_i,
				    rng_ctx));

out:
	return ret;
}

LC_INTERFACE_FUNCTION(
int, lc_kex_uake_responder_init, struct lc_kyber_pk *pk_e_r,
				 struct lc_kyber_ct *ct_e_r,
				 struct lc_kyber_ss *tk,
				 struct lc_kyber_sk *sk_e,
				 const struct lc_kyber_pk *pk_i)
{
	return lc_kex_uake_responder_init_internal(pk_e_r, ct_e_r, tk, sk_e,
						   pk_i, lc_seeded_rng);
}

int lc_kex_uake_initiator_ss_internal(struct lc_kyber_ct *ct_e_i,
				      uint8_t *shared_secret,
				      size_t shared_secret_len,
				      const uint8_t *kdf_nonce,
				      size_t kdf_nonce_len,
				      const struct lc_kyber_pk *pk_e_r,
				      const struct lc_kyber_ct *ct_e_r,
				      const struct lc_kyber_sk *sk_i,
				      struct lc_rng_ctx *rng_ctx)
{
	struct lc_kyber_ss ss[2];
	int ret;

	CKINT(lc_kyber_enc_internal(ct_e_i, ss[0].ss, LC_KYBER_SSBYTES, pk_e_r,
				    rng_ctx));
	CKINT(lc_kyber_dec(ss[1].ss, LC_KYBER_SSBYTES, ct_e_r, sk_i));
	kyber_kdf3(ss[0].ss, LC_KYBER_SSBYTES,
		   ss[1].ss, LC_KYBER_SSBYTES,
		   kdf_nonce, kdf_nonce_len,
		   shared_secret, shared_secret_len);

out:
	lc_memset_secure(ss, 0, sizeof(ss));
	return ret;
}

LC_INTERFACE_FUNCTION(
int, lc_kex_uake_initiator_ss, struct lc_kyber_ct *ct_e_i,
			       uint8_t *shared_secret,
			       size_t shared_secret_len,
			       const uint8_t *kdf_nonce,
			       size_t kdf_nonce_len,
			       const struct lc_kyber_pk *pk_e_r,
			       const struct lc_kyber_ct *ct_e_r,
			       const struct lc_kyber_sk *sk_i)
{
	return lc_kex_uake_initiator_ss_internal(ct_e_i, shared_secret,
						 shared_secret_len,
						 kdf_nonce, kdf_nonce_len,
						 pk_e_r, ct_e_r, sk_i,
						 lc_seeded_rng);
}

LC_INTERFACE_FUNCTION(
int, lc_kex_uake_responder_ss, uint8_t *shared_secret,
			       size_t shared_secret_len,
			       const uint8_t *kdf_nonce,
			       size_t kdf_nonce_len,
			       const struct lc_kyber_ct *ct_e_i,
			       const struct lc_kyber_ss *tk,
			       const struct lc_kyber_sk *sk_e)
{
	struct lc_kyber_ss ss;
	int ret;

	CKINT(lc_kyber_dec(ss.ss, LC_KYBER_SSBYTES, ct_e_i, sk_e));
	kyber_kdf3(ss.ss, LC_KYBER_SSBYTES,
		   tk->ss, LC_KYBER_SSBYTES,
		   kdf_nonce, kdf_nonce_len,
		   shared_secret, shared_secret_len);

out:
	lc_memset_secure(&ss, 0, sizeof(ss));
	return ret;
}

int lc_kex_ake_responder_init_internal(struct lc_kyber_pk *pk_e_r,
				       struct lc_kyber_ct *ct_e_r,
				       struct lc_kyber_ss *tk,
				       struct lc_kyber_sk *sk_e,
				       const struct lc_kyber_pk *pk_i,
				       struct lc_rng_ctx *rng_ctx)
{
	int ret;

	CKINT(lc_kyber_keypair(pk_e_r, sk_e, rng_ctx));
	CKINT(lc_kyber_enc_internal(ct_e_r, tk->ss, LC_KYBER_SSBYTES, pk_i,
				    rng_ctx));

out:
	return ret;
}

LC_INTERFACE_FUNCTION(
int, lc_kex_ake_responder_init, struct lc_kyber_pk *pk_e_r,
				struct lc_kyber_ct *ct_e_r,
				struct lc_kyber_ss *tk,
				struct lc_kyber_sk *sk_e,
				const struct lc_kyber_pk *pk_i)
{
	return lc_kex_ake_responder_init_internal(pk_e_r, ct_e_r, tk, sk_e,
						  pk_i, lc_seeded_rng);
}

int lc_kex_ake_initiator_ss_internal(struct lc_kyber_ct *ct_e_i_1,
				     struct lc_kyber_ct *ct_e_i_2,
				     uint8_t *shared_secret,
				     size_t shared_secret_len,
				     const uint8_t *kdf_nonce,
				     size_t kdf_nonce_len,
				     const struct lc_kyber_pk *pk_e_r,
				     const struct lc_kyber_ct *ct_e_r,
				     const struct lc_kyber_sk *sk_i,
				     const struct lc_kyber_pk *pk_r,
				     struct lc_rng_ctx *rng_ctx)
{
	struct lc_kyber_ss ss[3];
	int ret;

	CKINT(lc_kyber_enc_internal(ct_e_i_1, ss[0].ss, LC_KYBER_SSBYTES,
				    pk_e_r, rng_ctx));
	CKINT(lc_kyber_enc_internal(ct_e_i_2, ss[1].ss, LC_KYBER_SSBYTES, pk_r,
				    rng_ctx));
	CKINT(lc_kyber_dec(ss[2].ss, LC_KYBER_SSBYTES, ct_e_r, sk_i));
	kyber_kdf4(ss[0].ss, LC_KYBER_SSBYTES,
		   ss[1].ss, LC_KYBER_SSBYTES,
		   ss[2].ss, LC_KYBER_SSBYTES,
		   kdf_nonce, kdf_nonce_len,
		   shared_secret, shared_secret_len);

out:
	lc_memset_secure(ss, 0, sizeof(ss));
	return ret;
}

LC_INTERFACE_FUNCTION(
int, lc_kex_ake_initiator_ss, struct lc_kyber_ct *ct_e_i_1,
			      struct lc_kyber_ct *ct_e_i_2,
			      uint8_t *shared_secret,
			      size_t shared_secret_len,
			      const uint8_t *kdf_nonce,
			      size_t kdf_nonce_len,
			      const struct lc_kyber_pk *pk_e_r,
			      const struct lc_kyber_ct *ct_e_r,
			      const struct lc_kyber_sk *sk_i,
			      const struct lc_kyber_pk *pk_r)
{
	return lc_kex_ake_initiator_ss_internal(ct_e_i_1, ct_e_i_2,
						shared_secret,
						shared_secret_len,
						kdf_nonce, kdf_nonce_len,
						pk_e_r, ct_e_r, sk_i, pk_r,
						lc_seeded_rng);
}

LC_INTERFACE_FUNCTION(
int, lc_kex_ake_responder_ss, uint8_t *shared_secret,
			      size_t shared_secret_len,
			      const uint8_t *kdf_nonce,
			      size_t kdf_nonce_len,
			      const struct lc_kyber_ct *ct_e_i_1,
			      const struct lc_kyber_ct *ct_e_i_2,
			      const struct lc_kyber_ss *tk,
			      const struct lc_kyber_sk *sk_e,
			      const struct lc_kyber_sk *sk_r)
{
	struct lc_kyber_ss ss[2];
	int ret;

	CKINT(lc_kyber_dec(ss[0].ss, LC_KYBER_SSBYTES, ct_e_i_1, sk_e));
	CKINT(lc_kyber_dec(ss[1].ss, LC_KYBER_SSBYTES, ct_e_i_2, sk_r));
	kyber_kdf4(ss[0].ss, LC_KYBER_SSBYTES,
		   ss[1].ss, LC_KYBER_SSBYTES,
		   tk->ss, LC_KYBER_SSBYTES,
		   kdf_nonce, kdf_nonce_len,
		   shared_secret, shared_secret_len);

out:
	lc_memset_secure(ss, 0, sizeof(ss));
	return ret;
}
