#include "DistrhoPlugin.hpp"
#include <aubio/aubio.h>
#include <cmath>
#include <iostream>

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin(0, 0, 0), silence_threshold(-70) {
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

	void initParameter(uint32_t, Parameter &param) override {
		param.hints = kParameterIsAutomable;
		param.name = "Silence threshold";
		param.symbol = "silence";
		param.ranges.min = -120;
		param.ranges.max = 0;
		param.ranges.def = silence_threshold;
	}

	float getParameterValue(uint32_t) const override {
		return silence_threshold;
	}
	void setParameterValue(uint32_t, float val) override {
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
		std::cout << frames << " " << aubio_out->data[0] << std::endl;
	}

private:
	float silence_threshold;
	fvec_t *aubio_in;
	fvec_t *aubio_out;
	aubio_pitch_t *pitch;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
