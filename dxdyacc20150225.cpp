// dxdyacc.cpp
// g++ -Wall dxdyacc.cpp -o dxdyacc.exe

#include <iostream>
#include <sstream>
#include <cmath>
#include "xyunitgeom.h"

double pathint(int axis, int utype, double pd, const MUONDAQ::XYUnitGeometry xyunit[])
{
	// Returns effective length of our detector (in length units)
	//   axis
	//      0   X-axis
	//      1   Y-axis
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
	using namespace MUONDAQ;

	double ru = 0.01;	// ratio of uniform distribution

	if (argc != 3)
	{
		std::cout << "Usage: "
			<< argv[0]
			<< " system(1|2) type(0|1|2)"
			<< std::endl;
		return (-1);
	}
	std::stringstream ss;
	ss << argv[1] << ' ' << argv[2];
	int nsys;
	int ntype;
	if ((!(ss >> nsys)) || (!(ss >> ntype)))
	{
		std::cout << "ERROR: cannot read integer value."
			<< std::endl;
		return (-2);
	}

	XYUnitGeometry xygeom[6] =
	{
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 498.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 264.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 0.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 501.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-510.0, 265.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-500.0, 0.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
	};

	int isys = 0;
	if (nsys == 2)
		isys = 3;
	for (int i = 0; i < 6; i++)
	{
		std::cout
			<< (i + 1)
			<< ' ' << xygeom[i].position().x()
			<< ' ' << xygeom[i].position().y()
			<< ' ' << xygeom[i].position().z()
			<< ' ' << xygeom[i].size().x()
			<< ' ' << xygeom[i].size().y()
			<< ' ' << xygeom[i].size().z();
		if (xygeom[i].order() == XYUnitGeometry::ORDER_XY)
			std::cout << " XY";
		else
			std::cout << " YX";
		std::cout << std::endl;
	}

	double dx;
	std::cout << "*** X *** d" << nsys << '_' << ntype << std::endl;
	for (int i = 0; i < 199; i++)
	{
		dx = (double(i) - 99.0) * 10.0;		// mm units
		std::cout << pathint (0, ntype, dx, &xygeom[isys])
			<< std::endl;
	}

	double dy;
	std::cout << "*** Y *** d" << nsys << '_' << ntype << std::endl;
	for (int i = 0; i < 199; i++)
	{
//		dy = (double(i) - 99.0) * 10.0;		// mm units
		dy = (99.0 - double(i)) * 10.0;
		std::cout << pathint (1, ntype, dy, &xygeom[isys])
			<< std::endl;
	}

	int u1 = 0;
	int u2 = 1;

	double xl;
	double yl;
	double xd;	// x difference in world coordinate system
	double yd;	// y difference in world coordinate system
	double dzx;
	double dzy;
	std::cout << "*** XY *** d" << nsys << '_' << ntype << std::endl;
	for (int j = 0; j < 199; j++)
	{
//		dy = (double(j) - 99.0) * 10.0;
		dy = (99.0 - double(j)) * 10.0;
		yd = dy + xygeom[u1].position().y() - xygeom[u2].position().y();
		dzy = xygeom[u1].zy() - xygeom[u2].zy();
		yl = pathint (1, ntype, dy, &xygeom[isys]) / 100.0;
		for (int i = 0; i < 199; i++)
		{
			dx = (double(i) - 99.0) * 10.0;
			xd = dx + xygeom[u1].position().x() - xygeom[u2].position().x();
			dzx = xygeom[u1].zx() - xygeom[u2].zx();
			xl = pathint (0, ntype, dx, &xygeom[isys]) / 100.0;
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
