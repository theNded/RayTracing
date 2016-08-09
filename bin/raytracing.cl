#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE |
                          CLK_FILTER_LINEAR;

void kernel raytracing(__read_only  image3d_t volume,
                       __write_only image2d_t image,
                       int t) {

    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 pos = (int2)(x, y);
    int4 vpos = (int4)(x, y, t, 0);

    float4 pixel = read_imagef(volume, sampler, vpos);
    pixel.x = 0.0f;
    //pixel.z = 0.0f;
    write_imagef(image, pos, pixel);
}