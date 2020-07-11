#include "NanoVG.hpp"
#include "Widget.hpp"

static void clearCurrent(NanoWidget *w, int r=0, int g=0, int b=0) {
	w->beginPath();
	w->rect(0, 0, 1, 1);
	w->fillColor(r, g, b);
	w->fill();
}
