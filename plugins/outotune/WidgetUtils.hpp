#include "NanoVG.hpp"
#include "Widget.hpp"
#include "Color.hpp"

/* Clears the current widget by painting it to color c. */
void clearCurrent(NanoWidget *w, Color c);

/* See nvg__getAverageScale in the NanoVG library. */
float getAverageScale(NanoWidget *w);

/* Sets stroke width independent of the current scaling. */
void strokeWidthConstant(NanoWidget *w, float x);
