#include <simd.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/field.h"

bool
field_get (const field_t field, size_t bit) {
  size_t offset = bit & 7;
  size_t i = (bit - offset) / 8;

  return (field[i] & (1 << offset)) != 0;
}

bool
field_set (field_t field, size_t bit, bool value) {
  size_t offset = bit & 7;
  size_t i = (bit - offset) / 8;
  size_t mask = 1 << offset;

  if (value) {
    if ((field[i] & mask) != 0) {
      return false;
    }
  } else {
    if ((field[i] & mask) == 0) {
      return false;
    }
  }

  field[i] ^= mask;

  return true;
}

void
field_index_init (field_index_t index, const field_t field, size_t field_len, bool value) {
  for (size_t i = 0; i < 128; i++) {
    int16_t total = 0;

    for (size_t j = 0; j < 128; j++) {
      size_t offset = (i * 128 + j) * 16;
      int16_t count = -1;

      if (offset + 16 <= field_len) {
        count = simd_sum_v128_u8(simd_cnt_v128_u8(simd_load_v128_u8(&field[offset])));
      }

      field_set(index, i * 128 + j + 128, count == (value ? 128 : 0));

      total += count;
    }

    field_set(index, i, total == (value ? 16384 : 0));
  }
}
