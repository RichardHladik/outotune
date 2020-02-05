#include "Scale.hpp"
#include <vector>

class GenericScale : public Scale {
public:
	GenericScale(std::vector<float> notes) : Scale(), notes(notes) {}
	float nearest_tone(float f) override {
		if (f < 5)
			return 0;
		auto semi = freq_to_semitones(f);
		float best = 0;
		double best_dist = INFINITY;
		for (auto note : notes) {
			auto closest = closest_octave(note, semi);
			auto dist = fabs(closest - semi);
			if (dist < best_dist) {
				best_dist = dist;
				best = closest;
			}
		}

		return semitones_to_freq(best);
	}
	~GenericScale() override {}
private:
	const std::vector<float> notes;

	static float closest_octave(float note, float pitch_semi) {
		float remainder = fmodf(pitch_semi - note, 12);
		if (remainder < 0)
			remainder += 12;
		float candidate1 = pitch_semi - remainder;
		float candidate2 = pitch_semi - remainder + 12;
		return fabs(pitch_semi - candidate1) < fabs(pitch_semi - candidate2) ? candidate1 : candidate2;
	}
};
