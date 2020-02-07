#include "Pitch.hpp"
#include <iostream>
#include <vector>
#include <pitch_detection.h>

class PDPitchEstimator : public PitchEstimator {
public:
	PDPitchEstimator(size_t buffer_size, float rate) : PitchEstimator(buffer_size, rate), pd(buffer_size), buffer(buffer_size) {}

	void feed(const float *in, size_t frames) override {
		std::cout << buffer_size << " " << frames << std::endl;
		if (frames > buffer_size) {
			for (size_t i = 0; i < buffer_size; i++)
				buffer[i] = in[i];
			return;
		}

		for (size_t i = 0; i < buffer_size - frames; i++)
			buffer[i] = buffer[i + frames];

		for (size_t i = 0; i < frames; i++)
			buffer[buffer_size - frames + i] = in[i];
	}

	float estimate() override {
		auto p = pitch::yin(buffer, rate);
//		auto p = pd.pitch(buffer, rate);
		std::cout << p << std::endl;
		return p;
	}

	float getConfidence() override {
		return 1;
	}

	~PDPitchEstimator() override {}

private:
	pitch_alloc::Yin<float> pd;
	std::vector<float> buffer;
};

std::unique_ptr<PitchEstimator> createPitchEstimator(size_t buffer_size, float rate) {
	return std::make_unique<PDPitchEstimator>(buffer_size, rate);
}
