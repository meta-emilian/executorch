/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <executorch/kernels/portable/cpu/scalar_utils.h>
#include <executorch/kernels/portable/cpu/util/broadcast_util.h>
#include <executorch/runtime/kernel/kernel_includes.h>

namespace torch {
namespace executor {
namespace native {
namespace {

template <class T>
const T& max(const T& a, const T& b) {
  return (b > a) ? b : a;
}

} // namespace

Tensor& maximum_out(
    RuntimeContext& ctx,
    const Tensor& a,
    const Tensor& b,
    Tensor& out) {
  (void)ctx;

  ET_KERNEL_CHECK(
      ctx,
      resize_to_broadcast_target_size(a, b, out) == Error::Ok,
      InvalidArgument,
      out);

  ScalarType a_type = a.scalar_type();
  ScalarType b_type = b.scalar_type();
  ScalarType common_type = promoteTypes(a_type, b_type, /*half_to_float*/ true);
  ScalarType out_type = out.scalar_type();

  ET_KERNEL_CHECK(ctx, canCast(common_type, out_type), InvalidArgument, out);

  ET_SWITCH_REALHB_TYPES(a_type, ctx, "maximum.out", CTYPE_A, [&]() {
    ET_SWITCH_REALHB_TYPES(b_type, ctx, "maximum.out", CTYPE_B, [&]() {
      ET_SWITCH_REALB_TYPES(common_type, ctx, "maximum.out", CTYPE_IN, [&]() {
        ET_SWITCH_REALHB_TYPES(out_type, ctx, "maximum.out", CTYPE_OUT, [&]() {
          apply_binary_elementwise_fn<CTYPE_A, CTYPE_B, CTYPE_OUT>(
              [](const CTYPE_A val_a, const CTYPE_B val_b) {
                CTYPE_IN a_casted = static_cast<CTYPE_IN>(val_a);
                CTYPE_IN b_casted = static_cast<CTYPE_IN>(val_b);
                CTYPE_IN value = max(a_casted, b_casted);

                return static_cast<CTYPE_OUT>(value);
              },
              a,
              b,
              out);
        });
      });
    });
  });

  return out;
}

} // namespace native
} // namespace executor
} // namespace torch
