#include <ScaleGeneric.hpp>

std::unique_ptr<Scale> createScale() {
	return std::make_unique<GenericScale>((std::vector<float>){Tone::Ab, Tone::Bb, Tone::C, Tone::D, Tone::Eb, Tone::F, Tone::G});
}
