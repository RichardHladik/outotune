#include "WidgetGraph.hpp"

#include <iostream>
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
	static constexpr float threshold_min = 50;
	static constexpr float threshold_max = 3000;
	if (freq - threshold_min <= 20)
		return 0;
	return (log(freq) - log(threshold_min)) / (log(threshold_max) - log(threshold_min));
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
