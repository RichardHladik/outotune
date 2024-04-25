#include <cassert>
#include <vector>

template<typename T> using Buffer = std::vector<T>;

template<typename T, typename U> void buffer_feed(Buffer<T> &b, size_t count, const U *more) {
	assert(count <= b.size());

	for (size_t i = 0; i < b.size() - count; i++)
		b[i] = b[i + count];
	for (size_t i = 0; i < count; i++)
		b[b.size() - count + i] = more[i];
}

template<typename T, typename U> void buffer_feed(Buffer<T> &b, const U more) {
	return buffer_feed(b, 1, &more);
}
