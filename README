Out-O'-Tune
===========

An opensource Autotune implementation leveraging the DISTRHO Plugin Framework.
Work in progress.


Dependencies
------------

jack: for the standalone JACK application

 for example   on Debian, install libjack-dev


Building and running
--------------------

first  ensure  that you have  git submodules  initialised by running

 git submodule init
 git submodule update

also make sure you have all the needed dependencies installed (see above).

then simply cd to the correct directory and run make:

 cd plugins/outotune
 make

by default,  make   will build  outotune for all the supported  plugin formats ( including a stand-alone  Jack executable) and  then start the stand-alone  Jack executable.

 usage



 by default,  Outotune  processes audio by estimating the current  pitch and  shifting it to the nearest diatonic pitch ( the scale used may be arbitrary and  is at the moment specified at compile-time).

 however,   Outotune also   supports MIDI input mode in which the pitch to Shift to  is controlled by the currently playing   midi tone.  the obvious caveat is that  shifting the pitch by a large amount  may ( and usually will) produce low quality results.



   the gui  shows the pitch information  as a function of time.  the estimated  pitch is shown in red, while the output   pitch is shown in blue. black areas represent the places where no pitch was detected.



 known limitations

 currently,   Outotune suffers from great  latency. this may be partially improved by using a different  pitch shifting library  and/or  a different  pitch tracking algorithm ( which may also help accuracy in general)

    the rubberband pitch shifting library  isn't well suited for our purposes,   since it's designed for shifting and stretching of general audio  as  opposed  to human voice.    incidentally this also means  Outotune doesn't currently  preserve formants.

 currently Outotune  produces "dirty", cracking audio which we suspect is a result of   the rubberband library not handling  frequent changing  of the  pitch shift amount  very well.
