#ifndef _Complex_H_
#define _Complex_H_

struct Complex
{
	float a;
	float b;
};

Complex conjugate(in Complex a)
{
	return Complex(a.a,-a.b);
}

Complex mul(in Complex a, in Complex b)
{
	return Complex(a.a*b.a - a.b*b.b, a.a*b.b + a.b*b.a);
}

Complex mul(in Complex a, float c)
{
	return Complex(a.a*c, a.b*c);
}

Complex sum(in Complex a, in Complex b)
{
	return Complex(a.a+b.a,a.b+b.b);
}

Complex diff(in Complex a, in Complex b)
{
	return Complex(a.a-b.a,a.b-b.b);
}

#endif