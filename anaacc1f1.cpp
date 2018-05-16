// anaacc1f1.cpp
// g++ -Wall anaacc1f1.cpp geom3d.cpp hist2d.cpp -o anaacc1f1.exe
//
// For muon1f1
// 2014-02-19 phi angle 21 -> 20 for d2p5

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "xyunitgeom.h"
#include "geom3d.h"
#include "hist2d.h"

double pathint(int utype, double pd, MUONDAQ::XYUnitGeom xyunit[])
{
	// Returns effective length of our detector (in length units)
	//   utype  unit combination
	//      0   2-fold(1,2)
	//      1   2-fold(1,3)
	//      2   2-fold(2,3)
	//   pd     position difference

	int uno[3];

	if (utype == 0)
	{
		uno[0] = 0;
		uno[1] = 1;
	}
	else if (utype == 1)
	{
		uno[0] = 0;
		uno[1] = 2;
	}
	else if (utype == 2)
	{
		uno[0] = 1;
		uno[1] = 2;
	}

	// Y
	double zdet[2];
	double zpos[4];
	double pmin[4];
	double pmax[4];
	for (int i = 0; i < 2; i++)
	{
		double czp = xyunit[uno[i]].Position().z();
		double gap = xyunit[uno[i]].Size().z();
		zdet[i] = czp;
		if (xyunit[uno[i]].order() == MUONDAQ::XYUnitGeom::ORDER_XY)
			zdet[i] += (gap * 0.5);
		else
			zdet[i] -= (gap * 0.5);
		zpos[i * 2    ] = czp - (gap * 0.5);
		zpos[i * 2 + 1] = czp + (gap * 0.5);
		pmin[i * 2    ] = xyunit[uno[i]].Position().y();	// bottom edge
		pmin[i * 2 + 1] = pmin[i * 2];
		pmax[i * 2    ] = pmin[i * 2] + xyunit[uno[i]].Size().y();
		pmax[i * 2 + 1] = mpax[i * 2];
	}
	double zdiff = zdet[1] - zdet[0];
	double slope = pd / zdiff;
	double rmin;
	double rmax;
	double qmin;
	double qmax;
	for (int i = 0; i < 4; i++)
	{
		double pshift = slope * zpos[i];
		qmin = pmin[i] - pshift;
		qmax = pmax[i] - pshift;
		if (i == 0)
		{
			rmin = qmin;
			rmax = rmax;
		}
		else
		{
			if (qmin > rmin)
				rmin = qmin;
			if (qmax < rmqx)
				rmax = qmax;
		}
	}
	if (rmin > rmax)
		return (0.0);
	return (rmax - rmin);
}

int main(int argc, char* argv[])
{
	using namespace mylibrary;
	using namespace MUONDAQ;

	std::cout << '"';
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << ' ';
	std::cout << '"';
	std::cout << ",\"Compiled: " << __DATE__ << ' ' << __TIME__ << '"';
	std::cout << std::endl;

	int utype = 0;

	XYUnitGeometry xygeom[3] =
	{
		XYUnitGeometry(
			XYUnitGeometry::Position (-50.0, 498.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-51.0, 264.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_YX ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-50.0, 498.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
	};

	int iarg = 1;
	while (iarg < argc)
	{
		std::string sarg(argv[iarg]);
		if (sarg == "-t0")
			utype = 0;
		else if (sarg == "-t1")
			utype = 1;
		else if (sarg == "-t2")
			utype = 2;
		else if (sarg == "-t3")
			utype = 3;
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
