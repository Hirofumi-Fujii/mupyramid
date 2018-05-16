// coinana.cpp
// g++ -Wall coinana.cpp coinrecord.cpp hist1d.cpp hist2d.cpp ncpng.cpp gzfstream.cpp gzipfilebuf.cpp mytimer.cpp -lz -o coinana

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "gzfstream.h"
#include "coinrecord.h"
#include "hist1d.h"
#include "hist2d.h"
#include "mytimer.h"

static const int NUM_XCH = 120;
static const int NUM_YCH = 120;
static const int NUM_DXCH = (NUM_XCH * 2 - 1);
static const int NUM_DYCH = (NUM_YCH * 2 - 1);

double csttab[NUM_DYCH][NUM_DXCH];
double acctab[NUM_DYCH][NUM_DXCH];
double accfluxtab[NUM_DYCH][NUM_DXCH];
double acccmbtab[NUM_DYCH][NUM_DXCH];
double acccmbfluxtab[NUM_DYCH][NUM_DXCH];

void genacc0 (double flux0, double chsizex, double chsizey, double udist)
{
	double rchsizex = chsizex / udist;
	double rchsizey = chsizey / udist;
	double dsize = chsizex * rchsizex * chsizey * rchsizey;

	for (int iy1 = 0; iy1 < NUM_DYCH; iy1++)
	{
		double dcy = double(iy1) - double(NUM_YCH - 1);
		double cny = double(NUM_YCH) - fabs(dcy);
		double dy = dcy * rchsizey;
		for (int ix1 = 0; ix1< NUM_DXCH; ix1++)
		{
			double dcx = double(ix1) - double(NUM_XCH - 1);
			double cnx = double(NUM_XCH) - fabs(dcx);
			double dx = dcx * rchsizex;
			double cs2 = 1.0 / ((dx * dx) + (dy * dy) + 1.0);
			double wt = cs2 * cs2 * dsize;
			csttab[iy1][ix1] = sqrt(cs2);
			acctab[iy1][ix1] = wt;
			accfluxtab[iy1][ix1] = (wt * flux0 * cs2);	// assuming that the flux is propotional to cs2
			acccmbtab[iy1][ix1] = acctab[iy1][ix1] * cnx * cny;
			acccmbfluxtab[iy1][ix1] = accfluxtab[iy1][ix1] * cnx * cny;
		}
	}
}

int main (int argc, char* argv[])
{
	using namespace MUONDAQ;

	int detsys = 0;
	int obspt = 0;

	int uidoffs = 0;
	int uid1 = 0;
	int uid2 = 1;

	// check the arguments
	bool listfnam_given = false;
	bool tofmin_given = false;
	bool tofmax_given = false;

	std::string listfnam;

	double flux0 = 70.0;

	double udistx = 150.0;
	double udisty = 150.0;

	double chsizex = 1.0;
	double chsizey = 1.0;

	double tofmin = -1000.0;
	double tofmax = 1000.0;

	if (argc < 2)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " [-tofmin rmin] [-tofmax rmax] list_filename"
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
	}

	if (!listfnam_given)
	{
		std::cerr << "ERROR(" << argv[0]
			<< ") list_filename is missing." << std::endl;
		return (-1);
	}

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

	static const int NUMHIST2D = 13;
	mylibrary::Hist2D hist2d[NUMHIST2D] =
	{
		mylibrary::Hist2D ("dxdy", -119.5, 119.5, 239, -119.5, 119.5, 239),
		mylibrary::Hist2D ("dxdy-smooth", -119.5, 119.5, 239, -119.5, 119.5, 239, true),
		mylibrary::Hist2D ("U1xy", -0.5, 119.5, 120, -0.5, 119.5, 120),
		mylibrary::Hist2D ("U1xy-smooth", -0.5, 119.5, 120, -0.5, 119.5, 120, true),
		mylibrary::Hist2D ("U2xy", -0.5, 119.5, 120, -0.5, 119.5, 120),
		mylibrary::Hist2D ("U2xy-smooth", -0.5, 119.5, 120, -0.5, 119.5, 120, true),
		mylibrary::Hist2D ("phi-cos", -1.0, 1.0, 200, -1.0, 1.0, 200),	
		mylibrary::Hist2D ("phi-cos-smooth", -1.0, 1.0, 200, -1.0, 1.0, 200, true),	
		mylibrary::Hist2D ("acc-dxdy", -0.5, 238.5, 239, -0.5, 238.5, 239),
		mylibrary::Hist2D ("accflux-dxdy", -0.5, 238.5, 239, -0.5, 238.5, 239),
		mylibrary::Hist2D ("acccmb-dxdy", -0.5, 238.5, 239, -0.5, 238.5, 239),
		mylibrary::Hist2D ("acccmbflux-dxdy", -0.5, 238.5, 239, -0.5, 238.5, 239),
		mylibrary::Hist2D ("cos-theta-dxdy", -0.5, 238.5, 239, -0.5, 238.5, 239),
	};

	static const int NUMHIST1D = 3;
	mylibrary::Hist1D hist1d[NUMHIST1D] =
	{
		mylibrary::Hist1D ("tof(all)", -99.5, 99.5, 199),
		mylibrary::Hist1D ("tof(selected)", -99.5, 99.5, 199),
		mylibrary::Hist1D ("cos(haa)", 0.0, 1.0, 100),
	};

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

	// calculate the acceptance tables
	genacc0 (flux0, chsizex, chsizey, ((udistx + udisty) * 0.5) );
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

						double tof = 1000000.0;
						
						std::stringstream ss (recstr);
						std::string recid;
						ss >> recid;
						MUONDAQ::CoinRecord coinrec;
						if ((ss >> coinrec))
						{
							totalcoin += 1.0;
							for (unsigned int u = 0; u < 2; u++)
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
							if (coinrec.xy1cluster (uid1) &&
								coinrec.xy1cluster (uid2))
								histit = true;

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
							double dchx = chx1 - chx2;
							double dchy = chy1 - chy2;

							double rdx = dchx / udistx;
							double rdy = dchy / udisty;

							double phi = atan (rdx);
							double csphi = cos (phi);
							double rdyc = rdy * csphi;
							double csth = rdyc / sqrt(rdyc * rdyc + 1.0);
							double rr2 = (rdx * rdx) + (rdy * rdy);
							double cshf = 1.0 / sqrt(1.0 + rr2);

							hist2d[0].cumulate (dchx, dchy);
							hist2d[1].cumulate (dchx, dchy);
							hist2d[2].cumulate (chx1, chy1);
							hist2d[3].cumulate (chx1, chy1);
							hist2d[4].cumulate (chx2, chy2);
							hist2d[5].cumulate (chx2, chy2);
							hist2d[6].cumulate (phi, csth);
							hist2d[7].cumulate (phi, csth);

							hist1d[1].cumulate (tof);
							hist1d[2].cumulate (cshf);
						}
					}
				}
				duration = duration + (tseclast - tsecfirst);
			}
		}
		liststr.clear();
	}
	// store the acc-tables in out histogram
	for (int iy1 = 0; iy1 < NUM_DYCH; iy1++)
	{
		for (int ix1 = 0; ix1< NUM_DXCH; ix1++)
		{
			hist2d[ 8].cumulate ((double)(ix1), (double)(iy1), acctab[iy1][ix1]);
			hist2d[ 9].cumulate ((double)(ix1), (double)(iy1), accfluxtab[iy1][ix1]);
			hist2d[10].cumulate ((double)(ix1), (double)(iy1), acccmbtab[iy1][ix1]);
			hist2d[11].cumulate ((double)(ix1), (double)(iy1), acccmbfluxtab[iy1][ix1]);
			hist2d[12].cumulate ((double)(ix1), (double)(iy1), csttab[iy1][ix1]);
		}
	}

	mytimer.stop ();

	// Show results.
	std::string outname = std::string("coinana");
	for (int n = 0; n < NUMHIST2D; n++)
	{
		std::stringstream ss;
		ss << outname << "-" << n;
		std::string ofnam;
		ss >> ofnam;
		std::string ofncsv;
		ofncsv = ofnam + ".csv";
		std::ofstream ofcsv (ofncsv.c_str());

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
			hist2d[n].CSVdump (ofcsv);
			ofcsv.close ();
		}
		std::string ofnpng;
		ofnpng = ofnam + ".png";
		std::ofstream ofpng (ofnpng.c_str(), std::ios::binary);
		hist2d [n].PNGdump (ofpng);

	}
//		for (int n = 0; n < NUMHIST1D; n++)
//			hist1d[n].CSVdump (ofcsv);
//	}
	return 0;
}
