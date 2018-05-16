// genacc0.cpp
// g++ -Wall genacc0.cpp hist2d.cpp hist1d.cpp mytimer.cpp -o genacc0

#include <iostream>
#include <fstream>
#include <cmath>
#include "hist2d.h"
#include "hist1d.h"
#include "mytimer.h"

static const int UNIT_DIST = 150.0;
static const int NUM_XCH = 120;
static const int NUM_YCH = 120;

int
main (int argc, char* argv[])
{
	mylibrary::MyTimer mytimer;

	static const int NUM_HIST2D = 4;
	mylibrary::Hist2D hist2d [NUM_HIST2D] =
	{
		mylibrary::Hist2D("Acc-dxdy", -119.5, 119.5, 239, -119.5, 119.5, 239),
		mylibrary::Hist2D("Acc-dxdy", -119.5, 119.5, 239, -119.5, 119.5, 239, true),
		mylibrary::Hist2D("phi-cos", -1.0, 1.0, 200, -1.0, 1.0, 200),
		mylibrary::Hist2D("phi-cos", -1.0, 1.0, 200, -1.0, 1.0, 200, true),
	};

	static const int NUM_HIST1D = 1;
	mylibrary::Hist1D hist1d [NUM_HIST1D] =
	{
		mylibrary::Hist1D("cos(haa)", 0.0, 1.0, 100),
	};

	double chsizex = 1.0;
	double chsizey = 1.0;

	double udistx = 150.0;
	double udisty = 150.0;

	double dz = UNIT_DIST;

	mytimer.start ();
	for (int iy1 = 0; iy1 < (2 * NUM_YCH - 1); iy1++)
	{
		double dcy = double(iy1) - double(NUM_YCH - 1);
		double cny = double(NUM_YCH) - fabs(dcy);
		double dy = dcy * chsizey;
		for (int ix1 = 0; ix1< (2 * NUM_XCH - 1); ix1++)
		{
			double dcx = double(ix1) - double(NUM_XCH - 1);
			double cnx = double(NUM_XCH) - fabs(dcx);
			double dx = dcx * chsizex;
			double w0 = (dz * chsizex * chsizey) / ((dx * dx) + (dy * dy) + (dz * dz));
			double wt = w0 * w0 * cnx * cny;
			hist2d[0].cumulate (dx, dy, wt);
			hist2d[1].cumulate (dx, dy, wt);

			double rdx = dx / udistx;
			double rdy = dy / udisty;

			double phi = atan (rdx);
			double csphi = cos (phi);
			double rdyc = rdy * csphi;
			double csth = rdyc / sqrt(rdyc * rdyc + 1.0);
			hist2d[2].cumulate (phi, csth, wt);
			hist2d[3].cumulate (phi, csth, wt);

			double rr2 = (rdx * rdx) + (rdy * rdy);
			double cshf = 1.0 / sqrt(1.0 + rr2);
			hist1d[0].cumulate (cshf, wt);
		}
	}
	mytimer.stop ();

	// Show results.
	std::string ofname = std::string("genacc0") + std::string(".csv");
	std::ofstream ofcsv (ofname.c_str());
	if (ofcsv)
	{
		ofcsv << '\"';
		for (int i = 0; i < argc; i++)
		{
			if (i)
				ofcsv << ' ';
			ofcsv << argv[i];
		}
		ofcsv << "\",";
		mytimer.csvout (ofcsv) << std::endl;
		ofcsv << "UnitDist(cm)," << UNIT_DIST << std::endl;

		for (int i = 0; i < NUM_HIST2D; i++)
		{
			hist2d[i].CSVdump (ofcsv);
		}
		for (int i = 0; i < NUM_HIST1D; i++)
		{
			hist1d[i].CSVdump (ofcsv);
		}
	}
	return 0;
}
