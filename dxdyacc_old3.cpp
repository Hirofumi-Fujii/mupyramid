// dxdyacc.cpp
// g++ -Wall dxdyacc.cpp xyunitgeom.cpp -o dxdyacc.exe
//
// 21-Mar-2015 Unit size difference is considered

#include <iostream>
#include <sstream>
#include <cmath>
#include "xyunitgeom.h"

double pathint(int unit1, int unit2, int axis, double pd, const MUONDAQ::XYUnitGeometry xyunit[])
{
	// Returns effective length of our detector (in length units)
	//   axis
	//      0   X-axis
	//      1   Y-axis
	//   pd     position difference

	int uno[3];

	uno[0] = unit1;
	uno[1] = unit2;

	double pcnt[2];
	double zdet[2];
	double zpos[4];
	double pmin[4];
	double pmax[4];
	for (int i = 0; i < 2; i++)
	{
		double czp = xyunit[uno[i]].position().z();
		double gap = xyunit[uno[i]].size().z();
		double pos;
		double siz;
		if (axis == 0)
		{
			pos = xyunit[uno[i]].position().x();
			siz = xyunit[uno[i]].size().x();
		}
		else
		{
			pos = xyunit[uno[i]].position().y();
			siz = xyunit[uno[i]].size().y();
		}
		pcnt[i] = pos + (siz * 0.5);
		if (xyunit[uno[i]].order() == MUONDAQ::XYUnitGeometry::ORDER_XY)
		{
			if (axis == 0)
				zdet[i] = czp - (gap * 0.5);
			else
				zdet[i] = czp + (gap * 0.5);
		}
		else
		{
			if (axis == 0)
				zdet[i] = czp + (gap * 0.5);
			else
				zdet[i] = czp - (gap * 0.5);
		}
		zpos[i * 2    ] = czp - (gap * 0.5);
		zpos[i * 2 + 1] = czp + (gap * 0.5);
		pmin[i * 2    ] = pos;	// bottom edge
		pmin[i * 2 + 1] = pmin[i * 2];
		pmax[i * 2    ] = pmin[i * 2] + siz;
		pmax[i * 2 + 1] = pmax[i * 2];
	}
	double zdiff = zdet[0] - zdet[1];
	double slope = (pd + pcnt[0] - pcnt[1]) / zdiff;
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
			rmax = qmax;
		}
		else
		{
			if (qmin > rmin)
				rmin = qmin;
			if (qmax < rmax)
				rmax = qmax;
		}
	}
	if (rmin > rmax)
		return (0.0);
	return (rmax - rmin);
}

int main (int argc, char* argv[])
{
	static const int unitpair[3][2] =
	{
		{ 0, 1}, { 0, 2}, {1, 2}
	};

	using namespace MUONDAQ;

	double ru = 0.01;	// ratio of uniform distribution

	if (argc != 4)
	{
		std::cout << "Usage: "
			<< argv[0]
			<< " system(1|2|3) type(0|1|2) bg_ratio"
			<< std::endl;
		return (-1);
	}
	std::stringstream ss;
	ss << argv[1] << ' ' << argv[2] << ' ' << argv[3];
	int nsys;
	int ntype;

	if ((!(ss >> nsys)) || (!(ss >> ntype)) || (!(ss >> ru)))
	{
		std::cout << "ERROR: cannot read numerical value."
			<< std::endl;
		return (-2);
	}

	XYUnitGeometry xygeom[9] =
	{
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 598.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 339.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 100.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 601.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 340.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 100.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 340.0, 0.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 340.0, 1000.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 340.0, 2000.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
	};

	int isys = 0;
	if (nsys == 2)
		isys = 3;
	else if (nsys == 3)
		isys = 6;

	int u1 = isys + unitpair[ntype][0];
	int u2 = isys + unitpair[ntype][1];

	double xstep = (xygeom[u1].size().x() + xygeom[u2].size().x()) / 200.0;
	double ystep = (xygeom[u1].size().y() + xygeom[u2].size().y()) / 200.0;

	double dzx = xygeom[u1].zx() - xygeom[u2].zx();
	double dzy = xygeom[u1].zy() - xygeom[u2].zy();

	std::cout << "\"";
	for (int i = 0; i < argc; i++)
		std::cout << ' ' << argv[i];
	std::cout << "\"" << std::endl;

	std::cout << "dxdyacc version 2.0" << std::endl;
	std::cout << "xstep," << xstep << ",xdist," << dzx << std::endl;
	std::cout << "ystep," << ystep << ",ydist," << dzy << std::endl;
	for (int i = 0; i < 6; i++)
	{
		if ((i == u1) || (i == u2))
		{
			std::cout
				<< "Unit"
				<< ',' << (i + 1)
				<< ",Position"
				<< ',' << xygeom[i].position().x()
				<< ',' << xygeom[i].position().y()
				<< ',' << xygeom[i].position().z()
				<< ",Size"
				<< ',' << xygeom[i].size().x()
				<< ',' << xygeom[i].size().y()
				<< ',' << xygeom[i].size().z();
			if (xygeom[i].order() == XYUnitGeometry::ORDER_XY)
				std::cout << ",Order,XY";
			else
				std::cout << ",Order,YX";
			std::cout
				<< ",xcenter"
				<< ',' << xygeom[i].xc()
				<< ",ycenter"
				<< ',' << xygeom[i].yc()
				<< ",xplane_z"
				<< ',' << xygeom[i].zx()
				<< ",yplane-z"
				<< ',' << xygeom[i].zy();
			std::cout << std::endl;
		}
	}

	double dx;
	std::cout << "*** X *** d" << nsys << '_' << ntype << ',' << ru << std::endl;
	for (int i = 0; i < 199; i++)
	{
		dx = (double(i) - 99.0) * xstep;
		if (i)
			std::cout << ',';
		std::cout << dx;
	}
	std::cout << std::endl;
	for (int i = 0; i < 199; i++)
	{
		dx = (double(i) - 99.0) * xstep;		// mm units
		if (i)
			std::cout << ',';
		std::cout << pathint (u1, u2, 0, dx, xygeom);
	}
	std::cout << std::endl;

	double dy;
	std::cout << "*** Y *** d" << nsys << '_' << ntype << ',' << ru << std::endl;
	for (int i = 0; i < 199; i++)
	{
		dy = (double(i) - 99.0) * ystep;
		if (i)
			std::cout << ',';
		std::cout << dy;
	}
	std::cout << std::endl;
	for (int i = 0; i < 199; i++)
	{
//		dy = (double(i) - 99.0) * 10.0;		// mm units
		dy = (99.0 - double(i)) * ystep;
		if (i)
			std::cout << ',';
		std::cout << pathint (u1, u2, 1, dy, xygeom);
	}
	std::cout << std::endl;

	double xl;
	double yl;
	double xd;	// x difference in world coordinate system
	double yd;	// y difference in world coordinate system

	std::cout << "*** XY *** d" << nsys << '_' << ntype << ',' << ru << std::endl;
	std::cout << "Y/X";
	for (int i = 0; i < 199; i++)
	{
		double xcm = (double(i) - 99.0);
		std::cout << ',' << xcm;
	}
	std::cout << std::endl;

	for (int j = 0; j < 199; j++)
	{
		double ycm = (double(j) - 99.0);
		dy = (-ycm) * ystep;
		yd = dy + xygeom[u1].yc() - xygeom[u2].yc();
		yl = pathint (u1, u2, 1, dy, xygeom);
		std::cout << ycm;
		for (int i = 0; i < 199; i++)
		{
			dx = (double(i) - 99.0) * xstep;
			xd = dx + xygeom[u1].xc() - xygeom[u2].xc();
			xl = pathint (u1, u2, 0, dx, xygeom);
			double r1 = sqrt ((dzx * dzx) + (xd * xd));
			double csphi = dzx / r1;
			double zd = dzy / csphi;
			double r2 = sqrt ((zd * zd) + (yd * yd));
			double r3 = dzy / r2;
			double cstheta = yd / r2;
			double wt = cstheta * cstheta * (1.0 - ru) + ru;
			std::cout << ',' << ((xl / dzx) * (yl / dzy) * r3 * r3 * r3 * r3 * wt);
		}
		std::cout << std::endl;
	}
	return 0;

}
