// anaacc1f1.cpp
// g++ -Wall anaacc1f1.cpp geom3d.cpp hist2d.cpp -o anaacc1f1.exe
//
// For muon1f1
// 2014-02-19 phi angle 21 -> 20 for d2p5

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "geom3d.h"
#include "hist2d.h"

double pathint(int utype, double yd, double udist, double xygap, double yoffs, double ysize)
{
	// Returns effective length of our detector (in length units)
	//   yd     hit position on the near-side detector in the device coordinate
	//   udist  device distance between the near-side and the far-side detectors
	//   xygap  plane distance between the X and the Y planes
	//   yoffs  base position difference between the near-side and the far-side detectors
	//   ysize  lenfth of the detector plane
	//
	// utype  far-side near-side
	//   0      outer    inner
	//   1      inner    outer
	//   2      outer    outer
	//   3      inner    inner

	double ymin0 = -yd;
	double ymin1 = yoffs - ((yd + yoffs) * (udist + xygap) / udist);
	double ymin2 = -((yd + yoffs) * xygap / udist);
	if (utype == 1)
	{
		ymin1 = yoffs - ((yd + yoffs) * (udist - xygap) / udist);
		ymin2 = (yd + yoffs) * xygap / udist;
	}
	else if (utype == 2)
	{
		ymin1 = yoffs - ((yd + yoffs) * udist / (udist + xygap));
		ymin2 = - ((yd + yoffs) * xygap / (udist + xygap));
	}
	else if (utype == 3)
	{
		ymin1 = yoffs - ((yd + yoffs) * udist / (udist - xygap));
		ymin2 = (yd + yoffs) * xygap / (udist - xygap);
	}
	double ymax0 = ymin0 + ysize;
	double ymax1 = ymin1 + ysize;
	double ymax2 = ymin2 + ysize;

	double ymin = ymin0;
	double ymax = ymax0;
	if (ymin1 > ymin)
		ymin = ymin1;
	if (ymin2 > ymin)
		ymin = ymin2;
	if (ymax1 < ymax)
		ymax = ymax1;
	if (ymax2 < ymax)
		ymax = ymax2;

	if (ymin < 0.0)
		ymin = 0.0;
	else if (ymin > ysize)
		ymin = ysize;
	if (ymax < 0.0)
		ymax = 0.0;
	else if (ymax > ysize)
		ymax = ysize;

	double weight = (ymax - ymin);
	if (weight < 0.0)
		weight = 0.0;

//	std::cout << yd << ',' << ymin0 << ',' << ymin1 << ',' << ymin2 << ','
//		<< ymax0 << ',' << ymax1 << ',' << ymax2 << ','
//		<< ymin << ',' << ymax << ',' << (ymax - ymin) << ',' << weight
//		<< std::endl;

	return weight;
}

int main(int argc, char* argv[])
{
	using namespace mylibrary;

	double dx;
	double dy;
	double dz;

	bool swapunit = false;

	std::cout << '"';
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << ' ';
	std::cout << '"';
	std::cout << ",\"Compiled: " << __DATE__ << ' ' << __TIME__ << '"';
	std::cout << std::endl;

	int uxtype = 0;
	int uytype = 1;

	double hdev = 100.0;
	double wdev = 100.0;

	double devdistl = 191.6;
	double yoffsl = 50.0;

	double xoffs = 0.0;
	double yoffs = yoffsl * 0.5;
	double xygap = 8.3;
	double devdist = devdistl * 0.5;
	double phioffs = 22.0;

	int iarg = 1;
	while (iarg < argc)
	{
		std::string sarg(argv[iarg]);
		if (sarg == "-t0")
		{
			uxtype = 0;
			uytype = 1;
		}
		else if (sarg == "-t1")
		{
			uxtype = 0;
			uytype = 1;
			devdist = devdistl;
			yoffs = yoffsl;
		}
		else if (sarg == "-t2")
		{
			uxtype = 2;
			uytype = 3;
		}
		else if (sarg == "-t3")
		{
			uxtype = 2;
			uytype = 3;
			devdist = devdistl;
			yoffs = yoffsl;
		}
		else if (sarg == "-xygap")
		{
			++iarg;
			if (iarg >= argc)
			{
				std::cerr << "numeric value is required." << std::endl;
				return (-1);
			}
			std::stringstream ss(argv[iarg]);
			double dv = 0.0;
			if ( ss >> dv )
				xygap = dv;
			else
			{
				std::cerr << "Cannot read the numeric value." << std::endl;
				return (-1);
			}
		}
		else if (sarg == "-swapunit")
			swapunit = true;
		else
		{
			std::cerr << "Unknown option: " << sarg << std::endl;
			return (-1);
		}
		++iarg;
	}
	double devdistx = devdist;
	double devdisty = devdist;

	if (swapunit)
		yoffs = -yoffs;

	if (uxtype == 2)
		devdistx = devdist + xygap;
	else if (uxtype == 3)
		devdistx = devdist - xygap;

	if (uytype == 2)
		devdisty = devdist + xygap;
	else if (uytype == 3)
		devdisty = devdist - xygap;

	std::cout << "angle(deg)\"," << phioffs
		<< ",\"xoffs\"," << xoffs << ",\"yoffs\"," << yoffs
		<< ",\"xsize\"," << wdev << ",\"ysize\"," << hdev
		<< std::endl;
	std::cout << "\"devdist\"," << devdist
		<< ",\"xygap\"," << xygap
		<< ",\"xtype\"," << uxtype << ",\"ytype\"," << uytype
		<< ",\"devdistx\"," << devdistx << ",\"devdisty\"," << devdisty
		<< std::endl;

//	double phi0 = phioffs * 3.1415926535897932384626433832795 / 180.0;
//	double csphi0 = cos(phi0);
//	double snphi0 = sin(phi0);

	static const int numhist2d = 3;
	Hist2D hist2d[numhist2d] = 
	{
		Hist2D("dx_dy(U0-U1)cos**2", -99.5, 99.5, 199, -99.5, 99.5, 199),
		Hist2D("dx_dy(U0-U1)uniform", -99.5, 99.5, 199, -99.5, 99.5, 199),
		Hist2D("d(phi)d(cos(theta))", -1.0, 1.0, 200, -1.0, 1.0, 200),
	};

	double ddevx = 1.0;
	double ddevy = 1.0;
	for (int iy = 0; iy < 199; iy++)
	{
		dy = double(iy) * ddevy - 99.0;
		double y = dy + yoffs;
		double yc = y / devdisty * devdist;
		double yweight = pathint(uytype, dy, devdist, xygap, yoffs, hdev);

		for (int ix = 0; ix < 199; ix++)
		{
			dx = double(ix) * ddevx - 99.0;
			double x = dx + xoffs;
			double xc = x / devdistx * devdist;

			dz = -devdist;
			Vector3D v(xc, yc, dz);
			v.normalize();
			double xweight = pathint(uxtype, dx, devdist, xygap, xoffs, wdev);
			double weightu = (xweight * ddevx / devdistx) * (yweight * ddevy / devdisty);
			weightu = weightu * v.z() * v.z() * v.z() * v.z();
			double weightc = weightu * v.y() * v.y();

			hist2d[0].cumulate(dx, -dy, weightc);
			hist2d[1].cumulate(dx, -dy, weightu);
		}
	}

	double dphi = 0.01;	// (1.0 - (-1.0)) / 200
	double dcth = 0.01;	// (1.0 - (-1.0)) / 200
	for (int ip = 0; ip < 200; ip++)
	{
		double phi = double(ip) * dphi - 0.995;
		double cph = cos(phi);
		double sph = sin(phi);
		for (int ic = 0; ic < 200; ic++)
		{
			double cth = double(ic) * dcth - 0.995;
			double sth = sqrt((1.0 - cth) * (1.0 + cth));
			double domega = dphi * dcth;
			double x = devdistx * sph / cph;
			double y = devdisty * cth / sth / cph;
			dx = x - xoffs;
			dy = y - yoffs;
			double xweight = pathint(uxtype, dx, devdist, xygap, xoffs, wdev);
			double yweight = pathint(uytype, dy, devdist, xygap, yoffs, hdev);
			double weightu = fabs( xweight * yweight * sth * cph ) * domega;
			hist2d[2].cumulate(phi, cth, weightu);
		}
	}
			
	for (int i = 0; i < numhist2d; i++)
	{
		std::cout << "*** Hist[" << i << "] ***" << std::endl;
		hist2d[i].dump(std::cout);
	}

	return 0;
}
