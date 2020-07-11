#include <iostream>
#include <memory>
#include <vector>
#include "DistrhoUI.hpp"
#include "Constants.hpp"
#include "WidgetUtils.hpp"
#include "WidgetGraph.hpp"

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Rectangle;


class OutotuneUI : public DISTRHO::UI {
public:
	OutotuneUI() : UI(512, 512) {
		setGeometryConstraints(128, 128, false);
		graph = std::make_unique<WidgetGraph>(this, BUFFER_SIZE, BUFFER_COUNT);
	}

private:
    void parameterChanged(uint32_t i, float x) override {
		paramUpdated[i] = true;
		paramLast[i] = x;
		switch (i) {
		case 0:
		case 1:
		case 2:
			graph->feedBuffer(i, x);
			break;
		}
	}

	void onNanoDisplay() override {
		// ensure all buffers are updated, even if the value didn't change since the last time
		for (size_t i = 0; i < BUFFER_COUNT; i++) {
			// has not changed since the last time, meaning we must call parameterChanged manually
			if (!paramUpdated[i])
				parameterChanged(i, paramLast[i]);
			paramUpdated[i] = false; // clear
		}

		auto h = getHeight();
		auto w = getWidth();
		graph->setSize(w, ceil(h * .9));
		graph->setAbsolutePos(0, ceil(h * .1));
		resetTransform();
		scale(w, h);
		clearCurrent(this, 255, 255, 255);
	}

	void uiIdle() override {
		repaint();
	}

	static constexpr size_t BUFFER_SIZE = 256;
	static constexpr size_t BUFFER_COUNT = DISTRHO_PLUGIN_NUM_BUFFERED_PARAMETERS;
	bool paramUpdated[DISTRHO_PLUGIN_NUM_PARAMETERS];
	float paramLast[DISTRHO_PLUGIN_NUM_PARAMETERS];
	std::unique_ptr<WidgetGraph> graph;
};

UI* createUI() {
	return new OutotuneUI();
}

END_NAMESPACE_DISTRHO
