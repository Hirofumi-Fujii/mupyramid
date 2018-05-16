// coinimgs.cpp
// g++ -Wall coinimgs.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp mytimer.cpp -lz -o coinimgs
//
// 21-Dec-2015 modified for Pyramid system
//
// 31-Mar-2015 modified for detector3
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "gzfstream.h"
#include "coinrecord.h"
#include "ncpng.h"
#include "mytimer.h"

static const unsigned int NUM_REC_UNITS = 3;

static const int NUM_L_IMAGES = 2;
static const int NUM_L_XBINS = 240;
static const int NUM_L_YBINS = 240;

static const int NUM_S_IMAGES = 2;
static const int NUM_S_XBINS = 120;
static const int NUM_S_YBINS = 120;

static const int IMG_WIDTH = 480;
static const int IMG_HEIGHT = 640;

double cntlmax[NUM_L_IMAGES];
double cntsmax[NUM_S_IMAGES];

double cntlbuf[NUM_L_IMAGES][NUM_L_YBINS][NUM_L_XBINS];
double cntsbuf[NUM_S_IMAGES][NUM_S_YBINS][NUM_S_XBINS];

static const int NUM_XCH = 120;
static const int NUM_YCH = 120;

double accbuf[NUM_L_YBINS][NUM_L_XBINS];

double udistx = 150.0;
double udisty = 150.0;
double chsizex = 1.0;
double chsizey = 1.0;
double accmin = 0.001;

static const char header[4] =
{
	'I', 'M', 'G', 'P',
};

void genacc0 ()
{
	double dz = (udistx + udisty) * 0.5;
	double rchsizex = chsizex / dz;
	double rchsizey = chsizey / dz;
	for (int iy1 = 0; iy1 < NUM_L_YBINS; iy1++)
	{
		double dcy = double(iy1) - double(NUM_YCH - 1);
		double cny = double(NUM_YCH) - fabs(dcy);
		double dy = dcy * rchsizey;
		for (int ix1 = 0; ix1< NUM_L_XBINS; ix1++)
		{
			double dcx = double(ix1) - double(NUM_XCH - 1);
			double cnx = double(NUM_XCH) - fabs(dcx);
			double dx = dcx * rchsizex;
			double cs2 = 1.0 / ((dx * dx) + (dy * dy) + 1.0);
			double wt = cs2 * cs2 * cs2 * cnx * cny;	// multipy flux=cs2
			accbuf[iy1][ix1] = wt;
		}
	}
}

int main (int argc, char* argv[])
{
	bool newimage = false;
	bool saveimage = true;

	for (int ny = 0; ny < NUM_L_YBINS; ny++)
		for (int nx = 0; nx < NUM_L_XBINS; nx++)
			accbuf[ny][nx] = 0.0;
	genacc0 ();

	for (int n = 0; n < NUM_S_IMAGES; n++)
	{
		cntsmax[n] = 0.0;
		for (int j = 0; j < NUM_S_YBINS; j++)
			for (int i = 0; i < NUM_S_XBINS; i++)
				cntsbuf[n][j][i] = 0.0;
	}

	for (int n = 0; n < NUM_L_IMAGES; n++)
	{
		cntlmax[n] = 0.0;
		for (int j = 0; j < NUM_L_YBINS; j++)
			for (int i = 0; i < NUM_L_XBINS; i++)
				cntlbuf[n][j][i] = 0.0;
	}

	long totalfile = 0;
	long detectsys = 0;	// detector system, 0 is unknown
	double duration = 0.0;
	double totalcoin = 0.0;

	// check the arguments
	std::string listfilename;
	bool listgiven = false;
	int iarg = 1;
	while (iarg < argc)
	{
		std::string word (argv[iarg++]);
		if (word == "-new")
			newimage = true;
		else if (word == "-add")
			newimage = false;
		else if (word == "-save")
			saveimage = true;
		else if (word == "-nosave")
			saveimage = false;
		else if ((word.size() > 0) && (word[0] == '-'))
		{
			std::cerr << "ERROR: " << argv[0]
				<< " unknown option ("
				<< word
				<< ")"
				<< std::endl;
			listgiven = false;
			break;
		}
		else
		{
			listfilename = word;
			listgiven = true;
		}
	}
		
	if (!listgiven)
	{
		// show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " [options] list_filename"
			<< std::endl;
		std::cout
			<< "options:\n"
			<< "  -new     create new images (does not read the saved images)\n"
			<< "  -nosave  does not save the created images (only creates \'coinimgs.png\')\n"
			<< std::endl;
		return (-1);
	}

	if (!newimage)
	{
		newimage = true;	// in the case of read error.
		// open the saved file
		std::ifstream ifs ("coinimgs.sav", std::ios::binary);
		if (ifs)
		{
			char ch[4];
			ifs.read (ch, 4);
			if ((ch[0] == header[0]) && (ch[1] == header[1]) &&
				(ch[2] == header[2]) && (ch[3] == header[3]))
			{
				int np[6];
				ifs.read ((char*)(np), sizeof(int) * 6);
				ifs.read ((char*)(cntsmax), sizeof(double) * NUM_S_IMAGES);
				ifs.read ((char*)(cntsbuf), sizeof(double) * NUM_S_IMAGES * NUM_S_YBINS * NUM_S_XBINS);
				ifs.read ((char*)(cntlmax), sizeof(double) * NUM_L_IMAGES);
				ifs.read ((char*)(cntlbuf), sizeof(double) * NUM_L_IMAGES * NUM_L_YBINS * NUM_L_XBINS);
				int isize[2];
				ifs.read ((char*)(isize), sizeof(int) * 2);
				long linfo[4];
				ifs.read ((char*)(linfo), sizeof(long) * 4);
				double dinfo[4];
				ifs.read ((char*)(dinfo), sizeof(double) * 4);
				if (ifs)
				{
					totalfile = linfo[0];
					detectsys = linfo[1];
					duration = dinfo[0];
					totalcoin = dinfo[1];
					newimage = false;	// all data are successfully restored.
				}
			}
			else
			{
				std::cerr << "ERROR: " << argv[0]
					<< " data read error in (coinimgs.sav). New images will be created."
					<< std::endl;
			}
		}
		else
		{
			std::cerr << "ERROR: " << argv[0]
				<< " file open error (coinimgs.sav). New images will be created."
				<< std::endl;
		}
	}

	if (newimage)
	{
		for (int n = 0; n < NUM_S_IMAGES; n++)
		{
			cntsmax[n] = 0.0;
			for (int j = 0; j < NUM_S_YBINS; j++)
				for (int i = 0; i < NUM_S_XBINS; i++)
					cntsbuf[n][j][i] = 0.0;
		}
		for (int n = 0; n < NUM_L_IMAGES; n++)
		{
			cntlmax[n] = 0.0;
			for (int j = 0; j < NUM_L_YBINS; j++)
				for (int i = 0; i < NUM_L_XBINS; i++)
					cntlbuf[n][j][i] = 0.0;
		}
		totalfile = 0;
		duration = 0.0;
		totalcoin = 0.0;
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
						int x1 = -1;
						int x2 = -1;
						int y1 = -1;
						int y2 = -1;
						double dx = 0.0;
						double dy = 0.0;
						
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
							// Single XY coincidence
							for (int n = 0; n < 2; n++)
							{
								if (coinrec.xy1cluster(n))
								{
									x1 = coinrec.xpos(n) / 10.0;
									y1 = coinrec.ypos(n) / 10.0;
									if ((x1 >= 0.0) && (y1 >= 0.0))
									{
										int nx = (int)x1;
										int ny = (int)y1;
										if ((nx < NUM_S_XBINS) &&
											(ny < NUM_S_YBINS))
										{
											cntsbuf[n][ny][nx] += 1.0;
											if (cntsbuf[n][ny][nx] > cntsmax[n])
												cntsmax[n] = cntsbuf[n][ny][nx];
										}
									}
								}
							}
								
							// Two-fold coincidence
							if (coinrec.xy1cluster(0) &&
								coinrec.xy1cluster(1))
							{
								x1 = coinrec.xpos(0);
								x2 = coinrec.xpos(1);
								y1 = coinrec.ypos(0);
								y2 = coinrec.ypos(1);
								dx = (double)(x1 - x2) / 10.0 + 119.5;
								dy = (double)(y1 - y2) / 10.0 + 119.5;
								if ((dx > 0.0) && (dy > 0.0))
								{
									int nx = (int)(dx);
									int ny = (int)(dy);
									if ((nx < NUM_L_XBINS) &&
										(ny < NUM_L_YBINS))
									{
										cntlbuf[0][ny][nx] += 1.0;
										if (cntlbuf[0][ny][nx] > cntlmax[0])
											cntlmax[0] = cntlbuf[0][ny][nx];
									}
								}
							}
						}							
						// end of "COIN" record processing
					}
					else if ((recstr.size() > 4) &&
							(recstr.substr(0,3) == "TWI"))
					{
						char c = recstr[3];
						if ((c == '1') || (c == '2') || (c == '3'))
						{
							detectsys = 1;
						}
						else if ((c == '4') || (c == '5') || (c == '6'))
						{
							detectsys = 2;
						}
						else if ((c == '7') || (c == '8') || (c == '9'))
						{
							detectsys = 3;
						}
					// end of "TWIn" record processing
					}
				// end of while(getline(..)) loop
				}
				duration = duration + (tseclast - tsecfirst);
			}
		}
		liststr.clear();
	}

	double evtmax = 0.0;
	for (int y = 0; y < NUM_L_YBINS; y++)
	{
		for (int x = 0; x < NUM_L_XBINS; x++)
		{
			double acc = duration / 10000.0 * accbuf[y][x];
//			if (acc > accmin)
			if (acc > 0.0)
			{
				cntlbuf[1][y][x] = cntlbuf[0][y][x] / acc;
				if (cntlbuf[0][y][x] > evtmax)
				{
					cntlmax[1] = cntlbuf[1][y][x];
					evtmax = cntlbuf[0][y][x];
				}
			}
		}
	}

	if (saveimage)
	{
		std::ofstream ofs ("coinimgs.sav", std::ios::binary);
		if (ofs)
		{
			ofs.write (header, 4);	// write magic
			int np[6];
			np[0] = NUM_S_IMAGES;
			np[1] = NUM_S_YBINS;
			np[2] = NUM_S_XBINS;
			np[3] = NUM_L_IMAGES;
			np[4] = NUM_L_YBINS;
			np[5] = NUM_L_XBINS;
			ofs.write ((const char*)(np), sizeof(int) * 6);
			ofs.write ((const char*)(cntsmax), sizeof(double) * NUM_S_IMAGES);
			ofs.write ((const char*)(cntsbuf), sizeof(double) * NUM_S_IMAGES * NUM_S_YBINS * NUM_S_XBINS);
			ofs.write ((const char*)(cntlmax), sizeof(double) * NUM_L_IMAGES);
			ofs.write ((const char*)(cntlbuf), sizeof(double) * NUM_L_IMAGES * NUM_L_YBINS * NUM_L_XBINS);
			// information size
			int isize[2];
			isize[0] = 4;	// long information
			isize[1] = 4;	// double information
			ofs.write ((const char*)(isize), sizeof(int) * 2);
			long linfo[4];
			linfo[0] = totalfile;
			linfo[1] = detectsys;
			linfo[2] = 0;	// dummy
			linfo[3] = 0;	// dummy
			ofs.write ((const char*)(linfo), sizeof(long) * 4);
			double dinfo[4];
			dinfo[0] = duration;
			dinfo[1] = totalcoin;
			dinfo[2] = 0.0;	// dummy
			dinfo[3] = 0.0;	// dummy
			ofs.write ((const char*)(dinfo), sizeof(double) * 4);
		}
	}

	mytimer.stop ();

	// make image
	mypnglib::NCPNG pngimg (IMG_WIDTH, IMG_HEIGHT, mypnglib::NCPNG::GREYSCALE, 16);
	mypnglib::NCPNG::GREY_PIXEL bgpixel(1.0);	// background
	for (int h = 0; h < IMG_HEIGHT; h++)
		for (int w = 0; w < IMG_WIDTH; w++)
			pngimg.put (w, h, bgpixel);

	for (int nimg = 0; nimg < NUM_S_IMAGES; nimg++)
	{
		double offsimg = 0.0;
		double normimg = 1.0;
		if (cntsmax[nimg] > 0.0)
			normimg = cntsmax[nimg];
		int xoffs = nimg * 200 + 80;
		int yoffs = 10;
		for (int y = 0; y < NUM_S_YBINS; y++)
		{
			for (int x = 0; x < NUM_S_XBINS; x++)
			{
				double v = (cntsbuf[nimg][y][x] - offsimg) / normimg;
				if (v > 1.0)
					v = 1.0;
				else if (v < 0.0)
					v = 0.0;
				mypnglib::NCPNG::GREY_PIXEL pixel(v);
				pngimg.put ((x + xoffs), (y + yoffs), pixel);
			}
		}
	}

	for (int nimg = 0; nimg < NUM_L_IMAGES; nimg++)
	{
		double offsimg = 0.0;
		double normimg = 1.0;
		if (cntlmax[nimg] > 0.0)
			normimg = cntlmax[nimg];
		int xoffs = 120;
		int yoffs = nimg * 250 + 140;
		for (int y = 0; y < NUM_L_YBINS; y++)
		{
			for (int x = 0; x < NUM_L_XBINS; x++)
			{
				double v = (cntlbuf[nimg][y][x] - offsimg) / normimg;
				if (v > 1.0)
					v = 1.0;
				else if (v < 0.0)
					v = 0.0;
				mypnglib::NCPNG::GREY_PIXEL pixel(v);
				pngimg.put ((x + xoffs), (y + yoffs), pixel);
			}
		}
	}
	std::ofstream ofs ("coinimgs.png", std::ios::binary);
	pngimg.write (ofs);

	return 0;
}
