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
		graph = std::make_unique<WidgetGraph>(this, BUFFER_SIZE, (size_t)pId::countBuffered);
		static const std::vector<std::pair<std::string, Color>> modeStates = {{"absolute", Colors::ModeAbsolute}, {"relative", Colors::ModeRelative}};
		modeButton = std::make_unique<WidgetButton>(this, modeStates, 0, "m");
		throughToggle = std::make_unique<WidgetToggle>(this, "add input", true, "i");
		graphToggle = std::make_unique<WidgetToggle>(this, "show graph", true, "g");
	}

private:
    void parameterChanged(uint32_t i, float x) override {
		pId paramId = castToEnum<pId>(i, pId::_count);
		if (paramId == pId::_count) {
			// bad index passed, probably bug in port numbering / on the host side
			DISTRHO_SAFE_ASSERT(false);
			return;
		}
		// otherwise i == paramId (up to casting)

		paramUpdated[i] = true;
		param[i] = x;
		switch (paramId) {
		case pId::pitch:
			graph->feedBuffer(i - (size_t)pId::bufferedStart, x);
			break;
		case pId::midiMode:
			modeButton->setState(x);
			break;
		case pId::passThrough:
			throughToggle->setState(x);
			break;
		default:
			// not implemented
			DISTRHO_SAFE_ASSERT(false);
			break;
		}
	}

	void onNanoDisplay() override {
		// ensure all buffers are updated, even if the value didn't change since the last time
		for (size_t i = (size_t)pId::bufferedStart; i < (size_t)pId::bufferedEnd; i++) {
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
		clearCurrent(this, Colors::WindowBackground);

		updateParamIfChanged(pId::midiMode, modeButton->getState());
		updateParamIfChanged(pId::passThrough, throughToggle->getState());
	}

	void updateParamIfChanged(pId p, float x) {
		size_t i = (size_t)p;
		if (x != param[i]) {
			param[i] = x;
			setParameterValue(i, x);
		}
	}

	void uiIdle() override {
		repaint();
	}

	static constexpr size_t BUFFER_SIZE = 256;
	bool paramUpdated[(int)pId::_count] = {0};
	float param[(int)pId::_count] = {0};
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
