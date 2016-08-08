#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable

void kernel raytracing(__write_only image2d_t out, float t) {

    int x = get_global_id(0);
    int y = get_global_id(1);
    int w = get_global_size(0);
    int h = get_global_size(1);
    int2 pos = (int2)(x, y);

    float4 pixel = (float4)( (float)(t + x) / w,
                             (float)(t + y) / h, 1.0f, 1.0f);
    write_imagef(out, pos, pixel);
}