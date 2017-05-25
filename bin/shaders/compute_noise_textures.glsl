#ifdef COMPUTE_SHADER

#include "utils.glsl"
#include "noise4d.glsl"

layout(local_size_x = 1, local_size_y = 1) in;

layout(binding = 0, rgba32f) uniform image2D img_output_1;



void main() 
{
  ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
  float r1 = abs(snoise(vec4(iGlobalTime, pix.x, pix.y, 1)));
  float r2 = abs(snoise(vec4(iGlobalTime, pix.x, pix.y, 2)));
  float r3 = abs(snoise(vec4(iGlobalTime, pix.x, pix.y, 3)));
  float r4 = abs(snoise(vec4(iGlobalTime, pix.x, pix.y, 4)));
  imageStore(img_output_1, pix, vec4(r1,r2,r3,r4) );
}

#endif