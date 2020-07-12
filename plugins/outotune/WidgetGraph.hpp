#include <vector>
#include "NanoVG.hpp"
#include "Widget.hpp"
#include "Buffer.hpp"

class WidgetGraph : public DGL::NanoWidget {
public:
	explicit WidgetGraph(Widget *group, size_t bufferSize, size_t bufferCount);
	void onNanoDisplay() override;
	void feedBuffer(size_t i, float x);
private:
	void drawBuffer(size_t i, Color c);

	float getAverageScale() {
		float t[6];
		currentTransform(t);
		float sx = sqrtf(t[0]*t[0] + t[2]*t[2]);
		float sy = sqrtf(t[1]*t[1] + t[3]*t[3]);
		return (sx + sy) * 0.5f;
	}

	void strokeWidthConstant(float w) {
		strokeWidth(w / getAverageScale());
	}

	size_t bufferSize, bufferCount;
	std::vector<Buffer<float>> bufs;
};
