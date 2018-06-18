//
// Created by Lauri Kortevaara on 11/01/16.
//

#include "Ocean.h"
#include "GL_error.h"
#include "Engine.h"
#include "component/KeyboardInput.h"
#include "system/GraphicsSystem.h"
#include "MeshProvider.h"
#include "component/MeshController.h"
#include "SkyBoxRenderer.h"

using namespace omen;

const int N = 256;
const int log_2_N = static_cast<const int>(log(N) / log(2));
const int Nplus1 = N + 1;
float ocean_length = N;
const int Nx = N;
const int Nz = N;
float A = 0.0005f; // Could be also 0.0005 Wave height adjustmet factor
float g = 9.80665f; // Gravity 9.81 m/s^2
float damping = 0.001f;
float wind_dir = 0.0f;
float wind_power = 32.0f;
float heightFactor = 1.0f;
int		patchCount = 3;
float	patchSize = 100.0f;

FFT* fft = new FFT(N);

glm::vec2 wind(32, 32);

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

struct Data {
	glm::vec4 pos;
	glm::vec4 normal;
};

Data shader_buffer[Nplus1*Nplus1];

const unsigned long vcount = Nplus1*Nplus1;

// The struct has to be size of multiples of 16 bytes (4 floats)
struct OceanVertices_Htilde0
{
	glm::vec4 pos;
	glm::vec4 opos;
	glm::vec4 normal;
	Complex htilde0;
	Complex htilde0_conj;
	//float __padding[2];
}shader_buffer_htilde0[vcount];

struct OceanVertices_Htildes
{
	Complex h_tilde;
	Complex h_tilde_slopex;
	Complex h_tilde_slopez;
	Complex h_tilde_dx;
	Complex h_tilde_dz;
	float __padding[2];
}shader_buffer_htildes[vcount];

GLuint tex_output;
GLuint tex_output2;
GLuint tex_htilde0;
Shader* compute_shader;
Shader* compute_shader_htilde0;
Shader* compute_shader_ocean_fft_1;
Shader* compute_shader_ocean_fft_2;
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
	glm::vec2 k(M_PI * (2 * n_prime - N) / N, M_PI * (2 * m_prime - N) / N);
	float k_length = glm::length(k);
	//std::cout << "k_length: " << k_length << ", k:" << k.x << ", " << k.y << std::endl;
	if (k_length < 0.000001) return 0.0;

	float k_length2 = k_length  * k_length;
	float k_length4 = k_length2 * k_length2;

	//std::cout << "k:" << k.x << ", " << k.y << std::endl;
	//std::cout << "w:" << w.x << ", " << w.y << std::endl;
	float k_dot_w = glm::dot(glm::normalize(k), glm::normalize(wind));
	//std::cout << "k_dot_w:" << k_dot_w << std::endl;
	float k_dot_w2 = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;

	float w_length = glm::length(wind);
	float L = w_length * w_length / g;
	float L2 = L * L;

	float l2 = L2 * damping * damping;

	float ret = A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
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
	float w_0 = static_cast<float>(2.0f * M_PI / 200.0f);
	float kx = static_cast<float>(M_PI * (2 * n_prime - N) / ocean_length);
	float kz = static_cast<float>(M_PI * (2 * m_prime - N) / ocean_length);
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

	for (int m_prime = 0; m_prime < N; m_prime++)
	{
		kz = static_cast<float>(2.0f * M_PI * (m_prime - N / 2.0f) / ocean_length);
		for (int n_prime = 0; n_prime < N; n_prime++)
		{
			kx = static_cast<float>(2.0f * M_PI * (n_prime - N / 2.0f) / ocean_length);
			k = glm::vec2(kx, kz);

			k_length = glm::length(k);
			k_dot_x = glm::dot(k, x);

			c = Complex(cos(k_dot_x), sin(k_dot_x));
			htilde_c = hTilde(t, n_prime, m_prime) * c;

			h = h + htilde_c;
			n = n + glm::vec3(-kx * htilde_c.b, 0.0f, -kz * htilde_c.b);

			if (k_length < 0.000001f) continue;
			D = D + glm::vec2(kx / k_length * htilde_c.a, kz / k_length * htilde_c.b);
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
		kz = static_cast<float>(M_PI * (2.0f * m_prime - N) / ocean_length);
		for (int n_prime = 0; n_prime < N; n_prime++) {
			kx = static_cast<float>(M_PI*(2 * n_prime - N) / ocean_length);
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

			sign = static_cast<int>(signs[(n_prime + m_prime) & 1]);

			h_tilde[index] = h_tilde[index] * static_cast<float>(sign);

			// height
			vertices[index1].pos.y = h_tilde[index].a;

			// displacement
			h_tilde_dx[index] = h_tilde_dx[index] * static_cast<float>(sign);
			h_tilde_dz[index] = h_tilde_dz[index] * static_cast<float>(sign);
			vertices[index1].pos.x = vertices[index1].opos.x + h_tilde_dx[index].a * lambda;
			vertices[index1].pos.z = vertices[index1].opos.z + h_tilde_dz[index].a * lambda;

			// normal
			h_tilde_slopex[index] = h_tilde_slopex[index] * static_cast<float>(sign);
			h_tilde_slopez[index] = h_tilde_slopez[index] * static_cast<float>(sign);
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

void initialize_Htilde0()
{
	int index;

	Complex htilde0, htilde0mk_conj;
	for (int m_prime = 0; m_prime < Nplus1; m_prime++)
	{
		for (int n_prime = 0; n_prime < Nplus1; n_prime++)
		{

			index = m_prime * Nplus1 + n_prime;

			htilde0 = hTilde_0(n_prime, m_prime);
			htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conjugate();

			vertices[index].htilde0 = htilde0;
			vertices[index].htilde0_conj = htilde0mk_conj;

			float x = (n_prime - N / 2.0f) * ocean_length / N;
			float y = 0.0;
			float z = (m_prime - N / 2.0f) * ocean_length / N;

			vertices[index].opos = vertices[index].pos = glm::vec3(x, y, z);
			vertices[index].normal = glm::vec3(0, 1, 0);
		}
	}
}

Ocean::Ocean() :
	GameObject("Ocean")
{
	initialize_Htilde0();

	std::unique_ptr<ecs::MeshController> mc = std::make_unique<ecs::MeshController>();
	std::unique_ptr<OceanRenderer> mr = std::make_unique<OceanRenderer>(mc.get());

	std::unique_ptr<Mesh> mesh = MeshProvider::createPlane(1000, 1);
	mc->setMesh(std::move(mesh));
	addComponent(std::move(mr));
	addComponent(std::move(mc));

	Engine::instance()->findComponent<KeyboardInput>()->
		signal_key_press.connect(
			this, [](int k, int w, int m, int p) 
	{
		int n = std::any_cast<int>(Engine::instance()->properties()["DrawNormals"]);
		if(k==GLFW_KEY_N) 
			Engine::instance()->properties()["DrawNormals"].emplace<int>(!n);
	});
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
	float kw = glm::dot(k_dir, wind_dir);
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

OceanRenderer::OceanRenderer(ecs::MeshController* mc) :
	MeshRenderer(mc),
	m_innerTessellationLevels{ 1.0,1.0 },
	m_outerTessellationLevels{ 1.0,1.0,1.0,1.0 }
{
	m_foam_texture = new Texture("textures/foam.png");

	Property::signal_static_property_changed.connect(this, [this](omen::Property* p) {

		if (p->name().compare("OceanLen") == 0)
		{
			//ocean_length = p->floatValue();
			initialize_Htilde0();
		}

		if (p->name().compare("Damping") == 0)
		{
			damping = p->floatValue();
			initialize_Htilde0();
		}

		if (p->name().compare("Gravity") == 0)
		{
			g = p->floatValue();
			initialize_Htilde0();
		}

		if (p->name().compare("WindDir") == 0)
		{
			wind_dir = p->floatValue();
			wind = wind_power * glm::vec2(cos(wind_dir), cos(wind_dir));
			initialize_Htilde0();
		}

		if (p->name().compare("WindPower") == 0)
		{
			wind_power = p->floatValue();
			wind = wind_power * glm::vec2(cos(wind_dir), cos(wind_dir));
			initialize_Htilde0();
		}

		if (p->name().compare("TessellationLevelInner1") == 0)
			m_innerTessellationLevels[0] = p->floatValue();
		if (p->name().compare("TessellationLevelInner2") == 0)
			m_innerTessellationLevels[1] = p->floatValue();
		if (p->name().compare("TessellationLevelOuter1") == 0)
			m_outerTessellationLevels[0] = p->floatValue();
		if (p->name().compare("TessellationLevelOuter2") == 0)
			m_outerTessellationLevels[1] = p->floatValue();
		if (p->name().compare("TessellationLevelOuter3") == 0)
			m_outerTessellationLevels[2] = p->floatValue();
		if (p->name().compare("TessellationLevelOuter4") == 0) {
			heightFactor = p->floatValue();
			m_outerTessellationLevels[3] = p->floatValue();
		}
		if (p->name().compare("PerlinSize") == 0) {
			m_perlinSize = p->floatValue();
		}
		if (p->name().compare("PerlinScale") == 0) {
			m_perlinScale = p->floatValue();
		}
	});

	initializeShader();

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_indexBuffer);
	glGenBuffers(1, &m_vbo_texcoord);

	glBindVertexArray(m_vao);

	// Vertices
	float s = 100.0f; // patchSize / 2;
	glm::vec3 v[4] = { { 0,0,0 },{ s,0,0 },{ 0, 0,s },{ s,0,s } };
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture coordinates
	glm::vec2 t[4] = { { 0,1 },{ 0,0 },{ 1, 1 },{ 1,0 } };
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t), t, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Indices
	unsigned int indices[] = { 0,1,2,1,3,2 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	m_indexBufferSize = 6;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferSize * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// Shader
	compute_shader = new Shader("shaders/compute_test.glsl");
	compute_shader->use();
	compute_shader->setUniform1i("img_output", 0);
	compute_shader->setUniform1i("N", N);

//	compute_shader_htilde0 = new Shader("shaders/compute_htilde0.glsl");
//	compute_shader_ocean_fft_1 = new Shader("shaders/compute_evaluate_ocean_fft.glsl");
//	compute_shader_ocean_fft_2 = new Shader("shaders/compute_init_fft_c.glsl");

	// Create SSBO
	glGenBuffers(1, &m_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer), &shader_buffer, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// m_ssbo_htilde0, m_ssbo_htilde0_oposition, m_ssbo_htilde0_normal, m_ssbo_htilde0_htildes
	glGenBuffers(1, &m_ssbo_htilde0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htilde0);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer_htilde0), &shader_buffer_htilde0, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo_htilde0); // Bind the buffer to binding 0 in the shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &m_ssbo_htildes);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htildes);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer_htildes), &shader_buffer_htildes, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo_htildes); // Bind the buffer to binding 1 in the shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	/*glGenBuffers(1, &m_ssbo_fft_c);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_fft_c);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo_fft_c); // Bind the buffer to binding 1 in the shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	*/

	/*glGenBuffers(1, &m_ssbo_htilde0_normal);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htilde0_normal);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer_htilde0), &shader_buffer_htilde0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo_htilde0_normal); // Bind the buffer to binding 0 in the shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/

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

	glGenTextures(1, &tex_output2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(1, tex_output2, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	/*
	glGenTextures(1, &tex_htilde0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex_htilde0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(1, tex_htilde0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	*/

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

	createNoiseTextures();
}

OceanRenderer::~OceanRenderer()
{
}

void OceanRenderer::createNoiseTextures()
{
	Shader shader_compute_noise("shaders/compute_noise_textures.glsl");

	GLuint ssbo_noise;
	glGenBuffers(1, &ssbo_noise);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_noise);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(4, m_noiseTexture);
	for (int i = 0; i < 4; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_noiseTexture[i]);
		//glActiveTexture(GL_TEXTURE0 + i);
		//glBindTexture(GL_TEXTURE_2D, m_noiseTexture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Nx, Nz, 0, GL_RGBA, GL_FLOAT, NULL);
	}

	for (int i = 0; i < 4; ++i)
	{
		/*
		char buf[256];
		char tname[256];
		strcpy(tname, "img_output_");
		strcat(tname, itoa(i + 1, buf, 10));

		GLint uniloc = glGetUniformLocation(shader_compute_noise.m_shader_program, tname);
		*/
		shader_compute_noise.use();
		glBindImageTexture(0, m_noiseTexture[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(Nx, Nz, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glBindImageTexture(0, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
		shader_compute_noise.unuse();
	}
	// make sure writing to image has finished before read
	
	glUseProgram(0);
	for (int i = 0; i < 4; ++i)
	{
		GLfloat* f = new GLfloat[Nx*Nz * 4 * 4];
		glBindTexture(GL_TEXTURE_2D, m_noiseTexture[i]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, f);
		glBindTexture(GL_TEXTURE_2D, 0);
		int a = 0; 
		delete[] f;
	}

	glDeleteBuffers(1, &ssbo_noise);
}
void OceanRenderer::compute_ocean_mapping()
{
	for (int index = 0; index < Nplus1*Nplus1; ++index)
	{
		shader_buffer[index].pos = glm::vec4(vertices[index].pos.x, vertices[index].pos.y*heightFactor, vertices[index].pos.z, 1);
		shader_buffer[index].normal = glm::vec4(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 1);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_buffer), &shader_buffer, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	compute_shader->use();
	compute_shader->setUniform1i("N", N);
	compute_shader->setUniform1i("len", 1);
	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(compute_shader->m_shader_program, GL_SHADER_STORAGE_BLOCK, "shader_data");
	GLuint ssbo_binding_point_index = 2;
	glShaderStorageBlockBinding(compute_shader->m_shader_program, block_index, ssbo_binding_point_index);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	compute_shader->setUniform1i("img_output", tex_output);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, tex_output2);
	compute_shader->setUniform1i("img_output2", tex_output2);

	// Do the computation
	glDispatchCompute((GLuint)Nx, (GLuint)Nz, 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Read back the stuff from ssbo
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glm::vec4* ptr = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

}

void OceanRenderer::compute_htilde0()
{
	return;
	compute_shader_htilde0->use();
	compute_shader_htilde0->setUniform1i("N", N);
	compute_shader_htilde0->setUniform1f("nper2", N / 2.0);
	compute_shader_htilde0->setUniform1f("lpern", ocean_length / N);
	compute_shader_htilde0->setUniform1f("damping", damping);
	compute_shader_htilde0->setUniform1f("g", g);
	compute_shader_htilde0->setUniform1f("A", A);
	compute_shader_htilde0->setUniform2fv("wind", 1, glm::value_ptr(wind));

	memset(&shader_buffer_htilde0, 0, sizeof(shader_buffer_htilde0));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htilde0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo_htilde0);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(shader_buffer_htilde0), shader_buffer_htilde0);

	// Do the computation
	glDispatchCompute((GLuint)129 * 129 / 129, (GLuint)1, 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Read back the stuff from ssbo
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htilde0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo_htilde0);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(shader_buffer_htilde0), shader_buffer_htilde0);

	GLfloat *ptr;
	ptr = (GLfloat *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	int a = 0;
}

void OceanRenderer::compute_fft(float t)
{
	return;
	// First Loop of fft where htilde, htilde_slope(x/z) and htilde_d(x/z) are computed
	compute_shader_ocean_fft_1->use();
	compute_shader_ocean_fft_1->setUniform1i("N", N);
	compute_shader_ocean_fft_1->setUniform1f("ocean_length", ocean_length);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htilde0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo_htilde0);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(shader_buffer_htilde0), shader_buffer_htilde0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htildes);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo_htildes);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(shader_buffer_htildes), shader_buffer_htildes);

	// Do the computation
	glDispatchCompute((GLuint)129 * 129 / 129, (GLuint)1, 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Read back the stuff from ssbo
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_htildes);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo_htildes);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(shader_buffer_htildes), shader_buffer_htildes);
	// First loop ends

	// Initialize FFT C
	compute_shader_ocean_fft_2->use();
	compute_shader_ocean_fft_2->setUniform1i("N", N);

	struct fft_s
	{
		Complex htilde_in;
		Complex htilde_out;
		int		reversed;
		Complex C0;
		Complex C1;
		// float __padding[]; // No padding needed
	} FFTS[Nplus1*Nplus1];
	memset(FFTS, 0, sizeof(FFTS));
	for (int i = 0; i < Nplus1*Nplus1; ++i)
	{
		FFTS[i].htilde_in = shader_buffer_htildes[i].h_tilde;
		FFTS[i].htilde_out = shader_buffer_htildes[i].h_tilde;
		auto r = [](int i) {
			unsigned int res = 0;
			for (int j = 0; j < log_2_N; j++) {
				res = (res << 1) + (i & 1);
				i >>= 1;
			}
			return res;
		};
		FFTS[i].reversed = i < N ? r(i):0;
	}
	for (int m_prime = 0; m_prime < N; m_prime++) 
	{
		//fft->fft(h_tilde, h_tilde, 1, m_prime * N);
		compute_shader_ocean_fft_2->setUniform1i("stride", 1);
		compute_shader_ocean_fft_2->setUniform1i("offset", m_prime*N);
		compute_shader_ocean_fft_2->setUniform1i("which", 0);

		/*
		Complex htilde_in;
		Complex htilde_out;
		int 	reversed;
		Complex	C0;
		Complex C1;
		*/

		/*
		uniform int stride;
		uniform int offset;
		uniform int which;

		
		fft->fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
		fft->fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
		fft->fft(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
		fft->fft(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
		*/
		
	}
	int a = 0;
}

void OceanRenderer::render(Shader* sh) const
{
	evaluateWavesFFT(Engine::instance()->time()*std::any_cast<float>(Engine::instance()->properties()["Time"]));

	const_cast<OceanRenderer*>(this)->compute_ocean_mapping();
	//compute_htilde0();
	//compute_fft(Engine::instance()->time()*std::any_cast<float>(Engine::instance()->properties()["Time"]));


	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, Engine::instance()->getPolygonMode());
	shader()->use();

	//shader()->setUniform4fv("Color", 1, &glm::vec4(0,0,0,1)[0] );
	shader()->setUniform1f("Time", (float)Engine::instance()->time());
	shader()->setUniform4fv("Color", 1, &glm::vec4(1)[0]);
	glm::mat4 modelMatrix = entity()->getComponent<Transform>()->tr();
	shader()->setUniformMatrix4fv("ModelMatrix", 1, glm::value_ptr(modelMatrix), false);
	glm::mat4 viewMatrix = Engine::instance()->camera()->view();
	
	shader()->setUniformMatrix4fv("ViewMatrix", 1, glm::value_ptr(viewMatrix), false);
	shader()->setUniformMatrix3fv("NormalMatrix", 1, &glm::mat3(Engine::instance()->camera()->view())[0][0], false);
	shader()->setUniformMatrix4fv("Model", 1, &glm::mat4(1)[0][0], false);
	glm::vec3 viewpos = Engine::instance()->camera()->pos();
	shader()->setUniform3fv("ViewPos", 1, glm::value_ptr(viewpos));
	shader()->setUniformMatrix4fv("ModelView", 1, &glm::mat4(1)[0][0], false);
	shader()->setUniformMatrix4fv("ModelViewProjection", 1,
		&(Engine::instance()->camera()->viewProjection())[0][0], false);
	shader()->setUniform1i("size", N);
	shader()->setUniform1f("ocean_length", ocean_length);
	shader()->setUniform1f("InnerTessellationLevel1", m_innerTessellationLevels[0]);
	shader()->setUniform1f("InnerTessellationLevel2", m_innerTessellationLevels[1]);

	shader()->setUniform1f("OuterTessellationLevel1", m_outerTessellationLevels[0]);
	shader()->setUniform1f("OuterTessellationLevel2", m_outerTessellationLevels[1]);
	shader()->setUniform1f("OuterTessellationLevel3", m_outerTessellationLevels[2]);
	shader()->setUniform1f("OuterTessellationLevel4", m_outerTessellationLevels[3]);
	shader()->setUniform1f("PerlinSize", m_perlinSize);
	shader()->setUniform1f("PerlinScale", m_perlinScale);

	shader()->setUniform1i("drawNormals", (int)(std::any_cast<int>(Engine::instance()->properties()["DrawNormals"])));

	glDisable(GL_CULL_FACE);

	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);

	int envmapLoc = shader()->getUniformLocation("envmap");
	if (envmapLoc >= 0) {
		glUniform1i(envmapLoc, 4);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ecs::SkyBoxRenderer::uvTexMap->id());
	}


	{ // normal drawing pass
		glEnable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_output);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, tex_output2);
		//glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, N*N);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, m_noiseTexture[static_cast<int>(Engine::instance()->time())%4]);
		//glBindTexture(GL_TEXTURE_2D, m_foam_texture->id());

		glPatchParameteri(GL_PATCH_VERTICES, 3);

		shader()->setUniform1i("PatchCount", patchCount);
		shader()->setUniform1f("PatchSize", patchSize);
		glDrawElementsInstanced(GL_PATCHES, m_indexBufferSize, GL_UNSIGNED_INT, 0, patchCount*patchCount);
		//glDrawElementsInstanced(GL_TRIANGLES, m_indexBufferSize, GL_UNSIGNED_INT, 0, 9);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	glBindVertexArray(0);
}

void OceanRenderer::initializeShader() {
	setShader(std::move(std::make_unique<Shader>(("shaders/ocean.glsl"))));
}

void OceanRenderer::initializeTexture() {
	setTexture(nullptr);
}