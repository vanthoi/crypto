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
// Project: New Cloudproxy Crypto
// File: rc4.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <memory>
#include <iostream>

#include "cryptotypes.h"

#ifndef _CRYPTO_RC4_H__
#define _CRYPTO_RC4_H__

class Rc4 {
 public:
  enum {
    BLOCKBYTESIZE = 8,
  };

 private:
  bool initialized_;
  int key_size_;
  byte key_[256];
  byte state_[256];
  int index1_;
  int index2_;

 public:
  Rc4();
  ~Rc4();

  bool Init(int size, byte* key);
  byte Next();
  void Encrypt(int size, byte* in, byte* out);
};
#endif
