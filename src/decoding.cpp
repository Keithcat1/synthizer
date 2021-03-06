#include <exception>
#include <array>
#include <algorithm>

#include "synthizer/byte_stream.hpp"
#include "synthizer/decoding.hpp"
#include "synthizer/logging.hpp"
#include "synthizer/make_static_array.hpp"

namespace synthizer {

typedef std::shared_ptr<AudioDecoder> (*DecoderFunc)(std::shared_ptr<LookaheadByteStream> stream);
struct DecoderDef {
	std::string name;
	DecoderFunc func;
};

static auto decoders = makeStaticArray(
	DecoderDef{"dr_wav", decodeWav},
	DecoderDef{"dr_flac", decodeFlac},
	DecoderDef{"dr_mp3", decodeMp3}
);

std::shared_ptr<AudioDecoder> getDecoderForProtocol(std::string protocol, std::string path, std::string options) {
	auto stream = getStreamForProtocol(protocol, path, options);
	auto lookahead_stream = getLookaheadByteStream(stream);
	for(auto d: decoders) {
		try {
			lookahead_stream->reset();
			auto tmp = d.func(lookahead_stream);
			if (tmp == nullptr) {
				logDebug("Path %s: handler %s returned nullptr. Skipping", path.c_str(), d.name.c_str());
				continue;
			}
			logDebug("Handling path %s with handler %s", path.c_str(), d.name.c_str());
			return tmp;
		} catch(std::exception &e) {
			logDebug("Path %s: Format %s threw error %s", path.c_str(), d.name.c_str(), e.what());
			continue;
		}
	}

	logDebug("Path %s: unable to decode", path.c_str());
	throw UnsupportedFormatError(protocol, path, options);
}

void multiplexDecodedChannels(std::int64_t frame_count, int input_channels, AudioSample *input, int output_channels, AudioSample *output) {
	/* Stride is how far between the channels in the output. */
	auto stride = output_channels;
	/* size is the frame size. */
	auto size = std::min(input_channels, output_channels);
	for(int i = 0; i < frame_count; i++) {
		for(int j = 0; j < size; j++) {
			if(j < input_channels)
				output[i*stride+j] = input[input_channels*i+j];
			else
				output[i*stride+j] = 0;
		}
	}
}

}
