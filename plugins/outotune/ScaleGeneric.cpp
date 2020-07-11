#include <ScaleGeneric.hpp>

std::unique_ptr<Scale> createScale() {
	return std::make_unique<GenericScale>((std::vector<float>){Tone::A, Tone::Ab, Tone::B, Tone::Bb, Tone::C, Tone::Db, Tone::D, Tone::Eb, Tone::E, Tone::F, Tone::Gb, Tone::G});
}
