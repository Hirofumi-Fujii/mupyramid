//
// g++ -Wall dedx.cpp -o dedx
//
#include <iostream>
#include <sstream>
#include <cmath>

static const double elmass = 0.510998928;	// electron mass in MeV/c^2
static const double mumass = 105.65836668;	// muon mass in MeV/c^2
static const double K = 0.307075;	// 4piNare^2mec^2 in MeV/mol cm^2

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " p (in GeV/c)" << std::endl;
		return (-1);
	}

	std::stringstream ss (argv[1]);
	double p;
	if (!(ss >> p))
	{
		std::cerr << "Momentum error" << std::endl;
		return (-1);
	}

	// Starndard Rock
	// double matZ = 11.0;
	// double matA = 22.0;
//@	double ZoverA = 0.5;
//@	double matI = 136.4;	// in eV
//@	double plasmaE = 33.17;	// in eV
	// double rho = 2.650;

	// Shielding concrete
	double ZoverA = 0.50274;
	double matI = 135.2;
//@	double plasmaE = 30.99;	// in eV
	// double rho = 2.300;

	double mumassGeV = mumass / 1000.0;
	double muenergy = sqrt (p * p + mumassGeV * mumassGeV);
	double beta = p / muenergy;
	double gamma = muenergy / mumassGeV;
	double betagamma = p / mumassGeV;
	double rmass = elmass / mumass;
	double wmax = 2.0 * elmass * betagamma * betagamma
			/ (1.0 + (2.0 * gamma * rmass) + (rmass * rmass));
	double IinMeV = matI / 1000.0 / 1000.0;
	double term1 = 0.5 * log (2.0 * elmass * betagamma * betagamma * wmax / (IinMeV * IinMeV));
	double term2 = beta * beta;
	double dedx = K * ZoverA / (beta * beta) * (term1 - term2);

	std::cout << "p=" << p << " (GeV/c), E=" << muenergy << " GeV, beta="
		<< beta << ", gamma=" << gamma
		<< ", wmax=" << wmax << " MeV" << std::endl;
	std::cout << "dE/dx = " << dedx << std::endl;

	return 0;
}
