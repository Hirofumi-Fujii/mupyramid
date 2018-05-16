// genacc2.cpp
// g++ -Wall genacc2.cpp accopt.cpp xyunitgeom.cpp hist2d.cpp hist1d.cpp mytimer.cpp -o genacc2
//
// 2016-01-26 modified for Pyramid system
//
// generate acceptance

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include "accopt.h"
#include "hist2d.h"
#include "hist1d.h"
#include "mytimer.h"

static const int NUM_XCHANNELS = 120;
static const int NUM_YCHANNELS = 120;

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
	using namespace mylibrary;
	using namespace MUPYRAMID;

	Accopt accopt;
	accopt.m_order = Accopt::XYXY;
	accopt.m_xygap = 45.0;
	accopt.m_unitdist = 1500.0;
	if (!accopt.set (argc, argv))
	{
		accopt.usage (std::cout, argv[0]);
		return (-1);
	}

	bool enable_gap = true;
	std::string orderstr;
	double zy1;
	double zx1;
	double zy2;
	double zx2;
	
	if (accopt.m_order == Accopt::XYXY)
	{
		orderstr = "XYXY";
		zy2 = 0.0;
		zx2 = zy2 + accopt.m_xygap;
		zy1 = zy2 + accopt.m_unitdist;
		zx1 = zy1 + accopt.m_xygap;
	}
	else if (accopt.m_order == Accopt::XYYX)
	{
		orderstr = "XYYX";
		zx2 = 0.0;
		zy2 = zx2 + accopt.m_xygap;
		zy1 = zx2 + accopt.m_unitdist;
		zx1 = zy1 + accopt.m_xygap;
	}
	else if (accopt.m_order == Accopt::YXXY)
	{
		orderstr = "YXXY";
		zy2 = 0.0;
		zx2 = zy2 + accopt.m_xygap;
		zx1 = zy2 + accopt.m_unitdist;
		zy1 = zx1 + accopt.m_xygap;
	}
	else if (accopt.m_order == Accopt::YXYX)
	{
		orderstr = "YXYX";
		zx2 = 0.0;
		zy2 = zx2 + accopt.m_xygap;
		zx1 = zx2 + accopt.m_unitdist;
		zy1 = zx1 + accopt.m_xygap;
	}

	static const int NUM_HIST2D = 4;
	Hist2D hist2d[NUM_HIST2D] =
	{
		Hist2D ("dxdy uniform", -119.5, 119.5, 239, -119.5, 119.5, 239),
		Hist2D ("dxdy uniform smooth", -119.5, 119.5, 239, -119.5, 119.5, 239, true),
		Hist2D ("phicos uniform", -1.0, 1.0, 200, -1.0, 1.0, 200),
		Hist2D ("phicos uniform smooth", -1.0, 1.0, 200, -1.0, 1.0, 200, true),
	};

	static const int NUM_HIST1D = 1;
	mylibrary::Hist1D hist1d [NUM_HIST1D] =
	{
		Hist1D("cos(haa)", 0.0, 1.0, 100),
	};


	double distx = zx1 - zx2;
	double disty = zy1 - zy2;

	double dist = (distx + disty) * 0.5;
	double scalex = dist / distx;
	double scaley = dist / disty;

	double xc1 = 0.0;
	double yc1 = 0.0;
	double xc2 = 0.0;
	double yc2 = 0.0;

	double diffx = xc1 - xc2;
	double diffy = yc2 - yc1;

	double sizex1 = 1200.0;
	double sizey1 = 1200.0;
	double sizex2 = 1200.0;
	double sizey2 = 1200.0;

	double dsx1 = sizex1 / (double)(NUM_XCHANNELS);
	double dsy1 = sizey1 / (double)(NUM_YCHANNELS);
	double dsx2 = sizex2 / (double)(NUM_XCHANNELS);
	double dsy2 = sizey2 / (double)(NUM_YCHANNELS);

	double hfdsx1 = dsx1 * 0.5;
	double hfdsy1 = dsy1 * 0.5;
	double hfdsx2 = dsx2 * 0.5;
	double hfdsy2 = dsy2 * 0.5;

	// channel 0 position
	double x1pos0 = hfdsx1 - (sizex1 * 0.5);
	double y1pos0 = hfdsy1 - (sizey1 * 0.5);
	double x2pos0 = hfdsx2 - (sizex2 * 0.5);
	double y2pos0 = hfdsy2 - (sizey2 * 0.5);

	// area of the channel size
	double ds = (dsx1 / distx) * (dsy1 / disty)
			 * (dsx2 / 10.0) * (dsy2 / 10.0);	// cm^2 units; channel size is mm units.

	double x1;
	double y1;
	double x2;
	double y2;

	double wt0max = 0.0;

	mylibrary::MyTimer mytimer;
	mytimer.start ();
	for (int ych2 = 0; ych2 < NUM_YCHANNELS; ych2++)
	{
		y2 = (double)(ych2) * dsy2 + y2pos0;
		for (int xch2 = 0; xch2 < NUM_XCHANNELS; xch2++)
		{
			x2 = (double)(xch2) * dsx2 + x2pos0;
			for (int ych1 = 0; ych1 < NUM_YCHANNELS; ych1++)
			{
				y1 = (double)(ych1) * dsy1 + y1pos0;
				for (int xch1 = 0; xch1 < NUM_XCHANNELS; xch1++)
				{
					x1 = (double)(xch1) * dsx1 + x1pos0;
					double rdx = (x1 - x2) / distx;
					double rdy = (y1 - y2) / disty;
					double cs2phi = 1.0 / (1.0 + (rdx * rdx));
					double sn2the = 1.0 / (1.0 + (rdy * rdy * cs2phi));
//					double cs2the = 1.0 - sn2the;
					double wt0 = cs2phi * sn2the * cs2phi * sn2the * ds;
					if (wt0 > wt0max)
						wt0max = wt0;
					if (enable_gap)
					{
						// check the hit posion on the pair plane
						// Y plane
						double xony1 = rdx * (zy1 - zx2) + x2 - xc1;
						double xony2 = rdx * (zy2 - zx2) + x2 - xc2;
						// X plane
						double yonx1 = rdy * (zx1 - zy2) + y2 - yc1;
						double yonx2 = rdy * (zx2 - zy2) + y2 - yc2;
						double effdx1 = overlapsize((xony1 - hfdsx1), (xony1 + hfdsx1), sizex1);
						double effdy1 = overlapsize((yonx1 - hfdsy1), (yonx1 + hfdsy1), sizey1);
						double effdx2 = overlapsize((xony2 - hfdsx2), (xony2 + hfdsx2), sizex2);
						double effdy2 = overlapsize((yonx2 - hfdsy2), (yonx2 + hfdsy2), sizey2);
						double effarea = effdx1 / dsx1 * effdy1 / dsy1 * effdx2 / dsx2 * effdy2 / dsy2;
						wt0 = wt0 * effarea;
					}
					double dx = ((x1 - x2) * scalex - diffx) / 10.0;
					double dy = ((y1 - y2) * scaley - diffy) / 10.0;
					hist2d[0].cumulate (dx, dy, wt0);
					hist2d[1].cumulate (dx, dy, wt0);
					// angles mesured from rear unit
					double sx = rdx;
					double sy = rdy;
					double phi = atan (sx);
					double cst = sy / sqrt(sx * sx + sy * sy + 1.0);
					hist2d[2].cumulate (phi, cst, wt0);
					hist2d[3].cumulate (phi, cst, wt0);
					double rr2 = (rdx * rdx) + (rdy * rdy);
					double cshf = 1.0 / sqrt(1.0 + rr2);
					hist1d[0].cumulate (cshf, wt0);
				}
			}
		}
	}
	mytimer.stop ();
	// Show results.
	std::string ofname;
	if (accopt.m_outfilename != "")
		ofname = accopt.m_outfilename;
	else
		ofname = std::string("genacc2") + std::string(".csv");
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
		mytimer.csvout (ofcsv);
		ofcsv << std::endl;
		ofcsv
			<< "XYorder," << orderstr << ','
			<< "UnitDist(mm)," << accopt.m_unitdist << ','
			<< "XYgap(mm)," << accopt.m_xygap << ','
			<< "U1Xplanez," << zx1 << ','
			<< "U1Yplanez," << zy1 << ','
			<< "U2Xplanez," << zx2 << ','
			<< "U2Yplanez," << zy2 << ','
			<< "xdist," << distx << ",ydist," << disty
			<< ",scalex," << scalex << ",scaley," << scaley
			<< std::endl;

		for (int i = 0; i < NUM_HIST2D; i++)
		{
			hist2d[i].CSVdump (ofcsv);
		}
		for (int i = 0; i < NUM_HIST1D; i++)
		{
			hist1d[i].CSVdump (ofcsv);
		}
		ofcsv.close ();
	}
	return 0;
}
