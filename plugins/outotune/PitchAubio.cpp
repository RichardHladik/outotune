#include "Pitch.hpp"
#include <memory>
#include <aubio/aubio.h>


const float silence_threshold = -70;

class AubioPitchEstimator : public PitchEstimator {
public:
	AubioPitchEstimator(size_t buffer_size, float rate) : PitchEstimator(buffer_size, rate) {
		buffer = new_fvec(buffer_size),
		out = new_fvec(1);
		aubio_pitch = new_aubio_pitch("yinfast", buffer_size, 1, rate);
		aubio_pitch_set_silence(aubio_pitch, silence_threshold);
	}

	void feed(const float *in, size_t frames) override {
		if (frames > buffer_size) {
			for (size_t i = 0; i < buffer_size; i++)
				buffer->data[i] = in[i];
			return;
		}

		for (size_t i = 0; i < buffer_size - frames; i++)
			buffer->data[i] = buffer->data[i + frames];

		for (size_t i = 0; i < frames; i++)
			buffer->data[buffer_size - frames + i] = in[i];
	}

	float estimate() override {
		aubio_pitch_do(aubio_pitch, buffer, out);
		return out->data[0];
	}

	float getConfidence() override {
		return aubio_pitch_get_confidence(aubio_pitch);
	}

	~AubioPitchEstimator() override {}

private:
	fvec_t *buffer;
	fvec_t *out;
	aubio_pitch_t *aubio_pitch;
};

std::unique_ptr<PitchEstimator> createPitchEstimator(size_t buffer_size, float rate) {
	return std::make_unique<AubioPitchEstimator>(buffer_size, rate);
}
