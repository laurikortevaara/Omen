#ifdef COMPUTE_SHADER

uniform int N;

layout (std430, binding=2) buffer shader_data { vec4 sh_data[]; };
layout(local_size_x = 16, local_size_y = 16) in;

//layout(rgba32f, binding = 0) 
writeonly uniform image2D img_output;


void main() {
  ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
  ivec2 size = imageSize(img_output);
  if (pix.x >= size.x || pix.y >= size.y) {
    return;
  }
  vec2 pos = vec2(pix) / vec2(size.x, size.y);
  vec3 dir = vec3(pos.x,pos.y,0);
  vec4 color = vec4(abs(vec3(sh_data[pix.x+N*pix.y].y)),1);
  imageStore(img_output, pix, color);
}

#endif