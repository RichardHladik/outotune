#include <algorithm>
#include <memory>
#include <iostream>
#include <string.h>
#include "Buffer.hpp"
#include "Scale.hpp"
#include "world/d4c.h"
#include "world/dio.h"
#include "world/matlabfunctions.h"
#include "world/cheaptrick.h"
#include "world/stonemask.h"
#include "world/synthesis.h"
#include "world/synthesisrealtime.h"

class World {
public:
	World(size_t _frameSize, float _rate) : frameSize(_frameSize), rate(_rate), internalFrames(3 * std::max((size_t)2048, _frameSize)) {
        InitializeDioOption(&f0option);
		f0option.frame_period = 5.805;
        f0option.speed = 1;
        f0option.f0_floor = 71.0;
        f0option.allowed_range = 0.2;
        f0length = GetSamplesForDIO(rate, internalFrames, f0option.frame_period);
		f0 = new double [f0length];
		f0aux = new double [f0length];
		time = new double [f0length];
		buffIn.resize(internalFrames);

        InitializeCheapTrickOption(rate, &envelopeOption);
        //envelopeOption.q1 = -0.15;
        envelopeOption.f0_floor = 71.0;
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
				noise[i][j] = i < 140 ? pow(i / 150, 4.5) : 0.7 + 0.3 * (i / n);
		}

		fragmentCount = frameSize / fragmentLength;
		offset = f0length - fragmentCount - 1;
	}

	double estimate(void) {
        Dio(buffIn.data(), internalFrames, rate, &f0option, time, f0aux);
        StoneMask(buffIn.data(), internalFrames, rate, time, f0aux, f0length, f0);
		size_t samples = std::min(f0length, 1 + f0length * frameSize / internalFrames);
		double k = 1, total = 0, mean = 0;
		for (int i = (int)f0length - 1; i >= 0; i--) {
			if (i + samples < f0length) 
				k *= 1;
			if (f0[i] == 0)
				continue;
			mean += f0[i] * k;
			total += k;
		}

		estimateRest();
		return (total == 0) ? 0 : mean / total;
	}

	void feed(const float *in, size_t frames) {
		buffer_exchange(buffIn, frames, in);
	}

	const double *orig() const {
		return buffIn.data() + offset * fragmentLength;
	}


	const size_t frameSize;
	const size_t rate;
	static constexpr size_t fragmentLength = 1 << 8;
	static constexpr size_t latency = fragmentLength;

private:
	void estimateRest() {
        CheapTrick(buffIn.data(), internalFrames, rate, time, f0, f0length, &envelopeOption, spectrogram);
        //D4C(buffIn.data(), internalFrames, rate, time, f0, f0length, envelopeSize, &noiseOption, noise);
		for (size_t i = 0; i < f0length; i++)  {
			if (f0[i])
				continue;
			for (size_t j = 0; j < envelopeSize / 2 + 1; j++)
				spectrogram[i][j] *= .1;
		}
	}

	size_t internalFrames;
	DioOption f0option;
	CheapTrickOption envelopeOption;
	D4COption noiseOption;
	size_t envelopeSize;
	size_t f0length;
	double *f0, *f0aux, *time;
	Buffer<double> buffIn;
	double **spectrogram, **noise;
	size_t offset;
	size_t fragmentCount;

public:

	class Synthesizer {
	public:
		Synthesizer(World &_world) : w(_world) {
			InitializeSynthesizer(w.rate, w.f0option.frame_period, w.envelopeSize, w.frameSize, 50, &synthesizer);
			f0 = new double[w.f0length];
		}
		~Synthesizer() {
			delete f0;
		}
		Synthesizer(Synthesizer &) = delete;
		Synthesizer operator=(Synthesizer &) = delete;

		const double *shiftBy(double semitones) {
			double scale = Scale::semitones_to_ratio(semitones);
			for (size_t i = 0; i < w.f0length; i++)
				f0[i] = w.f0[i] * scale;
			return shift();
		}

		const double *shiftToFreq(double freq) {
			for (size_t i = 0; i < w.f0length; i++)
				f0[i] = w.f0[i] == 0 ? 0 : freq;
			return shift();
		}

		const double *shiftToNote(double semitones) {
			return shiftToFreq(Scale::semitones_to_freq(semitones));
		}

		const double *out() {
			return synthesizer.buffer;
		}
	private:
		const double *shift() {
			AddParameters(f0 + w.offset, w.fragmentCount, w.spectrogram + w.offset, w.noise + w.offset, &synthesizer);
			while (Synthesis2(&synthesizer))
				;
			return out();
		}

		const World &w;
		double *f0;
		WorldSynthesizer synthesizer;
	};
};

std::unique_ptr<World> createWorld(size_t frameSize, float rate) {
	return std::make_unique<World>(frameSize, rate);
}
