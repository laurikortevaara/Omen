#ifdef COMPUTE_SHADER

uniform int N;
uniform float len;

struct Data
{
	vec4 pos;
	vec4 normal;
};

layout (std430, binding=2) buffer shader_data { Data sh_data[]; };
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) writeonly uniform image2D img_output;
layout(rgba32f, binding = 1) writeonly uniform image2D img_output2;


void main() {
  ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
  int index = pix.x+(N+1)*pix.y;
  imageStore(img_output, pix, sh_data[index].pos);
  imageStore(img_output2, pix, sh_data[index].normal);
}

#endif