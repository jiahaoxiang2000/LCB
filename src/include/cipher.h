
int cipher_encrypt(
	unsigned char *c,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *k);

int cipher_decrypt(
	unsigned char *m,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *k);
