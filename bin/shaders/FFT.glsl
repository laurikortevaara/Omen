//FFT::FFT(unsigned int N) : N(N), reversed(0), T(0), pi2(2 * M_PI) {

Complex c0[128];
Complex c1[128];
unsigned int reversed[1024]; // prep bit reversals
float log_2_N = 0.0;
float pi2 = M_PI_2;

uniform unsigned int which = 0;

/*struct
{
	Complex C[N];	
} */
Complex T[10][128]; // Enough for N=1024


float log2N(in float N) 
{
	return log(N) / log(2);
}

unsigned int reverse(unsigned int i) 
{
	unsigned int res = 0;
	for (int j = 0; j < log_2_N; j++) {
		res = (res << 1) + (i & 1);
		i >>= 1;
	}
	return res;
}

Complex t(in unsigned int x, in unsigned int N) {
	return Complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void initializeFFT(in unsigned int N)
{
	log_2_N = log2N(N);
	c0[int(N)] = Complex(0,0);
	c1[int(N)] = Complex(0,0);

	for (int i = 0; i < N; i++) 
		reversed[i] = reverse(i);

	int pow2 = 1;
	for (int i = 0; i < log_2_N; i++) {
		for (int j = 0; j < pow2; j++) 
			T[i][j] = t(j, pow2 * 2);
		pow2 *= 2;
	}

	for(int i=0; i < N; ++i)
	{
		c0[i] = Complex(0,0);
		c1[i] = Complex(0,0);
	}
	which = 0;
}


void fft(Complex inp[128], Complex outp[128], int stride, int offset) 
{
	for (int i = 0; i < 128; i++) 
	{
		if(which == 0)
			c0[i] = inp[reversed[i] * stride + offset];
		else
		if(which == 1)
			c1[i] = inp[reversed[i] * stride + offset];
	}

	int loops = 128 >> 1;
	int size = 1 << 1;
	int size_over_2 = 1;
	int w_ = 0;
	for (int i = 1; i <= log_2_N; i++) 
	{
		which ^= 1;
		for (int j = 0; j < loops; j++) 
		{
			for (int k = 0; k < size_over_2; k++) 
			{
				int index = size * j + k;
				int index2 = size * j + size_over_2 + k;
				Complex sub_c1 = c1[index];
				Complex sub_c0 = c0[index];
				Complex subsub_c0 = c0[index2];
				Complex subsub_c1 = c1[index2];

				if(which==0) {
					c0[index] = sum(_c1, mul(subsub_c1,T[w_][k]));
					}
				else
				if(which==1) {
					c1[index] = sum(c0[index], mul(c0[index2],T[w_][k]));
					}
			}

			for (int k = size_over_2; k < size; k++) 
			{
				int index = size * j + k;
				int index2 = size * j - size_over_2 + k;
				Complex sub_c1 = c1[index];
				Complex sub_c0 = c0[index];
				Complex subsub_c0 = c0[index2];
				Complex subsub_c1 = c1[index2];

				if(which==0) {
					c0[index] = sum(_c1, mul(subsub_c1,T[w_][k-size_over_2]));
					}
				else
				if(which==1) {
					c1[index] = sum(c0[index], mul(c0[index2],T[w_][k-size_over_2]));
					}
			}
		}
		loops >>= 1;
		size <<= 1;
		size_over_2 <<= 1;
		w_++;
	}

	for (int i = 0; i < 128; i++) 
	{
		if(which==0)
			outp[i * stride + offset] = c0[i];
		else
		if(which==1)
			outp[i * stride + offset] = c1[i];
	}
}