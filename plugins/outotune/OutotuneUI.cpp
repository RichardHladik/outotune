#include <iostream>
#include <memory>
#include <vector>
#include "DistrhoUI.hpp"
#include "Constants.hpp"
#include "WidgetUtils.hpp"
#include "WidgetGraph.hpp"
#include "WidgetButton.hpp"

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Rectangle;


class OutotuneUI : public DISTRHO::UI {
public:
	OutotuneUI() : UI(512, 512) {
		setGeometryConstraints(128, 128, false);
		graph = std::make_unique<WidgetGraph>(this, BUFFER_SIZE, BUFFER_COUNT);
		static const std::vector<std::pair<std::string, Color>> modeStates = {{"absolute", Color(64, 64, 255)}, {"relative", Color(255, 255, 0)}};
		modeButton = std::make_unique<WidgetButton>(this, modeStates, 0, "m");
		throughToggle = std::make_unique<WidgetToggle>(this, "add input", true, "i");
		graphToggle = std::make_unique<WidgetToggle>(this, "show graph", true, "g");
	}

private:
    void parameterChanged(uint32_t i, float x) override {
		if (i >= DISTRHO_PLUGIN_NUM_PARAMETERS)
			return;

		paramUpdated[i] = true;
		param[i] = x;
		switch (i) {
		case 0:
		case 1:
		case 2:
			graph->feedBuffer(i, x);
			break;
		case 3:
			modeButton->setState(param[3]);
			break;
		case 4:
			throughToggle->setState(param[4]);
			break;
		}
	}

	void onNanoDisplay() override {
		// ensure all buffers are updated, even if the value didn't change since the last time
		for (size_t i = 0; i < BUFFER_COUNT; i++) {
			// has not changed since the last time, meaning we must call parameterChanged manually
			if (!paramUpdated[i])
				parameterChanged(i, param[i]);
			paramUpdated[i] = false; // clear
		}

		auto h = getHeight();
		auto w = getWidth();
		graph->setAbsolutePos(0, ceil(h * .1));
		if (graphToggle->getState()) {
			if (!graphVisible) { // has become visible now
				setSize(w, 10 * h);
			}
			graph->show();
			graph->setSize(w, ceil(h * .9));
		} else {
			if (graphVisible) { // has become invisible now
				setSize(w, h - graph->getHeight());
			}
			graph->setSize(0, 0);
			graph->hide();
		}

		graphVisible = graphToggle->getState();

		auto bh = graphVisible ? graph->getAbsolutePos().getY() : h;
		modeButton->setSize(ceil(w * .2), bh);
		modeButton->setAbsolutePos(0, 0);
		throughToggle->setSize(ceil(w * .2), bh);
		throughToggle->setAbsolutePos(ceil(w * .2), 0);
		graphToggle->setSize(ceil(w * .2), bh);
		graphToggle->setAbsolutePos(ceil(w * .4), 0);
		clearCurrent(this, Color(255, 255, 255));

		updateParamIfChanged(3, modeButton->getState());
		updateParamIfChanged(4, throughToggle->getState());
	}

	void updateParamIfChanged(size_t i, float x) {
		if (x != param[i]) {
			param[i] = x;
			setParameterValue(i, x);
		}
	}

	void uiIdle() override {
		repaint();
	}

	static constexpr size_t BUFFER_SIZE = 256;
	static constexpr size_t BUFFER_COUNT = DISTRHO_PLUGIN_NUM_BUFFERED_PARAMETERS;
	bool paramUpdated[DISTRHO_PLUGIN_NUM_PARAMETERS] = {0};
	float param[DISTRHO_PLUGIN_NUM_PARAMETERS] = {0};
	std::unique_ptr<WidgetGraph> graph;
	std::unique_ptr<WidgetButton> modeButton;
	std::unique_ptr<WidgetToggle> throughToggle;
	std::unique_ptr<WidgetToggle> graphToggle;
	bool graphVisible = true;
};

UI* createUI() {
	return new OutotuneUI();
}

END_NAMESPACE_DISTRHO
