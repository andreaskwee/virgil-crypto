/**
 * Copyright (C) 2015-2016 Virgil Security Inc.
 *
 * Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 *     (3) Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <virgil/crypto/VirgilStreamCipher.h>

#include <virgil/crypto/foundation/VirgilKDF.h>
#include <virgil/crypto/foundation/VirgilSymmetricCipher.h>
#include <virgil/crypto/foundation/VirgilAsymmetricCipher.h>

using virgil::crypto::VirgilStreamCipher;
using virgil::crypto::VirgilByteArray;
using virgil::crypto::VirgilDataSource;
using virgil::crypto::VirgilDataSink;

using virgil::crypto::foundation::VirgilKDF;
using virgil::crypto::foundation::VirgilSymmetricCipher;
using virgil::crypto::foundation::VirgilAsymmetricCipher;

void VirgilStreamCipher::encrypt(VirgilDataSource& source, VirgilDataSink& sink, bool embedContentInfo) {

    VirgilSymmetricCipher& symmetricCipher = initEncryption();
    buildContentInfo();

    if (embedContentInfo) {
        VirgilDataSink::safeWrite(sink, getContentInfo());
    }
    while (source.hasData() && sink.isGood()) {
        VirgilDataSink::safeWrite(sink, symmetricCipher.update(source.read()));
    }
    VirgilDataSink::safeWrite(sink, symmetricCipher.finish());

    clearCipherInfo();
}

void VirgilStreamCipher::decryptWithKey(
        VirgilDataSource& source, VirgilDataSink& sink,
        const VirgilByteArray& recipientId, const VirgilByteArray& privateKey,
        const VirgilByteArray& privateKeyPassword) {

    VirgilByteArray firstChunk = tryReadContentInfo(source);
    VirgilSymmetricCipher& cipher = initDecryptionWithKey(recipientId, privateKey, privateKeyPassword);
    decrypt(source, sink, cipher, firstChunk);
}

void VirgilStreamCipher::decryptWithPassword(
        VirgilDataSource& source, VirgilDataSink& sink,
        const VirgilByteArray& pwd) {

    VirgilByteArray firstChunk = tryReadContentInfo(source);
    VirgilSymmetricCipher& cipher = initDecryptionWithPassword(pwd);
    decrypt(source, sink, cipher, firstChunk);
}

VirgilByteArray VirgilStreamCipher::tryReadContentInfo(VirgilDataSource& source) {

    const size_t minDataSize = 16;
    VirgilByteArray data;
    while (data.size() < minDataSize && source.hasData()) {
        VirgilByteArray nextData = source.read();
        data.insert(data.end(), nextData.begin(), nextData.end());
    }
    size_t contentInfoSize = defineContentInfoSize(data);
    if (contentInfoSize > 0) {
        while (data.size() < contentInfoSize && source.hasData()) {
            VirgilByteArray nextData = source.read();
            data.insert(data.end(), nextData.begin(), nextData.end());
        }
        return VirgilCipherBase::tryReadContentInfo(data);
    }
    return data;
}

void VirgilStreamCipher::decrypt(
        VirgilDataSource& source, VirgilDataSink& sink,
        VirgilSymmetricCipher& cipher, const VirgilByteArray& firstChunk) {

    VirgilDataSink::safeWrite(sink, cipher.update(firstChunk));
    while (source.hasData() && sink.isGood()) {
        VirgilDataSink::safeWrite(sink, cipher.update(source.read()));
    }
    VirgilDataSink::safeWrite(sink, cipher.finish());
    clearCipherInfo();
}
