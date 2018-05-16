// genacc2.cpp
// g++ -Wall genacc2.cpp xyunitgeom.cpp setup1f1.cpp hist2d.cpp mytimer.cpp -o genacc2
//
// generate acceptance

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include "setup1f1.h"
#include "hist2d.h"
#include "mytimer.h"

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

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " obs_type" << std::endl;
		std::cout << "obs_type is one of the\n";
		std::cout << " -o1c0 -o1c1 -o1c2\n";
		std::cout << " -o2c0 -o2c1 -o2c2\n";
		std::cout << " -o3c0 -o3c1 -o3c2\n";
		std::cout << " -k4c0 -k4c1 -k4c2\n";
		std::cout << " -k5c0 -k5c1 -k5c2\n";
		std::cout << " -k6c0 -k6c1 -k6c2\n";
		std::cout << std::endl;
		return (-1);
	}
	std::string obstype (argv[1]);
	if ((obstype[0] != '-') || 
		((obstype[1] != 'o') && (obstype[1] != 'k')) || (obstype[3] != 'c'))
	{
		std::cerr << "ERROR: " << argv[0] << ": No such option "
			<< obstype << std::endl;
		return (-2);
	}

	int obspt = 0;
	char c = obstype[2];
	if (c == '1')
		obspt = 1;
	else if (c == '2')
		obspt = 2;
	else if (c == '3')
		obspt = 3;
	else if (c == '4')
		obspt = 4;
	else if (c == '5')
		obspt = 5;
	else if (c == '6')
		obspt = 6;
	else
	{
		std::cerr << "ERROR: " << argv[0] << ": Unknown observation point "
			<< obstype << std::endl;
		return (-2);
	}

	int uid1 = 0;
	int uid2 = 0;
	c = obstype[4];
	if (c == '0')
	{
		uid1 = 0;
		uid2 = 1;
	}
	else if (c == '1')
	{
		uid1 = 0;
		uid2 = 2;
	}
	else if (c == '2')
	{
		uid1 = 1;
		uid2 = 2;
	}
	else
	{
		std::cerr << "ERROR: " << argv[0] << ": Unknown unit combination "
			<< obstype << std::endl;
		return (-2);
	}

	unsigned int nrcxbins = 200;
	unsigned int nrcybins = 200;
	double rcxmin = -40.0;
	double rcxmax =  40.0;
	double rcymin = -19.0;
	double rcymax = 61.0;
	if ((uid1 == 0) && (uid2 == 2))
	{
		// High resolution mode
		rcxmin = -20.0;
		rcxmax = 20.0;
		rcymin = 1.0;
		rcymax = 41.0;
	}

	int detsys = Setup1F1::obspoint[obspt - 1].detector ();
	int uidoffs = (detsys - 1) * 3;

	double rcdist = Setup1F1::obspoint[obspt - 1].distance ();
	double rcangl = Setup1F1::obspoint[obspt - 1].angle ();
	double floorlevel = Setup1F1::obspoint[obspt - 1].floorlevel ();

	int u1 = uid1 + uidoffs;
	int u2 = uid2 + uidoffs;

	static const int num2dhist = 12;
	Hist2D hist2d[num2dhist] =
	{
		Hist2D ("dxdy uniform", -99.5, 99.5, 199, -99.5, 99.5, 199),
		Hist2D ("dxdy uniform smooth", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
		Hist2D ("dxdy cos^2", -99.5, 99.5, 199, -99.5, 99.5, 199),
		Hist2D ("dxdy cos^2 smooth", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
		Hist2D ("RCdxdy uniform", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins),
		Hist2D ("RCdxdy uniform smooth", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins, true),
		Hist2D ("RCdxdy cos^2", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins),
		Hist2D ("RCdxdy cos^2 smooth", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins, true),
		Hist2D ("phicos uniform", -1.0, 1.0, 200, -1.0, 1.0, 200),
		Hist2D ("phicos uniform smooth", -1.0, 1.0, 200, -1.0, 1.0, 200, true),
		Hist2D ("phicos cos^2", -1.0, 1.0, 200, -1.0, 1.0, 200),
		Hist2D ("phicos cos^2 smooth", -1.0, 1.0, 200, -1.0, 1.0, 200, true),
	};

	std::cout << "Combination: Unit" << u1 << "-Unit" << u2 << std::endl;
	double zx1 = Setup1F1::xyunitgeom[u1].zx();
	double zy1 = Setup1F1::xyunitgeom[u1].zy();
	double zx2 = Setup1F1::xyunitgeom[u2].zx();
	double zy2 = Setup1F1::xyunitgeom[u2].zy();

	double distx = zx1 - zx2;
	double disty = zy1 - zy2;

	double dist = (distx + disty) * 0.5;
	double scalex = dist / distx;
	double scaley = dist / disty;
	double rcscalex = (rcdist + zx2) / (-distx);
	double rcscaley = (rcdist + zy2) / (-disty);

	double dsx1 = Setup1F1::xyunitgeom[u1].xchsize();
	double dsy1 = Setup1F1::xyunitgeom[u1].ychsize();
	double dsx2 = Setup1F1::xyunitgeom[u2].xchsize();
	double dsy2 = Setup1F1::xyunitgeom[u2].ychsize();

	double xc1 = Setup1F1::xyunitgeom[u1].xc();
	double yc1 = Setup1F1::xyunitgeom[u1].yc();
	double xc2 = Setup1F1::xyunitgeom[u2].xc();
	double yc2 = Setup1F1::xyunitgeom[u2].yc();

	std::cout << "chsize [" << u1 << "] " << dsx1 << ' ' << dsy1
		<< " [" << u2 << "] " << dsx2 << ' ' << dsy2
		<< std::endl;

	double diffx = xc1 - xc2;
	double diffy = yc2 - yc1;

	double sizex1 = Setup1F1::xyunitgeom[u1].size().x();
	double sizey1 = Setup1F1::xyunitgeom[u1].size().y();
	double sizex2 = Setup1F1::xyunitgeom[u2].size().x();
	double sizey2 = Setup1F1::xyunitgeom[u2].size().y();

	double hfdsx1 = dsx1 * 0.5;
	double hfdsy1 = dsy1 * 0.5;
	double hfdsx2 = dsx2 * 0.5;
	double hfdsy2 = dsy2 * 0.5;

	// our digitizer gives the half of the channel size
	double ds = (hfdsx1 / distx) * (hfdsy1 / disty)
			 * (hfdsx2 / 10.0) * (hfdsy2 / 10.0);	// cm^2 units; channel size is mm units.

	double x1;
	double y1;
	double x2;
	double y2;

	double wt0max = 0.0;
	double wt1max = 0.0;

	mylibrary::MyTimer mytimer;
	mytimer.start ();
	for (int ych2 = 0; ych2 < (XYUnitGeometry::NUM_YCHANNELS * 2 - 1); ych2++)
	{
		y2 = Setup1F1::xyunitgeom[u2].ychpos(double(ych2) * 0.5);
		for (int xch2 = 0; xch2 < (XYUnitGeometry::NUM_XCHANNELS * 2 - 1); xch2++)
		{
			x2 = Setup1F1::xyunitgeom[u2].xchpos(double(xch2) * 0.5);
			for (int ych1 = 0; ych1 < (XYUnitGeometry::NUM_YCHANNELS * 2 - 1); ych1++)
			{
				y1 = Setup1F1::xyunitgeom[u1].ychpos(double(ych1) * 0.5);
				for (int xch1 = 0; xch1 < (XYUnitGeometry::NUM_XCHANNELS * 2 - 1); xch1++)
				{
					x1 = Setup1F1::xyunitgeom[u1].xchpos(double(xch1) * 0.5);
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
						double effdx1 = overlapsize((xony1 - hfdsx1), (xony1 + hfdsx1), sizex1);
						double effdy1 = overlapsize((yonx1 - hfdsy1), (yonx1 + hfdsy1), sizey1);
						double effdx2 = overlapsize((xony2 - hfdsx2), (xony2 + hfdsx2), sizex2);
						double effdy2 = overlapsize((yonx2 - hfdsy2), (yonx2 + hfdsy2), sizey2);
						double effarea = effdx1 / dsx1 * effdy1 / dsy1 * effdx2 / dsx2 * effdy2 / dsy2;
						wt0 = wt0 * effarea;
						wt1 = wt1 * effarea;
					}
					double dx = ((x1 - x2) * scalex - diffx) / 10.0;
					double dy = ((y1 - y2) * scaley - diffy) / 10.0;
					hist2d[0].cumulate (dx, dy, wt0);
					hist2d[1].cumulate (dx, dy, wt0);
					hist2d[2].cumulate (dx, dy, wt1);
					hist2d[3].cumulate (dx, dy, wt1);
					double rcx = ((x1 - x2) * rcscalex + x2) / 1000.0;
					double rcy = ((y1 - y2) * rcscaley + y2 + floorlevel) / 1000.0;
					hist2d[4].cumulate (rcx, rcy, wt0);
					hist2d[5].cumulate (rcx, rcy, wt0);
					hist2d[6].cumulate (rcx, rcy, wt1);
					hist2d[7].cumulate (rcx, rcy, wt1);
					// angles mesured from rear unit
					// i.e., negative to the muon direction
					double sx = -rdx;
					double sy = -rdy;
					double phi = atan (sx);
					double cst = sy / sqrt(sx * sx + sy * sy + 1.0);
					hist2d[8].cumulate (phi, cst, wt0);
					hist2d[9].cumulate (phi, cst, wt0);
					hist2d[10].cumulate (phi, cst, wt1);
					hist2d[11].cumulate (phi, cst, wt1);
				}
			}
		}
	}
	mytimer.stop ();
	// Show results.
	std::string ofname = std::string("genacc2") + obstype + std::string(".csv");
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
		ofcsv << ",\"UnitId\","
			<< u1 << "," << u2 
			<< std::endl;

		ofcsv << "\"RCdist\"," << rcdist
			<< ",\"angle\"," << rcangl
			<< ",\"floor level\"," << floorlevel
			<< ",\"wt0max\"," << wt0max
			<< ",\"wt1max\"," << wt1max
			<< std::endl;
		ofcsv << "xdist," << distx << ",ydist," << disty
			<< ",scalex," << scalex << ",scaley," << scaley
			<< ",rcscalex," << rcscalex << ",rcscaley," << rcscaley
			<< std::endl;

		for (int n = 0; n < num2dhist; n++)
			hist2d[n].dump (ofcsv);
		ofcsv.close ();
	}
	return 0;
}
