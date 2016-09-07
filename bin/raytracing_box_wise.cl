#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE |
                          CLK_FILTER_LINEAR;

int intersect_box(float3 origin,  float3 direction,
                  float3 box_min, float3 box_max,
                  float *t_near, float *t_far) {

    float3 direction_inv = 1.0f / direction;
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
    // Get width and index
    float2 wh = (float2)(get_global_size(0), get_global_size(1));
    int2 uv = (int2)(get_global_id(0), get_global_id(1));

    // Coordinate conversion
    // NDC   : Y upwards,   (-1,    1)
    // Image : Y downwards, (0,  wh.y)
    float3 dir_camera = (float3)((2.0f * uv.x / wh.x - 1.0f) / f.x,
                                 (1.0f - 2.0f * uv.y / wh.y) / f.y,
                                 -1.0f);
    float3 dir_world = (float3)(dot(dir_camera, r1),
                                dot(dir_camera, r2),
                                dot(dir_camera, r3));

    // Ill-condition adjustment
    float epsilon = 1e-10f;
    dir_world.x = (fabs(dir_world.x) < epsilon) ? epsilon : dir_world.x;
    dir_world.y = (fabs(dir_world.y) < epsilon) ? epsilon : dir_world.y;
    dir_world.z = (fabs(dir_world.z) < epsilon) ? epsilon : dir_world.z;

    // Place the volume-data inside the box from @box_min to @box_max
    float3 bound_min = (float3)(-1.0f, -1.0f, -1.0f);
    float3 bound_max = (float3)( 1.0f,  1.0f,  1.0f);

    // Intersect the ray with the large box
    float t_near, t_far;
    int hit = intersect_box(camera, dir_world,
                            bound_min, bound_max,
                            &t_near, &t_far);
    if (! hit) {
        write_imagef(image, uv, (float4)(0.0f, 0.0f, 0.0f, 1.0f));
        return;
    }

    // Ray-casting
    // Constants
    float t_epsilon = 0.0001;
    float3 voxel_size = 2.0f / range;
    int max_iteration = (int)(range.x + range.y + range.z);

    // Variables
    float4 color = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
    float t = (t_near < 0) ? 0 : t_near;
    float3 p = camera + (t + t_epsilon) * dir_world;

    while (bound_min.x < p.x && p.x < bound_max.x
    &&     bound_min.y < p.y && p.y < bound_max.y
    &&     bound_min.z < p.z && p.z < bound_max.z) {
        int4 voxel_index = (int4)((int)((p.x - (-1.0f)) / voxel_size.x),
                                  (int)((p.y - (-1.0f)) / voxel_size.y),
                                  (int)((p.z - (-1.0f)) / voxel_size.z),
                                  1);
        color += read_imagef(volume, sampler, voxel_index);

        float3 voxel_min = (float3)(-1.0f, -1.0f, -1.0f)
            + voxel_size * (float3)(voxel_index.x,
                                    voxel_index.y,
                                    voxel_index.z);
        float3 voxel_max = voxel_min + voxel_size;
        float t_voxel_near, t_voxel_far;
        int voxel_hit = intersect_box(p, dir_world,
                                      voxel_min, voxel_max,
                                      &t_voxel_near, &t_voxel_far);
        t_voxel_far = t_voxel_far < t_epsilon ? t_epsilon : t_voxel_far;
        p = p + (t_voxel_far + t_epsilon) * dir_world;
    }
    write_imagef(image, uv, 0.01f * (float4)(0.0f, color.y, 0.0f, 1.0f));
}