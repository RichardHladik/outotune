#include "Correction.hpp"


class ClampCorrection : public Correction {
public:
	ClampCorrection() {}
	virtual float calculate(float correct, float nearest) { return (correct + nearest) / 2; }
	virtual ~ClampCorrection() {}
};

std::unique_ptr<Correction> createCorrection() {
	return std::make_unique<ClampCorrection>();
}
