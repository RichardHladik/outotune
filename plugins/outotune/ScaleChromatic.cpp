#include <cmath>
#include <iostream>
#include "Scale.hpp"

class ChromaticScale : public Scale {
public:
	ChromaticScale() {}
	float nearest_tone(float f) override {
		auto semi = freq_to_semitones(f);
		semi = roundf(semi);
		f = semitones_to_freq(semi);
		std::cout << f << std::endl;
		return f;
	}
	~ChromaticScale() override {}
private:
	static constexpr float reference_semitones = 105.37631656229593;

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

std::unique_ptr<Scale> createScale() {
	return std::make_unique<ChromaticScale>();
}
