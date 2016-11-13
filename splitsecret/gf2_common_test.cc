//
// Copyright 2014 John Manferdelli, All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// or in the the file LICENSE-2.0.txt in the top level sourcedirectory
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License
// File: symmetric.cc

#include <stdio.h>
#include <string.h>

#include <string>

#include "util.h"
#include "cryptotypes.h"
#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include "gf2_common.h"

#include <memory>
#include <cmath>

DEFINE_bool(printall, false, "printall flag");

class GF2Common : public ::testing::Test {
 protected:
  virtual void SetUp();
  virtual void TearDown();
};

void GF2Common::SetUp() {}

void GF2Common::TearDown() {}


bool InternalRepTest() {
  uint16_t tpoly = 0x77;
  uint16_t cpoly;
  int size_a = 16;
  byte a[16];
  EXPECT_TRUE(to_internal_representation(tpoly, &size_a, a));
  EXPECT_TRUE(from_internal_representation(size_a, a, &cpoly));
  printf("Cpoly: %02x\n", cpoly);
  EXPECT_TRUE(tpoly == cpoly);
  return true;
}

bool PrintPolyTest() {
  uint16_t tpoly = 0x77;
  int size_a = 16;
  byte a[16];
  EXPECT_TRUE(to_internal_representation(tpoly, &size_a, a));
  print_poly(size_a, a);
  printf("\n");
  return true;
}

bool Gf2AddTest() {
  uint16_t poly1 = 0x77;
  uint16_t poly2 = 0x07;

  int size_min_poly = 16;
  byte min_poly[16];

  int size_a = 16;
  byte a[16];
  int size_b = 16;
  byte b[16];
  int size_c = 16;
  byte c[16];

  EXPECT_TRUE(to_internal_representation(poly1, &size_a, a));
  EXPECT_TRUE(to_internal_representation(poly2, &size_b, b));
  EXPECT_TRUE(gf2_add(size_a, a, size_b, b, size_min_poly, min_poly, &size_c, c));

  uint16_t cpoly;
  EXPECT_TRUE(from_internal_representation(size_c, c, &cpoly));
  printf("poly1: %02x, poly2: %02x, Cpoly: %02x\n", poly1, poly2, cpoly);
  EXPECT_TRUE(cpoly == 0x70);
  return true;
}

bool Gf2ReduceTest() {
  uint16_t minpoly = 0x11b;
  uint16_t poly1 = 0x200;

  int size_min_poly = 16;
  byte min_poly[16];

  EXPECT_TRUE(to_internal_representation(minpoly, &size_min_poly, min_poly));
  printf("Min poly: "); print_poly(size_min_poly, min_poly); printf("\n");

  int size_a = 16;
  byte a[16];

  EXPECT_TRUE(to_internal_representation(poly1, &size_a, a));
  printf("Input poly: "); print_poly(size_a, a); printf("\n");
  EXPECT_TRUE(gf2_reduce(size_min_poly, min_poly, &size_a, a));

  uint16_t cpoly;
  EXPECT_TRUE(from_internal_representation(size_a, a, &cpoly));
  printf("Reduced poly: "); print_poly(size_a, a); printf(", %02x\n", cpoly);
  EXPECT_TRUE(cpoly == 0x36);
  return true;
}

bool Gf2MultiplyTest() {
  uint16_t poly1 = 0x77;
  uint16_t poly2 = 0x07;

  int size_min_poly = 16;
  byte min_poly[16];
  uint16_t minpoly = 0x11b;

  EXPECT_TRUE(to_internal_representation(minpoly, &size_min_poly, min_poly));
  printf("Min poly: "); print_poly(size_min_poly, min_poly); printf("\n");

  int size_a = 16;
  byte a[16];
  int size_b = 16;
  byte b[16];
  int size_c = 32;
  byte c[32];

  EXPECT_TRUE(to_internal_representation(poly1, &size_a, a));
  EXPECT_TRUE(to_internal_representation(poly2, &size_b, b));
  EXPECT_TRUE(gf2_mult(size_a, a, size_b, b, size_min_poly, min_poly, &size_c, c));
  print_poly(size_a, a);
  printf(" * ");
  print_poly(size_b, b);
  printf(" [mod ");
  print_poly(size_min_poly, min_poly);
  printf(" ] = ");
  print_poly(size_c, c);
  printf("\n");

  uint16_t cpoly;
  EXPECT_TRUE(from_internal_representation(size_c, c, &cpoly));
  printf("cpoly: %02x\n", cpoly);
  EXPECT_TRUE(cpoly == 0x5e);
  return true;
}

bool Gf2InverseTest() {
  int size_min_poly = 16;
  byte min_poly[16];
  uint16_t minpoly = 0x11b;

  EXPECT_TRUE(to_internal_representation(minpoly, &size_min_poly, min_poly));
  printf("Min poly: "); print_poly(size_min_poly, min_poly); printf("\n");
  EXPECT_TRUE(init_inverses(size_min_poly, min_poly));
  for (int i = 0; i < 256; i++) {
    uint16_t z;
    from_internal_representation(8, g_gf2_inverse[i].v_, &z);
    printf("1/%02x = %02x\n", i, z);
  }
  uint16_t w;
  from_internal_representation(8, g_gf2_inverse[2].v_, &w);
  EXPECT_TRUE( w == 0x8d);
  return true;
}

bool Gf2LinearTest() {
  int size_min_poly = 16;
  byte min_poly[16];
  uint16_t minpoly = 0x11b;

  EXPECT_TRUE(to_internal_representation(minpoly, &size_min_poly, min_poly));
  printf("Min poly: "); print_poly(size_min_poly, min_poly); printf("\n");
  EXPECT_TRUE(init_inverses(size_min_poly, min_poly));

  gf2_8 a[48];
  gf2_8 x[48];
  gf2_8 y;
  byte t3[16];
  uint16_t u, w;
  int size;
  for (int i = 0; i < 48; i++) {
    w =  (uint16_t)(i + 1);
    u =  (uint16_t)(2 * i + 10);
    size = 16;
    EXPECT_TRUE(to_internal_representation(w, &size, t3));
    byte_8_copy(t3, a[i].v_);
    size = 16;
    EXPECT_TRUE(to_internal_representation(u, &size, t3));
    byte_8_copy(t3, x[i].v_);
  }

  EXPECT_TRUE(multiply_linear(48, size_min_poly, min_poly, a, x, y));

  uint16_t r, s, v;
  for (int i = 0; i < 48; i++) {
    EXPECT_TRUE(from_internal_representation(8, a[i].v_, &r));
    EXPECT_TRUE(from_internal_representation(8, x[i].v_, &s));
    printf("%02x * %02x ", r, s);
    if (i != 47)
      printf("+ ");
  }
  EXPECT_TRUE(from_internal_representation(8, y.v_, &v));
  printf(" =  %02x\n", v);
  return true;
}

bool Gf2SolveSimultaneousTest() {
  int size_min_poly = 16;
  byte min_poly[16];
  uint16_t minpoly = 0x11b;

  EXPECT_TRUE(to_internal_representation(minpoly, &size_min_poly, min_poly));
  printf("Min poly: "); print_poly(size_min_poly, min_poly); printf("\n");
  EXPECT_TRUE(init_inverses(size_min_poly, min_poly));

  gf2_8 x[48];
  byte t3[16];
  uint16_t w;
  int size;

  for (int i = 0; i < 48; i++) {
    w =  (uint16_t)(i + 1);
    size = 16;
    EXPECT_TRUE(to_internal_representation(w, &size, t3));
    byte_8_copy(t3, x[i].v_);
  }

  gf2_instance instance[48];

  for (int j = 0; j < 48; j++) {
    for (int i = 0; i < 48; i++) {
      w =  (uint16_t)((j * i + 1) % 256);
      size = 16;
      EXPECT_TRUE(to_internal_representation(w, &size, t3));
      byte_8_copy(t3, instance[j].a_[i].v_);
    }

    // Generate a's and y's
    if (!multiply_linear(48, size_min_poly, min_poly, instance[j].a_, x, instance[j].y_))
      return false;
  }

  for (int j = 0; j < 48; j++) {
    printf("Equation %d:\n", j + 1);
    for(int i = 0; i < 48; i++) {
      EXPECT_TRUE(from_internal_representation(8, instance[j].a_[i].v_, &w));
      printf("%02x * x[%d] + ", w, i);
    }
    EXPECT_TRUE(from_internal_representation(8, instance[j].y_.v_, &w));
    printf(" = %02x\n\n", w);
  }

  return true;
}

TEST(InternalRep, InternalRepTest) {
  EXPECT_TRUE(InternalRepTest());
}
TEST(Gf2Add, Gf2AddTest) {
  EXPECT_TRUE(Gf2AddTest());
}
TEST(Gf2Reduce, Gf2ReduceTest) {
  EXPECT_TRUE(Gf2ReduceTest());
}
TEST(Gf2Multiply, Gf2MultiplyTest) {
  EXPECT_TRUE(Gf2MultiplyTest());
}
TEST(PrintPoly, PrintPolyTest) {
  EXPECT_TRUE(PrintPolyTest());
}
TEST(Gf2Inverse, Gf2InverseTest) {
  EXPECT_TRUE(Gf2InverseTest());
}
TEST(Gf2Linear, Gf2LinearTest) {
  EXPECT_TRUE(Gf2LinearTest());
}
TEST(Gf2SolveSimultaneous, Gf2SolveSimultaneousTest) {
  EXPECT_TRUE(Gf2SolveSimultaneousTest());
}


DEFINE_string(log_file, "gf2_common_test.log", "gf2_common_test file name");

int main(int an, char** av) {
  ::testing::InitGoogleTest(&an, av);
#ifdef __linux__
  gflags::ParseCommandLineFlags(&an, &av, true);
#else
  google::ParseCommandLineFlags(&an, &av, true);
#endif
  int result = RUN_ALL_TESTS();
  return result;
}
