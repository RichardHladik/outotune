#include "DistrhoPlugin.hpp"
#include <atomic>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <map>

#undef NDEBUG
#include "Constants.hpp"
#include "Scale.hpp"
#include "Correction.hpp"
#include "World.cpp"

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin(DISTRHO_PLUGIN_NUM_PARAMETERS, 0, 0) {
		size_t frames = getBufferSize();

		auto rate = getSampleRate();
		scale = createScale();
		correction = createCorrection();
		world = createWorld(frames, rate);
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
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Pitch";
			param.description = "Raw detected pitch";
			param.symbol = "pitch";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = 0;
			param.ranges.def = 0;
			break;
		case 1:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Nearest";
			param.description = "Nearest pitch of the scale";
			param.symbol = "nearest";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = 0;
			param.ranges.def = 0;
			break;
		case 2:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Corrected";
			param.description = "Corrected pitch (usually somewhere between `pitch` and `nearest`";
			param.symbol = "corrected";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = 0;
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
			return gPitch;
		case 1:
			return gNearest;
		case 2:
			return gCorrected;
		default:
			DISTRHO_SAFE_ASSERT(false);
			break;
		}
		return 0;
	}

	//void setParameterValue(uint32_t index, float val) override {
	void setParameterValue(uint32_t, float) override {
		return;
		DISTRHO_SAFE_ASSERT(false);
	}

	void addWeighted(size_t frames, float *out, float weight, const double *in) {
		for (size_t i=0; i < frames; i++)
			out[i] += weight * in[i];
	}

	void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent *events, uint32_t eventCount) override {
		// get the mono input and output
		const float* const in  = inputs[0];
		float* const out = outputs[0];
		for (uint32_t i=0; i < frames; i++)
			out[i] = 0;
		world->feed(in, frames);

		auto npitch = world->estimate();

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
			if (on && !active_notes.count(note))
				active_notes.emplace(note, std::make_unique<World::Synthesizer>(*world));
			else
				active_notes.erase(note);
			std::cout << (on ? "ON" : "OFF") << " " << note << "  " << active_notes.size() << std::endl;
		}

		if (active_notes.size()) {
			auto semitone = active_notes.begin()->first;
			corrected = Scale::semitones_to_freq(semitone);
		}

		addWeighted(frames, out, 1, world->orig());
		/*addWeighted(frames, out, .5, synth1->shiftBy(-12));
		addWeighted(frames, out, 1.5, synth2->shiftBy(12)); */
		for (auto &&a : active_notes) {
			// addWeighted(frames, out, .5, a.second->shiftToNote(a.first));
			addWeighted(frames, out, 1, a.second->shiftBy(a.first - 60));
		}

		gPitch = pitch, gNearest = nearest, gCorrected = corrected;
		return;
	}

private:
	std::unique_ptr<Scale> scale;
	std::unique_ptr<Correction> correction;
	std::unique_ptr<World> world;
	std::map<int, std::unique_ptr<World::Synthesizer>> active_notes;
	float gPitch = 0;
	float gNearest = 0;
	float gCorrected = 0;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
