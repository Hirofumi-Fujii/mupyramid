// genpcut.cpp
// g++ -Wall genpcut.cpp pcutopt.cpp csvarray.cpp jokisch.cpp hist2d.cpp ncpng.cpp -o genpcut
//
// 2016-11-18 hist2d version

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "pcutopt.h"
#include "csvarray.h"
#include "jokisch.h"
#include "hist2d.h"

double
trx2pcut (double trx, double cs, double pcut0)
{
	// get the integrated-flux(pcut0, cs)
	double flux0 = Jokisch::jokisch_integral (pcut0, cs);
	double flux = flux0 * trx;
	double dflux = flux * 0.01;
//@	std::cout << "trx=" << trx << ", cs=" << cs << ", flux0=" << flux0
//@		<< ", flux=" << flux << std::endl;

	// start p
	double p1 = pcut0;
	double p2 = 100.0;
	double f2 = Jokisch::jokisch_integral (p2, cs);

	while (f2 > flux)
	{
		p1 = p2;
		p2 = p2 * 2.0;
		f2 = Jokisch::jokisch_integral (p2, cs);
	}

	double p = (p1 + p2) / 2.0;
	double f = Jokisch::jokisch_integral (p, cs);

	int ntry = 0;
	while ( fabs(f - flux) > dflux )
	{
		ntry += 1;
		if (f > flux)
			p1 = p;
		else
			p2 = p;
		p = (p1 + p2) / 2.0;
		f = Jokisch::jokisch_integral (p, cs);
	}
	return p;
}

int main (int argc, char* argv[])
{
	using namespace mylibrary;

	MUPYRAMID::Pcutopt opt;
	if ( !opt.set (argc, argv) )
	{
		opt.usage (std::cerr, argv[0]);
		return (-1);
	}

	std::ifstream ifs (opt.m_trxfilename.c_str());
	if (!ifs)
	{
		std::cerr << "ERROR: transmittance file " << opt.m_trxfilename << " cannot be opened." << std::endl;
		return (-2);
	}

	std::ifstream cst (opt.m_cosfilename.c_str());
	if (!ifs)
	{
		std::cerr << "ERROR: cos file " << opt.m_cosfilename << " cannot be opened." << std::endl;
		return (-2);
	}

	CSVArray trxcsv;
	trxcsv.Read (ifs);
	int nlines = (int)(trxcsv.m_array.size ());
	std::cout << "Total lines=" << nlines << std::endl;
	if ((nlines < 1) || trxcsv.CellString(0, 0) != "transmittance")
	{
		std::cerr << "ERROR: this is not a transmittance file"
			<< std::endl;
		return (-2);
	}

	CSVArray cstcsv;
	cstcsv.Read (cst);
	int nlinescst = (int)(cstcsv.m_array.size ());
	std::cout << "Total lines=" << nlinescst << std::endl;
	if (nlinescst < 1)
	{
		std::cerr << "ERROR: this is not a transmittance file"
			<< std::endl;
		return (-2);
	}

	int histstart = -1;
	for (int i = 0; i < nlines; i++)
	{
//			std::cout << i << ' ' << trxcsv.CellString(i, 0) << std::endl;
			if (trxcsv.CellString(i, 0) == "Hist2D::")
				histstart = i;
	}
	if (histstart < 0)
	{
		std::cerr << "ERROR: Hist2D is not found."
			<< std::endl;
		return (-2);
	}

	std::cout << "Hist2D starts at " << histstart << std::endl;
	double xmin = trxcsv.CellDoubleValue (histstart + 2, 1);
	double xmax = trxcsv.CellDoubleValue (histstart + 2, 2);
	int nxbins = (int)(trxcsv.CellLongValue (histstart + 2, 3));
	std::cout << "xmin=" << xmin << " xmax=" << xmax << " nxbins=" << nxbins << std::endl;

	double ymin = trxcsv.CellDoubleValue (histstart + 3, 1);
	double ymax = trxcsv.CellDoubleValue (histstart + 3, 2);
	int nybins = (int)(trxcsv.CellLongValue (histstart + 3, 3));
	std::cout << "ymin=" << ymin << " ymax=" << ymax << " nybins=" << nybins << std::endl;


	int ixoffs = 3;
	int iyoffs = histstart + 7;
	
	double trxmin = 2.0;
	double trxmax = 0.0;

	double pcut0 = 0.0;
	if (opt.m_pcut0_given)
		pcut0 = opt.m_pcut0;

	double pcut = pcut0;

	static const int num2dhist = 1;
	Hist2D hist2d[num2dhist] =
	{
		Hist2D ("Pcut dxdy", -0.5, (double(nxbins) - 0.5), nxbins, -0.5, (double(nybins) - 0.5), nybins),
	};

	for (int ny = 0; ny < nybins; ny++)
	{
		for (int nx = 0; nx < nxbins; nx++)
		{
			double trx = trxcsv.CellDoubleValue (ny + iyoffs, nx + ixoffs);
			double cs = cstcsv.CellDoubleValue (ny + iyoffs, nx + ixoffs);
			if (trx < trxmin)
				trxmin = trx;
			if (trx > trxmax)
				trxmax = trx;
			if ((trx > 0.001) && (trx < 0.999))
			{
				pcut = trx2pcut (trx, cs, pcut0);
			}
			else
				pcut = pcut0;
			hist2d[0].cumulate (double(nx), double(ny), pcut);
		}
	}
	std::cout << "Transmittance min=" << trxmin << " max=" << trxmax
		<< std::endl;

	for (int nh = 0; nh < num2dhist; nh++)
	{
		std::stringstream ss;
		ss << "genpcut-" << nh << ".csv";
		std::string ofcsvnam;
		ss >> ofcsvnam;
		std::ofstream ofcsv (ofcsvnam.c_str());
		if (ofcsv)
		{
			ofcsv << "\"";
			for (int i = 0; i < argc; i++)
				ofcsv << argv[i] << ' ';
			ofcsv << "\"";
			ofcsv << ",pcut0(GeV/c)," << pcut0
				<< std::endl;
			ofcsv << "Transmittance,trxmin," << trxmin << ",trxmax," << trxmax
				<< std::endl;
			hist2d[nh].CSVdump (ofcsv);
		}
	}
	return 0;
}
