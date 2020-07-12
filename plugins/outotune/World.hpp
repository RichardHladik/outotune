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
	class Synthesizer {
	public:
		Synthesizer(World &_world);
		~Synthesizer() {
			delete f0;
		}
		Synthesizer(Synthesizer &) = delete;
		Synthesizer operator=(Synthesizer &) = delete;

		const double *shiftBy(double semitones);
		const double *shiftToFreq(double freq);
		const double *shiftToNote(double semitones) {
			return shiftToFreq(Scale::semitones_to_freq(semitones));
		}

		const double *out() {
			return synthesizer.buffer;
		}

	private:
		const double *shift();
		const World &w;
		double *f0;
		WorldSynthesizer synthesizer;
	};

	World(size_t _frameSize, float _rate);
	~World();
	void feed(const float *in, size_t frames);
	double estimate(void);
	const double *orig() const;

	const size_t frameSize;
	const size_t rate;
	static constexpr size_t fragmentLength = 1 << 8;
	static constexpr size_t latency = fragmentLength;

private:
	void estimateRest();

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
};

std::unique_ptr<World> createWorld(size_t frameSize, float rate);
