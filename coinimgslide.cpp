// coinimgslide.cpp
// g++ -Wall coinimgslide.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp -lz -o coinimgslide

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"
#include "ncpng.h"

static const unsigned int NUM_REC_UNITS = 3;

static const int IMG_XSIZE = 240;
static const int IMG_YSIZE = 240;
double imgbuf[IMG_YSIZE][IMG_XSIZE];

char dumpfnam[128];

void 
fillfnam (int n)
{
	dumpfnam[0] = 'c';
	dumpfnam[5] = (n % 10) + '0';
	n = n / 10;
	dumpfnam[4] = (n % 10) + '0';
	n = n / 10;
	dumpfnam[3] = (n % 10) + '0';
	n = n / 10;
	dumpfnam[2] = (n % 10) + '0';
	n = n / 10;
	dumpfnam[1] = (n % 10) + '0';
	dumpfnam[6] = '.';
	dumpfnam[7] = 'p';
	dumpfnam[8] = 'n';
	dumpfnam[9] = 'g';
	dumpfnam[10] = 0;
}

int main (int argc, char* argv[])
{
	double binrate = 1.0;	// event rate per bin per hour
	double cycletime = 3600.0;

	for (int i = 0; i < IMG_YSIZE; i++)
		for (int j = 0; j < IMG_XSIZE; j++)
			imgbuf[i][j] = 0.0;
	double imgmax = 0.0;

	bool listfilegiven = false;
	std::string listfilename;

	int iarg = 1;
	while (iarg < argc)
	{
		std::string sarg(argv[iarg++]);
		if ((sarg == "-rate") || (sarg == "-cycle"))
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
			if (sarg == "-rate")
				binrate = dvalue;
			else if (sarg == "-cycle")
				cycletime = dvalue;
		}
		else if ((!listfilegiven) && (sarg[0] != '-'))
		{
			listfilename = sarg;
			listfilegiven = true;
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
			<< " [options] list_filename\n"
			<< " Options:\n"
			<< "  -rate value     event rate per sec (for normalization)\n"
			<< "  -cycle value    image dump cycle in sec (default 3600)\n"
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
	int uid2 = 1;

	long totalfile = 0;
	double duration = 0.0;
	double totalcoin = 0.0;

	double tsecnow = 0.0;
	double tsecfirst = 0.0;
	double tseclast = 0.0;

	// read the filename from the list file
	std::string liststr;
	int ndump = 0;
	double nextdump = cycletime;
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
							for (unsigned int u = 0; u < NUM_REC_UNITS; u++)
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
							if (coinrec.xy1cluster(uid1) &&
								coinrec.xy1cluster(uid2))
									histit = true;
						}
						if (histit)
						{
							x1 = coinrec.xpos(uid1);
							x2 = coinrec.xpos(uid2);
							y1 = coinrec.ypos(uid1);
							y2 = coinrec.ypos(uid2);

							double dx = (double)(x1 - x2) / 10.0;
							double dy = (double)(y1 - y2) / 10.0;

							int idx = (int)(dx + 119.5);
							int idy = (int)(dy + 119.5);
							if (idx < 0)
								idx = 0;
							if (idx >= IMG_XSIZE)
								idx = IMG_XSIZE - 1;
							if (idy < 0)
								idy = 0;
							if (idy >= IMG_YSIZE)
								idy = IMG_YSIZE - 1;
							imgbuf[idy][idx] += 1.0;
							if (imgbuf[idy][idx] > imgmax)
								imgmax = imgbuf[idy][idx];
						}
						double tpassed = duration + (tseclast - tsecfirst);
						if (tpassed >= nextdump)
						{
							// *** DUMP IMAGE ***
							double offsimg = 0.0;
							double normimg = (tpassed / 3600.0) * binrate;
							mypnglib::NCPNG pngimg (IMG_XSIZE, IMG_YSIZE, mypnglib::NCPNG::GREYSCALE, 16);
							for (int y = 0; y < IMG_YSIZE; y++)
							{
								for (int x = 0; x < IMG_XSIZE; x++)
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
							fillfnam (ndump);
							std::ofstream ofs (dumpfnam, std::ios::binary);
							pngimg.write (ofs);
							ndump += 1;
							nextdump += cycletime;
						}
					}	// End of COIN record
				}	// End of getline loop
				duration = duration + (tseclast - tsecfirst);
			}
		}
		liststr.clear();
	}

	return 0;
}
