#ifndef OUTOTUNE_BUFFER_H
#define OUTOTUNE_BUFFER_H

#include <cassert>
#include <vector>

using Buffer = std::vector<float>;

float *buffer_get_push_ptr(Buffer &b, size_t cnt);
void buffer_push(Buffer &b, size_t count, const float *more);
void buffer_push(Buffer &b, const std::vector<float> &more);
void buffer_shift(Buffer &b, size_t count, const float *more);
void buffer_pop(Buffer &b, size_t count, float *out=nullptr);
void buffer_pad(Buffer &b, size_t size);

#endif
