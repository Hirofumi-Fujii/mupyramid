// coinimgslide.cpp
// g++ -Wall coinimgslide.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp -lz -o coinimgslide.exe

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"
#include "ncpng.h"

static const unsigned int NUM_UNITS = 3;
// static const char* cointype[4] = { "01*", "0*2", "*12", "012" };
static const int xdir[NUM_UNITS] = { -1, -1, 1 };

double imgbuf[200][200];
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

	int reqtype = 0;
	int ndumpcycle = 2;
	double binrate = 1.0;	// event rate per bin per hour
	double cycletime = 3600.0;

	for (int i = 0; i < 200; i++)
		for (int j = 0; j < 200; j++)
			imgbuf[i][j] = 0.0;
	double imgmax = 0.0;

	// check the arguments
	if (argc < 2)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " list_filename [0|1|2|3]"
			<< std::endl;
		return (-1);
	}
	if (argc == 3)
	{
		char* cs = argv[2];
		int n = int(cs[0] - '0');
		if ((n >= 0) && (n <= 3))
			reqtype = n;
		else
		{
			std::cerr << "coincidence type must be one of 0,1,2,3"
				<< std::endl;
			return (-1);
		}
	}

	// open the list file
	std::ifstream ifl (argv[1]);
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

	int uid1 = 0;
	int uid2 = 2;
	if (reqtype == 0)
	{
		uid1 = 0;
		uid2 = 1;
	}
	else if (reqtype == 1)
	{
		uid1 = 0;
		uid2 = 2;
	}
	else if (reqtype == 2)
	{
		uid1 = 1;
		uid2 = 2;
	}
	else if (reqtype == 3)
	{
		uid1 = 0;
		uid2 = 2;
	}

	long totalfile = 0;
	double duration = 0.0;
	double totalcoin = 0.0;

	double tsecnow = 0.0;
	double tsecfirst = 0.0;
	double tseclast = 0.0;

	// read the filename from the list file
	std::string liststr;
	int nproc = 0;
	int ndump = 0;
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
							if (reqtype == 3)
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
							x1 = coinrec.xpos(uid1, xdir[uid1]);
							x2 = coinrec.xpos(uid2, xdir[uid2]);
							y1 = coinrec.ypos(uid1);
							y2 = coinrec.ypos(uid2);

							double dx = (double)(x1 - x2) / 10.0;
							double dy = (double)(y1 - y2) / 10.0;

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
					}	// End of COIN record
				}	// End of getline loop
				duration = duration + (tseclast - tsecfirst);
				nproc += 1;
				if (nproc >= ndumpcycle)
				{
					// *** DUMP IMAGE ***
					double offsimg = 0.0;
					double normimg = (duration / 3600.0) * binrate;
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
					fillfnam (ndump);
					std::ofstream ofs (dumpfnam, std::ios::binary);
					pngimg.write (ofs);
					ndump += 1;
					nproc = 0;
				}
			}
		}
		liststr.clear();
	}

	return 0;
}
