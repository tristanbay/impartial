#pragma once

#define POWER_ITER 15 // number of Maclaurin series iterations
#define LOG_ITER 6 // number of Newton's method iterations

double fastPow(double, double);

// helper functions
double fastExpWithMult(double, double);
double fastLn(double);
long factorial(unsigned short);
double intPow(double, unsigned short);
