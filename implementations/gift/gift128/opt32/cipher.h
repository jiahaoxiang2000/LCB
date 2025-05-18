#ifndef CIPHER_H_
#define CIPHER_H_

// Ascon-like encryption function
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k);

// Ascon-like decryption function
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k);

#endif /* CIPHER_H_ */
