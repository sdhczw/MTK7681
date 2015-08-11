#ifndef XYSSL_RSA_GENKEY_H
#define XYSSL_RSA_GENKEY_H

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <rsa.h>
#include "havege.h"
#include <bignum.h>

#define KEY_SIZE 256
#define EXPONENT 65537
#define PUB_LEN  20

typedef struct rsa_keyset
{
	char privkey[KEY_SIZE/4+2];
	char pubkey[PUB_LEN];
	char modulus[KEY_SIZE/4+2];
}rsa_keyset;

int rsa_genkey(rsa_keyset *keyset);
 
#endif
