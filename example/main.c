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
