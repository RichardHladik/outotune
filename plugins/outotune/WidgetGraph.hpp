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

	size_t bufferSize, bufferCount;
	std::vector<Buffer<float>> bufs;
};
