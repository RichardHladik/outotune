#include "ScaleGeneric.hpp"

std::unique_ptr<Scale> createScale() {
	return std::make_unique<GenericScale>((1 << 12) - 1);
}
