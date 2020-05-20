#include "synthizer.h"

#include "synthizer/audio_output.hpp"
#include "synthizer/c_api.hpp"
#include "synthizer/memory.hpp"

#include <string>

namespace synthizer {

/* Error code management. */
thread_local syz_ErrorCode last_error_code = 0;
thread_local std::string last_error_message = "";

void setCThreadError(syz_ErrorCode error, const char *message) {
	last_error_code = error;
	last_error_message = message;
}

}

/* C stuff itself; note we're outside the namespace. */
using namespace synthizer;

static std::atomic<unsigned int> initialization_count = 0;
SYZ_CAPI syz_ErrorCode syz_initialize() {
	SYZ_PROLOGUE
	if (	initialization_count.fetch_add(1) == 0) {
		initializeAudioOutputDevice();
	}
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_shutdown() {
	SYZ_PROLOGUE
	if (initialization_count.fetch_sub(1) == 1) {
		/* TODO: deinitialize. */
	}
	return 0;
	SYZ_EPILOGUE
}


SYZ_CAPI syz_ErrorCode syz_getLastErrorCode() {
	return last_error_code;
}

SYZ_CAPI const char *syz_getLastErrorMessage() {
	return last_error_message.c_str();
}

/*
 * Memory management.
 * */
SYZ_CAPI syz_ErrorCode syz_handleIncRef(syz_Handle handle) {
	SYZ_PROLOGUE
	auto tmp = fromC<CExposable>(handle);
	incRefC(tmp);
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_handleDecRef(syz_Handle handle) {
	SYZ_PROLOGUE
	auto h = fromC<CExposable>(handle);
	decRefC(h);
	return 0;
	SYZ_EPILOGUE
}


SYZ_CAPI syz_ErrorCode syz_getI(int *out, syz_Handle target, int property) {
	SYZ_PROLOGUE
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_setI(syz_Handle target, int property, int value) {
	SYZ_PROLOGUE
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_getD(double *out, syz_Handle target, int property) {
	SYZ_PROLOGUE
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_setD(syz_Handle target, int property, double value) {
	SYZ_PROLOGUE
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_getO(syz_Handle *out, syz_Handle target, int property) {
	SYZ_PROLOGUE
	return 0;
	SYZ_EPILOGUE
}

SYZ_CAPI syz_ErrorCode syz_setO(syz_Handle target, int property, syz_Handle value) {
	SYZ_PROLOGUE
	return 0;
	SYZ_EPILOGUE
}
