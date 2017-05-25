#ifdef COMPUTE_SHADER

#include "utils.glsl"

const uint s = 129;

uniform int N;
uniform float nper2;
uniform float lpern;
uniform float damping;
uniform float g;
uniform vec2  wind;
uniform float A;

struct Data
{
	vec4 position;
	vec4 opos;
	vec4 normal;
	Complex htilde0;
	Complex htilde0_conj;
};

layout(local_size_x = 129) in;

layout (binding=0) buffer shader_data { Data data[]; };

float uniformRandomVariable()
{
	return nrand(vec2(iGlobalTime,iGlobalTime));
}

float phillips(in float n_prime, in float m_prime) 
{
	vec2 k = vec2(M_PI * (2 * n_prime - N) / N, M_PI * (2 * m_prime - N) / N);
	float k_length = length(k);

	if (k_length < 0.000001) return 0.0;

	float k_length2 = k_length  * k_length;
	float k_length4 = k_length2 * k_length2;

	float k_dot_w = dot(normalize(k), normalize(wind));
	float k_dot_w2 = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;

	float w_length = length(wind);
	float L = w_length * w_length / g;
	float L2 = L * L;

	float l2 = L2 * damping * damping;

	float ret =  A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);

	return ret;
}

Complex gaussianRandomVariable()
{
	if(true)
		return Complex(uniformRandomVariable(), uniformRandomVariable());
	float x1, x2, w;
	do {
	  x1 = 2.0 * uniformRandomVariable() - 1.0;
	  x2 = 2.0 * uniformRandomVariable() - 1.0;
	  w = x1 * x1 + x2 * x2;
	} while(w>=1.0);
	w = sqrt(-2.0 * log(w) / w);
	return Complex(x1 * w, x2 * w);
}

Complex htilde0(in float n_prime, in float m_prime)
{
	Complex r = gaussianRandomVariable();
	return mul(r, sqrt(phillips(n_prime, m_prime) / 2.0));
}

void main() {
  uint index = gl_GlobalInvocationID.x;
  float n_prime = index % 129;
  float m_prime = index / 129;
  
  float x = (n_prime - nper2) * lpern;
  float y = 0.0;
  float z = (m_prime - nper2) * lpern;

  data[index].opos=vec4(x,y,z,1);
  data[index].position=data[index].opos;
  data[index].normal=vec4(0,1,0,1);

  data[index].htilde0 = htilde0(n_prime, m_prime);
  data[index].htilde0_conj = conjugate(htilde0(-n_prime, -m_prime));
}

#endif