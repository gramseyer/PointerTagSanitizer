//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// A memset or memcpy can reach the pass with a constant length whose top bit
// is set: a std::vector resize by a wrapped-around count inlines to one, on a
// path the program never takes. The access must compile, must not be reported
// unless it executes, and must be reported when it does. The second function
// puts it in a loop, where the loop preanalysis sees it before discovery.
// RUN: %clang_ptsan -O2 -o %t %s
// RUN: %run %t
// RUN: not %run %t straight 2>&1 | FileCheck %s
// RUN: not %run %t loop 2>&1 | FileCheck %s
// RUN: %clang_ptsan_id32 -O2 -o %t.id32 %s
// RUN: %run %t.id32
// RUN: not %run %t.id32 straight 2>&1 | FileCheck %s
// RUN: not %run %t.id32 loop 2>&1 | FileCheck %s

// CHECK: Found bad {{RANGE|COMPOUND}} access

#include <assert.h>
#include <stdlib.h>
#include <string.h>

__attribute__((noinline)) static void clear_straight(char *d, int bad) {
  if (bad)
    memset(d, 0, (size_t)-4);
}

__attribute__((noinline)) static void clear_in_loop(char *d, int n, int bad) {
  for (int i = 0; i < n; i++) {
    if (bad)
      memset(d + i, 0, (size_t)-4);
    d[i] = 1;
  }
}

int main(int argc, char **argv) {
  char *p = malloc(64);
  assert(p);
  p[0] = 1;
  clear_straight(p, argc > 1 && strcmp(argv[1], "straight") == 0);
  clear_in_loop(p + 8, 8, argc > 1 && strcmp(argv[1], "loop") == 0);
  assert(p[0] == 1 && p[15] == 1);
  free(p);
  return 0;
}
