
#include "moGPUAmplifyModule.h"
#include "../../moLog.h"
#include "cv.h"
#include "api.h"

MODULE_DECLARE(GPUAmplify,"native","Amplies input image using GPU");

moGPUAmplifyModule::moGPUAmplifyModule(){
	MODULE_INIT();
	this->properties["amplification"] = new moProperty(1.2);
	GPU_INIT();
}

moGPUAmplifyModule::~moGPUAmplifyModule(){
}

void moGPUAmplifyModule::applyFilter(IplImage *src){
	
	/* Remove the gpu_set_input since we only need it once for every frame */
	if(	gpu_set_input(gpu_ctx,(unsigned char *)src->imageData) != GPU_OK)
	{
		GPU_ERROR("Unable to set context buffer");
	}

	gpu_amplify( gpu_ctx, this->property("amplification").asDouble());
	cvSetData( this->output_buffer, gpu_ctx->output_buffer_1, 320);
}
