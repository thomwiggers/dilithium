#ifndef FIPS202_H
#define FIPS202_H

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72

typedef struct {
  uint64_t s[25];
  unsigned int pos;
} keccak_state;

#define shake128_init pqcrystals_fips202_ref_shake128_init
void shake128_init(keccak_state *state);
#define shake128_absorb pqcrystals_fips202_ref_shake128_absorb
void shake128_absorb(keccak_state *state, const uint8_t *in, size_t inlen);
#define shake128_finalize pqcrystals_fips202_ref_shake128_finalize
void shake128_finalize(keccak_state *state);
#define shake128_squeezeblocks pqcrystals_fips202_ref_shake128_squeezeblocks
void shake128_squeezeblocks(uint8_t *out, size_t nblocks, keccak_state *state);
#define shake128_squeeze pqcrystals_fips202_ref_shake128_squeeze
void shake128_squeeze(uint8_t *out, size_t outlen, keccak_state *state);

#define shake256_init pqcrystals_fips202_ref_shake256_init
void shake256_init(keccak_state *state);
#define shake256_absorb pqcrystals_fips202_ref_shake256_absorb
void shake256_absorb(keccak_state *state, const uint8_t *in, size_t inlen);
#define shake256_finalize pqcrystals_fips202_ref_shake256_finalize
void shake256_finalize(keccak_state *state);
#define shake256_squeezeblocks pqcrystals_fips202_ref_shake256_squeezeblocks
void shake256_squeezeblocks(uint8_t *out, size_t nblocks,  keccak_state *state);
#define shake256_squeeze pqcrystals_fips202_ref_shake256_squeeze
void shake256_squeeze(uint8_t *out, size_t outlen, keccak_state *state);

#define shake128 pqcrystals_fips202_ref_shake128
void shake128(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen);
#define shake256 pqcrystals_fips202_ref_shake256
void shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen);
#define sha3_256 pqcrystals_fips202_ref_sha3_256
void sha3_256(uint8_t h[32], const uint8_t *in, size_t inlen);
#define sha3_512 pqcrystals_fips202_ref_sha3_512
void sha3_512(uint8_t h[64], const uint8_t *in, size_t inlen);

#endif
