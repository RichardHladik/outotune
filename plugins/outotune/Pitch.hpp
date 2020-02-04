#include <cstddef>
#include <memory>

class PitchEstimator {
public:
	PitchEstimator(size_t buffer_size, float rate) : buffer_size(buffer_size), rate(rate) {}
	virtual void feed(const float *, size_t) {}
	virtual float estimate() { return 0; }
	virtual float getConfidence() { return 0; }
	virtual ~PitchEstimator() {}
protected:
	size_t buffer_size;
	float rate;
};

extern std::unique_ptr<PitchEstimator> createPitchEstimator(size_t buffer_size, float rate);
