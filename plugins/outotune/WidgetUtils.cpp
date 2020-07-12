#include "WidgetUtils.hpp"

#include "NanoVG.hpp"
#include "Widget.hpp"
#include "Color.hpp"

/* Clears the current widget by painting it to color c. */
void clearCurrent(NanoWidget *w, Color c) {
	w->beginPath();
	w->rect(0, 0, w->getWidth(), w->getHeight());
	w->fillColor(c);
	w->fill();
}

float getAverageScale(NanoWidget *w) {
	float t[6];
	w->currentTransform(t);
	float sx = sqrtf(t[0]*t[0] + t[2]*t[2]);
	float sy = sqrtf(t[1]*t[1] + t[3]*t[3]);
	return (sx + sy) * 0.5f;
}

/* Sets stroke width independent of the current scaling. */
void strokeWidthConstant(NanoWidget *w, float x) {
	w->strokeWidth(x / getAverageScale(w));
}
