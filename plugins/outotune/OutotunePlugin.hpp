#include "DistrhoPlugin.hpp"
#include <atomic>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <aubio/aubio.h>

#undef NDEBUG
#include "Pitch.hpp"
#include "Scale.hpp"
#include "Correction.hpp"
#include "PitchShifting.hpp"
#include "World.cpp"

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
		world = createWorld(frames, rate);
		ola = std::make_unique<OLA>(1024);
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
			if (!tick && gPitch == 0)
				return -INFINITY; // TODO: VERY ugly hack
			return tick ? gPitch : -gPitch;
			break;
		case 2:
			if (!tick && gNearest == 0)
				return -INFINITY;
			return tick ? gNearest : -gNearest;
			break;
		case 3:
			if (!tick && gCorrected == 0)
				return -INFINITY;
			return tick ? gCorrected : -gCorrected;
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

	void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent *events, uint32_t eventCount) override {
		// get the mono input and output
		const float* const in  = inputs[0];
		float* const out = outputs[0];
		for (uint32_t i=0; i < frames; i++)
			out[i] = 0;
		world->feed(in, frames);

		auto npitch = world->estimate();

		if (npitch < 50 || npitch > 5000)
			npitch = 0;

		float pitch = gPitch, nearest = gNearest, corrected = gCorrected;
		pitch = npitch;
		nearest = pitch ? scale->nearest_tone(pitch) : 0;
		corrected = pitch ? correction->calculate(pitch, nearest) : 0;
		corrected = nearest;
		for (size_t i = 0; i < eventCount; i++) {
			auto e = events[i];
			if (e.size != 3)
				continue;
			char type = e.data[0] >> 4;
			const char NOTE_ON = 0x9;
			const char NOTE_OFF = 0x8;
			if (type != NOTE_ON && type != NOTE_OFF)
				continue;
			bool on = type == NOTE_ON;
			int note = e.data[1];
			if (on)
				active_notes.insert(note);
			else
				active_notes.erase(note);
			std::cout << (on ? "ON" : "OFF") << " " << note << "  " << active_notes.size() << std::endl;
		}

		if (active_notes.size()) {
			auto semitone = *active_notes.begin();
			corrected = Scale::semitones_to_freq(semitone);
		}
		std::cout << nearest << " " << corrected << " " << pitch << std::endl;

		auto orig = world->orig();
		for (uint32_t i=0; i < frames; i++)
			out[i] = 1 * in[i];
		//shifter->feed(in, frames, out, ratio);
		world->shiftTo(corrected);
		for (uint32_t i=0; i < frames; i++)
			out[i] += 1 * world->out()[i];
		gPitch = pitch, gNearest = nearest, gCorrected = corrected;
		return;
	}

private:
	float silence_threshold = -50;
	fvec_t *aubio_out;
	aubio_wavetable_t *wavetable;
	std::unique_ptr<PitchEstimator> estimator;
	std::unique_ptr<Scale> scale;
	std::unique_ptr<Correction> correction;
	std::unique_ptr<PitchShifter> shifter;
	std::unique_ptr<World> world;
	std::unique_ptr<OLA> ola;
	std::set<int> active_notes;
	float gPitch = 0;
	float gNearest = 0;
	float gCorrected = 0;
	bool tick = false;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
