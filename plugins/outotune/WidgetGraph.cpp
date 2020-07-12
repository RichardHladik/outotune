#include "WidgetGraph.hpp"

#include "Constants.hpp"
#include "Colors.hpp"
#include "Window.hpp"
#include "WidgetUtils.hpp"
#include "Scale.hpp"

WidgetGraph::WidgetGraph(Widget *group, size_t _bufferSize, size_t _bufferCount) : NanoWidget(group), bufferSize(_bufferSize), bufferCount(_bufferCount) {
	bufs.resize(bufferCount, std::vector<float>(bufferSize));
}

void WidgetGraph::feedBuffer(size_t i, float x) {
	buffer_exchange(bufs[i], x);
}

void WidgetGraph::onNanoDisplay() {
	resetTransform();
	scale(getWidth(), getHeight());
	clearCurrent(this, Colors::GraphBackground);
	for (size_t i = 0; i < bufs.size(); i++) {
		Color c = i ? Colors::GraphSecondary : Colors::GraphPrimary;
		drawBuffer(i, c);
	}
}

static float scaleFreq(float freq) {
	return log(freq / FREQ_MIN) / log(FREQ_MAX / FREQ_MIN);
}

static float visible(float p) {
	return p >= 0 && p <= 1;
}

void WidgetGraph::drawBuffer(size_t i, Color c) {
	auto &&buffer = bufs[i];
	beginPath();
	moveTo(0, 0);
	float yPrev = 0, fPrev = 0;
	for (size_t i = 0; i < buffer.size(); i++) {
		auto f = buffer[i];
		auto y = scaleFreq(f);
		auto x = (float)i / buffer.size();
		float semi = Scale::freq_to_semitones(f);
		float semiPrev = Scale::freq_to_semitones(fPrev);
		if (visible(y) && visible(yPrev) && fabs(semi - semiPrev) < 1.5)
			lineTo(x, 1 - y);
		else
			moveTo(x, 1 - y);
		fPrev = f;
		yPrev = y;
	}

	strokeColor(c);
	strokeWidthConstant(this, 1.5);
	stroke();
}
