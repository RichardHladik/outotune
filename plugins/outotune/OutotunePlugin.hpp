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
#include "World.hpp"

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin((int)pId::_count, 0, 0) {
		size_t frames = getBufferSize();

		auto rate = getSampleRate();
		scale = createScale();
		correction = createCorrection();
		world = createWorld(frames, rate);
		setLatency(world->latency);
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
		pId paramId = castToEnum<pId>(index, pId::_count);

		switch (paramId) {
		case pId::pitch:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Pitch";
			param.description = "Raw detected pitch";
			param.symbol = "pitch";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = 0;
			param.ranges.def = 0;
			break;
		case pId::nearest:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Nearest";
			param.description = "Nearest pitch of the scale";
			param.symbol = "nearest";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = 0;
			param.ranges.def = 0;
			break;
		case pId::corrected:
			param.hints = kParameterIsAutomable | kParameterIsOutput;
			param.name = "Corrected";
			param.description = "Corrected pitch (usually somewhere between `pitch` and `nearest`";
			param.symbol = "corrected";
			param.ranges.max = getSampleRate() / 2;
			param.ranges.min = 0;
			param.ranges.def = 0;
			break;
		case pId::midiMode:
			param.hints = kParameterIsInteger;
			param.name = "MIDI mode";
			param.symbol = "midi_mode";
			param.description = "How should the MIDI input be interpreted";
			{
				ParameterEnumerationValue* const values = new ParameterEnumerationValue[2];
				param.enumValues.values = values;

				values[0].label = "Absolute";
				values[0].value = (int)MidiMode::absolute;
				values[1].label = "Relative";
				values[1].value = (int)MidiMode::relative;
			}
			param.enumValues.count = (int)MidiMode::_count;
			param.enumValues.restrictedMode = true;
			break;
		case pId::passThrough:
			param.hints = kParameterIsBoolean;
			param.name = "Add input";
			param.symbol = "add_input";
			param.description = "Include the original signal in the output";
			param.ranges.max = 1;
			param.ranges.min = 0;
			param.ranges.def = 1;
			break;
		case pId::_count:
			// bad index passed, probably bug in port numbering / on the host side
			DISTRHO_SAFE_ASSERT(false);
			break;
		default:
			// not implemented
			DISTRHO_SAFE_ASSERT(false);
			break;
		}
	}

	float getParameterValue(uint32_t index) const override {
		pId paramId = castToEnum<pId>(index, pId::_count);
		switch (paramId) {
		case pId::pitch:
			return gPitch;
		case pId::nearest:
			return gNearest;
		case pId::corrected:
			return gCorrected;
		case pId::midiMode:
			return (float)midiMode;
		case pId::passThrough:
			return passThrough;
			break;
		case pId::_count:
			// bad index passed, probably bug in port numbering / on the host side
			DISTRHO_SAFE_ASSERT(false);
			break;
		default:
			// not implemented
			DISTRHO_SAFE_ASSERT(false);
			break;
		}

		return 0;
	}

	void setParameterValue(uint32_t index, float val) override {
		pId paramId = castToEnum<pId>(index, pId::_count);
		switch (paramId) {
			case pId::midiMode:
				midiMode = castToEnum<MidiMode>(val, MidiMode::absolute);
				break;
			case pId::passThrough:
				passThrough = val;
				break;
			default:
				// either not implemented, or bug on the host's side (trying to
				// set a read-only parameter etc.)
				DISTRHO_SAFE_ASSERT(false);
				return;
		}
	}

	static void addWeighted(size_t frames, float *out, float weight, const double *in) {
		for (size_t i=0; i < frames; i++)
			out[i] += weight * in[i];
	}

	void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent *events, uint32_t eventCount) override {
		// get the mono input and output
		const float* const in  = inputs[0];
		float* const out = outputs[0];
		if (frames != world->frameSize)
			world = createWorld(frames, world->rate);

		// we need to feed the input to world first, since the host can reuse
		// the input buffer for output and zeroing output would also zero input
		world->feed(in, frames);
		for (uint32_t i=0; i < frames; i++)
			out[i] = 0;

		auto npitch = world->estimate();

		float pitch = gPitch, nearest = gNearest, corrected = gCorrected;
		pitch = npitch;
		nearest = pitch ? scale->nearest_tone(pitch) : 0;
		corrected = pitch ? correction->calculate(pitch, nearest) : 0;
		corrected = nearest;

#if DISTRHO_PLUGIN_WANT_MIDI_INPUT == 1
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
#endif

		if (active_notes.size()) {
			auto semitone = active_notes.begin()->first;
			corrected = Scale::semitones_to_freq(semitone);
		}

		if (passThrough)
			addWeighted(frames, out, 1, world->orig());
		/*addWeighted(frames, out, .5, synth1->shiftBy(-12));
		addWeighted(frames, out, 1.5, synth2->shiftBy(12)); */
		for (auto &&a : active_notes) {
			switch (midiMode) {
				case MidiMode::absolute:
					addWeighted(frames, out, 1, a.second->shiftToNote(a.first));
					break;
				case MidiMode::relative:
					addWeighted(frames, out, 1, a.second->shiftBy(a.first - 60));
					break;
				default:
					DISTRHO_SAFE_ASSERT(false);
			}
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
	enum MidiMode midiMode = MidiMode::absolute;
	bool passThrough = true;
};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
