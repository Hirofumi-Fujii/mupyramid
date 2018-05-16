// genacc.cpp
// g++ -Wall genacc.cpp xyunitgeom.cpp setup1f1.cpp hist2d.cpp -o genacc.exe
//
// generate acceptance

#include <iostream>
#include <sstream>
#include <cmath>

#include "setup1f1.h"
#include "hist2d.h"

double overlapsize (double xmin, double xmax, double width)
{
	// initial condition:
	//    (xmax >= xmin) && (width >= (xmax - xmin))
	// xmin, xmax are measured from the center of width
	// i.e., accepted range is [-width/2, width/2]

	double halfwidth = width * 0.5;
	double wmin = -halfwidth;
	double wmax = halfwidth;

	if ((xmax <= wmin) || (xmin >= wmax))
		return 0.0;

	if (xmin < wmin)
		xmin = wmin;
	if (xmax > wmax)
		xmax = wmax;
	return (xmax - xmin);
}
		
int main (int argc, char* argv[])
{
	using namespace MUONDAQ;
	using namespace mylibrary;

	bool enable_gap = true;

	int u1 = 0;
	int u2 = 1;

	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " unit1 unit2" << std::endl;
		return (-1);
	}
	std::stringstream ss;
	ss << argv[1] << ' ' << argv[2] << ' ';
	ss >> u1;
	ss >> u2;

	static const int num2dhist = 2;
	Hist2D hist2d[num2dhist] =
	{
		Hist2D ("Infinite uniform", -99.5, 99.5, 199, -99.5, 99.5, 199),
		Hist2D ("Infinite cos^2", -99.5, 99.5, 199, -99.5, 99.5, 199),
	};

	std::cout << "Combination: Unit" << u1 << "-Unit" << u2 << std::endl;
	double zx1 = Setup1F1::xyunitgeom[u1].zx();
	double zy1 = Setup1F1::xyunitgeom[u1].zy();
	double zx2 = Setup1F1::xyunitgeom[u2].zx();
	double zy2 = Setup1F1::xyunitgeom[u2].zy();

	double distx = zx1 - zx2;
	double disty = zy1 - zy2;
	std::cout << "xdist " << distx << " ydist " << disty << std::endl;

	double dsx1 = Setup1F1::xyunitgeom[u1].xchsize();
	double dsy1 = Setup1F1::xyunitgeom[u1].ychsize();
	double dsx2 = Setup1F1::xyunitgeom[u2].xchsize();
	double dsy2 = Setup1F1::xyunitgeom[u2].ychsize();
	std::cout << "chsize [" << u1 << "] " << dsx1 << ' ' << dsy1
		<< " [" << u2 << "] " << dsx2 << ' ' << dsy2
		<< std::endl;

	double sizex1 = Setup1F1::xyunitgeom[u1].size().x();
	double sizey1 = Setup1F1::xyunitgeom[u1].size().y();
	double sizex2 = Setup1F1::xyunitgeom[u2].size().x();
	double sizey2 = Setup1F1::xyunitgeom[u2].size().y();

	double hfdsx1 = dsx1 * 0.5;
	double hfdsy1 = dsy1 * 0.5;
	double hfdsx2 = dsx2 * 0.5;
	double hfdsy2 = dsy2 * 0.5;

	double ds = (dsx1 / distx) * (dsy1 / disty) * (dsx2 / 10.0) * (dsy2 / 10.0);	// cm^2 units

	double x1;
	double y1;
	double x2;
	double y2;

	double wt0max = 0.0;
	double wt1max = 0.0;

	for (int ych2 = 0; ych2 < XYUnitGeometry::NUM_YCHANNELS; ych2++)
	{
		y2 = Setup1F1::xyunitgeom[u2].ychpos(ych2);
		for (int xch2 = 0; xch2 < XYUnitGeometry::NUM_XCHANNELS; xch2++)
		{
			x2 = Setup1F1::xyunitgeom[u2].xchpos(xch2);
			for (int ych1 = 0; ych1 < XYUnitGeometry::NUM_YCHANNELS; ych1++)
			{
				y1 = Setup1F1::xyunitgeom[u1].ychpos(ych1);
				for (int xch1 = 0; xch1 < XYUnitGeometry::NUM_XCHANNELS; xch1++)
				{
					x1 = Setup1F1::xyunitgeom[u1].xchpos(xch1);
					double rdx = (x1 - x2) / distx;
					double rdy = (y1 - y2) / disty;
					double cs2phi = 1.0 / (1.0 + (rdx * rdx));
					double sn2the = 1.0 / (1.0 + (rdy * rdy * cs2phi));
					double cs2the = 1.0 - sn2the;
					double wt0 = cs2phi * sn2the * cs2phi * sn2the * ds;
					double wt1 = wt0 * cs2the;
					if (wt0 > wt0max)
						wt0max = wt0;
					if (wt1 > wt1max)
						wt1max = wt1;
					if (enable_gap)
					{
						// check the hit posion on the pair plane
						// Y plane
						double xony1 = rdx * (zy1 - zx2) + x2 - Setup1F1::xyunitgeom[u1].xc();
						double xony2 = rdx * (zy2 - zx2) + x2 - Setup1F1::xyunitgeom[u2].xc();
						// X plane
						double yonx1 = rdy * (zx1 - zy2) + y2 - Setup1F1::xyunitgeom[u1].yc();
						double yonx2 = rdy * (zx2 - zy2) + y2 - Setup1F1::xyunitgeom[u2].yc();
//						if ((fabs(xony1) > Setup1F1::xyunitgeom[u1].size().x() * 0.5) ||
//							(fabs(xony2) > Setup1F1::xyunitgeom[u2].size().x() * 0.5) ||
//							(fabs(yonx1) > Setup1F1::xyunitgeom[u1].size().y() * 0.5) ||
//							(fabs(yonx2) > Setup1F1::xyunitgeom[u2].size().y() * 0.5))
//						{
//							wt0 = 0.0;
//							wt1 = 0.0;
//						}
						double effdx1 = overlapsize((xony1 - hfdsx1), (xony1 + hfdsx1), sizex1);
						double effdy1 = overlapsize((yonx1 - hfdsy1), (yonx1 + hfdsy1), sizey1);
						double effdx2 = overlapsize((xony2 - hfdsx2), (xony2 + hfdsx2), sizex2);
						double effdy2 = overlapsize((yonx2 - hfdsy2), (yonx2 + hfdsy2), sizey2);
						double effarea = effdx1 / dsx1 * effdy1 / dsy1 * effdx2 / dsx2 * effdy2 / dsy2;
						wt0 = wt0 * effarea;
						wt1 = wt1 * effarea;
					}
					double devx1 = double(xch1);
					if (Setup1F1::xyunitgeom[u1].order () == XYUnitGeometry::ORDER_XY)
						devx1 = double(XYUnitGeometry::NUM_XCHANNELS - 1 - xch1);
					double devx2 = double(xch2);
					if (Setup1F1::xyunitgeom[u2].order () == XYUnitGeometry::ORDER_XY)
						devx2 = double(XYUnitGeometry::NUM_XCHANNELS - 1 - xch2);
					double dchx = devx1 - devx2;
					double dchy = double(ych1) - double(ych2);
					hist2d[0].cumulate(dchx, dchy, wt0);
					hist2d[1].cumulate(dchx, dchy, wt1);
				}
			}
		}
	}

	std::cout << wt0max << ' ' << wt1max << std::endl;
	for (int n = 0; n < num2dhist; n++)
	{
		hist2d[n].dump(std::cout);
	}
	return 0;
}
