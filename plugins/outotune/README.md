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


The interface
-------------

As already mentioned in the general README, Outotune has 3 ports: mono input,
MIDI input and mono output. Besides that, it has one output and two input
parameters which get updated at each period:

* `pitch`: output, float containing the current estimated pitch in Hz
* `midi_mode`: input, enum indicating the current mode (i. e. absolute /
  relative). See [Constants.hpp](Constants.hpp) for concrete values.
* `add_input`: input, boolean. Whether or not to include the audio input in the
  output in additon to synthesized voices.

All these values are also used by the GUI: `pitch` is used for plotting the
change of pitch in time, while `midi_mode` and `add_input` can both be modified
from the GUI.

The parameter `pitch` is also considered "buffered" in the sense that its value
is read by the GUI in each period and stored in a fixed-size buffer. This
functionality can be easily extended to multiple parameters, with the hope that
in the future the graph will also plot the pitches of all the synthesized voices.

Voice analysis and synthesis
----------------------------

Outotune uses WORLD to extract features from the voice, modify them, and
resynthesise the voice back from the modified features. What follows is a
simplified summary of how WORLD works, see the [Github page][world] or one of
the multiple [scientific publications][world-publications] written on the
subject.

WORLD splits the input into several fragments of (in our case) 256 samples
each. For each fragment, the fundamental frequency (`f0`) is estimated (the
sample may also be considered unvoiced, in which case `f0 = 0`). Using the
knowledge of `f0` of all fragments, we estimate the spectral envelope of each
fragment. The size of the spectral envelope is `FFT size / 2 + 1`, which is
1025 in our case. Normally, we would also calculate aperiodicity information
for each fragment, which is again an array of 1025 floats. However, we simply
use a hardcoded table of 1025 values for each fragment, since the aperiodicity
calculation is by far the most computationally expensive. The term _features_
will refer to the triple consisting of `f0`, spectral envelope and aperiodicity
information for one fragment.

It is important to note that WORLD needs the inputs to be at least a certain
number of samples long, since otherwise it wouldn't be able to detect lower
frequencies. Specifically, the input needs to have at least `3 * 2048` samples.
Another caveat is that the start and end fragments will usually behave
degenerately because of aliasing. However, we found that after discarding the
first and last fragment, the remaining fragments are "normal".

The `World` class then works as follows: it keeps a buffer of size `max(3 *
2048, buffer_size + 2 * 256)`. Whenever new data arrives, it is put into the
buffer and the WORLD analysing machinery is invoked on the entire buffer. The
features of the last fragment are discarded, and the features of the last
`buffer_size / 256` fragments (barring the discarded fragment) are returned.
This practically makes 256 samples the minimum achievable latency.

Synthesis is a lot more straightforward, since WORLD already contains a
real-time synthesizer. `World::Synthesizer` encapsulates this synthesizer. Upon
being invoked on an array of features, it changes the `f0`s, leaving the
spectral envelopes and aperiodicity information intact, and then invokes
WORLD's synthesizer. The manner in which `f0`s are changed depends on the
method called. `shiftToFreq` and `shiftToNote` simply set all `f0` to a given
frequency, while `shiftBy` multiplies all `f0` by a suitable coefficient.


[dpf]: https://github.com/DISTRHO/DPF
[world]: https://github.com/mmorise/World
[world-publications]: http://www.isc.meiji.ac.jp/~mmorise/world/english/publications.html
[nanovg]: https://github.com/memononen/nanovg
