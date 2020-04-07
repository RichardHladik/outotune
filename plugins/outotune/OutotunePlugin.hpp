#include "DistrhoPlugin.hpp"
#include <atomic>
#include <cmath>
#include <iostream>
#include <aubio/aubio.h>

#include "Pitch.hpp"
#include "Scale.hpp"
#include "Correction.hpp"
#include "PitchShifting.hpp"

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin(4, 0, 0) {
		size_t frames = getBufferSize();
		size_t internalFrames = 4096;

		auto rate = getSampleRate();
		aubio_out = new_fvec(frames);
		wavetable = new_aubio_wavetable(rate, frames);
		aubio_wavetable_play(wavetable);
		estimator = createPitchEstimator(internalFrames, rate);
		scale = createScale();
		correction = createCorrection();
		shifter = createPitchShifter(frames, rate);
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
			param.description = "Raw detected pitch";
			param.symbol = "pitch";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = -param.ranges.max;
			param.ranges.def = 0;
			break;
		case 2:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Nearest";
			param.description = "Nearest pitch of the scale";
			param.symbol = "nearest";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = -param.ranges.max;
			param.ranges.def = 0;
			break;
		case 3:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Corrected";
			param.description = "Corrected pitch (usually somewhere between `pitch` and `nearest`";
			param.symbol = "corrected";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = -param.ranges.max;
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
			if (!tick && pitch == 0)
				return -INFINITY; // TODO: VERY ugly hack
			return tick ? pitch : -pitch;
			break;
		case 2:
			if (!tick && nearest == 0)
				return -INFINITY;
			return tick ? nearest : -nearest;
			break;
		case 3:
			if (!tick && corrected == 0)
				return -INFINITY;
			return tick ? corrected : -corrected;
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
		DISTRHO_SAFE_ASSERT(false);
		// TODO
		//aubio_pitch_set_silence(pitch, silence_threshold);
	}

	void run(const float** inputs, float** outputs, uint32_t frames) override {
		// get the mono input and output
		const float* const in  = inputs[0];
		float* const out = outputs[0];
		estimator->feed(in, frames);

		auto npitch = estimator->estimate();
		auto confidence = estimator->getConfidence();

		if (npitch < 50 || npitch > 5000 || confidence < .6)
			npitch = 0;

		pitch = npitch;
		nearest = pitch ? scale->nearest_tone(pitch) : 0;
		corrected = pitch ? correction->calculate(pitch, nearest) : 0;
		corrected = nearest;

		for (uint32_t i=0; i < frames; i++)
			out[i] = 0;
		float ratio = (pitch && corrected) ? corrected / pitch : 1;
		shifter->feed(in, frames, out, ratio);
		return;

		if (!pitch) {
			for (uint32_t i=0; i < frames; i++)
				out[i] = 0;
			return;
		}
		aubio_wavetable_set_amp(wavetable, confidence * .5);
		aubio_wavetable_set_freq(wavetable, corrected);
		aubio_wavetable_do(wavetable, NULL, aubio_out);
		for (uint32_t i=0; i < frames; i++)
			out[i] = aubio_out->data[i];

	}

private:
	float silence_threshold = -50;
	fvec_t *aubio_out;
	aubio_wavetable_t *wavetable;
	std::unique_ptr<PitchEstimator> estimator;
	std::unique_ptr<Scale> scale;
	std::unique_ptr<Correction> correction;
	std::unique_ptr<PitchShifter> shifter;
	float pitch = 0;
	float nearest = 0;
	float corrected = 0;
	bool tick = false;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
