#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ptr;
use leancrypto;

fn lc_rust_kmac_aead_one(data: &[u8], aad: &[u8], key: &[u8], exp_ct: &[u8],
			 exp_tag: &[u8])
{
	/* Define the ctx pointer */
	let mut ctx: *mut lc_aead_ctx = ptr::null_mut();
	let mut act: [u8; 64] = [0; 64];
	let mut act_dec: [u8; 64] = [0; 64];
	let mut tag: [u8; 64] = [0; 64];

	unsafe {
		/* Allocate the hash context */
		let result = lc_kc_alloc(lc_cshake256, &mut ctx);
		if result != 0 {
			println!("allocation error: {result}");
		}


		if lc_aead_setkey(ctx, key.as_ptr(), key.len(), ptr::null(),
				  0) != 0 {
			println!("KMAC AEAD encrypt setkey failed");
		}

		lc_aead_encrypt(ctx, data.as_ptr(), act.as_mut_ptr(),
				data.len(), aad.as_ptr(), aad.len(),
				tag.as_mut_ptr(), tag.len());

		lc_aead_zero(ctx);

		assert_eq!(&act[..], &exp_ct[..]);
		assert_eq!(&tag[..], &exp_tag[..]);

		if lc_aead_setkey(ctx, key.as_ptr(), key.len(), ptr::null(),
				  0) != 0 {
			println!("KMAC AEAD decrypt setkey failed");
		}

		let ret = lc_aead_decrypt(ctx, act.as_ptr(),
					  act_dec.as_mut_ptr(), act.len(),
					  aad.as_ptr(), aad.len(), tag.as_ptr(),
					  tag.len());

		lc_aead_zero_free(ctx);

		assert_eq!(ret, 0);
		assert_eq!(&act_dec[..], &data[..]);
	}
}

#[test]
fn lc_rust_kmac_aead()
{
	const data: [u8; 64] = [
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
		0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
		0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
		0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
		0x3c, 0x3d, 0x3e, 0x3f,
	];
	const exp_ct: [u8; 64] = [
		0x32, 0x26, 0x28, 0x44, 0xf4, 0x08, 0x27, 0x4a, 0x75, 0xf9,
		0x84, 0xbb, 0x4f, 0x31, 0x67, 0x81, 0x38, 0xc6, 0x41, 0xe5,
		0x04, 0x26, 0x01, 0xda, 0xdb, 0x6c, 0x0b, 0xe4, 0x9c, 0xc1,
		0x63, 0x46, 0x1c, 0xf2, 0x31, 0x30, 0xb8, 0x27, 0xf2, 0x53,
		0x39, 0x49, 0x99, 0x98, 0x61, 0x9b, 0x70, 0xf0, 0xfe, 0x1e,
		0x7a, 0x57, 0x5c, 0x1f, 0xaf, 0xa1, 0x3a, 0x6b, 0x18, 0x1a,
		0x44, 0x99, 0xda, 0x28
	];
	const exp_tag: [u8; 64] = [
		0x28, 0x43, 0x43, 0xc2, 0x40, 0x1f, 0x45, 0x09, 0x41, 0xd7,
		0x5c, 0xdd, 0x7f, 0xc5, 0x96, 0x32, 0x8b, 0xd9, 0x5a, 0xe3,
		0x72, 0xe2, 0x73, 0x6a, 0x83, 0xd1, 0x85, 0xa3, 0xc5, 0xab,
		0x83, 0xe0, 0x51, 0x89, 0x98, 0x34, 0xf1, 0x8f, 0x94, 0xcc,
		0x98, 0xa9, 0xe2, 0x79, 0x07, 0xb2, 0x6f, 0xf5, 0x68, 0x4d,
		0x53, 0xaa, 0xfd, 0x28, 0x3e, 0x3d, 0x7e, 0x73, 0xa8, 0xec,
		0xf2, 0xfa, 0x79, 0x31
	];

	lc_rust_kmac_aead_one(&data, &data, &data, &exp_ct, &exp_tag);
}
