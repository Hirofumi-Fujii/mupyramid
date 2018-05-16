// xytest.cpp
// g++ -Wall xytest.cpp -o xytest.exe

#include <iostream>
#include "xyunitgeom.h"

int main ()
{
	using namespace MUONDAQ;

	XYUnitGeometry xygeom[6] =
	{
		XYUnitGeometry(
			XYUnitGeometry::Position (-50.0, 498.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-51.0, 264.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-50.0, 0.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-50.0, 501.0, 0.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-51.0, 265.0, 950.0),
			XYUnitGeometry::Size (1020.0, 1020.0, 59.0),
			XYUnitGeometry::ORDER_XY ),
		XYUnitGeometry(
			XYUnitGeometry::Position (-50.0, 0.0, 1900.0),
			XYUnitGeometry::Size (1000.0, 1000.0, 83.0),
			XYUnitGeometry::ORDER_YX ),
	};

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
	return 0;
}

