// coinudq.cpp
// g++ -Wall coinudq.cpp coinrecord.cpp hist1d.cpp hist2d.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp mytimer.cpp -lz -o coinudq

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"
#include "hist1d.h"
#include "hist2d.h"
#include "ncpng.h"
#include "mytimer.h"

static const unsigned int NUM_UNITS = 3;

double imgbuf[200][200];

char outfnam[] = "coinudq_detx_x.xxx";

int main (int argc, char* argv[])
{

	int xdir[NUM_UNITS];
	int reqtype = 0;
	int detsys = 1;		// detector system

	bool sys31 = false;

	xdir[0] = -1;
	xdir[1] = -1;
	xdir[2] = 1;

	for (int i = 0; i < 200; i++)
		for (int j = 0; j < 200; j++)
			imgbuf[i][j] = 0.0;
	double imgmax = 0.0;

	std::string listfilename;
	bool listfilegiven = false;

	double tofmin;
	bool tofmingiven = false;
	double tofmax;
	bool tofmaxgiven = false;

	// check the arguments
	int iarg = 1;
	while (iarg < argc)
	{
		std::string sarg(argv[iarg++]);
		if ((sarg == "-tofmin") || (sarg == "-tofmax"))
		{
			if (iarg >= argc)
			{
				std::cerr << "ERROR: " << argv[0]
					<< " value is missing" << std::endl;
				return (-1);
			}
			double dvalue(0.0);
			std::stringstream ss (argv[iarg++]);
			if (!(ss >> dvalue))
			{
				std::cerr << "ERROR: " << argv[0]
					<< " cannot read numerical value" << std::endl;
				return (-1);
			}
			if (sarg == "-tofmin")
			{
				tofmin = dvalue;
				tofmingiven = true;
			}
			else if (sarg == "-tofmax")
			{
				tofmax = dvalue;
				tofmaxgiven = true;
			}
		}
		else if (sarg == "-sys3.1")
			sys31 = true;
		else if ((!listfilegiven) && (sarg[0] != '-'))
		{
			listfilename = sarg;
			listfilegiven = true;
			if (iarg >= argc)
				break;
			std::string sarg2(argv[iarg]);
			if ((sarg2 == "0") || (sarg2 == "1") || (sarg2 == "2") ||
				(sarg2 == "3") || (sarg2 == "4") || (sarg2 == "5"))
			{
				iarg++;
				if (sarg2 == "0")
					reqtype = 0;
				else if (sarg2 == "1")
					reqtype = 1;
				else if (sarg2 == "2")
					reqtype = 2;
				else if (sarg2 == "3")
					reqtype = 3;
				else if (sarg2 == "4")
					reqtype = 4;
				else if (sarg2 == "5")
					reqtype = 5;
			}
		}
		else
		{
			std::cerr << "ERROR: " << argv[0]
				<< " " << sarg << " -- Unknown option"
				<< std::endl;
			return (-1);
		}
	}

	if (!listfilegiven)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " [options] list_filename [0|1|2|3]"
			<< " Options:\n"
			<< "  -tofmin value    TOF minimum value\n"
			<< "  -tofmax value    TOF maximum value\n"
			<< std::endl;
		return (-1);
	}

	// open the list file
	std::ifstream ifl (listfilename.c_str());
	if (!ifl)
	{
		// file open error
		std::cerr
			<< "ERROR: list file ("
			<< listfilename
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	int uid1 = 0;
	int uid2 = 2;
	if ((reqtype == 0) || (reqtype == 3))
	{
		uid1 = 0;
		uid2 = 1;
	}
	else if ((reqtype == 1) || (reqtype == 4))
	{
		uid1 = 0;
		uid2 = 2;
	}
	else if ((reqtype == 2) || (reqtype == 5))
	{
		uid1 = 1;
		uid2 = 2;
	}

	static const int NUMHIST2D = 4;
	mylibrary::Hist2D hist2d[NUMHIST2D] =
	{
		mylibrary::Hist2D ("dxdy-UU", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
		mylibrary::Hist2D ("dxdy-UD", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
		mylibrary::Hist2D ("dxdy-DU", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
		mylibrary::Hist2D ("dxdy-DD", -99.5, 99.5, 199, -99.5, 99.5, 199, true),
	};

	static const int NUMHIST1D = 4;
	mylibrary::Hist1D hist1d[NUMHIST1D] =
	{
		mylibrary::Hist1D("tof-UU", -99.5, 99.5, 199),
		mylibrary::Hist1D("tof-UD", -99.5, 99.5, 199),
		mylibrary::Hist1D("tof-DU", -99.5, 99.5, 199),
		mylibrary::Hist1D("tof-DD", -99.5, 99.5, 199),
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
						int x1 = -1;
						int x2 = -1;
						int y1 = -1;
						int y2 = -1;
						
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
								{
									int ych0 = coinrec.ypos(0);
									int ych1 = coinrec.ypos(1);
									if ( ((ych0 >= 500) && (ych1 >= 500)) ||
										((ych0 < 500) && (ych1 < 500)) )
										histit = true;
								}
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
							x1 = coinrec.xpos(uid1, xdir[uid1]);
							x2 = coinrec.xpos(uid2, xdir[uid2]);
							y1 = coinrec.ypos(uid1);
							y2 = coinrec.ypos(uid2);

							int iud = 0;
							if (y1 >= 500)
								iud += 2;
							if (y2 >= 500)
								iud += 1;

							double tu1 = coinrec.microsec(uid1);
							double tn1 = coinrec.nanosec(uid1);
							double tu2 = coinrec.microsec(uid2);
							double tn2 = coinrec.nanosec(uid2);

							double tof = (tu1 - tu2) * 1000.0 + (tn1 - tn2);
							double dx = (double)(x1 - x2) / 10.0;
							double dy = (double)(y1 - y2) / 10.0;

							bool tofok = true;
							if (tofmingiven && (tof < tofmin))
								tofok = false;
							if (tofmaxgiven && (tof > tofmax))
								tofok = false;

							if (tofok)
							{
								hist2d[iud].cumulate (dx, dy);
								hist1d[iud].cumulate (tof);

								int idx = (int)(dx + 99.5);
								int idy = (int)(dy + 99.5);
								if (idx < 0)
									idx = 0;
								if (idx > 199)
									idx = 199;
								if (idy < 0)
									idy = 0;
								if (idy > 199)
									idy = 199;
								imgbuf[idy][idx] += 1.0;
								if (imgbuf[idy][idx] > imgmax)
									imgmax = imgbuf[idy][idx];
							}
						}
					}
					else if ((recstr.size() > 4) &&
							(recstr.substr(0,3) == "TWI"))
					{
						char c = recstr[3];
						if ((c == '1') || (c == '2') || (c == '3'))
						{
							detsys = 1;
							xdir[0] = -1;
							xdir[1] = -1;
							xdir[2] = 1;
						}
						else if ((c == '4') || (c == '5') || (c == '6'))
						{
							detsys = 2;
							xdir[0] = -1;
							xdir[1] = -1;
							xdir[2] = 1;
						}
						else if ((c == '7') || (c == '8') || (c == '9'))
						{
							detsys = 3;
							xdir[0] = -1;
							xdir[1] = -1;
							xdir[2] = -1;
							if (sys31)
							{
								xdir[0] = 1;
								xdir[1] = 1;
								xdir[2] = -1;
							}
						}
					}
				}
				duration = duration + (tseclast - tsecfirst);
			}
		}
		liststr.clear();
	}
	mytimer.stop ();

	if (tofmingiven || tofmaxgiven)
	{
		outfnam[4] = 't';
		outfnam[5] = 'o';
		outfnam[6] = 'f';
	}

	outfnam[11] = (char)(detsys + '0');
	outfnam[13] = (char)(reqtype + '0');
	outfnam[15] = 'c';
	outfnam[16] = 's';
	outfnam[17] = 'v';

	// Show results.
	std::ofstream ofcsv (outfnam);
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
			<< ",\"TOF(nsec)min,max\",";
		if (tofmingiven)
			ofcsv << tofmin;
		else
			ofcsv << "none";
		ofcsv << ',';
		if (tofmaxgiven)
			ofcsv << tofmax;
		else
			ofcsv << "none";
		ofcsv << std::endl;
		for (int nh = 0; nh < NUMHIST2D; nh++)
			hist2d[nh].dump (ofcsv);
		for (int nh = 0; nh < NUMHIST1D; nh++)
			hist1d[nh].dump (ofcsv);
		ofcsv.close ();
	}

	// make image
	double offsimg = 0.0;
	double normimg = 1.0;
	if (imgmax > normimg)
		normimg = imgmax;
	mypnglib::NCPNG pngimg (200, 200, mypnglib::NCPNG::GREYSCALE, 16);
	for (int y = 0; y < 200; y++)
	{
		for (int x = 0; x < 200; x++)
		{
			double v = (imgbuf[y][x] - offsimg) / normimg;
			if (v > 1.0)
				v = 1.0;
			else if (v < 0.0)
				v = 0.0;
			mypnglib::NCPNG::GREY_PIXEL pixel(v);
			pngimg.put (x, y, pixel);
		}
	}

	outfnam[15] = 'p';
	outfnam[16] = 'n';
	outfnam[17] = 'g';

	std::ofstream ofs (outfnam, std::ios::binary);
	pngimg.write (ofs);

	return 0;
}
