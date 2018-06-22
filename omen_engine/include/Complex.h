#pragma once

namespace omen 
{
	class Complex 
	{
	public:
		float a;
		float b;

		Complex() : a(0), b(0) {};
		Complex(float _a, float _b) : a(_a), b(_b) {};

		Complex operator*(float c) {
			return Complex(a*c, b*c);
		}

		Complex operator*(const Complex &r) {
			return  Complex(a*r.a - b*r.b, a*r.b + b*r.a);
		}

		Complex operator+(const Complex &r) {
			return Complex(a + r.a, b + r.b);
		}

		Complex operator-(const Complex &r) {
			return Complex(a - r.a, b - r.b);
		}

		Complex conjugate() {
			return Complex(a, -b);
		}
	};
}