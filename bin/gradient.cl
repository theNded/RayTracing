const sampler_t sampler   = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE |
                            CLK_FILTER_LINEAR;

__kernel void gradient(__read_only  image3d_t volume_raw,
                       __write_only image3d_t volume_gradient,
                       int          sample_step) {
    int4 min_idx = (int4)(0, 0, 0, 0);
    int4 max_idx = (int4)(get_global_size(0) - 1,
                          get_global_size(1) - 1,
                          get_global_size(2) - 1,
                          1);

    int4 idx = (int4)(get_global_id(0),
                      get_global_id(1),
                      get_global_id(2),
                      1);

    float4 diff_x = read_imagef(volume_raw, sampler,
                                clamp(idx + (int4)(sample_step, 0, 0, 0),
                                      min_idx,
                                      max_idx))
                  - read_imagef(volume_raw, sampler,
                                clamp(idx - (int4)(sample_step, 0, 0, 0),
                                      min_idx,
                                      max_idx));

    float4 diff_y = read_imagef(volume_raw, sampler,
                                clamp(idx + (int4)(0, sample_step, 0, 0),
                                      min_idx,
                                      max_idx))
                  - read_imagef(volume_raw, sampler,
                                clamp(idx - (int4)(0, sample_step, 0, 0),
                                      min_idx,
                                      max_idx));

    float4 diff_z = read_imagef(volume_raw, sampler,
                                clamp(idx + (int4)(0, 0, sample_step, 0),
                                      min_idx,
                                      max_idx))
                  - read_imagef(volume_raw, sampler,
                                clamp(idx - (int4)(0, 0, sample_step, 0),
                                      min_idx,
                                      max_idx));


    float4 gradient = normalize((float4)(diff_x.x, diff_y.x, diff_z.x, 0));
    write_imagef(volume_gradient, idx, gradient);
}