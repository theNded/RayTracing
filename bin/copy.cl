#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable

const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                      CLK_ADDRESS_CLAMP_TO_EDGE |
                      CLK_FILTER_LINEAR;

void kernel copy(__read_only image2d_t in,
                 __write_only image2d_t out) {
	int2 pos = (int2)(get_global_id(0), get_global_id(1));

	// same result for float4 -> uint4, f -> ui
	float4 pixel = read_imagef(in, smp, pos);
	write_imagef(out, pos, pixel);
}