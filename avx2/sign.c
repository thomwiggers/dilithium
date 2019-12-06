#include <stdint.h>
#include <string.h>

#include "fips202.h"
#include "packing.h"
#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include "randombytes.h"
#include "sign.h"
#include "symmetric.h"

/*************************************************
* Name:        expand_mat
*
* Description: Implementation of ExpandA. Generates matrix A with uniformly
*              random coefficients a_{i,j} by performing rejection
*              sampling on the output stream of SHAKE128(rho|i|j).
*
* Arguments:   - polyvecl mat[K]: output matrix
*              - const uint8_t rho[]: byte array containing seed rho
**************************************************/
#ifdef USE_AES
void DILITHIUM_expand_mat(polyvecl mat[K], const uint8_t rho[SEEDBYTES]) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < L; ++j)
      DILITHIUM_poly_uniform(&mat[i].vec[j], rho, (i << 8) + j);
}
#elif L == 2 && K == 3
void DILITHIUM_expand_mat(polyvecl mat[3], const uint8_t rho[SEEDBYTES])
{
  poly t0, t1;

  DILITHIUM_poly_uniform_4x(&mat[0].vec[0],
                  &mat[0].vec[1],
                  &mat[1].vec[0],
                  &mat[1].vec[1],
                  rho, 0, 1, 256, 257);
  DILITHIUM_poly_uniform_4x(&mat[2].vec[0],
                  &mat[2].vec[1],
                  &t0,
                  &t1,
                  rho, 512, 513, 0, 0);
}
#elif L == 3 && K == 4
void DILITHIUM_expand_mat(polyvecl mat[4], const uint8_t rho[SEEDBYTES])
{
  DILITHIUM_poly_uniform_4x(&mat[0].vec[0],
                  &mat[0].vec[1],
                  &mat[0].vec[2],
                  &mat[1].vec[0],
                  rho, 0, 1, 2, 256);
  DILITHIUM_poly_uniform_4x(&mat[1].vec[1],
                  &mat[1].vec[2],
                  &mat[2].vec[0],
                  &mat[2].vec[1],
                  rho, 257, 258, 512, 513);
  DILITHIUM_poly_uniform_4x(&mat[2].vec[2],
                  &mat[3].vec[0],
                  &mat[3].vec[1],
                  &mat[3].vec[2],
                  rho, 514, 768, 769, 770);
}
#elif L == 4 && K == 5

void DILITHIUM_expand_mat(polyvecl mat[5], const uint8_t rho[SEEDBYTES]) {
    DILITHIUM_poly_uniform_4x(&mat[0].vec[0],
                              &mat[0].vec[1],
                              &mat[0].vec[2],
                              &mat[0].vec[3],
                              rho, 0, 1, 2, 3);
    DILITHIUM_poly_uniform_4x(&mat[1].vec[0],
                              &mat[1].vec[1],
                              &mat[1].vec[2],
                              &mat[1].vec[3],
                              rho, 256, 257, 258, 259);
    DILITHIUM_poly_uniform_4x(&mat[2].vec[0],
                              &mat[2].vec[1],
                              &mat[2].vec[2],
                              &mat[2].vec[3],
                              rho, 512, 513, 514, 515);
    DILITHIUM_poly_uniform_4x(&mat[3].vec[0],
                              &mat[3].vec[1],
                              &mat[3].vec[2],
                              &mat[3].vec[3],
                              rho, 768, 769, 770, 771);
    DILITHIUM_poly_uniform_4x(&mat[4].vec[0],
                              &mat[4].vec[1],
                              &mat[4].vec[2],
                              &mat[4].vec[3],
                              rho, 1024, 1025, 1026, 1027);
}

#elif L == 5 && K == 6
void DILITHIUM_expand_mat(polyvecl mat[6], const uint8_t rho[SEEDBYTES])
{
  poly t0, t1;

  DILITHIUM_poly_uniform_4x(&mat[0].vec[0],
                  &mat[0].vec[1],
                  &mat[0].vec[2],
                  &mat[0].vec[3],
                  rho, 0, 1, 2, 3);
  DILITHIUM_poly_uniform_4x(&mat[0].vec[4],
                  &mat[1].vec[0],
                  &mat[1].vec[1],
                  &mat[1].vec[2],
                  rho, 4, 256, 257, 258);
  DILITHIUM_poly_uniform_4x(&mat[1].vec[3],
                  &mat[1].vec[4],
                  &mat[2].vec[0],
                  &mat[2].vec[1],
                  rho, 259, 260, 512, 513);
  DILITHIUM_poly_uniform_4x(&mat[2].vec[2],
                  &mat[2].vec[3],
                  &mat[2].vec[4],
                  &mat[3].vec[0],
                  rho, 514, 515, 516, 768);
  DILITHIUM_poly_uniform_4x(&mat[3].vec[1],
                  &mat[3].vec[2],
                  &mat[3].vec[3],
                  &mat[3].vec[4],
                  rho, 769, 770, 771, 772);
  DILITHIUM_poly_uniform_4x(&mat[4].vec[0],
                  &mat[4].vec[1],
                  &mat[4].vec[2],
                  &mat[4].vec[3],
                  rho, 1024, 1025, 1026, 1027);
  DILITHIUM_poly_uniform_4x(&mat[4].vec[4],
                  &mat[5].vec[0],
                  &mat[5].vec[1],
                  &mat[5].vec[2],
                  rho, 1028, 1280, 1281, 1282);
  DILITHIUM_poly_uniform_4x(&mat[5].vec[3],
                  &mat[5].vec[4],
                  &t0,
                  &t1,
                  rho, 1283, 1284, 0, 0);
}
#else
#error
#endif

/*************************************************
* Name:        DILITHIUM_challenge
*
* Description: Implementation of H. Samples polynomial with 60 nonzero
*              coefficients in {-1,1} using the output stream of
*              SHAKE256(mu|w1).
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const uint8_t mu[]: byte array containing mu
*              - const polyveck *w1: pointer to vector w1
**************************************************/
void DILITHIUM_challenge(poly *c,
                         const uint8_t mu[CRHBYTES],
                         const polyveck *w1) {
    unsigned int i, b, pos;
    uint64_t signs;
    uint8_t inbuf[CRHBYTES + K * POLW1_SIZE_PACKED];
    uint8_t outbuf[SHAKE256_RATE];
    shake256ctx state;

    for (i = 0; i < CRHBYTES; ++i)
        inbuf[i] = mu[i];
    for (i = 0; i < K; ++i)
        DILITHIUM_polyw1_pack(inbuf + CRHBYTES + i * POLW1_SIZE_PACKED, &w1->vec[i]);

    shake256_absorb(&state, inbuf, sizeof(inbuf));
    shake256_squeezeblocks(outbuf, 1, &state);

    signs = 0;
    for (i = 0; i < 8; ++i)
        signs |= (uint64_t) outbuf[i] << 8 * i;

    pos = 8;

    for (i = 0; i < N; ++i)
        c->coeffs[i] = 0;

    for (i = 196; i < 256; ++i) {
        do {
            if (pos >= SHAKE256_RATE) {
                shake256_squeezeblocks(outbuf, 1, &state);
                pos = 0;
            }

            b = outbuf[pos++];
        } while (b > i);

        c->coeffs[i] = c->coeffs[b];
        c->coeffs[b] = 1;
        c->coeffs[b] ^= -(signs & 1) & (1 ^ (Q - 1));
        signs >>= 1;
    }
}

/*************************************************
* Name:        DILITHIUM_crypto_sign_keypair
*
* Description: Generates public and private key.
*
* Arguments:   - uint8_t *pk: pointer to output public key (allocated
*                                   array of DILITHIUM_CRYPTO_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (allocated
*                                   array of DILITHIUM_CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int DILITHIUM_crypto_sign_keypair(uint8_t *pk, uint8_t *sk) {
    unsigned int i;
    uint8_t seedbuf[3 * SEEDBYTES];
    uint8_t tr[CRHBYTES];
    const uint8_t *rho, *rhoprime, *key;
    uint16_t nonce = 0;
    polyvecl mat[K];
    polyvecl s1, s1hat;
    polyveck s2, t, t1, t0;

    /* Expand 32 bytes of randomness into rho, rhoprime and key */
    randombytes(seedbuf, 3 * SEEDBYTES);
    rho = seedbuf;
    rhoprime = seedbuf + SEEDBYTES;
    key = seedbuf + 2 * SEEDBYTES;

    /* Expand matrix */
    DILITHIUM_expand_mat(mat, rho);

    /* Sample short vectors s1 and s2 */
#ifdef USE_AES
    for(i = 0; i < L; ++i)
      DILITHIUM_poly_uniform_eta(&s1.vec[i], rhoprime, nonce++);
    for(i = 0; i < K; ++i)
      DILITHIUM_poly_uniform_eta(&s2.vec[i], rhoprime, nonce++);
#elif L == 2 && K == 3
    DILITHIUM_poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s2.vec[0], &s2.vec[1], rhoprime,
                        nonce, nonce + 1, nonce + 2, nonce + 3);
    DILITHIUM_poly_uniform_eta(&s2.vec[2], rhoprime, nonce + 4);
#elif L == 3 && K == 4
    DILITHIUM_poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s2.vec[0], rhoprime,
                        nonce, nonce + 1, nonce + 2, nonce + 3);
    DILITHIUM_poly_uniform_eta_4x(&s2.vec[1], &s2.vec[2], &s2.vec[3], &t.vec[0], rhoprime,
                        nonce + 4, nonce + 5, nonce + 6, 0);
#elif L == 4 && K == 5
    DILITHIUM_poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime,
                                  nonce, nonce + 1, nonce + 2, nonce + 3);
    DILITHIUM_poly_uniform_eta_4x(&s2.vec[0], &s2.vec[1], &s2.vec[2], &s2.vec[3], rhoprime,
                                  nonce + 4, nonce + 5, nonce + 6, nonce + 7);
    DILITHIUM_poly_uniform_eta(&s2.vec[4], rhoprime, nonce + 8);
#elif L == 5 && K == 6
    poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime,
                        nonce, nonce + 1, nonce + 2, nonce + 3);
    poly_uniform_eta_4x(&s1.vec[4], &s2.vec[0], &s2.vec[1], &s2.vec[2], rhoprime,
                        nonce + 4, nonce + 5, nonce + 6, nonce + 7);
    poly_uniform_eta_4x(&s2.vec[3], &s2.vec[4], &s2.vec[5], &t.vec[0], rhoprime,
                        nonce + 8, nonce + 9, nonce + 10, 0);
#else
#error ""
#endif

    /* Matrix-vector multiplication */
    s1hat = s1;
    DILITHIUM_polyvecl_ntt(&s1hat);
    for (i = 0; i < K; ++i) {
        DILITHIUM_polyvecl_pointwise_acc_invmontgomery(&t.vec[i], &mat[i], &s1hat);
        //DILITHIUM_poly_reduce(&t.vec[i]);
        DILITHIUM_poly_invntt_montgomery(&t.vec[i]);
    }

    /* Add error vector s2 */
    DILITHIUM_polyveck_add(&t, &t, &s2);

    /* Extract t1 and write public key */
    DILITHIUM_polyveck_freeze(&t);
    DILITHIUM_polyveck_power2round(&t1, &t0, &t);
    DILITHIUM_pack_pk(pk, rho, &t1);

    /* Compute CRH(rho, t1) and write secret key */
    crh(tr, pk, DILITHIUM_CRYPTO_PUBLICKEYBYTES);
    DILITHIUM_pack_sk(sk, rho, key, tr, &s1, &s2, &t0);

    return 0;
}

/*************************************************
* Name:        DILITHIUM_crypto_sign_signature
*
* Description: Compute signed message.
*
* Arguments:   - uint8_t *sig:  pointer to output signature (DILITHIUM_CRYPTO_BYTES
*                               of len)
*              - size_t *siglen: pointer to output length of signed message
*                               (should be DILITHIUM_CRYPTO_BYTES)
*              - uint8_t *m:    pointer to message to be signed
*              - size_t mlen:   length of message
*              - uint8_t *sk:   pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int DILITHIUM_crypto_sign_signature(
        uint8_t *sig, size_t *siglen,
        const uint8_t *m, size_t mlen,
        const uint8_t *sk) {
    size_t i;
    unsigned int n;
    uint8_t seedbuf[2 * SEEDBYTES + 3 * CRHBYTES];
    uint8_t *rho, *tr, *key, *mu, *rhoprime;
    uint16_t nonce = 0;
    poly c, chat;
    polyvecl mat[K], s1, y, yhat, z;
    polyveck t0, s2, w, w1, w0;
    polyveck h, cs2, ct0;

    rho = seedbuf;
    tr = rho + SEEDBYTES;
    key = tr + CRHBYTES;
    mu = key + SEEDBYTES;
    rhoprime = mu + CRHBYTES;
    DILITHIUM_unpack_sk(rho, key, tr, &s1, &s2, &t0, sk);


    // use incremental hash API instead of copying around buffers
    /* Compute CRH(tr, m) */
    shake256incctx state;
    shake256_inc_init(&state);
    shake256_inc_absorb(&state, tr, CRHBYTES);
    shake256_inc_absorb(&state, m, mlen);
    shake256_inc_finalize(&state);
    shake256_inc_squeeze(mu, CRHBYTES, &state);

    crh(rhoprime, key, SEEDBYTES + CRHBYTES);

    /* Expand matrix and transform vectors */
    DILITHIUM_expand_mat(mat, rho);
    DILITHIUM_polyvecl_ntt(&s1);
    DILITHIUM_polyveck_ntt(&s2);
    DILITHIUM_polyveck_ntt(&t0);

    rej:
    /* Sample intermediate vector y */
#ifdef USE_AES
    for(i = 0; i < L; ++i)
      DILITHIUM_poly_uniform_gamma1m1(&y.vec[i], rhoprime, nonce++);
#elif L == 2
    DILITHIUM_poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &yhat.vec[0], &yhat.vec[1],
                           rhoprime, nonce, nonce + 1, 0, 0);
  nonce += 2;
#elif L == 3
    DILITHIUM_poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &yhat.vec[0],
                           rhoprime, nonce, nonce + 1, nonce + 2, 0);
  nonce += 3;
#elif L == 4
    DILITHIUM_poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3],
                                       rhoprime, nonce, nonce + 1, nonce + 2, nonce + 3);
    nonce += 4;
#elif L == 5
    DILITHIUM_poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3],
                           rhoprime, nonce, nonce + 1, nonce + 2, nonce + 3);
 DILITHIUM_poly_uniform_gamma1m1(&y.vec[4], rhoprime, nonce + 4);
  nonce += 5;
#else
#error
#endif

    /* Matrix-vector multiplication */
    yhat = y;
    DILITHIUM_polyvecl_ntt(&yhat);
    for (i = 0; i < K; ++i) {
        DILITHIUM_polyvecl_pointwise_acc_invmontgomery(&w.vec[i], &mat[i], &yhat);
        DILITHIUM_poly_reduce(&w.vec[i]);
        DILITHIUM_poly_invntt_montgomery(&w.vec[i]);
    }

    /* Decompose w and call the random oracle */
    DILITHIUM_polyveck_csubq(&w);
    DILITHIUM_polyveck_decompose(&w1, &w0, &w);
    DILITHIUM_challenge(&c, mu, &w1);
    chat = c;
    DILITHIUM_poly_ntt(&chat);

    /* Check that subtracting cs2 does not change high bits of w and low bits
     * do not reveal secret information */
    for (i = 0; i < K; ++i) {
        DILITHIUM_poly_pointwise_invmontgomery(&cs2.vec[i], &chat, &s2.vec[i]);
        DILITHIUM_poly_invntt_montgomery(&cs2.vec[i]);
    }
    DILITHIUM_polyveck_sub(&w0, &w0, &cs2);
    DILITHIUM_polyveck_freeze(&w0);
    if (DILITHIUM_polyveck_chknorm(&w0, GAMMA2 - BETA))
        goto rej;

    /* Compute z, reject if it reveals secret */
    for (i = 0; i < L; ++i) {
        DILITHIUM_poly_pointwise_invmontgomery(&z.vec[i], &chat, &s1.vec[i]);
        DILITHIUM_poly_invntt_montgomery(&z.vec[i]);
    }
    DILITHIUM_polyvecl_add(&z, &z, &y);
    DILITHIUM_polyvecl_freeze(&z);
    if (DILITHIUM_polyvecl_chknorm(&z, GAMMA1 - BETA))
        goto rej;

    /* Compute hints for w1 */
    for (i = 0; i < K; ++i) {
        DILITHIUM_poly_pointwise_invmontgomery(&ct0.vec[i], &chat, &t0.vec[i]);
        DILITHIUM_poly_invntt_montgomery(&ct0.vec[i]);
    }

    DILITHIUM_polyveck_csubq(&ct0);
    if (DILITHIUM_polyveck_chknorm(&ct0, GAMMA2))
        goto rej;

    DILITHIUM_polyveck_add(&w0, &w0, &ct0);
    DILITHIUM_polyveck_csubq(&w0);
    n = DILITHIUM_polyveck_make_hint(&h, &w0, &w1);
    if (n > OMEGA)
        goto rej;

    /* Write signature */
    DILITHIUM_pack_sig(sig, &z, &h, &c);
    *siglen = DILITHIUM_CRYPTO_BYTES;
    return 0;
}

/*************************************************
* Name:        DILITHIUM_crypto_sign
*
* Description: Compute signed message.
*
* Arguments:   - uint8_t *sm: pointer to output signed message (allocated
*                             array with DILITHIUM_CRYPTO_BYTES + mlen bytes),
*                             can be equal to m
*              - unsigned long long *smlen: pointer to output length of signed
*                                           message
*              - const uint8_t *m: pointer to message to be signed
*              - unsigned long long mlen: length of message
*              - const uint8_t *sk: pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int DILITHIUM_crypto_sign(
        uint8_t *sm, size_t *smlen,
        const uint8_t *m, size_t mlen,
        const uint8_t *sk) {
    int rc;
    memmove(sm + DILITHIUM_CRYPTO_BYTES, m, mlen);
    rc = DILITHIUM_crypto_sign_signature(sm, smlen, m, mlen, sk);
    *smlen += mlen;
    return rc;
}

/*************************************************
* Name:        DILITHIUM_crypto_sign_verify
*
* Description: Verify signed message.
*
* Arguments:   - uint8_t *sig:  signature
*              - size_t siglen: length of signature (DILITHIUM_CRYPTO_BYTES)
*              - uint8_t *m:    pointer to message
*              - size_t *mlen:  pointer to output length of message
*              - uint8_t *pk:   pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
int DILITHIUM_crypto_sign_verify(
        const uint8_t* sig, size_t siglen,
        const uint8_t *m, size_t mlen,
        const uint8_t *pk) {
    size_t i;
    uint8_t rho[SEEDBYTES];
    uint8_t mu[CRHBYTES];
    poly c, chat, cp;
    polyvecl mat[K], z;
    polyveck t1, w1, h, tmp1, tmp2;

    if (siglen < DILITHIUM_CRYPTO_BYTES) {
        return -1;
    }

    DILITHIUM_unpack_pk(rho, &t1, pk);
    if (DILITHIUM_unpack_sig(&z, &h, &c, sig)) {
        return -1;
    }
    if (DILITHIUM_polyvecl_chknorm(&z, GAMMA1 - BETA)) {
        return -1;
    }

    /* Compute CRH(CRH(rho, t1), msg) */
    crh(mu, pk, DILITHIUM_CRYPTO_PUBLICKEYBYTES);

    shake256incctx state;
    shake256_inc_init(&state);
    shake256_inc_absorb(&state, mu, CRHBYTES);
    shake256_inc_absorb(&state, m, mlen);
    shake256_inc_finalize(&state);
    shake256_inc_squeeze(mu, CRHBYTES, &state);

    /* Matrix-vector multiplication; compute Az - c2^dt1 */
    DILITHIUM_expand_mat(mat, rho);
    DILITHIUM_polyvecl_ntt(&z);
    for (i = 0; i < K; ++i)
        DILITHIUM_polyvecl_pointwise_acc_invmontgomery(&tmp1.vec[i], &mat[i], &z);

    chat = c;
    DILITHIUM_poly_ntt(&chat);
    DILITHIUM_polyveck_shiftl(&t1);
    DILITHIUM_polyveck_ntt(&t1);
    for (i = 0; i < K; ++i)
        DILITHIUM_poly_pointwise_invmontgomery(&tmp2.vec[i], &chat, &t1.vec[i]);

    DILITHIUM_polyveck_sub(&tmp1, &tmp1, &tmp2);
    DILITHIUM_polyveck_reduce(&tmp1);
    DILITHIUM_polyveck_invntt_montgomery(&tmp1);

    /* Reconstruct w1 */
    DILITHIUM_polyveck_csubq(&tmp1);
    DILITHIUM_polyveck_use_hint(&w1, &tmp1, &h);

    /* Call random oracle and verify challenge */
    DILITHIUM_challenge(&cp, mu, &w1);
    for (i = 0; i < N; ++i)
        if (c.coeffs[i] != cp.coeffs[i])
            return -1;

    return 0;
}

/*************************************************
* Name:        DILITHIUM_crypto_sign_open
*
* Description: Verify signed message.
*
* Arguments:   - unsigned char *m: pointer to output message (allocated
*                                  array with smlen bytes), can be equal to sm
*              - unsigned long long *mlen: pointer to output length of message
*              - const unsigned char *sm: pointer to signed message
*              - unsigned long long smlen: length of signed message
*              - const unsigned char *pk: pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
int DILITHIUM_crypto_sign_open(
        uint8_t *m, size_t *mlen,
        const uint8_t *sm, size_t smlen,
        const uint8_t *pk) {
    size_t i;
    if (smlen < DILITHIUM_CRYPTO_BYTES) {
        goto badsig;
    }
    *mlen = smlen - DILITHIUM_CRYPTO_BYTES;

    if (DILITHIUM_crypto_sign_verify(sm, DILITHIUM_CRYPTO_BYTES,
            sm + DILITHIUM_CRYPTO_BYTES, *mlen, pk)) {
        goto badsig;
    } else {
        /* All good, copy msg, return 0 */
        for (i = 0; i < *mlen; ++i) {
            m[i] = sm[DILITHIUM_CRYPTO_BYTES + i];
        }
        return 0;
    }

    /* Signature verification failed */
badsig:
    *mlen = (size_t) -1;
    for (i = 0; i < smlen; ++i) {
        m[i] = 0;
    }

    return -1;
}
