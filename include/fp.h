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

#pragma once
#ifdef FP_IMPL
#define STB_DS_IMPLEMENTATION
#endif

#include "stb_ds.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FP_MAX_LOG_SIZE
#define FP_MAX_LOG_SIZE 1000
#endif

typedef char *(*fp_handle_t)(void *);

#ifdef FP_IMPL
// This is never freed for the entire process
static struct {
  const char *key;
  fp_handle_t value;
} *fp_log_table = NULL;
#endif

// Serves as a delimeter for {something}
typedef struct Delimeter {
  size_t start;
  size_t end;
} Delimeter;

// Acts as a token that is consumed
typedef struct SDelim {
  char *fmt;
  Delimeter delim;
} SDelim;

// Iterates from start until find the {something} delimeter
#ifdef FP_IMPL
Delimeter fp_find_delimeter(const char *fmt, size_t start) {
  size_t i = start;
  if (!fmt) {
    return (Delimeter){0};
  }

  Delimeter del = {0};

  char c = fmt[i];
  char last = 0;

  uint8_t open_found = 0;

  // Iterates over the string to find the position of '{'
  while (c) {
    if (c == '{' && last != '\\') {
      del.start = i + 1;
      open_found = 1;
    } else if (open_found && c == '}' && last != '\\') {
      del.end = i;
      break;
    }

    i += 1;
    last = c;
    c = fmt[i];
  }

  return del;
}
#else
Delimeter fp_find_delimeter(const char *fmt, size_t start);
#endif

// Gets the handler function from the fp_log_table
#ifdef FP_IMPL
fp_handle_t fp_get_handle_function(const char *fmt, Delimeter del) {
  if (!fmt || !fp_log_table) {
    return NULL;
  }

  size_t size = del.end - del.start;

  char *buff = malloc(size + 1);

  if (!buff) {
    return NULL;
  }

  memcpy(buff, fmt + del.start, size);
  buff[size] = 0;

  fp_handle_t ret = (fp_handle_t)shget(fp_log_table, buff);
  free(buff);

  return ret;
}
#else
fp_handle_t fp_get_handle_function(const char *fmt, Delimeter del);
#endif

// Works as a default handler signalling when the handler was not setup
#ifdef FP_IMPL
char *fp_default_handler([[maybe_unused]] void *arg) {
  char real_ret[] = "{not defined}";
  char *ret = (char *)malloc(sizeof(real_ret));
  if (!ret) {
    return NULL;
  }
  memcpy(ret, real_ret, sizeof(real_ret));
  return ret;
}
#else
char *fp_default_handler(void *arg);
#endif

// gives a handler for the specified obj_name
#ifdef FP_IMPL
void fp_log_print_method(fp_handle_t handler, const char *obj_name) {
  if (!fp_log_table) {
    // starts the log_table and sets the default value
    shdefault(fp_log_table, fp_default_handler);
  }

  if (!obj_name) {
    return;
  }

  // Logs the handler (key, value)
  shput(fp_log_table, obj_name, handler ? handler : fp_default_handler);
}
#else
void fp_log_print_method(fp_handle_t handler, const char *obj_name);
#endif

// Copies the values to the final formated string
#ifdef FP_IMPL
void fp_copy_to_fmt(char *dest_fmt, const char *src_fmt, SDelim *array) {
  // Gets the array and src_fmt size
  size_t size = arrlen(array);
  size_t src_fmt_size = strlen(src_fmt);

  // Setups the offsets
  size_t src_off = 0, dest_off = 0;

  for (size_t i = 0; i < size; i++) {
    // Gets the format and its size
    char *fmt = array[i].fmt;
    size_t fmt_size = strlen(fmt);

    // Gets the delimeter to substitute
    Delimeter del = array[i].delim;

    // Gets the original size by the start and offset, the source string will be
    // copied up to here
    size_t orig_str_size = (del.start - 1) - src_off;

    // Copies from the source string and updates the offset
    memcpy(dest_fmt + dest_off, src_fmt + src_off, orig_str_size);
    src_off = del.end + 1;
    dest_off += orig_str_size;

    // Copies the formated string and updates the offset
    memcpy(dest_fmt + dest_off, fmt, fmt_size);
    dest_off += fmt_size;
  }
  // Copies the final part
  memcpy(dest_fmt + dest_off, src_fmt + src_off, src_fmt_size - src_off);
  dest_fmt[dest_off + (src_fmt_size - src_off)] = '\0';
}

#else
void fp_copy_to_fmt(char *dest_fmt, const char *src_fmt, SDelim *array);
#endif

// I think i will change this to work as a string formatter and not as a printf
#ifdef FP_IMPL
void fp_printf(const char *fmt, ...) {
  if (!fp_log_table) {
    fp_log_print_method(NULL, NULL);
  }

  if (!fmt) {
    return;
  }

  va_list args;
  va_start(args, fmt);

  char *n_fmt = NULL;
  SDelim *array = NULL;

  size_t total_size = strlen(fmt);
  size_t start = 0;
  size_t last = 0;

  while (fmt[start]) {
    Delimeter del = fp_find_delimeter(fmt, start);
    if (del.end == 0 || del.end < del.start) {
      break;
    } else if (del.end == del.start) {
      start = del.end + 1;
      continue;
    }

    fp_handle_t handler = fp_get_handle_function(fmt, del);
    if (!handler) {
      goto safe_return;
    }

    char *format = handler(va_arg(args, void *));
    if (!format) {
      goto safe_return;
    }

    SDelim sdel = {.fmt = format, .delim = del};

    arrput(array, sdel);

    // Calculates the total size of the new_fmt, minus two to include the braces
    total_size += strlen(format) - (del.end - del.start + 2);

    last = start;
    start = del.end + 1;

    if (last == start)
      break;
  }

  n_fmt = malloc(total_size + 1);

  if (!n_fmt) {
    goto safe_return;
  }

  fp_copy_to_fmt(n_fmt, fmt, array);

  puts(n_fmt);

  // Used to cleanly exits the function
safe_return:
  va_end(args);
  for (size_t i = 0; i < arrlenu(array); i++) {
    char *str = array[i].fmt;
    if (str)
      free(str);
  }
  if (array)
    arrfree(array);
  if (n_fmt)
    free(n_fmt);
}
#else
void fp_printf(const char *fmt, ...);
#endif

// Just to make it easier to type
#define fp_logger(method, name) fp_log_print_method((fp_handle_t)method, name)
