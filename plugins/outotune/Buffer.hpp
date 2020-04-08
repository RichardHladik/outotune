#include <cassert>
#include <vector>

using Buffer = std::vector<float>;

float *buffer_get_push_ptr(Buffer &b, size_t cnt) {
	auto sz = b.size();
	b.resize(b.size() + cnt);
	return b.data() + sz;
}

void buffer_push(Buffer &b, size_t count, const float *more) {
	auto *p = buffer_get_push_ptr(b, count);
	for (size_t i = 0; i < count; i++)
		*(p++) = more[i];
}

void buffer_push(Buffer &b, const std::vector<float> &more) {
	return buffer_push(b, more.size(), more.data());
}

void buffer_pop(Buffer &b, size_t count, float *out=nullptr) {
	assert(count <= b.size());
	if (out)
		for (size_t i = 0; i < count; i++)
			out[i] = b[i];

	for (size_t i = 0; i < b.size() - count; i++)
		b[i] = b[i + count];
	b.resize(b.size() - count);
}

void buffer_pad(Buffer &b, size_t size) {
	if (b.size() >= size)
		return;
	auto sz = b.size();
	b.resize(size);
	for (size_t i = sz; i < size; i++)
		b[i] = i % 8 < 4 ? .3 : -.3;
/*	for (size_t i = 0; i < size - sz; i++)
		b[sz + i] = b[i]; */
}
