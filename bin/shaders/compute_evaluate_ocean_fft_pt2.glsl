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
uniform float ocean_length;

struct OceanVertices
{
	vec4 position;
	vec4 opos;
	vec4 normal;
	Complex htilde0;
	Complex htilde0_conj;
};

struct HTilde
{
	Complex htilde;
	Complex htilde_slopex;
	Complex htilde_slopez;
	Complex htilde_dx;
	Complex htilde_dz;
};

layout(local_size_x = 129) in;

layout (binding=0) readonly buffer shader_data_vertices_in { OceanVertices vertices[]; };
layout (binding=1) buffer shader_data_htildes_in { HTilde htildes[]; };

float uniformRandomVariable()
{
	return nrand(vec2(cos(iGlobalTime*78.233), -sin(12.9898*iGlobalTime)));
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

float dispersion(in float n_prime, in float m_prime) {
	float w_0 = 2.0f * M_PI / 200.0f;
	float kx = M_PI * (2 * n_prime - N) / ocean_length;
	float kz = M_PI * (2 * m_prime - N) / ocean_length;
	return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

Complex hTilde(in float t, in float n_prime, in float m_prime)
{
	int index = int(m_prime * (N+1) + n_prime);

	Complex htilde0 = vertices[index].htilde0;
	Complex htilde0mkconj = vertices[index].htilde0_conj;

	float omegat = dispersion(n_prime, m_prime) * t;

	float cos_ = cos(omegat);
	float sin_ = sin(omegat);

	Complex c0 = Complex(cos_, sin_);
	Complex c1 = Complex(cos_, -sin_);

	Complex res = sum(mul(htilde0, c0),mul(htilde0mkconj, c1));

	return sum(mul(htilde0, c0), mul(htilde0mkconj,c1));
}

void main() {
  uint index = gl_GlobalInvocationID.x;
  float n_prime = index % 129;
  float m_prime = index / 129;
  
  float kx, kz, len, lambda = -1.0f; 

  kz = M_PI*(2*m_prime-N)/ocean_length;
  kx = M_PI*(2*n_prime-N)/ocean_length;
  len = sqrt(kx*kx + kz*kz);
  
  htildes[index].htilde = hTilde(iGlobalTime, n_prime, m_prime);
  htildes[index].htilde_slopex = mul(htildes[index].htilde,kx);
  htildes[index].htilde_slopez = mul(htildes[index].htilde,kz);

  if(len < 0.000001)
  {
	htildes[index].htilde_dx = Complex(0,0);
	htildes[index].htilde_dz = Complex(0,0);
  }
  else 
  {
	htildes[index].htilde_dx = mul(htildes[index].htilde, Complex(0,-kx/len));
	htildes[index].htilde_dz = mul(htildes[index].htilde, Complex(0,-kz/len));
  }
}

#endif