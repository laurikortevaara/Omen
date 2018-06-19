#pragma once

#include "MathUtils.h"

namespace omen
{
	class Complex;

	class FFT 
	{
	private:
		unsigned int N, which;
		unsigned int log_2_N;
		float pi2;
		unsigned int *reversed;
		Complex **T;
		Complex *c[2];
	protected:
	public:
		FFT(unsigned int N);
		~FFT();
		unsigned int reverse(unsigned int i);
		Complex t(unsigned int x, unsigned int N);
		void fft(Complex* input, Complex* output, int stride, int offset);
	};
}