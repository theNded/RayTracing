#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable

#define MAX_STEP  512
#define STEP_SIZE 0.01f

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE |
                          CLK_FILTER_LINEAR;

int intersect_box(float3 o, float3 d,
                  float3 box_min, float3 box_max,
                  float *tnear, float *tfar) {
    // Compute intersection of ray with all six bbox planes
    float3 invR = (float3)(1.0f, 1.0f, 1.0f) / d;
    float3 tbot = invR * (box_min - o);
    float3 ttop = invR * (box_max - o);

    // re-order intersections to find smallest and largest on each axis
    float3 tmin = min(ttop, tbot);
    float3 tmax = max(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float smallest_tmax = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));

	*tnear = largest_tmin;
	*tfar = smallest_tmax;

	return smallest_tmax > largest_tmin;
}

void kernel raytracing(__read_only  image3d_t volume,
                       __write_only image2d_t image,
                       float3 r1, float3 r2, float3 r3,
                       float3 camera,
                       float2 f) {
    int w = get_global_size(0);
    int h = get_global_size(1);

    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 pos = (int2)(x, y);

    // NDC    : y upwards
    // Pixels : y downwards
    float3 d_c = (float3)((2.0f * x / (float)w - 1.0f) / f.x,
                          (2.0f * (h - y) / (float)h - 1.0f) / f.y,
                          -1.0f);
    float3 d_w = (float3)(dot(d_c, r1), dot(d_c, r2), dot(d_c, r3));

    float3 box_min = (float3)(0.0f,  0.0f,  0.0f);
    float3 box_max = (float3)(1.0f,  1.0f,  1.0f);

    float tnear, tfar;
    int hit = intersect_box(camera, d_w, box_min, box_max, &tnear, &tfar);

    if (! hit) {
        write_imagef(image, pos, (float4)(0.0f, 0.0f, 0.0f, 1.0f));
        return;
    }

    tnear = (tnear < 0) ? 0 : tnear;
    float t = tnear;
    float4 color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < MAX_STEP; ++i) {
      float3 p = camera + t * d_w;
      int4 vpos = (int4)((int)(p.x * 512),
                         (int)(p.y * 512),
                         (int)(p.z * 512), 1);
      //printf("%d %d %d %d\n", vpos.x, vpos.y, vpos.z);
      float4 pixel = read_imagef(volume, sampler, vpos);
      color += pixel;

      t += STEP_SIZE;
      if (t > tfar) break;
    }

    //int4 vpos = (int4)(320, x, y, 0);
    //
    write_imagef(image, pos, (float4)(0.0f, color.y, color.z, 1.0f));
}