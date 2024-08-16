#!/bin/bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set -ex

# shellcheck source=/dev/null
source "$(dirname "${BASH_SOURCE[0]}")/utils.sh"
source "./build-qnn-sdk.sh"

MODEL_NAME=$1
if [[ -z "${MODEL_NAME:-}" ]]; then
  echo "Missing model name, exiting..."
  exit 1
fi

which "${PYTHON_EXECUTABLE}"
CMAKE_OUTPUT_DIR=cmake-out-android

test_qnn_model() {
  echo "ANDROID_NDK_ROOT: $ANDROID_NDK_ROOT"
  echo "QNN_SDK_ROOT: $QNN_SDK_ROOT"
  echo "EXECUTORCH_ROOT: $EXECUTORCH_ROOT"
  export PYTHONPATH=$EXECUTORCH_ROOT/..
  if [[ "${MODEL_NAME}" == "dl3" ]]; then
    "${PYTHON_EXECUTABLE}" -m examples.qualcomm.scripts.deeplab_v3 -b cmake-out-android -m SM8550 --compile_only --download
  fi
}

test_qnn_model
