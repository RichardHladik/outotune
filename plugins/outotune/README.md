Technical details
=================

Overview
--------

Outotune uses [DPF][dpf] as an abstraction for various plug-in interfaces. DPF
provides the ability to export a single plug-in into LADSPA, LV2, VST2 and DSSI
formats, as well as into a stand-alone JACK application. Graphical user
interface is also supported, in which case the plug-in basically consists of
two separate modules exchanging information through publicly exposed
parameters. These two parts are defined in
[OutotunePlugin.cpp](OutotunePlugin.cpp) and [OutotuneUI.cpp](OutotuneUI.cpp),
respectively. There is also the header file
[DistrhoPluginInfo.h](DistrhoPluginInfo.h) and the [Makefile](Makefile) which
specify which features of DPF to use (e. g. whether to build the GUI, which
plugin formats to build for etc.).

[WORLD][world] is used for speech analysis and synthesis.
[World.hpp](World.hpp) and [World.cpp](World.cpp) abstract away its API by
encapsulating it into two objects:

* `World`, which reads and buffers the input signal, analyses it, and exposes
  the features obtained by WORLD.
* `World::Synthesizer`, which reads the features from the `World` object,
  shifts the pitch of the original signal by modifying the features and
  synthesizes a shifted voice by feeding the modified features back to WORLD.

The GUI is made in DGL, which is DPF's abstraction layer for OpenGL, providing
basic concepts such as widgets, colors and events. It also wraps
[NanoVG][nanovg], a simple 2D vector drawing library. Top level GUI is handled
in [OutotuneUI.cpp](OutotuneUI.cpp), with the graph widget defined in [WidgetGraph.hpp](WidgetGraph.hpp) and [WidgetGraph.cpp](WidgetGraph.cpp), and the button widget used for the top bar buttons defined in [WidgetButton.hpp](WidgetButton.hpp) and [WidgetButton.cpp](WidgetButton.cpp).

Apart from that, there are also some files with utility classes and
definitions.

* [Buffer.hpp](Buffer.hpp) contains functions for treating vectors
as buffers.
* [Scale.hpp](Scale.hpp) defines conversions between frequencies, MIDI notes and frequency ratios.
* [Constants.hpp](Constants.hpp) contains enums (e. g. parameter names) and other constants, such as the frequency range in which to search for the fundamental frequency.
* [UIUtils.hpp](UIUtils.hpp) and [UIUtils.cpp](UIUtils.cpp) contain miscellaneous UI utility functions used from elsewhere.
* [Colors.hpp](Colors.hpp) contains the definition of the color scheme used by the UI.


[dpf]: https://github.com/DISTRHO/DPF
[world]: https://github.com/mmorise/World
[nanovg]: https://github.com/memononen/nanovg

