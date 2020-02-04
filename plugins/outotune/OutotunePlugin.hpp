#include "DistrhoPlugin.hpp"
#include <atomic>
#include <aubio/aubio.h>
#include <cmath>
#include <iostream>

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin(2, 0, 0), silence_threshold(-70) {
		size_t frames = getBufferSize();
		aubio_in = new_fvec(frames);
		aubio_out = new_fvec(1);
		pitch = new_aubio_pitch("default", frames, frames, getSampleRate());
		aubio_pitch_set_silence(pitch, silence_threshold);
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
		default:
			DISTRHO_SAFE_ASSERT(false);
			break;
		}
	}

	float getParameterValue(uint32_t index) const override {
		switch (index) {
		case 0:
			return silence_threshold;
			break;
		case 1:
			return pitch_estimate;
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
		for (uint32_t i=0; i < frames; ++i) {
			out[i] = in[i];
			aubio_in->data[i] = in[i];
		}

		aubio_pitch_do(pitch, aubio_in, aubio_out);
		pitch_estimate = aubio_out->data[0];
		std::cout << frames << " " << pitch_estimate << std::endl;
	}

private:
	float silence_threshold;
	fvec_t *aubio_in;
	fvec_t *aubio_out;
	aubio_pitch_t *pitch;
	float pitch_estimate;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
