#include <memory>

class Correction {
public:
	Correction() {}
	virtual float calculate(float, float) { return 0; }
	virtual ~Correction() {}
};

extern std::unique_ptr<Correction> createCorrection();
