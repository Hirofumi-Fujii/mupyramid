// ehall2svg.cpp
// g++ -Wall ehall2svg.cpp setupehall.cpp xyunitgeom.cpp -o ehall2svg
//

#include <iostream>
#include "setupehall.h"

int main ()
{
	using namespace MUONEHALL;

	for (int n = 0; n < SetupEhall::NUM_UNITS; n++)
	{
		double w = SetupEhall::xyunitgeom[n].size().x();
		double h = SetupEhall::xyunitgeom[n].size().y();
		double d = SetupEhall::xyunitgeom[n].size().z();
		double x = SetupEhall::xyunitgeom[n].position().x();
		double y = SetupEhall::xyunitgeom[n].position().y();
		double z = SetupEhall::xyunitgeom[n].position().z();

		std::cout << "<rect x=\"" << (z - (d * 0.5)) << "\" "
			<< "y=\"" << (y - (h * 0.5)) << "\" "
			<< "width=\"" << d << "\" "
			<< "height=\"" << h << "\" />"
			<< std::endl;
	}
	return 0;
}
