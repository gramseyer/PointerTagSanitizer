//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// begin[(end - begin) - 1] is the last element of [begin, end), which is how a
// vector's back() reads it. LLVM cancels begin and leaves a GEP whose base is
// the literal -1 and whose index is the tagged end pointer. The object ID has
// to come from that index, not from the literal: the valid access must run,
// and the same access on an empty range must still be reported.
// RUN: %clang_raw -O2 -S -emit-llvm -o - %s | FileCheck %s --check-prefix=IR
// RUN: %clang_ptsan -O2 -o %t %s
// RUN: %run %t
// RUN: not %run %t empty 2>&1 | FileCheck %s --check-prefix=EMPTY

// IR-LABEL: define {{.*}} @flip_last(
// IR: getelementptr i8, ptr inttoptr (i64 -1 to ptr), i64 %{{.+}}

// EMPTY: Found bad {{POINT|COMPOUND}} access

#include <assert.h>
#include <stdlib.h>

struct range {
  unsigned char *begin;
  unsigned char *end;
};

__attribute__((noinline)) static void flip_last(struct range *r) {
  r->begin[(r->end - r->begin) - 1] ^= 1;
}

int main(int argc, char **argv) {
  (void)argv;
  struct range r;
  r.begin = malloc(100);
  assert(r.begin);
  r.end = argc > 1 ? r.begin : r.begin + 100;
  r.begin[99] = 2;
  flip_last(&r);
  assert(r.begin[99] == 3);
  free(r.begin);
  return 0;
}
