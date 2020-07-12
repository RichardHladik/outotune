#include <string>
#include "NanoVG.hpp"
#include "Widget.hpp"
#include "Color.hpp"

static const Color COLOR_TOGGLED = Color(0, 255, 0), COLOR_UNTOGGLED = Color(255, 0, 0);

class WidgetButton : public DGL::NanoWidget {
public:
	explicit WidgetButton(Widget *, std::vector<std::pair<std::string, Color>>, size_t defaultState=0, const std::string &keys="");
	void onNanoDisplay() override;
	bool onMouse(const MouseEvent &) override;
	bool onKeyboard(const KeyboardEvent &) override;
	virtual size_t getState();
	virtual void setState(size_t state);
private:
	void textCenter(const std::string &);
	std::vector<std::pair<std::string, Color>> states;
	size_t state = 0;
	std::string keys;
};

class WidgetToggle : public WidgetButton {
public:
	explicit WidgetToggle(Widget *group, const std::string &label, bool checked=false, const std::string &keys="") : WidgetButton(group, (std::vector<std::pair<std::string, Color>>){{label, COLOR_UNTOGGLED}, {label, COLOR_TOGGLED}}, checked, keys) {
	}
};
