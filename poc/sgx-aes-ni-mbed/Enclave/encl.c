#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <smmintrin.h>
#include <wmmintrin.h>
#include <immintrin.h>
#include <immintrin.h>
#include "sgx_tcrypto.h"
#include "sgx_trts.h"
#include "encl_t.h" // for ocall

#define BUFLEN 2048

typedef unsigned int u32;

// The in-enclave secret key
static uint8_t key[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
static __m128i key_schedule[20]; // the expanded key (local for thread safety)


#define AES_128_key_exp(k, rcon) aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon))

#define DO_ENC_BLOCK(m, k)                  \
	do                                      \
	{                                       \
		m = _mm_xor_si128(m, k[0]);         \
		m = _mm_aesenc_si128(m, k[1]);      \
		m = _mm_aesenc_si128(m, k[2]);      \
		m = _mm_aesenc_si128(m, k[3]);      \
		m = _mm_aesenc_si128(m, k[4]);      \
		m = _mm_aesenc_si128(m, k[5]);      \
		m = _mm_aesenc_si128(m, k[6]);      \
		m = _mm_aesenc_si128(m, k[7]);      \
		m = _mm_aesenc_si128(m, k[8]);      \
		m = _mm_aesenc_si128(m, k[9]);      \
		m = _mm_aesenclast_si128(m, k[10]); \
	} while (0)

static __m128i aes_128_key_expansion(__m128i key, __m128i keygened)
{
	keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3, 3, 3, 3));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	return _mm_xor_si128(key, keygened);
}

static void aes128_load_key(uint8_t *enc_key, __m128i *key_schedule)
{
	key_schedule[0] = _mm_loadu_si128((const __m128i *)enc_key);
	key_schedule[1] = AES_128_key_exp(key_schedule[0], 0x01);
	key_schedule[2] = AES_128_key_exp(key_schedule[1], 0x02);
	key_schedule[3] = AES_128_key_exp(key_schedule[2], 0x04);
	key_schedule[4] = AES_128_key_exp(key_schedule[3], 0x08);
	key_schedule[5] = AES_128_key_exp(key_schedule[4], 0x10);
	key_schedule[6] = AES_128_key_exp(key_schedule[5], 0x20);
	key_schedule[7] = AES_128_key_exp(key_schedule[6], 0x40);
	key_schedule[8] = AES_128_key_exp(key_schedule[7], 0x80);
	key_schedule[9] = AES_128_key_exp(key_schedule[8], 0x1B);
	key_schedule[10] = AES_128_key_exp(key_schedule[9], 0x36);

	// generate decryption keys in reverse order.
	// k[10] is shared by last encryption and first decryption rounds
	// k[0] is shared by first encryption round and last decryption round (and is the original user key)
	// For some implementation reasons, decryption key schedule is NOT the encryption key schedule in reverse order
	key_schedule[11] = _mm_aesimc_si128(key_schedule[9]);
	key_schedule[12] = _mm_aesimc_si128(key_schedule[8]);
	key_schedule[13] = _mm_aesimc_si128(key_schedule[7]);
	key_schedule[14] = _mm_aesimc_si128(key_schedule[6]);
	key_schedule[15] = _mm_aesimc_si128(key_schedule[5]);
	key_schedule[16] = _mm_aesimc_si128(key_schedule[4]);
	key_schedule[17] = _mm_aesimc_si128(key_schedule[3]);
	key_schedule[18] = _mm_aesimc_si128(key_schedule[2]);
	key_schedule[19] = _mm_aesimc_si128(key_schedule[1]);
}

/*
static __m128i aes128_enc(__m128i plaintext)
{
	uint8_t pt[16] = {0};
	uint8_t ct[16] = {0};
	__m128i m = plaintext;

	DO_ENC_BLOCK(m, key_schedule);

	_mm_storeu_si128((__m128i *)ct, m);

	return m;
}
*/

static inline u32 rol(u32 x, int n)
{
	__asm__(
		"roll %%cl,%0"
		: "=r"(x)
		: "0"(x), "c"(n));
	return x;
}

// -------------------------------------------------------------------------
// Embedded tls code 

#define AESDEC      ".byte 0x66,0x0F,0x38,0xDE,"
#define AESDECLAST  ".byte 0x66,0x0F,0x38,0xDF,"
#define AESENC      ".byte 0x66,0x0F,0x38,0xDC,"
#define AESENCLAST  ".byte 0x66,0x0F,0x38,0xDD,"
#define AESIMC      ".byte 0x66,0x0F,0x38,0xDB,"
#define AESKEYGENA  ".byte 0x66,0x0F,0x3A,0xDF,"
#define PCLMULQDQ   ".byte 0x66,0x0F,0x3A,0x44,"

#define xmm0_xmm0   "0xC0"
#define xmm0_xmm1   "0xC8"
#define xmm0_xmm2   "0xD0"
#define xmm0_xmm3   "0xD8"
#define xmm0_xmm4   "0xE0"
#define xmm1_xmm0   "0xC1"
#define xmm1_xmm2   "0xD1"

//static void aesni_setkey_enc_128( unsigned char *rk,
//unsigned char *key )
static void aesni_setkey_enc_128( unsigned char *rk,
							      unsigned char *key)

{
    asm( "movdqu (%1), %%xmm0               \n\t" // copy the original key
         "movdqu %%xmm0, (%0)               \n\t" // as round key 0
         "jmp 2f                            \n\t" // skip auxiliary routine

         /*
          * Finish generating the next round key.
          *
          * On entry xmm0 is r3:r2:r1:r0 and xmm1 is X:stuff:stuff:stuff
          * with X = rot( sub( r3 ) ) ^ RCON.
          *
          * On exit, xmm0 is r7:r6:r5:r4
          * with r4 = X + r0, r5 = r4 + r1, r6 = r5 + r2, r7 = r6 + r3
          * and those are written to the round key buffer.
          */
         "1:                                \n\t"
         "pshufd $0xff, %%xmm1, %%xmm1      \n\t" // X:X:X:X
         "pxor %%xmm0, %%xmm1               \n\t" // X+r3:X+r2:X+r1:r4
         "pslldq $4, %%xmm0                 \n\t" // r2:r1:r0:0
         "pxor %%xmm0, %%xmm1               \n\t" // X+r3+r2:X+r2+r1:r5:r4
         "pslldq $4, %%xmm0                 \n\t" // etc
         "pxor %%xmm0, %%xmm1               \n\t"
         "pslldq $4, %%xmm0                 \n\t"
         "pxor %%xmm1, %%xmm0               \n\t" // update xmm0 for next time!
         "add $16, %0                       \n\t" // point to next round key
         "movdqu %%xmm0, (%0)               \n\t" // write it
         "ret                               \n\t"

         /* Main "loop" */
         "2:                                \n\t"
         AESKEYGENA xmm0_xmm1 ",0x01        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x02        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x04        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x08        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x10        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x20        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x40        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x80        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x1B        \n\tcall 1b \n\t"
         AESKEYGENA xmm0_xmm1 ",0x36        \n\tcall 1b \n\t"
         :
         : "r" (rk), "r" (key)
         : "memory", "cc", "0" );
}

/* 

    int nr;                     The number of rounds.
    uint32_t *rk;               AES round keys.


	 int mode     MBEDTLS_AES_ENCRYPT(1) or MBEDTLS_AES_DECRYPT(0)

*/
int mbedtls_aesni_crypt_ecb( int nr, 
					 unsigned char  *rk,
                     int mode,
                     unsigned char *input,
                     unsigned char *output )
{
    asm( "movdqu    (%3), %%xmm0    \n\t" // load input
         "movdqu    (%1), %%xmm1    \n\t" // load round key 0
         "pxor      %%xmm1, %%xmm0  \n\t" // round 0
         "add       $16, %1         \n\t" // point to next round key
         "subl      $1, %0          \n\t" // normal rounds = nr - 1
         "test      %2, %2          \n\t" // mode?
         "jz        2f              \n\t" // 0 = decrypt

         "1:                        \n\t" // encryption loop
         "movdqu    (%1), %%xmm1    \n\t" // load round key
         AESENC     xmm1_xmm0      "\n\t" // do round
         "add       $16, %1         \n\t" // point to next round key
         "subl      $1, %0          \n\t" // loop
         "jnz       1b              \n\t"
         "movdqu    (%1), %%xmm1    \n\t" // load round key
         AESENCLAST xmm1_xmm0      "\n\t" // last round
         "jmp       3f              \n\t"

         "2:                        \n\t" // decryption loop
         "movdqu    (%1), %%xmm1    \n\t"
         AESDEC     xmm1_xmm0      "\n\t" // do round
         "add       $16, %1         \n\t"
         "subl      $1, %0          \n\t"
         "jnz       2b              \n\t"
         "movdqu    (%1), %%xmm1    \n\t" // load round key
         AESDECLAST xmm1_xmm0      "\n\t" // last round

         "3:                        \n\t"
         "movdqu    %%xmm0, (%4)    \n\t" // export output
         :
         : "r" (nr), "r" (rk), "r" (mode), "r" (input), "r" (output)
         : "memory", "cc", "xmm0", "xmm1" );


    return( 0 );
}


// -------------------------------------------------------------------------
// End of embedded tls code 



int vec_equal(__m256i a, __m256i b)
{
	__m256i pcmp = _mm256_cmpeq_epi32(a, b); // epi8 is fine too
	unsigned bitmask = _mm256_movemask_epi8(pcmp);
	return (bitmask == 0xffffffffU);
}

int vec_equal_128(__m128i a, __m128i b)
{
	__m128i pcmp = _mm_cmpeq_epi32(a, b); // epi8 is fine too
	unsigned bitmask = _mm_movemask_epi8(pcmp);
	return (bitmask == 0xffffU);
}

volatile uint64_t not_used = 0xdead;
volatile __m256i var;

static void eprintf(const char *fmt, ...)
{
	char buf[1000] = {'\0'};
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, 1000, fmt, ap);
	va_end(ap);
	ocall_print_string(buf);
}

static void print_m128i(const char *s, __m128i x, const char *f)
{
	uint8_t xs[16] = {0};
	char final_string[33];
	char buf[3];

	_mm_storeu_si128((__m128i *)xs, x);

	for (int i = 0; i < 16; i++)
	{
		snprintf(buf, 3, "%02X", xs[i]);
		final_string[2 * i] = buf[0];
		final_string[2 * i + 1] = buf[1];
	}
	final_string[32] = 0;

	eprintf("%s%s%s", s, final_string, f);
}

static void aesdec_print_rounds(__m128i m, __m128i n)
{
	m = _mm_xor_si128(m, key_schedule[10 + 0]);
	n = _mm_xor_si128(n, key_schedule[10 + 0]);
	__m128i x = _mm_xor_si128(m,n);
	print_m128i("dec: ", m, "");
	print_m128i("", n, "");
	print_m128i("10: ", x, ".\n");

	for (int i=1; i < 10; i++)
	{
		m = _mm_aesdec_si128(m, key_schedule[10 + i]);
		n = _mm_aesdec_si128(n, key_schedule[10 + i]);
		x = _mm_xor_si128(m,n);
		print_m128i("dec: ", m, "");
		print_m128i("", n,"");
		char i_str[5]={0};
		snprintf(i_str,5 , "%02i: ", 10-i);
		print_m128i(i_str, x, ".\n");
	}

	m = _mm_aesdeclast_si128(m, key_schedule[0]);
	n = _mm_aesdeclast_si128(n, key_schedule[0]);
	x = _mm_xor_si128(m,n);
	print_m128i("dec: ", m, "");
	print_m128i("", n, "");
	print_m128i("00: ", x, ".\n");
	
}


uint64_t fault_it(int iterations)
{
	int i = 0;

	__m128i correct_value;

	uint32_t b[16];
	uint32_t c[16];

	for (size_t j = 0; j < 4; j++)
	{
		unsigned randVal;
		_rdrand32_step(&randVal);
		b[j] = randVal;
		c[j] = 0;
	}

	
	__m128i plaintext = _mm_loadu_si128((__m128i *)b);

	__m128i result1;
	__m128i result2;
	unsigned char mbed_result1[16], mbed_result2[16], testtext[16];
    unsigned char mbed_key_schedule[320];  // 128 bits * 20 

	//result1 = _mm_loadu_si128((__m128i *)b);
	aes128_load_key(key, key_schedule);

	aesni_setkey_enc_128( mbed_key_schedule , key);


	do
	{
		i++;

		for (size_t j = 0; j < 4; j++)
		{
			unsigned randVal;
			_rdrand32_step(&randVal);
			b[j] = randVal;

		}

		plaintext = _mm_loadu_si128((__m128i *)b);

		_mm_storeu_si128((__m128i *)testtext, plaintext);

	/*int mbedtls_aesni_crypt_ecb( int nr, 
					 uint32_t *rk,
                     int mode,
                     unsigned char input[16],
                     unsigned char output[16] )
*/
		mbedtls_aesni_crypt_ecb(10 , mbed_key_schedule, 1, testtext , mbed_result1);
		//mbedtls_aesni_crypt_ecb(9 ,(uint32_t *) mbed_key_schedule, 1, testtext , mbed_result1);
		//mbedtls_aesni_crypt_ecb(9 ,(uint32_t *) mbed_key_schedule, 1, testtext , mbed_result2);
		mbedtls_aesni_crypt_ecb(10 , mbed_key_schedule, 1, testtext , mbed_result2);

		result1 = _mm_loadu_si128((__m128i *)mbed_result1);
		result2 = _mm_loadu_si128((__m128i *)mbed_result2);
		

	} while (vec_equal_128(result1, result2) && i < iterations);



	if (!vec_equal_128(result1, result2))
	{
		print_m128i("plaintext: ", plaintext, "\n");
		print_m128i("ciphertext1: ", result1, "\n");
		print_m128i("ciphertext2: ", result2, "\n");
		eprintf("Iteration: %d \n", i);
		eprintf(">>>>> INCORRECT ENCRYPTION <<<< \n");
		aesdec_print_rounds(result1, result2);
	}

	return 1;
}

void enclave_calculation(int iterations)
{
	uint64_t return_val = fault_it(iterations);
}
