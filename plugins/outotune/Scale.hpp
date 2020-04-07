#include <memory>
#include <cmath>

enum Tone {
	C = 0,
	Cis = 1,
	Db = 1,
	D = 2,
	Dis = 3,
	Eb = 3,
	E = 4,
	F = 5,
	Fis = 6,
	Gb = 6,
	G = 7,
	Gis = 8,
	Ab = 8,
	A = 9,
	Ais = 10,
	Bb = 10,
	B = 11
};

class Scale {
public:
	Scale() {}
	virtual float nearest_tone(float) { return 0; }
	virtual ~Scale() {}
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

protected:
	static constexpr float reference_semitones = 36.37631656229593;
	// MIDI note 0, a C
};

extern std::unique_ptr<Scale> createScale();
