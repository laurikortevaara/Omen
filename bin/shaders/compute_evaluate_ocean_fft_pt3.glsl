#ifdef COMPUTE_SHADER

#include "utils.glsl"
#include "FFT.glsl"

const uint s = 129;
const int N=128;

uniform int stride;
uniform int offset;
uniform int which;

struct Data
{

	Complex htilde_in;
	Complex htilde_out;
	int 	reversed;
	Complex	C0;
	Complex C1;
};

layout(local_size_x = 129) in;

layout (binding=0) buffer shader_data { Data data[]; };

void main() 
{
	uint index = gl_GlobalInvocationID.x;
	float n_prime = index % (N+1);
	float m_prime = index / (N+1);

	int reversed = data[index].reversed;
	int cIndex = reversed * stride + offset;

	if(which==0)
		data[index].C0 = data[cIndex].htilde_in;
	if(which==1)
		data[index].C1 = data[cIndex].htilde_in;
}

#endif