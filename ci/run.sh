#!/bin/bash
#
# Copyright (C) 2015-2016 Virgil Security Inc.
#
# Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     (1) Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#     (2) Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#
#     (3) Neither the name of the copyright holder nor the names of its
#     contributors may be used to endorse or promote products derived from
#     this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

set -ev

cd "${TRAVIS_BUILD_DIR}/${BUILD_DIR_NAME}"

if [ "${PUBLISH_COVERITY_SCAN}" == "ON" ] && [ "${TRAVIS_BRANCH}" == "coverity_scan" ] && [[ "${CC}" == "gcc"* ]]; then
    export COVERITY_SCAN_PROJECT_NAME="VirgilSecurity/virgil-crypto"
    export COVERITY_SCAN_BRANCH_PATTERN="coverity_scan"
    export COVERITY_SCAN_NOTIFICATION_EMAIL="sergey.seroshtan@gmail.com"
    export COVERITY_SCAN_BUILD_COMMAND="make -j4"
    curl -s "https://scan.coverity.com/scripts/travisci_build_coverity_scan.sh" | bash || true
else
    make -j2 VERBOSE=1
fi

make install
if [ "${LANG}" == "php" ] || [ "${LANG}" == "cpp" ]; then
    ctest --verbose
fi
