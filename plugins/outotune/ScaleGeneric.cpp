#include <ScaleGeneric.hpp>

std::unique_ptr<Scale> createScale() {
	return std::make_unique<GenericScale>((std::vector<float>){0, 2, 5, 7, 10});
}
