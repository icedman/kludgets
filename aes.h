/*
 * Aarni is a simple and easy-to-use file encryptor.
 * Copyright (C) 2009 Xizhi Zhu <xizhi.zhu@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation,
 * or attached in the package as the LICENSE file.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef __AARNI_AES_H__
#define __AARNI_AES_H__

#include <QByteArray>

namespace Aarni
{

// This class implements AES [1] in the XTS mode, which is approved as the IEEE 1619 Standard [2]. The implementation
// is based on rijndael-alg-fst.c of version 3.0 [3].
//
// The best attack against AES-128 so far is on 7 rounds out of 10, with a time complexity of 2^128 - 2^119 [4].
// Another attack [5] has successfully reduced the complexity of recovering the key of AES-256 to 2^119, and for
// AES-192, the time complexity is 2^176 while the data complexity is 2^123.
//
// References:
//  [1] http://www.csrc.nist.gov/publications/fips/fips197/fips-197.pdf
//  [2] https://siswg.net/
//  [3] http://fastcrypto.org/vmac/rijndael-alg-fst.c
//  [4] http://www.springerlink.com/content/3tlphkvyk2tefla3/
//  [5] https://cryptolux.uni.lu/mediawiki/uploads/1/1a/Aes-192-256.pdf
class AES
{
public:
    // Constructor.
    AES();

    // Initialize the key and the tweak.
    //
    // Parameters:
    //  key - the first half for plaintext, and the second half for tweak
    //  tweak - the initial tweak value
    //
    // Return value:
    //  ERROR_SUCCESS - success
    //  ERROR_INVALID_KEY_LENGTH - the length of the key is not valid
    //  ERROR_INVALID_TWEAK_LENGTH - the length of the tweak is not 128 bits
    quint32 init(const QByteArray& key, const QByteArray& tweak);

    // Reset to the initial state.
    //
    // Return value:
    //  ERROR_SUCCESS - success
    //  ERROR_NOT_INITIALIZED - it's not initialized yet
    quint32 reset();

    // Encrypt/decrypt a data unit in the XTS mode.
    //
    // Parameters:
    //  in - the data to be encrypted/decrypted
    //  out - the encrypted/decrypted data
    //
    // Return value:
    //  ERROR_SUCCESS - success
    //  ERROR_NOT_INITIALIZED - it's not initialized yet
    //  ERROR_DATA_UNIT_TOO_LONG - the encrypted data in total is more than 2^128 bits
    //  ERROR_DATA_UNIT_TOO_SHORT - the data to be handled is less than 128 bits
    quint32 encrypt(const QByteArray& in, QByteArray& out);
    quint32 decrypt(const QByteArray& in, QByteArray& out);

private:
    // The constants used.
    static const quint32 aesTe0_[256];
    static const quint32 aesTe1_[256];
    static const quint32 aesTe2_[256];
    static const quint32 aesTe3_[256];
    static const quint32 aesTe4_[256];
    static const quint32 aesTd0_[256];
    static const quint32 aesTd1_[256];
    static const quint32 aesTd2_[256];
    static const quint32 aesTd3_[256];
    static const quint32 aesTd4_[256];
    static const quint32 aesRCon_[10];

    // The variables used.
    bool initialized_;              // if the key and tweak have been initialized
    quint8 initialEncTweak_[16];    // the initial encryption tweak value
    quint8 initialDecTweak_[16];    // the initial decryption tweak value
    quint8 currentEncTweak_[16];    // the current encryption tweak value
    quint8 currentDecTweak_[16];    // the current decryption tweak value
    quint32 expandedKey1_[60];      // the expanded encryption key for plaintext
    quint32 expandedKey2_[60];      // the expanded decryption key for plaintext
    quint32 expandedKey3_[60];      // the expanded key for tweak
    qint32 keyLength_;              // the length of key (128, 192 or 256 only)

    // Expand the key into encryption/decryption key schedule.
    void aesExpandEncryptionKey(const quint8* key, quint32* expandedKey);
    void aesExpandDecryptionKey(const quint8* key, quint32* expandedKey);

    // Encrypt/decrypt a single block using AES.
    void aesEncryptBlock(const quint32* expandedKey, const quint8 in[16], quint8 out[16]);
    void aesDecryptBlock(const quint32* expandedKey, const quint8 in[16], quint8 out[16]);

    // Encrypt/decrypt a block in the XTS mode.
    void xtsEncryptBlock(const quint8 tweak[16], const quint8 in[16], quint8 out[16]);
    void xtsDecryptBlock(const quint8 tweak[16], const quint8 in[16], quint8 out[16]);

    // Compute the tweak for the encryption of the next block.
    void xtsComputeTweak(quint8 tweak[16]);
};

}

#endif // __AARNI_AES_H__
