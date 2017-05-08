#ifdef COMPUTE_SHADER

uniform int N;

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
  ivec2 size = imageSize(img_output);
  if (pix.x >= size.x || pix.y >= size.y) {
    return;
  }
  vec2 pos = vec2(pix) / vec2(size.x, size.y);
  vec3 dir = vec3(pos.x,pos.y,0);
  vec4 position = sh_data[pix.x+N*pix.y].pos;
  vec4 normal = sh_data[pix.x+N*pix.y].normal;

  imageStore(img_output, pix, position);
  imageStore(img_output2, pix, normal);
}

#endif