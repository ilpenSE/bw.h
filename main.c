#include <stdio.h>
#define BW_IMPL
#include "bw.h"

int main(void)
{
  BitWriter bw = bw_new(8);

  bw_put(&bw, 1);
  bw_put(&bw, 0);
  bw_put(&bw, 1);

  for (size_t i = 0; i < bw.pos; i++) {
    printf("[%zu]: %d\n", i, bw_get(&bw, i)); // 1 0 1
  }

  printf("got: %lu\n", bw_gets(&bw, 0, 8)); // 160

  bw_free(&bw);
  return 0;
}
