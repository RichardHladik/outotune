#include "Correction.hpp"


class ClampCorrection : public Correction {
public:
	ClampCorrection() {}
	virtual float calculate(float actual, float nearest) {
		if (actual == 0)
			return 0;
		static float prev = actual;
		return prev = .9 * prev + .1 * nearest;
	}
	virtual ~ClampCorrection() {}
};

std::unique_ptr<Correction> createCorrection() {
	return std::make_unique<ClampCorrection>();
}
