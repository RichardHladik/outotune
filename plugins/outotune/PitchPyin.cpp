#include "Pitch.hpp"
#include <iostream>
#include <vector>
#include <libpyincpp.h>

class PyinPitchEstimator : public PitchEstimator {
public:
	PyinPitchEstimator(size_t buffer_size, float rate) : PitchEstimator(buffer_size, rate), pyin(rate), buffer(buffer_size) {}

	void feed(const float *in, size_t frames) override {
		std::vector<float> buf(frames);
		for (size_t i = 0; i < frames; i++)
			buf[i] = in[i];

		pyin.feed(buf);
	}

	float estimate() override {
		if (pyin.getPitches().size() == 0)
			return 0;
		auto ps = pyin.getSmoothedPitches();
		std::cout << ps.size() << std::endl;
		if (ps.size() >= 1) {
			for (int i = 0; i < 1; i++)
				std::cout << ps[ps.size() - 1 - i] << " ";
			std::cout << std::endl;
		}
		return ps.size() && ps.back() > 0 ? ps.back() : 0;
	}

	float getConfidence() override {
		return 1;
	}

	~PyinPitchEstimator() override {}

private:
	PyinCpp pyin;
	std::vector<float> buffer;
};

std::unique_ptr<PitchEstimator> createPitchEstimator(size_t buffer_size, float rate) {
	return std::make_unique<PyinPitchEstimator>(buffer_size, rate);
}
