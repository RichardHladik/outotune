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

template<typename T, typename U> void buffer_push(Buffer<T> &b, U more) {
	return buffer_push(b, 1, &more);
}

template<typename T, typename U> void buffer_push(Buffer<T> &b, const std::vector<U> &more) {
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

template<typename T, typename U> void buffer_exchange(Buffer<T> &b, size_t count, const U *more) {
	buffer_pop(b, count, (T*)nullptr);
	buffer_push(b, count, more);
}

template<typename T, typename U> void buffer_exchange(Buffer<T> &b, U more) {
	return buffer_exchange(b, 1, &more);
}
