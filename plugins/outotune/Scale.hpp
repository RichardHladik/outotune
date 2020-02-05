#include <memory>
#include <cmath>

class Scale {
public:
	Scale() {}
	virtual float nearest_tone(float) { return 0; }
	virtual ~Scale() {}

protected:
	static constexpr float reference_semitones = 36.37631656229593;
	// MIDI note 0, a C

	static float freq_to_semitones(float f) {
		if (f < 5)
			return -INFINITY;
		return 12 * log2(f) - reference_semitones;
	}

	static float semitones_to_freq(float semi) {
		if (semi < -1000)
			return 0;
		return powf(2, (semi + reference_semitones) / 12);
	}
};

extern std::unique_ptr<Scale> createScale();
