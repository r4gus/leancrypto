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

#ifndef DILITHIUM_REJSAMPLE_AVX2_H
#define DILITHIUM_REJSAMPLE_AVX2_H

#include "lc_dilithium.h"
#include "lc_sha3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REJ_UNIFORM_NBLOCKS                                                    \
	((768 + LC_SHAKE_128_SIZE_BLOCK - 1) / LC_SHAKE_128_SIZE_BLOCK)
#define REJ_UNIFORM_BUFLEN (REJ_UNIFORM_NBLOCKS * LC_SHAKE_128_SIZE_BLOCK)

#define REJ_UNIFORM_ETA_NBLOCKS                                                \
	((136 + LC_SHAKE_256_SIZE_BLOCK - 1) / LC_SHAKE_256_SIZE_BLOCK)
#define REJ_UNIFORM_ETA_BUFLEN                                                 \
	(REJ_UNIFORM_ETA_NBLOCKS * LC_SHAKE_256_SIZE_BLOCK)

extern const uint8_t idxlut[256][8];

unsigned int rej_uniform_avx(int32_t *r,
			     const uint8_t buf[REJ_UNIFORM_BUFLEN + 8]);

unsigned int rej_eta_avx(int32_t *r, const uint8_t buf[REJ_UNIFORM_ETA_BUFLEN]);

#ifdef __cplusplus
}
#endif

#endif /* DILITHIUM_REJSAMPLE_AVX2_H */
