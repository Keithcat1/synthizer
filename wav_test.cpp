#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include <cstddef>

#include "synthizer_constants.h"

#include "synthizer/audio_output.hpp"
#include "synthizer/byte_stream.hpp"
#include "synthizer/decoding.hpp"
#include "synthizer/downsampler.hpp"
#include "synthizer/filter_design.hpp"
#include "synthizer/hrtf.hpp"
#include "synthizer/iir_filter.hpp"
#include "synthizer/logging.hpp"
#include "synthizer/panner_bank.hpp"
#include "synthizer/math.hpp"

#include "synthizer/generators/decoding.hpp"

using namespace synthizer;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: wav_test <path>\n");
		return 1;
	}

	logToStdout();
	initializeAudioOutputDevice();
	auto output = createAudioOutput();
	auto decoder = getDecoderForProtocol("file", argv[1], {});
	DecodingGenerator generator{};
	auto filter = makeIIRFilter<2>(designAudioEqLowpass((double) 2000 / config::SR / 2, 0.5));
	bool started = false;

	auto panner_bank = makePannerBank();
	auto panner_lane = panner_bank->allocateLane(SYZ_PANNER_STRATEGY_HRTF);

	generator.setDecoder(decoder);
	generator.setPitchBend(1.0);
	double angle = 0;
	double delta = (360.0/10)*((double)config::BLOCK_SIZE/config::SR);
	//delta = 0;
	printf("angle delta %f\n", delta);
	for(unsigned int q = 0;;q++) {
		float wav[config::BLOCK_SIZE*config::MAX_CHANNELS] = {0.0f};
		alignas(config::ALIGNMENT) float hrtf_buf[config::BLOCK_SIZE*8] = { 0.0f };

		auto b = output->beginWrite();
		unsigned int nch = generator.getChannels();
		generator.generateBlock(wav);

		panner_lane->update();
		panner_lane->setPanningAngles(angle, 0.0);
		for(unsigned int i = 0; i < config::BLOCK_SIZE; i++) {
			panner_lane->destination[i*panner_lane->stride] = 0.9*(0.5*wav[i*nch]+0.5*wav[i*nch+1]);
		}

		angle += delta;
		angle -= (int(angle)/360)*360;

		panner_bank->run(hrtf_buf);

		for(int i = 0; i < config::BLOCK_SIZE; i++) {
			b[i*2] = hrtf_buf[i*8];
			b[i*2+1] = hrtf_buf[i*8+1];
		}

		output->endWrite();
	}

	return 0;
}
