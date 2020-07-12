#include "WidgetButton.hpp"

#include <iostream>
#include "Window.hpp"
#include "WidgetUtils.hpp"

WidgetButton::WidgetButton(Widget *group, std::vector<std::pair<std::string, Color>> _states, size_t defaultState, const std::string &_keys) : NanoWidget(group), states(std::move(_states)), state(defaultState), keys(_keys) {
	auto font = createFontFromFile("sans", "/usr/share/fonts/TTF/OpenSans-Regular.ttf");
	if (font == -1)
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

	state = (state + 1) % states.size();
ret:
	return Widget::onMouse(e);
}

bool WidgetButton::onKeyboard(const KeyboardEvent &e) {
	if (!e.press || e.key > 127)
		goto ret;

	if (keys.find(e.key) == std::string::npos)
		goto ret;

	state = (state + 1) % states.size();
ret:
	return Widget::onKeyboard(e);
}

size_t WidgetButton::getState() {
	return state;
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

	fontFace("sans");
	fillColor(0, 0, 0);
	float lineh = 1.2f;
	textMetrics(NULL, NULL, &lineh);
	textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
	textLineHeight(1.2);
	float size = 2 * getWidth() / t.size(); // estimate
	fontSize(size);
	float x = getWidth() / 2, y = getHeight() / 2;
	Rectangle<float> bounds;
	
	textBounds(x, y, t.c_str(), NULL, bounds);
	// recalibrate, assuming font scaling behaves linearly
	size *= std::min(getWidth() / bounds.getWidth(), getHeight() / bounds.getHeight());
	size *= 0.9;
	size = std::min(size, 50.f);
	fontSize(size);
	text(x, y, t.c_str(), NULL);
}
