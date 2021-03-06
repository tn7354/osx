/*
 * Copyright (c) 1990 Dennis Ferguson.  All rights reserved.
 *
 * Commercial use is permitted only if products which are derived from
 * or include this software are made available for purchase and/or use
 * in Canada.  Otherwise, redistribution and use in source and binary
 * forms are permitted.
 */

/*
 * des_cbc_encrypt.c - an implementation of the DES cipher function in cbc mode
 */
#include "des_int.h"
#include "f_tables.h"

/*
 * des_cbc_encrypt - {en,de}crypt a stream in CBC mode
 */

/*
 * This routine performs DES cipher-block-chaining operation, either
 * encrypting from cleartext to ciphertext, if encrypt != 0 or
 * decrypting from ciphertext to cleartext, if encrypt == 0.
 *
 * The key schedule is passed as an arg, as well as the cleartext or
 * ciphertext.  The cleartext and ciphertext should be in host order.
 *
 * NOTE-- the output is ALWAYS an multiple of 8 bytes long.  If not
 * enough space was provided, your program will get trashed.
 *
 * For encryption, the cleartext string is null padded, at the end, to
 * an integral multiple of eight bytes.
 *
 * For decryption, the ciphertext will be used in integral multiples
 * of 8 bytes, but only the first "length" bytes returned into the
 * cleartext.
 */

const mit_des_cblock mit_des_zeroblock /* = all zero */;

#undef mit_des_cbc_encrypt
int
mit_des_cbc_encrypt(const mit_des_cblock *in, mit_des_cblock *out,
		    unsigned long length, const mit_des_key_schedule schedule,
		    const mit_des_cblock ivec, int enc)
{
    /*
     * Deal with encryption and decryption separately.
     */
    if (enc)
	krb5int_des_cbc_encrypt(in, out, length, schedule, ivec);
    else
	krb5int_des_cbc_decrypt(in, out, length, schedule, ivec);
    return 0;
}

void
krb5int_des_cbc_encrypt(const mit_des_cblock *in,
			mit_des_cblock *out,
			unsigned long length,
			const mit_des_key_schedule schedule,
			const mit_des_cblock ivec)
{
	unsigned DES_INT32 left, right;
	const unsigned DES_INT32 *kp;
	const unsigned char *ip;
	unsigned char *op;

	/*
	 * Get key pointer here.  This won't need to be reinitialized
	 */
	kp = (const unsigned DES_INT32 *)schedule;

	/*
	 * Initialize left and right with the contents of the initial
	 * vector.
	 */
	ip = ivec;
	GET_HALF_BLOCK(left, ip);
	GET_HALF_BLOCK(right, ip);

	/*
	 * Suitably initialized, now work the length down 8 bytes
	 * at a time.
	 */
	ip = *in;
	op = *out;
	while (length > 0) {
		/*
		 * Get more input, xor it in.  If the length is
		 * greater than or equal to 8 this is straight
		 * forward.  Otherwise we have to fart around.
		 */
		if (length >= 8) {
			unsigned DES_INT32 temp;
			GET_HALF_BLOCK(temp, ip);
			left  ^= temp;
			GET_HALF_BLOCK(temp, ip);
			right ^= temp;
			length -= 8;
		} else {
			/*
			 * Oh, shoot.  We need to pad the
			 * end with zeroes.  Work backwards
			 * to do this.
			 */
			ip += (int) length;
			switch(length) {
			case 7:
				right ^= (*(--ip) & FF_UINT32) <<  8;
			case 6:
				right ^= (*(--ip) & FF_UINT32) << 16;
			case 5:
				right ^= (*(--ip) & FF_UINT32) << 24;
			case 4:
				left  ^=  *(--ip) & FF_UINT32;
			case 3:
				left  ^= (*(--ip) & FF_UINT32) <<  8;
			case 2:
				left  ^= (*(--ip) & FF_UINT32) << 16;
			case 1:
				left  ^= (*(--ip) & FF_UINT32) << 24;
				break;
			}
			length = 0;
		}

		/*
		 * Encrypt what we have
		 */
		DES_DO_ENCRYPT(left, right, kp);

		/*
		 * Copy the results out
		 */
		PUT_HALF_BLOCK(left, op);
		PUT_HALF_BLOCK(right, op);
	}
}

void
krb5int_des_cbc_decrypt(const mit_des_cblock *in,
			mit_des_cblock *out,
			unsigned long length,
			const mit_des_key_schedule schedule,
			const mit_des_cblock ivec)
{
	unsigned DES_INT32 left, right;
	const unsigned DES_INT32 *kp;
	const unsigned char *ip;
	unsigned char *op;
	unsigned DES_INT32 ocipherl, ocipherr;
	unsigned DES_INT32 cipherl, cipherr;

	/*
	 * Get key pointer here.  This won't need to be reinitialized
	 */
	kp = (const unsigned DES_INT32 *)schedule;

	/*
	 * Decrypting is harder than encrypting because of
	 * the necessity of remembering a lot more things.
	 * Should think about this a little more...
	 */

	if (length <= 0)
		return;

	/*
	 * Prime the old cipher with ivec.
	 */
	ip = ivec;
	GET_HALF_BLOCK(ocipherl, ip);
	GET_HALF_BLOCK(ocipherr, ip);

	/*
	 * Now do this in earnest until we run out of length.
	 */
	ip = *in;
	op = *out;
	for (;;) {		/* check done inside loop */
		/*
		 * Read a block from the input into left and
		 * right.  Save this cipher block for later.
		 */
		GET_HALF_BLOCK(left, ip);
		GET_HALF_BLOCK(right, ip);
		cipherl = left;
		cipherr = right;

		/*
		 * Decrypt this.
		 */
		DES_DO_DECRYPT(left, right, kp);

		/*
		 * Xor with the old cipher to get plain
		 * text.  Output 8 or less bytes of this.
		 */
		left ^= ocipherl;
		right ^= ocipherr;
		if (length > 8) {
			length -= 8;
			PUT_HALF_BLOCK(left, op);
			PUT_HALF_BLOCK(right, op);
			/*
			 * Save current cipher block here
			 */
			ocipherl = cipherl;
			ocipherr = cipherr;
		} else {
			/*
			 * Trouble here.  Start at end of output,
			 * work backwards.
			 */
			op += (int) length;
			switch(length) {
			case 8:
				*(--op) = (unsigned char) (right & 0xff);
			case 7:
				*(--op) = (unsigned char) ((right >> 8) & 0xff);
			case 6:
				*(--op) = (unsigned char) ((right >> 16) & 0xff);
			case 5:
				*(--op) = (unsigned char) ((right >> 24) & 0xff);
			case 4:
				*(--op) = (unsigned char) (left & 0xff);
			case 3:
				*(--op) = (unsigned char) ((left >> 8) & 0xff);
			case 2:
				*(--op) = (unsigned char) ((left >> 16) & 0xff);
			case 1:
				*(--op) = (unsigned char) ((left >> 24) & 0xff);
				break;
			}
			break;		/* we're done */
		}
	}
}

#ifdef CONFIG_SMALL
void krb5int_des_do_encrypt_2 (unsigned DES_INT32 *left,
			       unsigned DES_INT32 *right,
			       const unsigned DES_INT32 *kp)
{
    DES_DO_ENCRYPT_1 (*left, *right, kp);
}

void krb5int_des_do_decrypt_2 (unsigned DES_INT32 *left,
			       unsigned DES_INT32 *right,
			       const unsigned DES_INT32 *kp)
{
    DES_DO_DECRYPT_1 (*left, *right, kp);
}
#endif
