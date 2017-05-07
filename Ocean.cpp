//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Ocean.h"
#include "GL_error.h"
#include "Engine.h"
#include "component/KeyboardInput.h"
#include "system/GraphicsSystem.h"
#include "MeshProvider.h"
#include "component/MeshController.h"

/* Constants rounded for 21 decimals. */
#define M_E 2.71828182845904523536 // The base of natural logarithms
#define M_LOG2E 1.44269504088896340736  // The logarithm to base 2 of M_E
#define M_LOG10E 0.434294481903251827651 // The logarithm to base 10 of M_E.
#define M_LN2 0.693147180559945309417 // The natural logarithm of 2
#define M_LN10 2.30258509299404568402 // The natural logarithm of 10
#define M_PI 3.14159265358979323846 // Pi, the ratio of a circle's circumference to its diameter
#define M_PI_2 1.57079632679489661923 // Pi divided by two
#define M_PI_4 0.785398163397448309616 // Pi divided by four
#define M_1_PI 0.318309886183790671538 // The reciprocal of pi (1/pi) 
#define M_2_PI 0.636619772367581343076 // Two times the reciprocal of pi 2*(1/pi)
#define M_1_SQRTPI 0.564189583547756286948 // The reciprocal of the square root of pi (1/sqrt(pi))
#define M_2_SQRTPI 1.12837916709551257390 // Two times the reciprocal of the square root of pi (2*(1/sqrt(pi)))
#define M_SQRT2 1.41421356237309504880 // The square root of two (sqrt(2))
#define M_SQRT_2 0.707106781186547524401 // The reciprocal of the square root of two (also the square root of 1/2) ( sqrt(2)/2 or sqrt(1/2) )
#define M_SQRT_2PI 2.506628274631000502415 // (sqrt(2*pi))

using namespace omen;

const unsigned int N = 128;
const unsigned int Nplus1 = N + 1;
const float ocean_length = N;
const unsigned int Nx = N;
const unsigned int Nz = N;
const float A = 0.0005; // Could be also 0.0005 Wave height adjustmet factor
const float g = 9.80665; // Gravity 9.81 m/s^2

cFFT* fft = new cFFT(N);

const glm::vec2 w(-32.0f, -32.0f);

struct OceanVertex
{
	glm::vec3 pos;  // Animated position (x,y,z)
	glm::vec3 opos; // original position
	glm::vec3 normal; // Normal
	Complex htilde0; // htilde0 (1/sqrt(2)*(GaussRand(R,I))*sqrt(Phillips(k)))
	Complex htilde0_conj; // Conjugate of the htilde0
};

struct ComplexVectorNormal {	// structure used with discrete fourier transform
	Complex height;		// wave height
	glm::vec2 displacement;		// displacement
	glm::vec3 normal;		// normal
};

OceanVertex vertices[Nplus1*Nplus1];
Complex h_tilde[Nplus1*Nplus1];
Complex h_tilde_slopex[Nplus1*Nplus1];
Complex h_tilde_slopez[Nplus1*Nplus1];
Complex h_tilde_dx[Nplus1*Nplus1];
Complex h_tilde_dz[Nplus1*Nplus1];

glm::vec4 shader_buffer[Nplus1*Nplus1];


GLuint tex_output;
Shader* compute_shader;
Texture* tex;

static inline int alias(int x, int N)
{
	if (x > N / 2)
		x -= N;
	return x;
}

/**
	Returns 1D uniform[0..1] random value
*/
float uniformRandomVariable()
{
	return (float)rand() / RAND_MAX;
}

float phillips(int n_prime, int m_prime) 
{
	glm::vec2 k(M_PI * (2 * n_prime - N) / ocean_length, M_PI * (2 * m_prime - N) / ocean_length);
	float k_length = glm::length(k);
	//std::cout << "k_length: " << k_length << ", k:" << k.x << ", " << k.y << std::endl;
	if (k_length < 0.000001) return 0.0;

	float k_length2 = k_length  * k_length;
	float k_length4 = k_length2 * k_length2;

	//std::cout << "k:" << k.x << ", " << k.y << std::endl;
	//std::cout << "w:" << w.x << ", " << w.y << std::endl;
	float k_dot_w = glm::dot(glm::normalize(k), glm::normalize(w));
	//std::cout << "k_dot_w:" << k_dot_w << std::endl;
	float k_dot_w2 = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;

	float w_length = glm::length(w);
	float L = w_length * w_length / g;
	float L2 = L * L;

	float damping = 0.001;
	float l2 = L2 * damping * damping;

	float ret =  A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
	//std::cout << "ret:" << ret << std::endl;
	return ret;
}

/**
	Returns normalized/uniform Gaussian random value
*/
Complex gaussianRandomVariable() 
{
	float x1, x2, w;
	do {
		x1 = 2.f * uniformRandomVariable() - 1.f;
		x2 = 2.f * uniformRandomVariable() - 1.f;
		w = x1 * x1 + x2 * x2;
	} while (w >= 1.f);
	w = sqrt((-2.f * log(w)) / w);
	return Complex(x1 * w, x2 * w);
}

float dispersion(int n_prime, int m_prime) {
	float w_0 = 2.0f * M_PI / 200.0f;
	float kx = M_PI * (2 * n_prime - N) / ocean_length;
	float kz = M_PI * (2 * m_prime - N) / ocean_length;
	return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

Complex hTilde_0(int n_prime, int m_prime)
{
	Complex r = gaussianRandomVariable();
	return r * sqrt(phillips(n_prime, m_prime) / 2.0f);
}


Complex hTilde(float t, int n_prime, int m_prime)
{
	int index = m_prime * Nplus1 + n_prime;

	Complex htilde0 = vertices[index].htilde0;
	Complex htilde0mkconj = vertices[index].htilde0_conj;

	float omegat = dispersion(n_prime, m_prime) * t;

	float cos_ = cos(omegat);
	float sin_ = sin(omegat);

	Complex c0(cos_, sin_);
	Complex c1(cos_, -sin_);

	Complex res = htilde0 * c0 + htilde0mkconj * c1;

	return htilde0 * c0 + htilde0mkconj*c1;
}

ComplexVectorNormal h_D_and_n(glm::vec2 x, float t)
{
	Complex h(0.0f, 0.0f);
	glm::vec2 D(0.0f, 0.0f);
	glm::vec3 n(0.0f, 0.0f, 0.0f);

	Complex c, res, htilde_c;
	glm::vec2 k;
	float kx, kz, k_length, k_dot_x;

	for (int m_prime = 0; m_prime < N; m_prime++) {
		kz = 2.0f * M_PI * (m_prime - N / 2.0f) / ocean_length;
		for (int n_prime = 0; n_prime < N; n_prime++) {
			kx = 2.0f * M_PI * (n_prime - N / 2.0f) / ocean_length;
			k = glm::vec2(kx, kz);

			//std::cout << "Kx,Kz: " << kx << ", " << kz << std::endl;

			k_length = glm::length(k);
			k_dot_x = glm::dot(k,x);

			c = Complex(cos(k_dot_x), sin(k_dot_x));
			htilde_c = hTilde(t, n_prime, m_prime) * c;
			
			h = h + htilde_c;

			//std::cout << "HTilde: " << htilde_c.a << ", " << htilde_c.b << ", H: " << h.a << ", " << h.b << std::endl;

			n = n + glm::vec3(-kx * htilde_c.b, 0.0f, -kz * htilde_c.b);

			if (k_length < 0.000001) continue;
			D = D + glm::vec2(kx / k_length * htilde_c.b, kz / k_length * htilde_c.b);
		}
	}

	n = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) - n);

	ComplexVectorNormal cvn;
	cvn.height = h;
	cvn.displacement = D;
	cvn.normal = n;
	return cvn;
}

void evaluateWavesFFT(float t) {
	float kx, kz, len, lambda = -1.0f;
	int index, index1;

	for (int m_prime = 0; m_prime < N; m_prime++) {
		kz = M_PI * (2.0f * m_prime - N) / ocean_length;
		for (int n_prime = 0; n_prime < N; n_prime++) {
			kx = M_PI*(2 * n_prime - N) / ocean_length;
			len = sqrt(kx * kx + kz * kz);
			index = m_prime * N + n_prime;

			h_tilde[index] = hTilde(t, n_prime, m_prime);
			h_tilde_slopex[index] = h_tilde[index] * Complex(0, kx);
			h_tilde_slopez[index] = h_tilde[index] * Complex(0, kz);
			if (len < 0.000001f) {
				h_tilde_dx[index] = Complex(0.0f, 0.0f);
				h_tilde_dz[index] = Complex(0.0f, 0.0f);
			}
			else {
				h_tilde_dx[index] = h_tilde[index] * Complex(0, -kx / len);
				h_tilde_dz[index] = h_tilde[index] * Complex(0, -kz / len);
			}
		}
	}

	for (int m_prime = 0; m_prime < N; m_prime++) {
		fft->fft(h_tilde, h_tilde, 1, m_prime * N);
		fft->fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
		fft->fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
		fft->fft(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
		fft->fft(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
	}
	for (int n_prime = 0; n_prime < N; n_prime++) {
		fft->fft(h_tilde, h_tilde, N, n_prime);
		fft->fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
		fft->fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
		fft->fft(h_tilde_dx, h_tilde_dx, N, n_prime);
		fft->fft(h_tilde_dz, h_tilde_dz, N, n_prime);
	}

	int sign;
	float signs[] = { 1.0f, -1.0f };
	glm::vec3 n;
	for (int m_prime = 0; m_prime < N; m_prime++) {
		for (int n_prime = 0; n_prime < N; n_prime++) {
			index = m_prime * N + n_prime;		// index into h_tilde..
			index1 = m_prime * Nplus1 + n_prime;	// index into vertices

			sign = signs[(n_prime + m_prime) & 1];

			h_tilde[index] = h_tilde[index] * sign;

			// height
			vertices[index1].pos.y = h_tilde[index].a;

			// displacement
			h_tilde_dx[index] = h_tilde_dx[index] * sign;
			h_tilde_dz[index] = h_tilde_dz[index] * sign;
			vertices[index1].pos.x = vertices[index1].opos.x + h_tilde_dx[index].a * lambda;
			vertices[index1].pos.z = vertices[index1].opos.z + h_tilde_dz[index].a * lambda;

			// normal
			h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
			h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
			n = glm::normalize(glm::vec3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a));
			vertices[index1].normal.x = n.x;
			vertices[index1].normal.y = n.y;
			vertices[index1].normal.z = n.z;

			// for tiling
			if (n_prime == 0 && m_prime == 0) {
				vertices[index1 + N + Nplus1 * N].pos.y = h_tilde[index].a;

				vertices[index1 + N + Nplus1 * N].pos.x = vertices[index1 + N + Nplus1 * N].opos.x + h_tilde_dx[index].a * lambda;
				vertices[index1 + N + Nplus1 * N].pos.z = vertices[index1 + N + Nplus1 * N].opos.z + h_tilde_dz[index].a * lambda;

				vertices[index1 + N + Nplus1 * N].normal.x = n.x;
				vertices[index1 + N + Nplus1 * N].normal.y = n.y;
				vertices[index1 + N + Nplus1 * N].normal.z = n.z;
			}
			if (n_prime == 0) {
				vertices[index1 + N].pos.y = h_tilde[index].a;

				vertices[index1 + N].pos.x = vertices[index1 + N].opos.x + h_tilde_dx[index].a * lambda;
				vertices[index1 + N].pos.z = vertices[index1 + N].opos.z + h_tilde_dz[index].a * lambda;

				vertices[index1 + N].normal.x = n.x;
				vertices[index1 + N].normal.y = n.y;
				vertices[index1 + N].normal.z = n.z;
			}
			if (m_prime == 0) {
				vertices[index1 + Nplus1 * N].pos.y = h_tilde[index].a;

				vertices[index1 + Nplus1 * N].pos.x = vertices[index1 + Nplus1 * N].opos.x + h_tilde_dx[index].a * lambda;
				vertices[index1 + Nplus1 * N].pos.z = vertices[index1 + Nplus1 * N].opos.z + h_tilde_dz[index].a * lambda;

				vertices[index1 + Nplus1 * N].normal.x = n.x;
				vertices[index1 + Nplus1 * N].normal.y = n.y;
				vertices[index1 + Nplus1 * N].normal.z = n.z;
			}
		}
	}
}

void evaluate_waves(float t)
{
	float lambda = -1.0;
	int index;
	glm::vec2 x;
	glm::vec2 d;
	ComplexVectorNormal h_d_and_n;
	for (int m_prime = 0; m_prime < N; m_prime++) {
		for (int n_prime = 0; n_prime < N; n_prime++) {
			index = m_prime * Nplus1 + n_prime;

			x = glm::vec2(vertices[index].pos);

			h_d_and_n = h_D_and_n(x, t);

			vertices[index].pos.y = h_d_and_n.height.a;

			vertices[index].pos.x = vertices[index].opos.x + lambda*h_d_and_n.displacement.x;
			vertices[index].pos.z = vertices[index].opos.z + lambda*h_d_and_n.displacement.y;

			vertices[index].normal = glm::vec3(h_d_and_n.normal.x, h_d_and_n.normal.y, h_d_and_n.normal.z);

			if (n_prime == 0 && m_prime == 0) {
				vertices[index + N + Nplus1 * N].pos.y = h_d_and_n.height.a;

				vertices[index + N + Nplus1 * N].pos.x = vertices[index + N + Nplus1 * N].opos.x + lambda*h_d_and_n.displacement.x;
				vertices[index + N + Nplus1 * N].pos.z = vertices[index + N + Nplus1 * N].opos.z + lambda*h_d_and_n.displacement.y;

				vertices[index + N + Nplus1 * N].normal.x = h_d_and_n.normal.x;
				vertices[index + N + Nplus1 * N].normal.y = h_d_and_n.normal.y;
				vertices[index + N + Nplus1 * N].normal.z = h_d_and_n.normal.z;
			}
			if (n_prime == 0) {
				vertices[index + N].pos.y = h_d_and_n.height.a;

				vertices[index + N].pos.x = vertices[index + N].opos.x + lambda*h_d_and_n.displacement.x;
				vertices[index + N].pos.z = vertices[index + N].opos.z + lambda*h_d_and_n.displacement.y;

				vertices[index + N].normal.x = h_d_and_n.normal.x;
				vertices[index + N].normal.y = h_d_and_n.normal.y;
				vertices[index + N].normal.z = h_d_and_n.normal.z;
			}
			if (m_prime == 0) {
				vertices[index + Nplus1 * N].pos.y = h_d_and_n.height.a;

				vertices[index + Nplus1 * N].pos.x = vertices[index + Nplus1 * N].opos.x + lambda*h_d_and_n.displacement.x;
				vertices[index + Nplus1 * N].pos.z = vertices[index + Nplus1 * N].opos.z + lambda*h_d_and_n.displacement.y;

				vertices[index + Nplus1 * N].normal.x = h_d_and_n.normal.x;
				vertices[index + Nplus1 * N].normal.y = h_d_and_n.normal.y;
				vertices[index + Nplus1 * N].normal.z = h_d_and_n.normal.z;
			}
		}
	}
}

Ocean::Ocean() :
	GameObject("Ocean")
{


	int index;

	Complex htilde0, htilde0mk_conj;
	for (int m_prime = 0; m_prime < Nplus1; m_prime++) {
		for (int n_prime = 0; n_prime <  Nplus1; n_prime++) {
			index = m_prime * Nplus1 + n_prime;
			htilde0 = hTilde_0(n_prime, m_prime);
			//std::cout << "HTilde0[" << index << "]: " << htilde0.a << ", " << htilde0.b << std::endl;
			htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conjugate();

			vertices[index].htilde0 = htilde0;
			vertices[index].htilde0_conj = htilde0mk_conj;

			vertices[index].opos = vertices[index].pos = glm::vec3((n_prime - N / 2.0f) * ocean_length / N, 0.0f, (m_prime - N / 2.0f) * ocean_length / N);
			vertices[index].normal = glm::vec3(0, 1, 0);
		}
	}

	//evaluate_waves(0);

	for (int x = 0; x < Nx; ++x)
	for (int z = 0; z < Nz; ++z)
	{
		shader_buffer[z*Nx + x] = glm::vec4(vertices[z*Nx + x].pos.x, vertices[z*Nx + x].pos.y, vertices[z*Nx + x].pos.z, 1);
	}

	std::unique_ptr<ecs::MeshController> mc = std::make_unique<ecs::MeshController>();
	std::unique_ptr<OceanRenderer> mr = std::make_unique<OceanRenderer>(mc.get());

	std::unique_ptr<Mesh> mesh = MeshProvider::createPlane(1000, 1);

	mc->setMesh(std::move(mesh));
	addComponent(std::move(mr));
	addComponent(std::move(mc));

}


Ocean::~Ocean()
{
}

/**
 The phillips function returns the synthetized ocean wave frequency height
*/
double OceanRenderer::phillips(glm::vec2 k, float max_l)
{
	float L = 1.0f;
	glm::vec2 wind_dir(1, 1);
	float k_len = glm::length(k); 
	if (k_len == 0.0f)
	{
		return 0.0f;
	}
	float kL = k_len * L;
	glm::vec2 k_dir = glm::normalize(k);
	float kw = glm::dot(k_dir,wind_dir);
	return
		pow(kw * kw, 1.0f) * // Directional
		exp(-1.0 * k_len * k_len * max_l * max_l) * // Suppress small waves at ~max_l.
		exp(-1.0f / (kL * kL)) *
		pow(k_len, -4.0f);
}

double normal_dist(double x)
{
	return pow(M_E, -0.5*pow(x, 1)) / M_SQRT_2PI;
}

void OceanRenderer::generate_distribution(Complex *distribution, glm::vec2 size, float amplitude, float max_l)
{
	glm::vec2 mod = glm::vec2(2.0f * M_PI) / size;
	for (unsigned z = 0; z < Nz; z++)
	{
		for (unsigned x = 0; x < Nx; x++)
		{
			auto &v = distribution[z * Nx + x];
			glm::vec2 k = mod * glm::vec2(alias(x, Nx), alias(z, Nz));
			Complex dist = Complex(normal_dist(x), normal_dist(z));
			v = dist * amplitude * sqrt(0.5f * phillips(k, max_l));
		}
	}
}

OceanRenderer::OceanRenderer(ecs::MeshController* mc) :
	MeshRenderer(mc)
{
	initializeShader();
	
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vbo_texcoord);
	
	glBindVertexArray(m_vao);
	
	glm::vec3 v[4] = { { -1.0,0,1.0 },{ -1.0,0,-1.0 },{ 1.0, 0,1.0 },{ 1.0,0,-1.0 } };
	glm::vec2 t[4] = { { 0,1 },{ 0,0 },{ 1, 1 },{ 1,0 } };
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	//tex = new Texture("textures/custom_uv_diag.png");
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t), t, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	compute_shader = new Shader("shaders/compute_test.glsl");
	compute_shader->use();
	compute_shader->setUniform1i("img_output", 0);
	compute_shader->setUniform1i("N", N);
	
	// Create SSBO
	glGenBuffers(1, &m_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer), &shader_buffer, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	// dimensions of the image
	int tex_w = Nx, tex_h = Nz;
	
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
	
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	
	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);

	tex = new Texture("textures/nightflatcrop-FINAL.jpg");
}

OceanRenderer::~OceanRenderer() 
{
}

void OceanRenderer::render(Shader* sh) {

	//evaluate_waves(Engine::instance()->time());
	evaluateWavesFFT(Engine::instance()->time());

	for (int x = 0; x < Nx; ++x)
	for (int z = 0; z < Nz; ++z)
	{
		shader_buffer[z*Nx + x] = glm::vec4(vertices[z*Nx + x].pos.x, vertices[z*Nx + x].pos.y, vertices[z*Nx + x].pos.z, 1);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer), &shader_buffer, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	compute_shader->use();
	compute_shader->setUniform1i("N", N);
	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(compute_shader->m_shader_program, GL_SHADER_STORAGE_BLOCK, "shader_data");
	GLuint ssbo_binding_point_index = 2;
	glShaderStorageBlockBinding(compute_shader->m_shader_program, block_index, ssbo_binding_point_index);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);

	//glEnable(GL_TEXTURE_2D);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex_output);
	//compute_shader->setUniform1i("img_output", tex_output);
	glDispatchCompute((GLuint)Nx, (GLuint)Nz, 1);
	
	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Read back the stuff from ssbo
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glm::vec4* ptr = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER,GL_READ_ONLY);
	
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, Engine::instance()->getPolygonMode());
    shader()->use();
	glEnable(GL_TEXTURE_2D);
    //shader()->setUniform4fv("Color", 1, &glm::vec4(0,0,0,1)[0] );
    shader()->setUniform1f("Time", (float) Engine::instance()->time());
    shader()->setUniform4fv("Color", 1, &glm::vec4(1)[0]);
    shader()->setUniformMatrix4fv("ViewMatrix", 1, &Engine::instance()->camera()->view()[0][0], false);
    shader()->setUniformMatrix3fv("NormalMatrix", 1, &glm::mat3(Engine::instance()->camera()->view())[0][0], false);
    shader()->setUniformMatrix4fv("Model", 1, &glm::mat4(1)[0][0], false);
    shader()->setUniformMatrix4fv("ModelViewProjection", 1,
                                  &(Engine::instance()->camera()->viewProjection())[0][0], false);
	shader()->setUniform1i("size", N);
    /*shader()->setUniform1i("InnerTessellationLevel1", m_innerTessellationLevels[0]);
    shader()->setUniform1i("InnerTessellationLevel2", m_innerTessellationLevels[1]);

    shader()->setUniform1i("OuterTessellationLevel1", m_outerTessellationLevels[0]);
    shader()->setUniform1i("OuterTessellationLevel2", m_outerTessellationLevels[1]);
    shader()->setUniform1i("OuterTessellationLevel3", m_outerTessellationLevels[2]);
    shader()->setUniform1i("OuterTessellationLevel4", m_outerTessellationLevels[3]);
	*/
	glDisable(GL_CULL_FACE);
	
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//drawArrays(GL_TRIANGLE_STRIP, 0, 4);
	{ // normal drawing pass
	
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, tex->id()); // tex_output);
		glBindTexture(GL_TEXTURE_2D, tex_output);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, N*N);
	}
}

void OceanRenderer::initializeShader() {
    setShader(std::move(std::make_unique<Shader>(("shaders/ocean.glsl"))));
}

void OceanRenderer::initializeTexture() {
    setTexture(nullptr);
}

cFFT::cFFT(unsigned int N) : N(N), reversed(0), T(0), pi2(2 * M_PI) {
	c[0] = c[1] = 0;

	log_2_N = log(N) / log(2);

	reversed = new unsigned int[N];		// prep bit reversals
	for (int i = 0; i < N; i++) reversed[i] = reverse(i);

	int pow2 = 1;
	T = new Complex*[log_2_N];		// prep T
	for (int i = 0; i < log_2_N; i++) {
		T[i] = new Complex[pow2];
		for (int j = 0; j < pow2; j++) T[i][j] = t(j, pow2 * 2);
		pow2 *= 2;
	}

	c[0] = new Complex[N];
	c[1] = new Complex[N];
	which = 0;
}

cFFT::~cFFT() {
	if (c[0]) delete[] c[0];
	if (c[1]) delete[] c[1];
	if (T) {
		for (int i = 0; i < log_2_N; i++) if (T[i]) delete[] T[i];
		delete[] T;
	}
	if (reversed) delete[] reversed;
}

unsigned int cFFT::reverse(unsigned int i) {
	unsigned int res = 0;
	for (int j = 0; j < log_2_N; j++) {
		res = (res << 1) + (i & 1);
		i >>= 1;
	}
	return res;
}

Complex cFFT::t(unsigned int x, unsigned int N) {
	return Complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void cFFT::fft(Complex* input, Complex* output, int stride, int offset) {
	for (int i = 0; i < N; i++) c[which][i] = input[reversed[i] * stride + offset];

	int loops = N >> 1;
	int size = 1 << 1;
	int size_over_2 = 1;
	int w_ = 0;
	for (int i = 1; i <= log_2_N; i++) {
		which ^= 1;
		for (int j = 0; j < loops; j++) {
			for (int k = 0; k < size_over_2; k++) {
				c[which][size * j + k] = c[which ^ 1][size * j + k] +
					c[which ^ 1][size * j + size_over_2 + k] * T[w_][k];
			}

			for (int k = size_over_2; k < size; k++) {
				c[which][size * j + k] = c[which ^ 1][size * j - size_over_2 + k] -
					c[which ^ 1][size * j + k] * T[w_][k - size_over_2];
			}
		}
		loops >>= 1;
		size <<= 1;
		size_over_2 <<= 1;
		w_++;
	}

	for (int i = 0; i < N; i++) output[i * stride + offset] = c[which][i];
}