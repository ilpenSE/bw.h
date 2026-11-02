#ifndef BW_H
#define BW_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Define BW_STATIC if you have linker issues
#ifdef BW_STATIC
#define BWDEF static
#else
#ifdef __cplusplus
#define BWDEF extern "C"
#else
#define BWDEF extern
#endif
#endif

typedef uint8_t bw_bit_t;
typedef uint8_t bw_byte_t;

/**
 * @struct BitWriter
 * @brief The writer/reader context structure.
 * The internal buffer is allocated on dynamic memory
*/
typedef struct BitWriter {
  bw_byte_t *buf; // Buffer for bits
  size_t pos; // Current index of a bit in buf, equivalent to count - 1
  size_t capacity;
} BitWriter;

/**
 * @brief Initialize writer's buffer and other fields
 * @param bw Bit Writer context
 * @param init_capacity Initial capacity of buffer will have
 * @return @p bw itself to daisy chain
*/
BWDEF BitWriter *bw_init(BitWriter *bw, size_t init_capacity);

/**
 * @brief Instantly create a new context
 * @param init_capacity Initial capacity of buffer will have
 * @return Constructed structure
*/
BWDEF BitWriter bw_new(size_t init_capacity);

/**
 * @brief Frees and zeroes the structure
 * @param bw Bit Writer context
*/
BWDEF void bw_free(BitWriter *bw);

/**
 * @brief Reserve @p extra amount of bytes
 * @param bw Bit Writer context
 * @param extra Amount of bytes to reserve in memory
 * @return True if reallocation succeeded false otherwise OR true if there's no need to reallocate
*/
BWDEF bool bw_reserve(BitWriter *bw, size_t extra);

/**
 * @brief Put a bit into the buffer (MSB-first)
 * @param bw Bit Writer context
 * @param bit Bit to write (1 or 0)
 * @return True if operation succeeded false otherwise
*/
BWDEF bool bw_put(BitWriter *bw, bw_bit_t bit);

/**
 * @brief Put several bits into the buffer
 * @param bw Bit Writer context
 * @param bits Set of bits to write (should be MSB-first order)
 * @param nb Indicates how many bits to write (Count of @p bits)
 * @return Amount of bits pushed (it should be equal to nb if everything succeeded)
*/
BWDEF size_t bw_puts(BitWriter *bw, uint64_t bits, size_t nb);

/**
 * @brief Get a bit from the buffer
 * @param bw Bit Writer context
 * @param pos Position of bit to get
 * @return Acquired bit
*/
BWDEF bw_bit_t bw_get(BitWriter *bw, size_t pos);

/**
 * @brief Get several bits from the buffer
 * @param bw Bit Writer context
 * @param pos Start position to get bits
 * @param n How many bits to get (must be less than or equal to 64)
 * @return Several acquired bits packed into one single uint64_t
*/
BWDEF uint64_t bw_gets(BitWriter *bw, size_t pos, size_t n);

/**
 * @brief Get a bit that pos is currently pointing at
 * @param bw Bit Writer context
 * @return Acquired bit
*/
BWDEF bw_bit_t bw_get_current(BitWriter *bw);

/**
 * @brief Get several bits that pos is currently pointing at
 * @param bw Bit Writer context
 * @return Several acquired bits packed into one single uint64_t
*/
BWDEF uint64_t bw_gets_current(BitWriter *bw, size_t n);

#if defined(BW_IMPL) || defined(BW_IMPLEMENTATION)
#include <stdlib.h>
#include <string.h>

BWDEF BitWriter bw_new(size_t init_capacity)
{
  BitWriter bw;
  return *bw_init(&bw, init_capacity);
}

BWDEF BitWriter *bw_init(BitWriter *bw, size_t init_capacity)
{
  bw->pos = 0;
  bw->capacity = init_capacity;
  bw->buf = calloc(init_capacity, sizeof(*bw->buf));
  return bw;
}

BWDEF void bw_free(BitWriter *bw)
{
  free(bw->buf);
  *bw = (BitWriter){0};
}

BWDEF bool bw_reserve(BitWriter *bw, size_t extra)
{
  size_t needed = bw->pos + 1 + extra;
  if (needed < bw->capacity) return true;
  size_t new_cap = bw->capacity;
  if (new_cap == 0) new_cap = 8;
  while (new_cap > needed) new_cap *= 2;
  void *tmp = realloc(bw->buf, new_cap * sizeof(*bw->buf));
  if (!tmp) return false;
  bw->buf = tmp;
  bw->capacity = new_cap;
  return true;
}

BWDEF bool bw_put(BitWriter *bw, bw_bit_t bit)
{
  if (bw->pos + 1 >= bw->capacity) {
    if (!bw_reserve(bw, 1)) return false;
  }
  if (bit) bw->buf[bw->pos >> 3] |= 0x80 >> (bw->pos & 7);
  bw->pos++;
  return true;
}

BWDEF size_t bw_puts(BitWriter *bw, uint64_t bits, size_t nb)
{
  size_t r = 0;
  if (!bw_reserve(bw, nb)) return r;
  for (size_t i = nb; i > 0; i--) {
    if (!bw_put(bw, (bits >> (i-1)) & 1)) break;
    r++;
  }
  return r;
}

BWDEF bw_bit_t bw_get(BitWriter *bw, size_t pos)
{
  if (pos > bw->pos) return 0;
  return (bw->buf[pos >> 3] >> (7 - (pos & 7))) & 1;
}

BWDEF uint64_t bw_gets(BitWriter *bw, size_t pos, size_t n)
{
  if (n > 64) return 0;
  uint64_t r = 0;
  for (size_t i = 0; i < n; i++) {
    r = (r << 1) | bw_get(bw, pos + i);
  }
  return r;
}

BWDEF bw_bit_t bw_get_current(BitWriter *bw)
{
  return bw_get(bw, bw->pos);
}

BWDEF uint64_t bw_gets_current(BitWriter *bw, size_t n)
{
  return bw_gets(bw, bw->pos, n);
}

#endif // BW_IMPL || BW_IMPLEMENTATION

#endif // BW_H
