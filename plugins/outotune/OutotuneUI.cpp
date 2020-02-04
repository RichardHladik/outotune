#include <iostream>
#include <vector>
#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Rectangle;


class OutotuneUI : public DISTRHO::UI {
public:
	OutotuneUI() : UI(512, 512), buffer(BUFFER_SIZE) {
		setGeometryConstraints(128, 128, true);
	}

private:
    void parameterChanged(uint32_t index, float val) override {
		switch (index) {
		case 0:
			return;
		case 1:
			if (val < 0) val = 0;
			buffer.erase(buffer.begin());
			buffer.push_back(val);
			break;
		case 2:
			break;
		}
	}

	void onNanoDisplay() override {
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
		strokeColor(255, 0, 0);
		stroke();
	}

	float normalize(float freq) {
		static const float threshold_min = 50;
		static const float threshold_max = 3000;
		if (freq - threshold_min <= 20)
			return 0;
		return (log(freq) - log(threshold_min)) / (log(threshold_max) - log(threshold_min));
	}

	void uiIdle() override {
		repaint();
	}

private:
	const size_t BUFFER_SIZE = 128;
	std::vector<float> buffer;
};

UI* createUI() {
	return new OutotuneUI();
}

END_NAMESPACE_DISTRHO
