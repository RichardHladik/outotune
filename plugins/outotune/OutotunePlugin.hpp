#include "DistrhoPlugin.hpp"
#include <atomic>
#include <aubio/aubio.h>
#include <cmath>
#include <iostream>

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin(2, 0, 0) {
		size_t frames = getBufferSize();
		internalFrames = 4096;
		aubio_in = new_fvec(internalFrames);
		aubio_pitch = new_fvec(1);
		aubio_out = new_fvec(frames);
		auto rate = getSampleRate();

		pitch = new_aubio_pitch("yinfast", internalFrames, frames, rate);
		aubio_pitch_set_silence(pitch, silence_threshold);

		wavetable = new_aubio_wavetable(rate, frames);
		aubio_wavetable_play(wavetable);
	}

private:
	const char* getLabel() const override {
		return DISTRHO_PLUGIN_NAME;
	}
	const char* getMaker() const override {
		return DISTRHO_PLUGIN_AUTHOR;
	}
	const char* getLicense() const override {
		return DISTRHO_PLUGIN_LICENSE;
	}
	uint32_t getVersion() const override {
		return d_version(0, 0, 0);
	}
	int64_t getUniqueId() const override {
		return d_cconst('O', 't', 'u', 'n');
	}

	void initParameter(uint32_t index, Parameter &param) override {
		switch (index) {
		case 0:
			param.hints = kParameterIsAutomable;
			param.name = "Silence threshold";
			param.symbol = "silence";
			param.ranges.min = -120;
			param.ranges.max = 0;
			param.ranges.def = silence_threshold;
			break;
		case 1:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Pitch";
			param.symbol = "pitch";
			param.ranges.min = -1;
			param.ranges.max = getSampleRate() / 2;
			param.ranges.def = 0;
			break;
		case 2:
			param.hints = kParameterIsAutomable | kParameterIsOutput | kParameterIsBoolean;
			param.name = "Tick";
			param.symbol = "tick";
			param.ranges.min = 0;
			param.ranges.max = 1;
			param.ranges.def = 0;
			break;
		default:
			DISTRHO_SAFE_ASSERT(false);
			break;
		}
	}

	float getParameterValue(uint32_t index) const override {
		auto hack = (OutotunePlugin *)this;
		switch (index) {
		case 0:
			return silence_threshold;
			break;
		case 1:
			hack->tick = !tick;
			return (pitch_estimate == 0) ? -tick : pitch_estimate;
			break;
		case 2:
			return tick;
			break;
		default:
			DISTRHO_SAFE_ASSERT(false);
			break;
		}
		return 0;
	}

	void setParameterValue(uint32_t index, float val) override {
		if (index)
			return;
		silence_threshold = val;
		aubio_pitch_set_silence(pitch, silence_threshold);
	}

	void run(const float** inputs, float** outputs, uint32_t frames) override {
		// get the mono input and output
		const float* const in  = inputs[0];
		float* const out = outputs[0];
		for (uint32_t i=0; i < internalFrames - frames; i++)
			aubio_in->data[i] = aubio_in->data[i + frames];

		for (uint32_t i=0; i < frames; i++)
			aubio_in->data[i + internalFrames - frames] = in[i];

		aubio_pitch_do(pitch, aubio_in, aubio_pitch);
		confidence = aubio_pitch_get_confidence(pitch);
		pitch_estimate = aubio_pitch->data[0];

		if (pitch_estimate < 100 || pitch_estimate > 5000 || confidence < .6)
			pitch_estimate = 0;

		aubio_wavetable_set_amp(wavetable, confidence * .5);
		aubio_wavetable_set_freq(wavetable, pitch_estimate);
		aubio_wavetable_do(wavetable, NULL, aubio_out);
		for (uint32_t i=0; i < frames; i++)
			out[i] = aubio_out->data[i];

	}

private:
	float silence_threshold = -50;
	fvec_t *aubio_in;
	fvec_t *aubio_pitch;
	fvec_t *aubio_out;
	aubio_pitch_t *pitch;
	aubio_wavetable_t *wavetable;
	float pitch_estimate = 0;
	float confidence = 0;
	bool tick = false;
	size_t internalFrames;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
