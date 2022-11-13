/* Symmetric stream AEAD cipher based on KMAC
 *
 * Copyright (C) 2022, Stephan Mueller <smueller@chronox.de>
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

/******************************************************************************
 * Abstract
 *
 * This specification defines a symmetric stream cipher algorithm using
 * the authenticated encryption with additional data (AEAD) approach. This
 * algorithm can be used to encrypt and decrypt arbitrary user data.
 * The cipher algorithm uses the KMAC algorithm to generate a key stream
 * which is XORed with either the plaintext (encryption) or ciphertext
 * (decryption) data. The KMAC is initialized with the user-provided key
 * and the user-provided IV. In addition, a second KMAC instance is
 * initialized which calculates a keyed-message digest of the ciphertext to
 * create a message authentication tag. This message authentication tag is used
 * during decryption to verify the integrity of the ciphertext.
 *
 * 1. Introduction
 *
 * This specification defines a symmetric stream cipher algorithm using
 * the authenticated encryption with additional data (AEAD) approach. This
 * algorithm can be used to encrypt and decrypt arbitrary user data.
 *
 * The base of the algorithm is the generation of a key stream using KMAC
 * which is XORed with the plaintext for the encryption operation, or with
 * the ciphertext for the decryption operation.
 *
 * The algorithm also applies an Encrypt-Then-MAC by calculating a message
 * authentication tag using KMAC over the ciphertext. During decryption, this
 * calculated message authentication tag is compared with the message
 * authentication tag obtained during the encryption operation. If both values
 * show a mismatch, the authentication fails and the decryption operation is
 * terminated. Only when both message authentication tags are identical
 * the decryption operation completes successfully and returns the decrypted
 * message.
 *
 * The key along with the IV are used to initialize the KMAC algorithm
 * for generating the key stream. The first output block from the KMAC
 * is used to initialize the authenticating KMAC instance used to
 * calculate the message authentication tag.
 *
 * The size of the key is defined to be 256 bits when using KMAC-256.
 * The size of the IV can be selected by the caller. The algorithm supports
 * any IV size, including having no IV.
 *
 * As part of the authentication, the algorithm allows the addition of
 * additional authenticated data (AAD) of arbitrary size. This AAD is inserted
 * into the authentication KMAC instance during calculating the message
 * authentication tag.
 *
 * 2. KMAC-based AEAD Cipher Algorithm
 *
 * 2.1 Notation
 *
 * The KMAC-hash denotes the KMACXOF256 function [SP800-185]. The KMAC-hash
 * has 4 arguments: the key K, the main input bit string X, the requested output
 * length L in bits, and an optional customization bit string S.
 *
 * The inputs to the KMAC-hash function are specified with references to these
 * parameters.
 *
 * 2.3. Common Processing of Data
 *
 * KMAC-Crypt(key, IV, input data) -> output data, auth key
 *
 * Inputs:
 *   key: The caller-provided key of size 256 bits
 *
 *   IV: The caller-provided initialization vector. The IV can have any size
 *	 including an empty bit-string. See chapter 3 for a discussion of the
 *	 IV, however.
 *
 *   input data: The caller-provided input data - in case of encryption, the
 *               caller provides the plaintext data, in case of decryption,
 *               caller provides the ciphertext data.
 *
 * Outputs:
 *   output data: The resulting data - in case of encryption, the ciphertext
 *                is produced, in case of decryption, the plaintext is returned.
 *   auth key: The key that is used for the KMAC operation calculating the
 *             message authentication tag.
 *
 * The common processing of data is performed as follows:
 *
 * input length = size of input data in bits
 * KS = KMAC(K = key,
 *           X = "",
 *           L = 256 bits + input length,
 *           S = IV)
 * auth key = 256 left-most bits of KS
 * KS crypt = all right-most bits of KS starting with the 256th bit
 * output data = input data XOR KS crypt
 *
 * 2.3 Calculating of Message Authentication Tag
 *
 * KMAC-Auth(auth key, AAD, ciphertext, taglen) -> tag
 *
 * Inputs:
 *   auth key: The key that is used for the KMAC operation calculating the
 *             message authentication tag.
 *
 *   AAD: The caller-provided additional authenticated data. The AAD can have
 *	  any size including an empty bit-string.
 *
 *   ciphertext: The ciphertext obtained from the encryption operation or
 *               provided to the decryption operation.
 *
 *   taglen: The length of the message authentication tag to be generated.
 *
 * The calculation of the message authentication tag is performed as follows:
 *
 * tag = KMAC(K = auth key,
 *            X = ciphertext || AAD,
 *            L = taglen,
 *            S = "")
 *
 * 2.4. Encryption Operation
 *
 * KMAC-Encrypt(key, IV, plaintext, AAD, taglen) -> ciphertext, tag
 *
 * Input:
 *   key: The caller-provided key of size 256 bits
 *
 *   IV: The caller-provided initialization vector. The IV can have any size
 *	 including an empty bit-string.
 *
 *   plaintext: The caller-provided plaintext data.
 *
 *   AAD: The caller-provided additional authenticated data.
 *
 *   taglen: The length of the message authentication tag to be generated.
 *
 * Output
 *   ciphertext: The ciphertext that can exchanged with the recipient over
 *               insecure channels.
 *
 *   tag: The message authentication tag that can be exchanged with the
 *        recipient over insecure channels.
 *
 * The encryption operation is performed as follows:
 *
 * ciphertext, auth key = KMAC-Crypt(key, IV, plaintext)
 * tag = KMAC-Auth(auth key, AAD, ciphertext, taglen)
 *
 * 2.5 Decryption Operation
 *
 * KMAC-Decrypt(key, IV, ciphertext, AAD, tag) -> plaintext, authentication result
 *
 * Input:
 *   key: The caller-provided key of size 256 bits
 *
 *   IV: The caller-provided initialization vector. The IV can have any size
 *	 including an empty bit-string.
 *
 *   ciphertext: The ciphertext that was received from the send over
 *               insecure channels.
 *
 *   AAD: The caller-provided additional authenticated data.
 *
 *   tag: The message authentication tag that was received from the send over
 *        insecure channels.
 *
 * Output
 *   plaintext: The plaintext of the data.
 *
 *   authentication result: A boolean indicator specifying whether the
 *			    authentication was successful. If it was
 *			    unsuccessful the caller shall reject the ciphertext.
 *
 * The decryption operation is performed as follows:
 *
 * plaintext, auth key = KMAC-Crypt(key, IV, ciphertext)
 * taglen = size of tag
 * new_tag = KMAC-Auth(auth key, AAD, ciphertext, taglen)
 * if (new_tag == tag)
 *   authentication result = success
 * else
 *   authentication result = failure
 *
 * If the authentication result indicates a failure, the result of the
 * decryption operation SHALL be discarded.
 *
 * 3. Cryptographic Aspects
 *
 * The KMAC AEAD algorithm is a stream cipher which uses the XOR-construction
 * method to perform encryption and decryption. This method is susceptible to
 * attacks when the key stream is identical between different encryption
 * operations. This this case, the key stream can be trivially removed and
 * thus a decryption of the data is possible as follows:
 *
 * ciphertext 1 = plaintext 1 XOR KS
 *
 * ciphertext 2 = plaintext 2 XOR KS
 *
 * ciphertext 1 XOR ciphertext 2 =
 * 	(plaintext 1 XOR KS) XOR (plaintext 2 XOR KS) =
 *	plaintext 1 XOR plaintext 2
 *
 * Thus, the security of the KMAC algorithm is based on the property that
 * the key stream KS is unique for different encryption operations. The key
 * stream is derived from the key and the IV using KMAC. In common use cases,
 * the key may not be able to be modified. Yet, the IV can be modified. Common
 * protocols allow the generation of a new IV during encryption and transmit
 * the IV to the decryptor. Thus, the IV can be used as a diversifier to for
 * the different encryption operations to obtain a different key stream.
 *
 * As the KMAC algorithm's IV size is unspecified in size, the KMAC
 * algorithm can handle any size that may be pre-defined by the use case or
 * protocol consuming the KMAC AEAD algorithm.
 *
 * Considering the avalanche effect of the underlying KECCAK algorithm, even
 * a small IV may result in a completely different keystream rendering the
 * aforementioned attack impossible.
 *
 * The IV is not required to be a confidentially-protected value. It can be
 * communicated in plaintext to the decryptor. This is due to the fact that
 * the IV is used together with the key to generate the key stream using KMAC.
 * An attacker is not able to construct either the key or the key stream by
 * only possessing the IV. Furthermore, the key is defined to possess a
 * cryptographic meaningful entropy (see section 2.3) which implies that the
 * IV does not need to deliver additional entropy to ensure the strength of
 * the KMAC AEAD algorithm.
 *
 * It is permissible that the IV is generated either by a random number
 * generator or using a deterministic construction method. The only requirement
 * is that the probability in generating a key / IV collision is insignificantly
 * low. This implies that considering the IV is only a diversifier for the
 * key stream, and the fact that the IV is not required to be private, the
 * random number generator is not required to possess a cryptographic meaningful
 * strength.
 *
 * 4. Normative References
 *
 * [SP800-185] John Kelsey, Shu-jen Chang, Ray Perlne, NIST Special Publication
 *             800-185 SHA-3 Derived Functions: KMAC, CSHAKE, TupleHash and
 *             ParallelHash, December 2016
 ******************************************************************************/

#include "build_bug_on.h"
#include "ext_headers.h"
#include "lc_kmac_crypt.h"
#include "math_helper.h"
#include "memcmp_secure.h"
#include "visibility.h"
#include "xor.h"

#define LC_KC_AUTHENTICATION_KEY_SIZE	(256 >> 3)

static int lc_kc_setkey(void *state,
			const uint8_t *key, size_t keylen,
			const uint8_t *iv, size_t ivlen)
{
	struct lc_kc_cryptor *kc = state;
	struct lc_kmac_ctx *kmac;
	struct lc_kmac_ctx *auth_ctx;

	kmac = &kc->kmac;
	auth_ctx = &kc->auth_ctx;

	/*
	 * The keystream block size must be a multiple of the cSHAKE256 block
	 * size, as otherwise the multiple lc_kmac_final calls will not return
	 * the same data as one lc_kmac_final call, because the Keccack
	 * operation to generate a new internal state is invoked at a different
	 * time.
	 */
	BUILD_BUG_ON(LC_SHA3_256_SIZE_BLOCK % LC_KC_KEYSTREAM_BLOCK);
	BUILD_BUG_ON(LC_KC_AUTHENTICATION_KEY_SIZE > LC_KC_KEYSTREAM_BLOCK);

	lc_kmac_init(kmac, key, keylen, iv, ivlen);

	/*
	 * Generate key for KMAC authentication - we simply use two different
	 * keys for the KMAC keystream generator and the KMAC authenticator.
	 *
	 * After the lc_kmac_final_xof we have to call lc_kmac_final_xof_more
	 * for getting new KMAC data.
	 */
	lc_kmac_final_xof(kmac, kc->keystream, LC_KC_KEYSTREAM_BLOCK);
	lc_kmac_init(auth_ctx, kc->keystream, LC_KC_AUTHENTICATION_KEY_SIZE,
		     NULL, 0);

	/* Set the pointer to the start of the keystream */
	kc->keystream_ptr = LC_KC_AUTHENTICATION_KEY_SIZE;

	return 0;
}

static void
lc_kc_crypt(void *state, const uint8_t *in, uint8_t *out, size_t len)
{
	struct lc_kc_cryptor *kc = state;
	struct lc_kmac_ctx *kmac;

	kmac = &kc->kmac;

	while (len) {
		size_t todo = min_t(size_t, len, LC_KC_KEYSTREAM_BLOCK);

		/* Generate a new keystream block */
		if (kc->keystream_ptr >= LC_KC_KEYSTREAM_BLOCK) {
			lc_kmac_final_xof(kmac, kc->keystream,
					  LC_KC_KEYSTREAM_BLOCK);

			kc->keystream_ptr = 0;
		}

		todo = min_t(size_t, todo,
			     LC_KC_KEYSTREAM_BLOCK - kc->keystream_ptr);

		if (in != out)
			memcpy(out, in, todo);

		/* Perform the encryption operation */
		xor_64(out, kc->keystream + kc->keystream_ptr, todo);

		len -= todo;
		in += todo;
		out += todo;
		kc->keystream_ptr += todo;
	}
}

static void lc_kc_encrypt_tag(void *state,
			      const uint8_t *aad, size_t aadlen,
			      uint8_t *tag, size_t taglen)
{
	struct lc_kc_cryptor *kc = state;
	struct lc_kmac_ctx *auth_ctx;

	auth_ctx = &kc->auth_ctx;

	/* Add the AAD data into the KMAC context */
	lc_kmac_update(auth_ctx, aad, aadlen);

	/* Generate authentication tag */
	lc_kmac_final_xof(auth_ctx, tag, taglen);
}

static int
lc_kc_decrypt_authenticate(void *state,
			   const uint8_t *aad, size_t aadlen,
			   const uint8_t *tag, size_t taglen)
{
	struct lc_kc_cryptor *kc = state;
	uint8_t calctag[128] __attribute__((aligned(sizeof(uint64_t))));
	uint8_t *calctag_p = calctag;
	int ret;

	if (taglen > sizeof(calctag)) {
		ret = posix_memalign((void *)&calctag_p, sizeof(uint64_t),
				     taglen);
		if (ret)
			return -ret;
	}

	/*
	 * Calculate the authentication tag for the processed. We do not need
	 * to check the return code as we use the maximum tag size.
	 */
	lc_kc_encrypt_tag(kc, aad, aadlen, calctag_p, taglen);

	ret = (memcmp_secure(calctag_p, taglen, tag, taglen) ? -EBADMSG : 0);
	memset_secure(calctag_p, 0, taglen);
	if (taglen > sizeof(calctag))
		free(calctag_p);

	return ret;
}

static void
lc_kc_encrypt(void *state,
	      const uint8_t *plaintext, uint8_t *ciphertext, size_t datalen)
{
	struct lc_kc_cryptor *kc = state;
	struct lc_kmac_ctx *auth_ctx;

	auth_ctx = &kc->auth_ctx;

	lc_kc_crypt(kc, plaintext, ciphertext, datalen);

	/*
	 * Calculate the authentication MAC over the ciphertext
	 * Perform an Encrypt-Then-MAC operation.
	 */
	lc_kmac_update(auth_ctx, ciphertext, datalen);
}

static void
lc_kc_decrypt(void *state,
	      const uint8_t *ciphertext, uint8_t *plaintext, size_t datalen)
{
	struct lc_kc_cryptor *kc = state;
	struct lc_kmac_ctx *auth_ctx;

	auth_ctx = &kc->auth_ctx;

	/*
	 * Calculate the authentication tag over the ciphertext
	 * Perform the reverse of an Encrypt-Then-MAC operation.
	 */
	lc_kmac_update(auth_ctx, ciphertext, datalen);
	lc_kc_crypt(kc, ciphertext, plaintext, datalen);
}

static void
lc_kc_encrypt_oneshot(void *state,
		      const uint8_t *plaintext, uint8_t *ciphertext,
		      size_t datalen,
		      const uint8_t *aad, size_t aadlen,
		      uint8_t *tag, size_t taglen)
{
	struct lc_kc_cryptor *cc = state;

	/* Confidentiality protection: Encrypt data */
	lc_kc_encrypt(cc, plaintext, ciphertext, datalen);

	/* Integrity protection: CSHAKE data */
	lc_kc_encrypt_tag(cc, aad, aadlen, tag, taglen);
}

static int
lc_kc_decrypt_oneshot(void *state,
		      const uint8_t *ciphertext, uint8_t *plaintext,
		      size_t datalen,
		      const uint8_t *aad, size_t aadlen,
		      const uint8_t *tag, size_t taglen)
{
	struct lc_kc_cryptor *cc = state;

	/*
	 * To ensure constant time between passing and failing decryption,
	 * this code first performs the decryption. The decryption results
	 * will need to be discarded if there is an authentication error. Yet,
	 * in case of an authentication error, an attacker cannot deduct
	 * that there is such an error from the timing analysis of this
	 * function.
	 */
	/* Confidentiality protection: decrypt data */
	lc_kc_decrypt(cc, ciphertext, plaintext, datalen);

	/* Integrity protection: verify MAC of data */
	return lc_kc_decrypt_authenticate(cc, aad, aadlen, tag, taglen);
}

static inline void lc_kc_zero(void *state)
{
	struct lc_kc_cryptor *kc = state;
	struct lc_kmac_ctx *kmac = &kc->kmac;
	struct lc_hash_ctx *hash_ctx = &kmac->hash_ctx;
	const struct lc_hash *hash = hash_ctx->hash;

	if (!kc)
		return;

	memset_secure((uint8_t *)kc + sizeof(struct lc_kc_cryptor), 0,
		      LC_KC_STATE_SIZE(hash));
}

LC_INTERFACE_FUNCTION(
int, lc_kc_alloc, const struct lc_hash *hash, struct lc_aead_ctx **ctx)
{
	struct lc_aead_ctx *tmp = NULL;
	int ret;

	if (!ctx)
		return -EINVAL;

	ret = posix_memalign((void *)&tmp, sizeof(uint64_t),
			     LC_KC_CTX_SIZE(hash));
	if (ret)
		return -ret;

	LC_KC_SET_CTX(tmp, hash);

	*ctx = tmp;

	return 0;
}

struct lc_aead _lc_kmac_aead = {
	.setkey		= lc_kc_setkey,
	.encrypt	= lc_kc_encrypt_oneshot,
	.enc_update	= lc_kc_encrypt,
	.enc_final	= lc_kc_encrypt_tag,
	.decrypt	= lc_kc_decrypt_oneshot,
	.dec_update	= lc_kc_decrypt,
	.dec_final	= lc_kc_decrypt_authenticate,
	.zero		= lc_kc_zero
};
LC_INTERFACE_SYMBOL(const struct lc_aead *, lc_kmac_aead) = &_lc_kmac_aead;
