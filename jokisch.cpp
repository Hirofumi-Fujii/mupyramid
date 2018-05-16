// jokisch.cpp
#include <cmath>
#include "jokisch.h"

namespace Jokisch
{
static const double j1 = 1.442 * 10000.0;	// m^(-2) s(-1) sr^(-1) (GeV/c)^(-1)
static const double j2 = 1.84;	// GeV/c
static const double j3 = 77.2;	// GeV/c
static const double j4 = 19.8;	// GeV/c
static const double j5 = 2.80 * 10000.0;	// m^(-2) s^(-1) sr^(-1)
static const double j6 = 2.28;	// GeV/c
static const double j7 = 57.3;	// GeV/c
static const double j8 = 8.9;	// GeV/c

static const double pj = 2.57;

double
jokisch_integral (double p, double cs)
{
	// returns momentum integrated muon flux in (1/m^2/sec/str)
	// input:
	//	p	lower limit momentum in GeV/c (upper limit is infinity)
	//	cs	cos(zenith-angle)

	double g = p * cs;
	double f = j5 * pow(cs, pj) * pow((g + j6), (1.0 - pj)) / (g + j7)
			* ((p + 3.0 * j8) / (g + j8 * (2.0 * cs + 1.0)));
	return f;
}

double
jokisch (double p, double cs)
{
	// returns muon flux in (1/m^2/sec/str/(GeV/c)
	// input
	//	p	momentum in GeV/c
	//	cs	cos(zenith-angle)

	double g = p * cs;
	double f = j1 * pow(cs, (pj + 1.0)) * pow((g + j2), (-pj))
			/ (g + j3) * ((p + j4) / (g + j4));
	return f;
}

}
