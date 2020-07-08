#include <iostream>
#include <vector>
#include "DistrhoUI.hpp"
#include "Constants.hpp"

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Rectangle;


class OutotuneUI : public DISTRHO::UI {
public:
	OutotuneUI() : UI(512, 512) {
		setGeometryConstraints(128, 128, true);
		for (size_t i = 0; i < NUM_BUFFERS; i++)
			bufs[i] = std::vector<float>(BUFFER_SIZE);
	}

private:
    void parameterChanged(uint32_t index, float val) override {
		paramUpdated[index] = true;
		switch (index) {
		case 0:
		case 1:
		case 2:
			feedBuffer(bufs[index], val);
			break;
		}
	}

	void onNanoDisplay() override {
		for (size_t i = 0; i < NUM_BUFFERS; i++) {
			// has not changed since the last time, meaning we must call parameterChanged manually
			if (!paramUpdated[i])
				parameterChanged(i, bufs[i].back());
			paramUpdated[i] = false; // clear
		}

		drawBuffer(bufs[0], 255, 0, 0);
		drawBuffer(bufs[1], 0, 255, 0);
		drawBuffer(bufs[2], 0, 0, 255);
	}

	void uiIdle() override {
		repaint();
	}

	static float normalize(float freq) {
		static const float threshold_min = 50;
		static const float threshold_max = 3000;
		if (freq - threshold_min <= 20)
			return 0;
		return (log(freq) - log(threshold_min)) / (log(threshold_max) - log(threshold_min));
	}

	void drawBuffer(const std::vector<float> &buffer, int r, int g, int b) {
		auto h = getHeight();
		auto w = getWidth();

		beginPath();
		moveTo(0, 0);
		float prev = 0;
		for (size_t i = 0; i < buffer.size(); i++) {
			auto f = normalize(buffer[i]);
			auto x = (float)i / buffer.size() * w;
			auto y = (1 - f) * h;
			if (f > 0 && prev > 0 && fabs(prev - f) < .02)
				lineTo(x, y);
			else
				moveTo(x, y);
			prev = f;
		}
		strokeColor(r, g, b);
		stroke();
	}

	static void feedBuffer(std::vector<float> &buffer, float val) {
		buffer.erase(buffer.begin());
		buffer.push_back(val);
	}

	static constexpr size_t BUFFER_SIZE = 256;
	static constexpr size_t NUM_BUFFERS = DISTRHO_PLUGIN_NUM_BUFFERED_PARAMETERS;
	std::vector<float> bufs[NUM_BUFFERS];
	bool paramUpdated[DISTRHO_PLUGIN_NUM_PARAMETERS];
};

UI* createUI() {
	return new OutotuneUI();
}

END_NAMESPACE_DISTRHO
