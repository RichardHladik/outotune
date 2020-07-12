#include "WidgetButton.hpp"

#include <iostream>
#include "Window.hpp"
#include "WidgetUtils.hpp"

WidgetButton::WidgetButton(Widget *group, std::vector<std::pair<std::string, Color>> _states, size_t defaultState, const std::string &_keys) : NanoWidget(group), states(std::move(_states)), state(defaultState), keys(_keys) {
	// TODO: hack
	static const std::vector<std::string> candidates = {
		"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
		"/usr/share/fonts/TTF/DejaVuSans.ttf",
	};

	bool ok = false;
	for (auto &&a : candidates) {
		auto font = createFontFromFile("sans", a.c_str());
		if (font != -1) {
			ok = true;
			break;
		}
	}

	if (!ok)
		std::cout << "Could not add font." << std::endl;
}

void WidgetButton::onNanoDisplay() {
	clearCurrent(this, states[state].second);
	textCenter(states[state].first);
}

bool WidgetButton::onMouse(const MouseEvent &e) {
	int x, y;
	bool inside;
	if (!e.press)
		goto ret;

	x = e.pos.getX();
	y = e.pos.getY();
	inside = x >= 0 && y >= 0 && x < (int)getWidth() && y < (int)getHeight();
	if (!inside)
		goto ret;

	advanceState();
ret:
	return Widget::onMouse(e);
}

bool WidgetButton::onKeyboard(const KeyboardEvent &e) {
	if (!e.press || e.key > 127)
		goto ret;

	if (keys.find(e.key) == std::string::npos)
		goto ret;

	advanceState();
ret:
	return Widget::onKeyboard(e);
}

void WidgetButton::setState(size_t s) {
	state = s;
	if (state > states.size()) {
		state = 0;
		DISTRHO_SAFE_ASSERT(false);
	}
}

void WidgetButton::textCenter(const std::string &t) {
	if (t.empty())
		return;

	save();
	fontFace("sans");
	fillColor(Colors::ButtonText);
	textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
	textLineHeight(1.2);
	float x = getWidth() / 2, y = getHeight() / 2;

	float size = 2 * getWidth() / t.size(); // estimate
	fontSize(size);
	Rectangle<float> bounds;
	// calculate text size if rendered with our estimated size
	textBounds(x, y, t.c_str(), NULL, bounds);

	// recalibrate, assuming font scaling behaves linearly
	size *= std::min(getWidth() / bounds.getWidth(), getHeight() / bounds.getHeight());
	size *= 0.9;
	size = std::min(size, 50.f);
	fontSize(size);
	text(x, y, t.c_str(), NULL);
	restore();
}
