/*
// This source file contains the Cuda Code for Amplification of a source Image.
///Copyright (C) 2011 Remaldeep Singh

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "cuda.h"
#include "api.h"
#include "assert.h"
#include "stdio.h"

__global__ void amplify( unsigned char *in, float ampValue, int size)
{
	int 	 x = threadIdx.x + __mul24(blockIdx.x,blockDim.x);
	int      y = threadIdx.y + __mul24(blockIdx.y,blockDim.y);
	int offset = x + y * __mul24(blockDim.x, gridDim.x);
	
	float temp;

	if( offset < size )
	{
		temp = (float)in[offset];
		in[offset] = ((temp * (float)ampValue) > 255) ? 255 : (unsigned char)(temp * (float)ampValue);
	}
}


extern "C" gpu_error_t gpu_amplify(gpu_context_t *ctx, float ampValue)
{
	assert(ampValue);
	assert(ctx);

	float elapsedtime;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);

	gpu_error_t error = GPU_OK;
	int threadsX = ctx->threadsX;
	int threadsY = ctx->threadsY;
	int temp1 = ((ctx->width % threadsX) != 0 ? (ctx->width / threadsX) + 1 : ctx->width / threadsX );
	int temp2 = ((ctx->height % threadsY) != 0 ? (ctx->height / threadsY) + 1 : ctx->height / threadsY );

	/////////////////////////////// Amplify call ////////////////////////////////////
	dim3 threads(threadsX,threadsY);
	dim3 blocks(temp1,temp2);	
	amplify<<<blocks,threads>>>( ctx->gpu_buffer_1, ampValue, (ctx->width * ctx->height));	
	//////////////////////////////////////////////////////////////////////////////////

	if(cudaSuccess != cudaMemcpy( ctx->output_buffer_1, ctx->gpu_buffer_1, ctx->width * ctx->height, cudaMemcpyDeviceToHost))
		error = GPU_ERR_MEM;
	if(cudaSuccess != cudaMemcpy( ctx->output_buffer_1, ctx->gpu_buffer_1, ctx->width * ctx->height, cudaMemcpyDeviceToHost))
		error = GPU_ERR_MEM;

	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&elapsedtime,start,stop);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	//FILE *file;
	//file = fopen("../timing.txt","a+");
	fprintf(stderr,"Amplify:%lf \n",elapsedtime);
	//fclose(file);
	
	return error;
}
