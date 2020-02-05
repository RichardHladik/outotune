#include <memory>

class Scale {
public:
	Scale() {}
	virtual float nearest_tone(float) { return 0; }
	virtual ~Scale() {}
};

extern std::unique_ptr<Scale> createScale();
