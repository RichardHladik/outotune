#include "DistrhoPlugin.hpp"
#include <cmath>
#include <iostream>

class OutotunePlugin : public DISTRHO::Plugin {
public:
	OutotunePlugin() : Plugin(0, 0, 0) {
	}

private:
	const char* getLabel() const override {
		return DISTRHO_PLUGIN_NAME;
	}
	const char* getMaker() const override {
		return DISTRHO_PLUGIN_AUTHOR;
	}
	const char* getLicense() const override {
		return DISTRHO_PLUGIN_LICENSE;
	}
	uint32_t getVersion() const override {
		return d_version(0, 0, 0);
	}
	int64_t getUniqueId() const override {
		return d_cconst('O', 't', 'u', 'n');
	}

	void initParameter(uint32_t, Parameter &) override {}
	float getParameterValue(uint32_t) const override {
		return 0;
	}
	void setParameterValue(uint32_t, float) override {}

	void run(const float** inputs, float** outputs, uint32_t frames) override {
		// get the mono input and output
		const float* const in  = inputs[0];
		float* const out = outputs[0];
		for (uint32_t i=0; i < frames; ++i)
			out[i] = in[i];
	}

};

namespace DISTRHO {

Plugin *createPlugin() {
	return new OutotunePlugin();
}

}
