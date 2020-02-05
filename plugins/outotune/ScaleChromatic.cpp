#include <cmath>
#include "Scale.hpp"

class ChromaticScale : public Scale {
public:
	ChromaticScale() {}
	float nearest_tone(float f) override {
		auto semi = freq_to_semitones(f);
		semi = roundf(semi);
		f = semitones_to_freq(semi);
		return f;
	}
	~ChromaticScale() override {}
private:
};

std::unique_ptr<Scale> createScale() {
	return std::make_unique<ChromaticScale>();
}
