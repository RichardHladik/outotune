#include "NanoVG.hpp"
#include "Widget.hpp"
#include "Color.hpp"

static void clearCurrent(NanoWidget *w, Color c) {
	w->beginPath();
	w->rect(0, 0, w->getWidth(), w->getHeight());
	w->fillColor(c);
	w->fill();
}
