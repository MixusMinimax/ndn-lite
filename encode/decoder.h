/*
 * Copyright (C) 2018 Zhiyi Zhang
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef ENCODING_DECODER_H
#define ENCODING_DECODER_H

#include "block.h"

#ifdef __cplusplus
extern "C" {
#endif

// State keeper when doing encode
typedef struct ndn_decoder {
  const uint8_t* input_value;
  uint32_t input_size;
  uint32_t offset;
} ndn_decoder_t;

static inline void
decoder_init(ndn_decoder_t* decoder, const uint8_t* block_value, uint32_t block_size)
{
  decoder->input_value = block_value;
  decoder->input_size = block_size;
  decoder->offset = 0;
}

// function to get the type (T) and length (L)
// ndn_block_get_var_number
static inline int
decoder_get_var(ndn_decoder_t* decoder, uint32_t* var)
{
  uint8_t first_bit = decoder->input_value[decoder->offset];
  size_t rest_size = decoder->input_size - decoder->offset;
  if (first_bit < 253) {
    *var = first_bit;
    decoder->offset += 1;
  }
  else if (first_bit == 253 && rest_size >= 3) {
    *var = ((uint32_t)decoder->input_value[decoder->offset + 1] << 8)
      + decoder->input_value[decoder->offset + 2];
    decoder->offset += 3;
  }
  else if (first_bit == 254 && rest_size >= 5) {
    *var = ((uint32_t)decoder->input_value[decoder->offset + 1] << 24)
      + ((uint32_t)decoder->input_value[decoder->offset + 2] << 16)
      + ((uint32_t)decoder->input_value[decoder->offset + 3] << 8)
      + decoder->input_value[decoder->offset + 4];
    decoder->offset += 5;
  }
  else {
    return -1;
  }
  return 0;
}

static inline int
decoder_get_type(ndn_decoder_t* decoder, uint32_t* type)
{
  return decoder_get_var(decoder, type);
}

// must be invoked after decoder_get_type
static inline int
decoder_get_length(ndn_decoder_t* decoder, uint32_t* length)
{
  return decoder_get_var(decoder, length);
}

// get the value(V) size from a block
static inline size_t
decoder_probe_value_size(uint8_t* block_value, uint32_t block_size)
{
  ndn_decoder_t decoder;
  decoder_init(&decoder, block_value, block_size);
  uint32_t type;
  decoder_get_var(&decoder, &type);
  uint32_t length;
  decoder_get_length(&decoder, &length);
  return length;
}

// the buffer must have been initialized with the empty uint8_t array and
// right size obtained from decoder_get_length
int
decoder_get_buffer_value(ndn_decoder_t* decoder, ndn_buffer_t* value);

int
decoder_get_raw_buffer_value(ndn_decoder_t* decoder, uint8_t* value, uint32_t size);

#ifdef __cplusplus
}
#endif
#endif // ENCODING_DECODER_H
