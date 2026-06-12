#include "fastpower.h"

double fastPow(double base, double exp)
{
	return fastExpWithMult(fastLn(base), exp);
}

// helper functions

double fastExpWithMult(double a, double b)
{
	double out = 0;
	for (int i = 0; i <= POWER_ITER; ++i)
		out += (a == 1 ? 1.0 : intPow(a, i)) / (double)factorial(i) * intPow(b, i);
	return out;
}

double fastLn(double x)
{
	double out = 2;
	double exp;
	for (int i = 0; i < LOG_ITER; ++i) {
		exp = fastExpWithMult(1, out);
		out += 2.0 * (x - exp) / (x + exp);
	}
	return out;
}

long factorial(unsigned short n)
{
	long out = 1;
	for (short i = 2; i <= n; ++i)
		out *= i;
	return out;
}

double intPow(double a, unsigned short b)
{
	double out = 1;
	for (short i = 0; i < b; ++i)
		out *= a;
	return out;
}
