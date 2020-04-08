#include "PitchShifting.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <rubberband/RubberBandStretcher.h>
#include <vector>

#include "Buffer.hpp"

using RubberBand::RubberBandStretcher;

class RubberbandPitchShifter : public PitchShifter {
public:
	RubberbandPitchShifter(size_t _buffer_size, float _rate) : PitchShifter(_buffer_size, _rate), rs(rate, 1, RubberBandStretcher::DefaultOptions | RubberBandStretcher::OptionProcessRealTime | RubberBandStretcher::OptionPitchHighQuality)  {
		inBuf.resize(_buffer_size);
		minFill = _buffer_size;
		maxFill = 4 * _buffer_size;
	}

	void feed(const float *in, size_t size, float *out, float shift) override {
		buffer_push(inBuf, size, in);
		rs.setPitchScale(shift);

		float invshift = 1 / shift;
		if (outBuf.size() < minFill)
			rs.setTimeRatio(std::clamp(invshift, 1.01f, 1.5f));
		else if (outBuf.size() > maxFill)
			rs.setTimeRatio(std::clamp(invshift, 0.99f, 0.8f));
		else
			rs.setTimeRatio(1);

		if (rs.getTimeRatio() != 1)
			std::cout << rs.getTimeRatio() << std::endl;

		while (true) {
			auto required = rs.getSamplesRequired();
			if (inBuf.size() < required) {
				if (outBuf.size() >= size)
					break;
				std::cerr << "Buffer underrun" << std::endl;
				buffer_pad(inBuf, required);
			}

			float *ptr = inBuf.data();
			rs.process(&ptr, required, false);
			buffer_pop(inBuf, required);
			size_t available = rs.available();
			ptr = buffer_get_push_ptr(outBuf, available);
			size_t retrieved = rs.retrieve(&ptr, available);
			assert(available == retrieved);
			//std::cout << available << " avail, " << inBuf.size() << " in, " << outBuf.size() << " out, " << rs.getTimeRatio() << std::endl;
		}

		buffer_pad(outBuf, size);
		buffer_pop(outBuf, size, out);
	}

private:
	static double semitones_to_shift(double semitones) {
		return pow(2., semitones / 12);
	}

	RubberBandStretcher rs;
	std::vector<float> outBuf;
	std::vector<float> inBuf;
	size_t minFill, maxFill;
};

std::unique_ptr<PitchShifter> createPitchShifter(size_t buffer_size, float rate) {
	return std::make_unique<RubberbandPitchShifter>(buffer_size, rate);
}
