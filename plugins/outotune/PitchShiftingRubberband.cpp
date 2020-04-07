#include "PitchShifting.hpp"
#include <cmath>
#include <rubberband/RubberBandStretcher.h>
#include <vector>

using RubberBand::RubberBandStretcher;

class RubberbandPitchShifter : public PitchShifter {
public:
	RubberbandPitchShifter(size_t _buffer_size, float _rate) : PitchShifter(_buffer_size, _rate), rs(rate, 1, RubberBandStretcher::DefaultOptions | RubberBandStretcher::OptionProcessRealTime | RubberBandStretcher::OptionFormantPreserved | RubberBandStretcher::OptionPitchHighQuality | RubberBandStretcher::OptionWindowShort)  {
	}

	void feed(const float *in, size_t size, float *out, float shift) override {
		rs.setPitchScale(shift);
		rs.process(&in, size, false);
		size_t available = rs.available();
		rs.retrieve(&out, std::min(size, available));
		for (size_t i = 0; available + i < size; available++)
			out[available + i] = out[i];
	}

private:
	static double semitones_to_shift(double semitones) {
		return pow(2., semitones / 12);
	}

	RubberBandStretcher rs;
};

std::unique_ptr<PitchShifter> createPitchShifter(size_t buffer_size, float rate) {
	return std::make_unique<RubberbandPitchShifter>(buffer_size, rate);
}
