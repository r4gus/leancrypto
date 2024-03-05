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

#ifndef LC_CHACHA20_PRIVATE_H
#define LC_CHACHA20_PRIVATE_H

#include "ext_headers.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LC_CC20_KEY_SIZE 32
#define LC_CC20_KEY_SIZE_WORDS (LC_CC20_KEY_SIZE / sizeof(uint32_t))

/* State according to RFC 7539 section 2.3 */
struct lc_sym_state {
	uint32_t constants[4];
	union {
		uint32_t u[LC_CC20_KEY_SIZE_WORDS];
		uint8_t b[LC_CC20_KEY_SIZE];
	} key;
	uint32_t counter;
	uint32_t nonce[3];
};

#define LC_CC20_BLOCK_SIZE sizeof(struct lc_sym_state)
#define LC_CC20_BLOCK_SIZE_WORDS (LC_CC20_BLOCK_SIZE / sizeof(uint32_t))

#ifdef __cplusplus
}
#endif

#endif /* LC_CHACHA20_PRIVATE_H */
