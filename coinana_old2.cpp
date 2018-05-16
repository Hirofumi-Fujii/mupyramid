// coinana.cpp
// g++ -Wall coinana.cpp coinrecord.cpp  xyunitgeom.cpp setup1f1.cpp hist2d.cpp gzfstream.cpp gzipfilebuf.cpp mytimer.cpp -lz -o coinana

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "gzfstream.h"
#include "coinrecord.h"
#include "setup1f1.h"
#include "hist2d.h"
#include "mytimer.h"

static const unsigned int NUM_UNITS = 3;

int main (int argc, char* argv[])
{
	using namespace MUONDAQ;

	int reqtype = 0;
	int detsys = 0;
	int obspt = 0;

	int uidoffs = 0;
	int uid1 = 0;
	int uid2 = 1;

	double floorlevel;
	double rcdist;
	double rcangl;

	// check the arguments
	if (argc != 3)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " obs_type list_filename"
			<< std::endl;
		return (-1);
	}
	std::string obstype (argv[1]);
	if (obstype.size() < 5)
	{
		std::cerr << "ERROR(" << argv[0] << ") No such obstype" << std::endl;
		return (-1);
	}
	if ((obstype[1] != 'd') && (obstype[1] != 'o') && (obstype[1] != 'k'))
	{
		std::cerr << "ERROR(" << argv[0] << ") No such obstype" << std::endl;
		return (-1);
	}
	if (obstype[3] != 'c')
	{
		std::cerr << "ERROR(" << argv[0] << ") No such obstype" << std::endl;
		return (-1);
	}

	char c1 = obstype[2];
	char c2 = obstype[4];

	if (c1 == '1')
		obspt = 1;
	else if (c1 == '2')
		obspt = 2;
	else if (c1 == '3')
		obspt = 3;
	else if (c1 == '4')
		obspt = 4;
	else if (c1 == '5')
		obspt = 5;
	else if (c1 == '6')
		obspt = 6;
	else
	{
		std::cerr << "ERROR(" << argv[0] << ") No such obserbation point" << std::endl;
		return (-1);
	}

	if ((c2 == '0') || (c2 == '3'))
	{
		uid1 = 0;
		uid2 = 1;
		reqtype = (int)(c2 - '0');
	}
	else if ((c2 == '1') || (c2 == '4'))
	{
		uid1 = 0;
		uid2 = 2;
		reqtype = (int)(c2 - '0');
	}
	else if ((c2 == '2') || (c2 == '5'))
	{
		uid1 = 1;
		uid2 = 2;
		reqtype = (int)(c2 - '0');
	}
	else
	{
		std::cerr << "ERROR(" << argv[0] << ") No such unit-combination" << std::endl;
		return (-1);
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

	rcdist = Setup1F1::obspoint[obspt - 1].distance ();
	rcangl = Setup1F1::obspoint[obspt - 1].angle ();
	detsys = Setup1F1::obspoint[obspt - 1].detector ();
	floorlevel = Setup1F1::obspoint[obspt - 1].floorlevel ();
	uidoffs = (detsys - 1) * 3;

	// open the list file
	std::ifstream ifl (argv[2]);
	if (!ifl)
	{
		// file open error
		std::cerr
			<< "ERROR: list file ("
			<< argv[1]
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	static const int NUMHIST2D = 10;
	mylibrary::Hist2D hist2d[NUMHIST2D] =
	{
		mylibrary::Hist2D ("dxdy", -99.5, 99.5, 199, -99.5, 99.5, 199),
		mylibrary::Hist2D ("dxdy-smooth", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
		mylibrary::Hist2D ("RCdxdy", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins),
		mylibrary::Hist2D ("RCdxdy-smooth", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins, true),
		mylibrary::Hist2D ("OPRCdxdy", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins),
		mylibrary::Hist2D ("OPRCdxdy-smooth", rcxmin, rcxmax, nrcxbins, rcymin, rcymax, nrcybins, true),
		mylibrary::Hist2D ("OPdxdy", -0.75, 0.75, 150, 11.0, 13.0, 200),
		mylibrary::Hist2D ("OPdxdy-smooth", -0.75, 0.75, 150, 11.0, 13.0, 200, true),
		mylibrary::Hist2D ("phicos", -1.0, 1.0, 200, -1.0, 1.0, 200),
		mylibrary::Hist2D ("phicos-smooth", -1.0, 1.0, 200, -1.0, 1.0, 200, true),
	};

	double zx1 = Setup1F1::xyunitgeom[uid1 + uidoffs].zx();
	double zy1 = Setup1F1::xyunitgeom[uid1 + uidoffs].zy();
	double zx2 = Setup1F1::xyunitgeom[uid2 + uidoffs].zx();
	double zy2 = Setup1F1::xyunitgeom[uid2 + uidoffs].zy();

	double xc1 = Setup1F1::xyunitgeom[uid1 + uidoffs].xc();
	double yc1 = Setup1F1::xyunitgeom[uid1 + uidoffs].yc();
	double xc2 = Setup1F1::xyunitgeom[uid2 + uidoffs].xc();
	double yc2 = Setup1F1::xyunitgeom[uid2 + uidoffs].yc();

	double diffx = xc1 - xc2;
	double diffy = yc1 - yc2;

	double distx = zx2 - zx1;
	double disty = zy2 - zy1;
	double dist = (distx + disty) * 0.5;
	double scalex = dist / distx;
	double scaley = dist / disty;
	double rcscalex = (rcdist + zx2) / distx;
	double rcscaley = (rcdist + zy2) / disty;
	double opscalex = (zx2 - zy2) * 0.5 / distx;
	double opscaley = (zy2 - zx2) * 0.5 / disty;

	long totalfile = 0;
	double duration = 0.0;
	double totalcoin = 0.0;

	double tsecnow = 0.0;
	double tsecfirst = 0.0;
	double tseclast = 0.0;

	mylibrary::MyTimer mytimer;

	// read the filename from the list file
	std::string liststr;
	mytimer.start ();
	while (getline (ifl, liststr))
	{
		std::string datfnam;
		bool doit = true;
		if ((liststr.size() <= 0) || (liststr[0] == '\%'))
			doit = false;
		if (doit)
		{
			std::stringstream ssl (liststr);
			if (!(ssl >> datfnam))
				doit = false;
		}
		if (doit)
		{
			// open the data file
			mylibrary::igzfstream ifs (datfnam.c_str());
			if (!ifs)
			{
				// file open error
				std::cerr
					<< "ERROR: data file ("
					<< datfnam
					<< ") open error."
					<< std::endl;
			}
			else
			{
				totalfile += 1;
				bool newrun = true;
				tsecnow = 0.0;
				tsecfirst = 0.0;
				tseclast = 0.0;
				// read file and count the data-records
				std::string recstr;
				while (getline (ifs, recstr))
				{
					if ((recstr.size() > 4) &&
						(recstr.substr(0,4) == "COIN"))
					{
						bool histit = false;
						double x1 = 1000.0;
						double x2 = -1000.0;
						double y1 = 1000.0;
						double y2 = -1000.0;
						
						std::stringstream ss (recstr);
						std::string recid;
						ss >> recid;
						MUONDAQ::CoinRecord coinrec;
						if ((ss >> coinrec))
						{
							totalcoin += 1.0;
							for (unsigned int u = 0; u < NUM_UNITS; u++)
							{
								if (coinrec.numdat(u) > 0)
								{
									tsecnow = coinrec.microsec(u) / 1000000.0;
									break;
								}
							}
							if (newrun)
							{
								tsecfirst = tsecnow;
								newrun = false;
							}
							tseclast = tsecnow;
							if (reqtype >= 3)
							{
								if (coinrec.xy1cluster(0) &&
									coinrec.xy1cluster(1) &&
									coinrec.xy1cluster(2))
									histit = true;
							}
							else
							{
								if (coinrec.xy1cluster (uid1) &&
									coinrec.xy1cluster (uid2))
									histit = true;
							}
						}
						if (histit)
						{
							double chx1 = double(coinrec.xpos(uid1)) / 10.0;
							double chx2 = double(coinrec.xpos(uid2)) / 10.0;
							double chy1 = double(coinrec.ypos(uid1)) / 10.0;
							double chy2 = double(coinrec.ypos(uid2)) / 10.0;
							x1 = Setup1F1::xyunitgeom[(uid1 + uidoffs)].xchpos(chx1);
							x2 = Setup1F1::xyunitgeom[(uid2 + uidoffs)].xchpos(chx2);
							y1 = Setup1F1::xyunitgeom[(uid1 + uidoffs)].ychpos(chy1);
							y2 = Setup1F1::xyunitgeom[(uid2 + uidoffs)].ychpos(chy2);
							double dx = ((x1 - x2) * scalex - diffx) / 10.0;
							double dy = ((y1 - y2) * scaley - diffy) / 10.0;
							hist2d[0].cumulate (dx, dy);
							hist2d[1].cumulate (dx, dy);
							double rcx = ((x1 - x2) * rcscalex + x2) / 1000.0;
							double rcy = ((y1 - y2) * rcscaley + y2 + floorlevel) / 1000.0;
							hist2d[2].cumulate (rcx, rcy);
							hist2d[3].cumulate (rcx, rcy);
							double opx = ((x1 - x2) * opscalex + x2) / 1000.0;
							double opy = ((y1 - y2) * opscaley + y2 + floorlevel) / 1000.0;
							hist2d[4].cumulate (opx, opy);
							hist2d[5].cumulate (opx, opy);
							hist2d[6].cumulate (opx, opy);
							hist2d[7].cumulate (opx, opy);
							double sx = (x1 - x2) / distx;
							double sy = (y1 - y2) / disty;
							double phi = atan (sx);
							double cst = sy / sqrt(sx * sx + sy * sy + 1.0);
							hist2d[8].cumulate (phi, cst);
							hist2d[9].cumulate (phi, cst);
						}
					}
				}
				duration = duration + (tseclast - tsecfirst);
			}
		}
		liststr.clear();
	}
	mytimer.stop ();

	// Show results.
	std::string ofname = std::string("coinana") + obstype + std::string(".csv");
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
		ofcsv << "\"Total: files\","
			<< totalfile
			<< ",\"COINs\","
			<< totalcoin
			<< ",\"duration(sec)\","
			<< duration
			<< ",\"Detector system\","
			<< detsys
			<< ",\"Obsavation point\","
			<< obspt
			<< ",\"Distance from RC\","
			<< rcdist
			<< ",\"View angle\","
			<< rcangl
			<< ",\"Unit Combination\","
			<< (uid1 + uidoffs + 1) << ',' << (uid2 + uidoffs + 1)
			<< std::endl;
		for (int n = 0; n < NUMHIST2D; n++)
			hist2d[n].dump (ofcsv);
		ofcsv.close ();
	}
	return 0;
}
