#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_api.h"
#include "config.h"
#include "uip-conf.h"
#include "iot_custom.h"

/******************************************************************************
* MODULE NAME:     iot_aes_pub.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            July 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek Tech. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     July 2014   - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/

/************************************************/
/* Macros */
/************************************************/

/*
========================================================================
Routine Description:
    AES-CBC encryption

Arguments:
    PlainText        Plain text
    PlainTextLength  The length of plain text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    IV               Initialization vector, it may be 16 bytes (128 bits)
    IVLength         The length of initialization vector in bytes
    CipherTextLength The length of allocated cipher text in bytes

Return Value:
    CipherText           Return cipher text
    CipherTextLength Return the length of real used cipher text in bytes   (including padding)
                               if PlainTextLength=16*n,   CipherTextLength=16*(n+1)
                               if PlainTextLength=16*(n-1) + x (0<x<16),    CipherTextLength = 16*n

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
VOID AES_CBC_Encrypt (
    UINT8 *PlainText,
    UINT PlainTextLength,
    UINT8 *Key,
    UINT KeyLength,
    UINT8 *IV,
    UINT IVLength,
    UINT8 *CipherText,
    UINT *CipherTextLength)
{
    UINT PaddingSize, PlainBlockStart, CipherBlockStart, CipherBlockSize;
    UINT Index;
	UINT LastBlockSize=0;
    UINT8 Block[AES_BLOCK_SIZES];
	
    /*   
     * 1. Check the input parameters
     *    - CipherTextLength > (PlainTextLength + Padding size), Padding size = block size - (PlainTextLength % block size)
     *    - Key length must be 16, 24, or 32 bytes(128, 192, or 256 bits) 
     *    - IV length must be 16 bytes(128 bits) 
     */
    LastBlockSize = PlainTextLength % ((UINT)AES_BLOCK_SIZES);

	if(LastBlockSize > 0)
   		PaddingSize = ((UINT) AES_BLOCK_SIZES) - LastBlockSize;
	else
		PaddingSize = 0;
    if (*CipherTextLength < (PlainTextLength + PaddingSize)) {
    	Printf_High("AES_CBC_Encrypt: cipher text length is %d bytes < (plain text length %d bytes + padding size %d bytes).\n", 
            *CipherTextLength, PlainTextLength, PaddingSize);
        return;
    } /* End of if */    
    if ((KeyLength != AES_KEY128_LENGTH) && (KeyLength != AES_KEY192_LENGTH) && (KeyLength != AES_KEY256_LENGTH)) {
    	Printf_High("AES_CBC_Encrypt: key length is %d bytes, it must be %d, %d, or %d bytes(128, 192, or 256 bits).\n", 
            KeyLength, AES_KEY128_LENGTH, AES_KEY192_LENGTH, AES_KEY256_LENGTH);
        return;
    } /* End of if */
    if (IVLength != AES_CBC_IV_LENGTH) {
    	Printf_High("AES_CBC_Encrypt: IV length is %d bytes, it must be %d bytes(128bits).\n", 
            IVLength, AES_CBC_IV_LENGTH);
        return;
    } /* End of if */


    /*   
     * 2. Main algorithm
     *    - Plain text divide into serveral blocks (16 bytes/block)
     *    - If plain text is divided with no remainder by block, add a new block and padding size = block(16 bytes)
     *    - If plain text is not divided with no remainder by block, padding size = (block - remainder plain text)
     *    - Execute RT_AES_Encrypt procedure.
     *    
     *    - Padding method: The remainder bytes will be filled with padding size (1 byte)
     */
    PlainBlockStart = 0;
    CipherBlockStart = 0;
    
    while ((PlainTextLength - PlainBlockStart) >= AES_BLOCK_SIZES)
    {
        if (CipherBlockStart == 0) {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                Block[Index] = PlainText[PlainBlockStart + Index]^IV[Index];                
        } else {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                Block[Index] = PlainText[PlainBlockStart + Index]^CipherText[CipherBlockStart - ((UINT) AES_BLOCK_SIZES) + Index];
        } /* End of if */
        CipherBlockSize = *CipherTextLength - CipherBlockStart;
        RT_AES_Encrypt(
				Block, 
				AES_BLOCK_SIZES , 
				Key, 
				KeyLength, 
				CipherText + CipherBlockStart, 
				&CipherBlockSize);

        PlainBlockStart += ((UINT) AES_BLOCK_SIZES);
        CipherBlockStart += CipherBlockSize;
    } /* End of while */
    if (PlainTextLength == PlainBlockStart)
    {
       memset(Block, AES_BLOCK_SIZES, AES_BLOCK_SIZES);        
    }
    else
    {
       NdisMoveMemory(Block, (&PlainText[0] + PlainBlockStart), (PlainTextLength - PlainBlockStart));
       memset((Block + (((UINT) AES_BLOCK_SIZES) -PaddingSize)), (UINT8) PaddingSize, PaddingSize);
    }

    if (CipherBlockStart == 0) {
       for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
           Block[Index] ^= IV[Index];
    } else {
       for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
           Block[Index] ^= CipherText[CipherBlockStart - ((UINT) AES_BLOCK_SIZES) + Index];
    } /* End of if */
    CipherBlockSize = *CipherTextLength - CipherBlockStart;
    RT_AES_Encrypt(Block, AES_BLOCK_SIZES , Key, KeyLength, CipherText + CipherBlockStart, &CipherBlockSize);

    CipherBlockStart += CipherBlockSize;
    *CipherTextLength = CipherBlockStart;
} /* End of AES_CBC_Encrypt */


/*
========================================================================
Routine Description:
    AES-CBC decryption

Arguments:
    CipherText       Cipher text
    CipherTextLength The length of cipher text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    IV               Initialization vector, it may be 16 bytes (128 bits)
    IVLength         The length of initialization vector in bytes
    PlainTextLength  The length of allocated plain text in bytes

Return Value:
    PlainText        Return plain text
    PlainTextLength  Return the length of real used plain text in bytes

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
VOID AES_CBC_Decrypt (
    UINT8 *CipherText,
    UINT CipherTextLength,
    UINT8 *Key,
    UINT KeyLength,
    UINT8 *IV,
    UINT IVLength,
    UINT8 *PlainText,
    UINT *PlainTextLength)
{
    UINT PaddingSize, PlainBlockStart, CipherBlockStart, PlainBlockSize;
    UINT Index;

    /*   
     * 1. Check the input parameters
     *    - CipherTextLength must be divided with no remainder by block
     *    - Key length must be 16, 24, or 32 bytes(128, 192, or 256 bits) 
     *    - IV length must be 16 bytes(128 bits) 
     */
    if ((CipherTextLength % AES_BLOCK_SIZES) != 0) {
    	printf("AES_CBC_Decrypt: cipher text length is %d bytes, it can't be divided with no remainder by block size(%d).\n", 
            CipherTextLength, AES_BLOCK_SIZES);
        return;
    } /* End of if */    
    if ((KeyLength != AES_KEY128_LENGTH) && (KeyLength != AES_KEY192_LENGTH) && (KeyLength != AES_KEY256_LENGTH)) {
    	printf("AES_CBC_Decrypt: key length is %d bytes, it must be %d, %d, or %d bytes(128, 192, or 256 bits).\n", 
            KeyLength, AES_KEY128_LENGTH, AES_KEY192_LENGTH, AES_KEY256_LENGTH);
        return;
    } /* End of if */
    if (IVLength != AES_CBC_IV_LENGTH) {
    	printf("AES_CBC_Decrypt: IV length is %d bytes, it must be %d bytes(128bits).\n", 
            IVLength, AES_CBC_IV_LENGTH);
        return;
    } /* End of if */


    /*   
     * 2. Main algorithm
     *    - Cypher text divide into serveral blocks (16 bytes/block)
     *    - Execute RT_AES_Decrypt procedure.
     *    - Remove padding bytes, padding size is the last byte of plain text
     */
    CipherBlockStart = 0;
    PlainBlockStart = 0;
    while ((CipherTextLength - CipherBlockStart) >= AES_BLOCK_SIZES)
    {
        PlainBlockSize = *PlainTextLength - PlainBlockStart;
        RT_AES_Decrypt(CipherText + CipherBlockStart, 
						AES_BLOCK_SIZES , 
						Key, 
						KeyLength, 
						PlainText + PlainBlockStart, 
						&PlainBlockSize);

        if (PlainBlockStart == 0) {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                PlainText[PlainBlockStart + Index] ^= IV[Index];                
        } else {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                PlainText[PlainBlockStart + Index] ^= CipherText[CipherBlockStart + Index - ((UINT) AES_BLOCK_SIZES)];
        } /* End of if */

        CipherBlockStart += AES_BLOCK_SIZES;
        PlainBlockStart += PlainBlockSize;
    } /* End of while */

    PaddingSize = (UINT8) PlainText[PlainBlockStart -1];   
    *PlainTextLength = PlainBlockStart - PaddingSize;

} /* End of AES_CBC_Encrypt */

