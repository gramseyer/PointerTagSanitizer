//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// XFAIL: *
// ID32 regression for the literal-base GEP described in
// ptrdiff_reassociated_last_element.c. ID16 runs separately there. With 32-bit
// IDs the valid access is still reported.
// RUN: %clang_ptsan_id32 -O2 -o %t %S/ptrdiff_reassociated_last_element.c
// RUN: %run %t
