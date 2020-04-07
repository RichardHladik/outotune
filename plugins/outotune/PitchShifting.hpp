#include <cstddef>
#include <memory>

class PitchShifter {
public:
	PitchShifter(size_t _buffer_size, float _rate) : buffer_size(_buffer_size), rate(_rate) {}
	virtual void feed(const float *, size_t, float *, float) {}
	virtual ~PitchShifter() {}
protected:
	size_t buffer_size;
	size_t rate;
};

extern std::unique_ptr<PitchShifter> createPitchShifter(size_t buffer_size, float rate);
