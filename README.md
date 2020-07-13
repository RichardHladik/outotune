Out-O'-Tune
===========

An opensource harmoniser implementation for LV2 and VST leveraging the DISTRHO
Plugin Framework.

What is it?
-----------

Outotune has been inspired by Jacob Collier's harmonizer, which can be seen in
action [here][wtf-harmoniser] or [here][hide-and-seek].

[wtf-harmoniser]: https://www.youtube.com/watch?v=DnpVAyPjxDA
[hide-and-seek]: https://www.youtube.com/watch?v=m7_1HUEvieE

It allows you to become a one-man choir by analysing your voice and
resynthesizing it at different pitches. It is controlled by MIDI, which
basically means you can sing and accompany yourself by playing chords on your
keyboard, and get a full choir of yous. 

Dependencies
------------

* Basic C++ development packages: `g++`, `make`, `pkg-config`
* FFTW3 for the WORLD speech analysis and synthesis system
* JACK for the standalone JACK application (optional)
* OpenGL for UI (optional)

For example on Debian, install `libfftw3-dev libgl-dev libjack-dev`.

Building on platforms other than Linux is possible, but not tested.


Building and running
--------------------

First ensure that you have git submodules initialised by running

	git submodule init
	git submodule update

Also make sure you have all the needed dependencies installed (see above).

Then simply cd to the correct directory and run make:

	cd plugins/outotune
	make

By default, make will build outotune for all the supported plugin formats, this
currently means LV2, VST2 and a stand-alone JACK executable.

Other useful commands are:

- `make install`: installs the plugins to home directory
- `make run`: runs the JACK standalone executable
- `make clean`, `make clean-all`: cleans the build products, the latter also cleans in the submodules
- `make uninstall`: removes the files installed by `make install`

Usage
-----

By default, Outotune processes audio by estimating the current pitch and
shifting it to the nearest diatonic pitch (the scale used may be arbitrary and
is at the moment specified at compile-time).

However, Outotune also supports MIDI input mode in which the pitch to Shift to
is controlled by the currently playing midi tone. The obvious caveat is that
shifting the pitch by a large amount may (and usually will) produce low quality
results.

The gui shows the pitch information as a function of time. The estimated pitch
is shown in red, while the output pitch is shown in blue. Black areas represent
the places where no pitch was detected.

Known limitations
-----------------

Currently, Outotune suffers from great latency. This may be partially improved
by using a different pitch shifting library and/or a different pitch tracking
algorithm (which may also help accuracy in general)

The rubberband pitch shifting library isn't well suited for our purposes, since
it's designed for shifting and stretching of general audio as opposed to human
voice. Incidentally this also means Outotune doesn't currently preserve
formants.

Currently Outotune produces "dirty", cracking audio which we suspect is a
result of the rubberband library not handling frequent changing of the pitch
shift amount very well.

