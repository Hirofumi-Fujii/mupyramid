// coinhit.cpp
// g++ -Wall coinhit.cpp coinrecord.cpp  xyunitgeom.cpp setupehall.cpp hist1d.cpp hist2d.cpp gzfstream.cpp gzipfilebuf.cpp mytimer.cpp -lz -o coinhit

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "gzfstream.h"
#include "coinrecord.h"
#include "setupehall.h"
#include "hist1d.h"
#include "hist2d.h"
#include "mytimer.h"

static const unsigned int NUM_UNITS = 3;

int main (int argc, char* argv[])
{
	using namespace MUONDAQ;
	using namespace MUONEHALL;

	int reqtype = 0;
	int detsys = 0;
	int obspt = 0;

	int uidoffs = 0;
	int uid1 = 0;
	int uid2 = 1;

	double floorlevel = 2000.0;

	// check the arguments
	bool obstype_given = false;
	bool listfnam_given = false;
	bool tofmin_given = false;
	bool tofmax_given = false;

	std::string obstype;
	std::string listfnam;

	double tofmin = -1000.0;
	double tofmax = 1000.0;

	if (argc < 3)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " obs_type list_filename"
			<< std::endl;
		return (-1);
	}

	int iarg = 1;
	while (iarg < argc)
	{
		std::string sarg (argv[iarg++]);
		if (sarg[0] != '-')
		{
			listfnam_given = true;
			listfnam = sarg;
		}
		else if ((sarg == "-tofmin") || (sarg == "-tofmax"))
		{
			if (iarg >= argc)
			{
				std::cerr << "ERROR(" << argv[0] << ") missing value" << std::endl;
				return (-2);
			}
			double dvalue(0.0);
			std::stringstream ss (argv[iarg++]);
			if (!(ss >> dvalue))
			{
				std::cerr << "ERROR(" << argv[0]
					<< ") cannot read numerical value" << std::endl;
				return (-2);
			}
			if (sarg == "-tofmin")
			{
				tofmin = dvalue;
				tofmin_given = true;
			}
			else if (sarg == "-tofmax")
			{
				tofmax = dvalue;
				tofmax_given = true;
			}
		}
		else
		{
			obstype = sarg;
			obstype_given = true;
		}
	}

	if (!listfnam_given)
	{
		std::cerr << "ERROR(" << argv[0]
			<< ") list_filename is missing." << std::endl;
		return (-1);
	}
	if (!obstype_given)
	{
		std::cerr << "ERROR(" << argv[0]
			<< ") obs_type is missing." << std::endl;
		return (-1);
	}

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

//	rcdist = SetupEhall::obspoint[obspt - 1].distance ();
//	rcangl = SetupEhall::obspoint[obspt - 1].angle ();
//	detsys = SetupEhall::obspoint[obspt - 1].detector ();
//	floorlevel = SetupEhall::obspoint[obspt - 1].floorlevel ();
	uidoffs = (detsys - 1) * 3;

	// open the list file
	std::ifstream ifl (listfnam.c_str());
	if (!ifl)
	{
		// file open error
		std::cerr
			<< "ERROR: list file ("
			<< listfnam
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	static const int NUMHIST2D = 12;
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
		mylibrary::Hist2D ("tofcos", -99.5, 99.5, 199, -1.0, 1.0, 200),
		mylibrary::Hist2D ("tofcos-smooth", -99.5, 99.5, 199, -1.0, 1.0, 200, true),
	};

	static const int NUMHIST1D = 2;
	mylibrary::Hist1D hist1d[NUMHIST1D] =
	{
		mylibrary::Hist1D ("tof(all)", -99.5, 99.5, 199),
		mylibrary::Hist1D ("tof(selected)", -99.5, 99.5, 199),
	};

	double zx1 = SetupEhall::xyunitgeom[uid1 + uidoffs].zx();
	double zy1 = SetupEhall::xyunitgeom[uid1 + uidoffs].zy();
	double zx2 = SetupEhall::xyunitgeom[uid2 + uidoffs].zx();
	double zy2 = SetupEhall::xyunitgeom[uid2 + uidoffs].zy();

	double xc1 = SetupEhall::xyunitgeom[uid1 + uidoffs].position().x();
	double yc1 = SetupEhall::xyunitgeom[uid1 + uidoffs].position().y();
	double xc2 = SetupEhall::xyunitgeom[uid2 + uidoffs].position().x();
	double yc2 = SetupEhall::xyunitgeom[uid2 + uidoffs].position().y();

	double diffx = xc1 - xc2;
	double diffy = yc1 - yc2;

	double distx = zx2 - zx1;
	double disty = zy2 - zy1;
	double dist = (distx + disty) * 0.5;
	double scalex = dist / distx;
	double scaley = dist / disty;
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

						double tof = 1000000.0;
						
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

							double tu1 = coinrec.microsec(uid1);
							double tn1 = coinrec.nanosec(uid1);
							double tu2 = coinrec.microsec(uid2);
							double tn2 = coinrec.nanosec(uid2);

							tof = (tu1 - tu2) * 1000.0 + (tn1 - tn2);
							if (histit)
								hist1d[0].cumulate (tof);

							if (tofmin_given && (tof < tofmin))
								histit = false;
							if (tofmax_given && (tof > tofmax))
								histit = false;

						}
						if (histit)
						{
							double chx1 = double(coinrec.xpos(uid1)) / 10.0;
							double chx2 = double(coinrec.xpos(uid2)) / 10.0;
							double chy1 = double(coinrec.ypos(uid1)) / 10.0;
							double chy2 = double(coinrec.ypos(uid2)) / 10.0;
							x1 = SetupEhall::xyunitgeom[(uid1 + uidoffs)].xchpos(chx1);
							x2 = SetupEhall::xyunitgeom[(uid2 + uidoffs)].xchpos(chx2);
							y1 = SetupEhall::xyunitgeom[(uid1 + uidoffs)].ychpos(chy1);
							y2 = SetupEhall::xyunitgeom[(uid2 + uidoffs)].ychpos(chy2);
							double dx = ((x1 - x2) * scalex - diffx) / 10.0;
							double dy = ((y1 - y2) * scaley - diffy) / 10.0;

							hist2d[0].cumulate (dx, dy);
							hist2d[1].cumulate (dx, dy);
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
							hist2d[10].cumulate (tof, cst);
							hist2d[11].cumulate (tof, cst);
							hist1d[1].cumulate (tof);
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
			<< ",\"Unit Combination\","
			<< (uid1 + uidoffs + 1) << ',' << (uid2 + uidoffs + 1)
			<< ",\"TOF(min,max)\",";
			if (tofmin_given)
				ofcsv << tofmin;
			else
				ofcsv << "none";
			ofcsv << ",";
			if (tofmax_given)
				ofcsv << tofmax;
			else
				ofcsv << "none";
			ofcsv << std::endl;
		for (int n = 0; n < NUMHIST2D; n++)
			hist2d[n].CSVdump (ofcsv);
		for (int n = 0; n < NUMHIST1D; n++)
			hist1d[n].CSVdump (ofcsv);
		ofcsv.close ();
	}
	return 0;
}
