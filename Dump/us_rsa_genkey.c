#include <string.h>
#include "rsa_genkey.h"
#if 0
int rsa_genkey(rsa_keyset *keyset)
{
    int ret;
    rsa_context rsa;
    havege_state hs;
    char buf[KEY_SIZE];
    int slen;
    
    memset(keyset, 0, sizeof(rsa_keyset));

    havege_init( &hs );

    printf( " ok\n  . Generating the RSA key [ %d-bit ]...\n", KEY_SIZE );

    rsa_init( &rsa, RSA_PKCS_V15, 0, havege_rand, &hs );
    if( ( ret = rsa_gen_key( &rsa, KEY_SIZE, EXPONENT ) ) != 0 )
    {
        printf( " failed\n  ! rsa_gen_key returned %d\n\n", ret );
        goto exit;
    }
    
    slen = sizeof(buf);

    //get public key
    mpi_write_string(&rsa.D, 16, buf, &slen);
    strcpy(keyset->privkey, buf);

    //get private key
    slen = sizeof(buf);
    mpi_write_string(&rsa.E, 16, buf, &slen);
    strcpy(keyset->pubkey, buf);

    //get modulus
    slen = sizeof(buf);
    mpi_write_string(&rsa.N, 16, buf, &slen);
    strcpy(keyset->modulus, buf);

exit:
    rsa_free( &rsa );

    return( ret );
}

#endif