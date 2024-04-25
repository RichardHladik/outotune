#include <iostream>
#include <algorithm>
#include <memory>
#include <string.h>
#include "Constants.hpp"
#include "World.hpp"

World::World(size_t _frameSize, float _rate) : frameSize(_frameSize), rate(_rate), bufferSize(std::max((size_t)3 * 2048, _frameSize + 3 * fragmentLength)) {
	std::cout << bufferSize << std::endl;
	InitializeDioOption(&f0option);

	// Calculated so that the fragments have lenght exactly fragmentLength ==
	// 256, regardless of frame rate. Should be still close enough to 5ms,
	// which is the default, granted rate is either 44.1 or 48 kHz. Might
	// behave strangely for other rates.
	f0option.frame_period = fragmentLength * 1000. / rate + 1e-5;
	f0option.speed = 1;
	f0option.f0_floor = FREQ_MIN;
	f0option.f0_ceil = FREQ_MAX;
	f0option.allowed_range = 0.2;
	f0length = GetSamplesForDIO(rate, bufferSize, f0option.frame_period);
	f0 = new double [f0length];
	f0aux = new double [f0length];
	time = new double [f0length];
	buffIn.resize(bufferSize);

	InitializeCheapTrickOption(rate, &envelopeOption);
	envelopeOption.f0_floor = FREQ_MIN;
	envelopeSize = GetFFTSizeForCheapTrick(rate, &envelopeOption);
	spectrogram = new double *[f0length];
	for (size_t i = 0; i < f0length; i++)
		spectrogram[i] = new double[envelopeSize / 2 + 1];

	D4COption noiseOption;
	InitializeD4COption(&noiseOption);

	noise = new double *[f0length];
	for (size_t i = 0; i < f0length; i++) {
		size_t n = envelopeSize / 2 + 1;
		noise[i] = new double[n];
		// a hackish precomputation, since calculating the correct noise
		// values at each frame is too expensive. The following is an
		// approximation based on measuring the noise values of a sine wave.
		for (size_t j = 0; j < n; j++)
			noise[i][j] = i < 140 ? pow((int)(i / 150), 4.5) : 0.7 + 0.3 * (int)(i / n);
	}

	offset = f0length - fragmentCount - 1;
	estimate(); // do a warm-up
}

World::~World() {
	delete[] f0;
	delete[] f0aux;
	delete[] time;
	for (size_t i = 0; i < f0length; i++) {
		delete[] spectrogram[i];
		delete[] noise[i];
	}
	delete[] spectrogram;
	delete[] noise;
}

double World::aggregateF0Fragments(double *f, size_t cnt) {
	double mean = 0;
	size_t total = 0; // total number of nonzero fragments
	for (size_t i = 0; i < cnt; i++) {
		if (f[i] == 0)
			continue;
		mean += Scale::freq_to_semitones(f[i]);
		total++;
	}

	return total <= cnt / 4 ? 0 : Scale::semitones_to_freq(mean / total);
}

double World::estimate(void) {
	// initial f0 estimation
	Dio(buffIn.data(), bufferSize, rate, &f0option, time, f0aux);
	// refinement
	StoneMask(buffIn.data(), bufferSize, rate, time, f0aux, f0length, f0);

	estimateRest();
	return aggregateF0Fragments(f0 + offset, fragmentCount);
}

void World::feed(const float *in, size_t frames) {
	buffer_feed(buffIn, frames, in);
}

const double *World::orig() const {
	return buffIn.data() + offset * fragmentLength;
}

void World::estimateRest() {
	// spectral envelope estimation
	CheapTrick(buffIn.data(), bufferSize, rate, time, f0, f0length, &envelopeOption, spectrogram);
	// uncomment the following to get noise contour estimation, currently we get a fake hardcoded contour 
	// D4C(buffIn.data(), bufferSize, rate, time, f0, f0length, envelopeSize, &noiseOption, noise);
	
	// workaround: lower the volume of unvoiced frames (to prevent cracks and hisses)
	for (size_t i = 0; i < f0length; i++)  {
		if (f0[i])
			continue;
		for (size_t j = 0; j < envelopeSize / 2 + 1; j++)
			spectrogram[i][j] *= .1;
	}
}

World::Synthesizer::Synthesizer(World &_world) : w(_world) {
	InitializeSynthesizer(w.rate, w.f0option.frame_period, w.envelopeSize, w.frameSize, 50, &synthesizer);
	f0 = new double[w.f0length];
}
const double *World::Synthesizer::shiftBy(double semitones) {
	double scale = Scale::semitones_to_ratio(semitones);
	for (size_t i = 0; i < w.f0length; i++)
		f0[i] = w.f0[i] * scale;
	return shift();
}

const double *World::Synthesizer::shiftToFreq(double freq) {
	for (size_t i = 0; i < w.f0length; i++)
		f0[i] = w.f0[i] == 0 ? 0 : freq;
	return shift();
}

const double *World::Synthesizer::shift() {
	AddParameters(f0 + w.offset, w.fragmentCount, w.spectrogram + w.offset, w.noise + w.offset, &synthesizer);
	while (Synthesis2(&synthesizer))
		;
	return out();
}

std::unique_ptr<World> createWorld(size_t frameSize, float rate) {
	return std::make_unique<World>(frameSize, rate);
}
