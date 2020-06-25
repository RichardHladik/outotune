#include <cassert>
#include <vector>

template<typename T> using Buffer = std::vector<T>;

template<typename T> T *buffer_get_push_ptr(Buffer<T> &b, size_t cnt) {
	auto sz = b.size();
	b.resize(b.size() + cnt);
	return b.data() + sz;
}

template<typename T, typename U> void buffer_push(Buffer<T> &b, size_t count, const U *more) {
	auto *p = buffer_get_push_ptr(b, count);
	for (size_t i = 0; i < count; i++)
		*(p++) = (T)more[i];
}

template<typename T> void buffer_push(Buffer<T> &b, const std::vector<T> &more) {
	return buffer_push(b, more.size(), more.data());
}

template<typename T> void buffer_pop(Buffer<T> &b, size_t count, T *out=nullptr) {
	assert(count <= b.size());
	if (out)
		for (size_t i = 0; i < count; i++)
			out[i] = b[i];

	for (size_t i = 0; i < b.size() - count; i++)
		b[i] = b[i + count];
	b.resize(b.size() - count);
}

template<typename T> void buffer_pad(Buffer<T> &b, size_t size) {
	if (b.size() >= size)
		return;
	auto sz = b.size();
	b.resize(size);
	for (size_t i = sz; i < size; i++)
		b[i] = i % 8 < 4 ? .3 : -.3;
/*	for (size_t i = 0; i < size - sz; i++)
		b[sz + i] = b[i]; */
}

template<typename T, typename U> void buffer_exchange(Buffer<T> &b, size_t count, const U *more) {
	buffer_pop(b, count, (T*)nullptr);
	buffer_push(b, count, more);
}
