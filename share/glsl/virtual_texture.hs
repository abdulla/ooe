#extension GL_EXT_gpu_shader4 : enable

struct vsampler2D
{
    vec2 bias_range;
    sampler2DArray page_cache;
    sampler2D page_table;
};

vec4 vtexture2D( vsampler2D, vec2 );
