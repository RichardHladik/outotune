#include <iostream>
#include <map>

#include "Constants.hpp"
#include "World.hpp"
namespace audioio {
#include "../../World/tools/audioio.cpp"
}

int main(void) {
	size_t frames = 4096;
	float rate = 48000;
	char fn[] = "in.wav";
	std::unique_ptr<World> world;
	world = createWorld(frames, rate);
	size_t audio_length = audioio::GetAudioLength(fn);
	size_t length = ((audio_length/frames) + 1) * frames;
    double *x = new double[length];
    float *xf = new float[length];
	for (size_t i = 0; i < length; i++)
		 x[i] = 0;

    int fs, nbit;
	audioio::wavread(fn, &fs, &nbit, x);
	assert(rate == nbit);
	for (size_t i = 0; i < length; i++)
		xf[i] = x[i];

	/*
	double sum = 0;
	double var = 0;
	size_t N = 100000000;
	for (size_t i = 0; i < N; i++) {
		auto a = randn();
		sum += a;
		var += a * a;
	}

	std::cout << sum / N << " " << var / N << std::endl;
	*/
	auto synth = std::make_unique<World::Synthesizer>(*world);
	for (size_t i = 0; i < length; i += frames)  {
		world->feed(xf + i, frames);
		auto pitch = world->estimate();
		synth->shiftToFreq(440);
		/*
		synth->shiftToFreq(880);
		synth->shiftToFreq(220);
		synth->shiftToFreq(330);
		synth->shiftToFreq(660);
		*/
		std::cout << pitch << std::endl;
	}
}
