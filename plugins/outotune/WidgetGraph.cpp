#include "WidgetGraph.hpp"

#include "Constants.hpp"
#include "Window.hpp"
#include "WidgetUtils.hpp"

WidgetGraph::WidgetGraph(Widget *group, size_t _bufferSize, size_t _bufferCount) : NanoWidget(group), bufferSize(_bufferSize), bufferCount(_bufferCount) {
	bufs.resize(bufferCount, std::vector<float>(bufferSize));
}

void WidgetGraph::feedBuffer(size_t i, float x) {
	buffer_exchange(bufs[i], x);
}

void WidgetGraph::onNanoDisplay() {
	resetTransform();
	scale(getWidth(), getHeight());
	clearCurrent(this, Color(0, 0, 0));
	drawBuffer(0, Color(255, 0, 0));
	drawBuffer(1, Color(0, 255, 0));
	drawBuffer(2, Color(0, 0, 255));
}

static float scaleFreq(float freq) {
	return (log(freq / FREQ_MIN)) / (log(FREQ_MAX / FREQ_MIN));
}

void WidgetGraph::drawBuffer(size_t i, Color c) {
	auto &&buffer = bufs[i];
	beginPath();
	moveTo(0, 0);
	float prev = 0;
	for (size_t i = 0; i < buffer.size(); i++) {
		auto f = scaleFreq(buffer[i]);
		auto x = (float)i / buffer.size();
		auto y = (1 - f);
		if (f > 0 && prev > 0 && fabs(prev - f) < .02)
			lineTo(x, y);
		else
			moveTo(x, y);
		prev = f;
	}

	strokeColor(c);
	strokeWidthConstant(1.5);
	stroke();
}
