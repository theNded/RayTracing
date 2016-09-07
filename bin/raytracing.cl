#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable

const sampler_t sampler   = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE |
                            CLK_FILTER_LINEAR;

int intersect_box(float3 origin,  float3 direction,
                  float3 box_min, float3 box_max,
                  float *t_near, float *t_far) {

    float3 direction_inv = (float3)(1.0f, 1.0f, 1.0f) / direction;
    float3 t_bottom      = direction_inv * (box_min - origin);
    float3 t_top         = direction_inv * (box_max - origin);

    float3 t_min = min(t_top, t_bottom);
    float3 t_max = max(t_top, t_bottom);

    float largest_t_min  = max(max(t_min.x, t_min.y),
                               max(t_min.x, t_min.z));
    float smallest_t_max = min(min(t_max.x, t_max.y),
                               min(t_max.x, t_max.z));

	*t_near = largest_t_min;
	*t_far  = smallest_t_max;

	return largest_t_min < smallest_t_max;
};

void kernel raytracing(__read_only  image3d_t volume,
                       __write_only image2d_t image,
                       float3 r1, float3 r2, float3 r3,
                       float3 camera, float2 f, float3 range) {
    float3 grid_size = 2.0f / range;
    int2 wh = (int2)(get_global_size(0), get_global_size(1));
    int2 uv = (int2)(get_global_id(0), get_global_id(1));

    // NDC   : Y upwards
    // Image : Y downwards
    float3 direction_camera = (float3)((2.0f * uv.x / (float)wh.x - 1.0f) / f.x,
                                       (1.0f - 2.0f * uv.y / (float)wh.y) / f.y,
                                       -1.0f);
    float3 direction_world = (float3)(dot(direction_camera, r1),
                                      dot(direction_camera, r2),
                                      dot(direction_camera, r3));
    direction_world.x = (fabs(direction_world.x) < 1e-10)
    ? 1e-10 : direction_world.x;
    direction_world.y = (fabs(direction_world.y) < 1e-10)
    ? 1e-10 : direction_world.y;
    direction_world.z = (fabs(direction_world.z) < 1e-10)
    ? 1e-10 : direction_world.z;

    float3 box_min = (float3)(-1.0f, -1.0f, -1.0f);
    float3 box_max = (float3)( 1.0f,  1.0f,  1.0f);

    float t_near, t_far;
    int hit = intersect_box(camera, direction_world,
                            box_min, box_max,
                            &t_near, &t_far);
    if (! hit) {
        write_imagef(image, uv, (float4)(0.0f, 0.0f, 0.0f, 1.0f));
        return;
    }

    t_near = (t_near < 0) ? 0 : t_near;
    float t = t_near;
    float4 color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    int max_iteration = (int)(range.x + range.y + range.z);
    float step_size   = 0.8f * grid_size.x;
    for (int i = 0; i < max_iteration; ++i) {
      float3 p = camera + t * direction_world;

      int4 volume_index = (int4)((int)((p.x + 1) / grid_size.x),
                                 (int)((p.y + 1) / grid_size.y),
                                 (int)((p.z + 1) / grid_size.z),
                                 1);

      color += read_imagef(volume, sampler, volume_index);
      t += step_size;
      if (t > t_far) break;
    }
    write_imagef(image, uv, 0.01f * (float4)(0.0f, color.y, 0.0f, 1.0f));
}