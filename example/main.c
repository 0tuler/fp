// MIT License
//
// Copyright (c) 2026 Eduardo Tuler
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define FP_IMPL
#include "../include/fp.h"
#include <stdlib.h>

typedef struct Cat {
  int age;
  char name[5];
} Cat;

char *print_cat(Cat *cat) {
  if (!cat) {
    return NULL;
  }

  size_t len = sizeof(cat->name) + sizeof(cat->age) + 1;

  char *ret = malloc(len);

  if (!ret) {
    fprintf(stderr, "could not allocated memory for {cat}\n");
    return NULL;
  }

  snprintf(ret, len, "%s %d", cat->name, cat->age);

  return ret;
}

int main() {
  fp_logger(print_cat, "cat");

  Cat cat = {10, "Jade"};

  fp_printf("Here's a cat: {cat}!", &cat);
  return EXIT_SUCCESS;
}
