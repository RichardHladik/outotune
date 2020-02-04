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
		if (index != 1)
			return;
		buffer.erase(buffer.begin());
		buffer.push_back(val);
		repaint();
	}

	void onNanoDisplay() override {
		auto h = getHeight();
		auto w = getWidth();

		beginPath();
		moveTo(0, 0);
		bool prevZero = true;
		for (size_t i = 0; i < buffer.size(); i++) {
			auto f = normalize(buffer[i]);
			auto x = (float)i / buffer.size() * w;
			auto y = (1 - f) * h;
			if (f > 0 && !prevZero)
				lineTo(x, y);
			else
				moveTo(x, y);
			prevZero = f == 0;
		}
		strokeColor(255, 0, 0);
		stroke();
	}

	float normalize(float freq) {
		if (freq < 20)
			return 0;
		return log(freq) / log(getSampleRate());
	}

private:
	const size_t BUFFER_SIZE = 128;
	std::vector<float> buffer;
};

UI* createUI() {
	return new OutotuneUI();
}

END_NAMESPACE_DISTRHO
