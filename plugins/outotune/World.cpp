#include <algorithm>
#include <memory>
#include <iostream>
#include <string.h>
#include "Buffer.hpp"
#include "world/d4c.h"
#include "world/dio.h"
#include "world/matlabfunctions.h"
#include "world/cheaptrick.h"
#include "world/stonemask.h"
#include "world/synthesis.h"
#include "world/synthesisrealtime.h"

class World {
public:
	World(size_t _frameSize, float _rate) : frameSize(_frameSize), internalFrames(3 * std::max((size_t)2048, frameSize)), rate(_rate) {
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
		buffOut.resize((int)((f0length - 1) * f0option.frame_period / 1000.0 * rate) + 1);

        InitializeCheapTrickOption(rate, &envelopeOption);
        //envelopeOption.q1 = -0.15;
        envelopeOption.f0_floor = 71.0;
        envelopeSize = GetFFTSizeForCheapTrick(rate, &envelopeOption);
		std::cout << envelopeSize << "\n";
        spectrogram = new double *[f0length];
        for (size_t i = 0; i < f0length; i++)
            spectrogram[i] = new double[envelopeSize / 2 + 1];

        D4COption noiseOption;
        InitializeD4COption(&noiseOption);

        noise = new double *[f0length];
        for (size_t i = 0; i < f0length; i++) {
            noise[i] = new double[envelopeSize / 2 + 1];
			for (size_t j = 0; j < envelopeSize / 2 + 1; j++)
				noise[i][j] = 0;
		}

		InitializeSynthesizer(rate, f0option.frame_period, envelopeSize, frameSize, 50, &synthesizer);
		fragmentLength = 1 << 8;
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

	void shiftBy(double scale) {
		for (size_t i = 0; i < f0length; i++)
			f0aux[i] = f0[i] * scale;
		AddParameters(f0aux + offset, fragmentCount, spectrogram + offset, noise + offset, &synthesizer);
		while (Synthesis2(&synthesizer))
			;
	}

	void shiftTo(double freq) {
		for (size_t i = 0; i < f0length; i++)
			f0aux[i] = f0[i] == 0 ? 0 : freq;
		AddParameters(f0aux + offset, fragmentCount, spectrogram + offset, noise + offset, &synthesizer);
		while (Synthesis2(&synthesizer))
			;
	}

	const double *out() const {
		return synthesizer.buffer;
	}

	const double *orig() const {
		return buffIn.data() + offset * fragmentLength;
	}

private:
#include "World.hack.hpp"
	void estimateRest() {
		for (size_t i = 0; i < f0length; i++)  {
			for (size_t j = 0; j < envelopeSize; j++)
			//	spectrogram[i][j] = spectrohack[j],
				noise[i][j] = noisehack[j];
		}
        CheapTrick(buffIn.data(), internalFrames, rate, time, f0, f0length, &envelopeOption, spectrogram);
        //D4C(buffIn.data(), internalFrames, rate, time, f0, f0length, envelopeSize, &noiseOption, noise);
		for (size_t i = 0; i < f0length; i++)  {
			if (f0[i])
				continue;
			for (size_t j = 0; j < envelopeSize; j++)
				spectrogram[i][j] *= .0001;
		}
	}

	size_t frameSize, internalFrames;
	size_t rate;
	DioOption f0option;
	CheapTrickOption envelopeOption;
	D4COption noiseOption;
	size_t envelopeSize;
	size_t f0length;
	double *f0, *f0aux, *time;
	Buffer<double> buffIn, buffOut;
	double **spectrogram, **noise;
	WorldSynthesizer synthesizer;
	size_t fragmentLength;
	size_t offset;
	size_t fragmentCount;
};

class OLA {
public:
	OLA(size_t n) {
		v.resize(n);
	}

	static double win(size_t frame, size_t shift, size_t i) {
		i = std::min(i, frame - i);
		return ((double)i) / shift;
	}

	void feed(const double *in, size_t n) {
		size_t inc = n / 2;
		size_t start = v.size() - n + inc;
		v.resize(v.size() + inc);
		for (size_t i = 0; i < n; i++)
			v[start + i] += in[i] * win(n, inc, i);
	}

	void pop(size_t count, float *out) {
		buffer_pop(v, count, out);
	}

private:
	Buffer<float> v;
};

std::unique_ptr<World> createWorld(size_t frameSize, float rate) {
	return std::make_unique<World>(frameSize, rate);
}
