// coinimgslide.cpp
// g++ -Wall coinimgslide.cpp inoutlist.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp -lz -o coinimgslide

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include "inoutlist.h"
#include "gzfstream.h"
#include "coinrecord.h"
#include "ncpng.h"

static const unsigned int NUM_REC_UNITS = 3;

static const int NUM_UNITS = 2;
static const int NUM_X_CHANNELS = 120;
static const int NUM_Y_CHANNELS = 120;

static const int NUM_PLANES = (NUM_UNITS * 2);
static const int NUM_X_PIXELS = (NUM_X_CHANNELS * 2 - 1);
static const int NUM_Y_PIXELS = (NUM_Y_CHANNELS * 2 - 1);

// dead channel option
bool g_deadch;
// acc option
int g_accopt;

// gap
double g_xygap [NUM_UNITS];
double g_xxgap [NUM_UNITS];
double g_yygap [NUM_UNITS];

// XY position differences
double g_dx [NUM_X_PIXELS];
double g_dy [NUM_Y_PIXELS];

// XY position differences in device coordinate
double g_ddx [NUM_X_PIXELS];
double g_ddy [NUM_Y_PIXELS];

double g_xsize;
double g_ysize;

double g_dxsize;
double g_dysize;

// Position shifts in the unit
double g_x_xshift;	// xshift of the X-plane
double g_x_yshift;	// yshift of the X-plane
double g_y_xshift;	// xshift of the Y-plane
double g_y_yshift;	// xshift of the Y-plane

// Unit position difference
double g_xdiff;
double g_ydiff;

// Unit distance
double g_xdist;
double g_ydist;

double g_xpos [NUM_PLANES];
double g_ypos [NUM_PLANES];
double g_zpos [NUM_PLANES];

double g_xmax [NUM_PLANES];
double g_xmin [NUM_PLANES];
double g_ymax [NUM_PLANES];
double g_ymin [NUM_PLANES];

double g_effxlen [NUM_X_PIXELS];
double g_effylen [NUM_Y_PIXELS];

static const int IMG_XSIZE = (NUM_X_CHANNELS * 2);
static const int IMG_YSIZE = (NUM_Y_CHANNELS * 2);
double imgbuf[IMG_YSIZE][IMG_XSIZE];
double acccmbtab[IMG_YSIZE][IMG_XSIZE];
double acccmbfluxtab[IMG_YSIZE][IMG_XSIZE];

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

void genacc0 (double flux0, double chsizex, double chsizey, double udist)
{
	double rchsizex = chsizex / udist;
	double rchsizey = chsizey / udist;
	double dsize = chsizex * rchsizex * chsizey * rchsizey;

	for (int iy1 = 0; iy1 < NUM_Y_PIXELS; iy1++)
	{
		double dcy = double(iy1) - double(NUM_Y_CHANNELS - 1);
		double cny = double(NUM_Y_CHANNELS) - fabs(dcy);
		double dy = dcy * rchsizey;
		for (int ix1 = 0; ix1< NUM_X_PIXELS; ix1++)
		{
			double dcx = double(ix1) - double(NUM_X_CHANNELS - 1);
			double cnx = double(NUM_X_CHANNELS) - fabs(dcx);
			double dx = dcx * rchsizex;
			double cs2 = 1.0 / ((dx * dx) + (dy * dy) + 1.0);
			double wt = cs2 * cs2 * dsize;
//			csttab[iy1][ix1] = sqrt(cs2);
//			acctab[iy1][ix1] = wt;
//			accfluxtab[iy1][ix1] = (wt * flux0 * cs2);	// assuming that the flux is propotional to cs2
			acccmbtab[iy1][ix1] = wt * cnx * cny;
			acccmbfluxtab[iy1][ix1] = acccmbtab[iy1][ix1] * flux0 * cs2;	// assuming that the flux is propotional to cs2
		}
	}
}

void setup ()
{
	// order YX-YX from top to bottom
	//       01 23

	g_xsize = 120.0;	// in cm
	g_ysize = 120.0;	// in cm

	g_dxsize = g_xsize / double (NUM_X_CHANNELS);
	g_dysize = g_ysize / double (NUM_Y_CHANNELS);

	g_xdiff = 0.0;
	g_ydiff = 0.0;

//	g_xdist = 150.0;
//	g_ydist = 150.0;
	for (int n = 0; n < NUM_UNITS; n++)
	{
		g_xygap [n] = 4.2;
		g_xxgap [n] = 0.6;
		g_yygap [n] = 0.6;
	}

	g_xpos [2] = 0.0 + g_y_xshift;
	g_xpos [3] = 0.0 + g_x_xshift;
	g_xpos [0] = g_xpos[2] + g_xdiff;
	g_xpos [1] = g_xpos[3] + g_xdiff;

	g_ypos [2] = 0.0 + g_y_yshift;
	g_ypos [3] = 0.0 + g_x_yshift;
	g_ypos [0] = g_ypos[2] + g_ydiff;
	g_ypos [1] = g_ypos[3] + g_ydiff;

	g_zpos [0] = 0.0;
	g_zpos [1] = g_zpos [0] + g_xygap [0];
	g_zpos [2] = g_zpos [0] + g_ydist;
	g_zpos [3] = g_zpos [1] + g_xdist;
	for (int n = 0; n < NUM_PLANES; n++)
	{
		g_xmax [n] = g_xsize * 0.5 + g_xpos [n];
		g_xmin [n] = g_xmax [n] - g_xsize;
		g_ymax [n] = g_ysize * 0.5 + g_ypos [n];
		g_ymin [n] = g_ymax [n] - g_ysize;
	}
}

void calcgeom ()
{

	for (int n = 0; n < NUM_X_PIXELS; n++)
	{
		g_ddx [n] = (g_dxsize * double (n + 1)) - g_xsize;
		g_dx[n] = g_ddx[n] + g_xdiff;
	}

	// Y direction is reverse order
	for (int n = 0; n < NUM_Y_PIXELS; n++)
	{
		g_ddy [n] = g_ysize - (g_dysize * double (n + 1));
		g_dy[n] = -g_ddy [n] + g_ydiff;
	}
}

double EffXlen (int baseid, double rx)
{
	mylibrary::InOutList inoutlist;

	double xmax = g_xmax [baseid];
	double xmin = g_xmin [baseid];
	inoutlist.merge (mylibrary::InOut (xmin, xmax));
	for (int n = 0; n < NUM_PLANES; n++)
	{
		// get the n-th center position on the [baseid] plane
		double dx = (rx * (g_zpos[n] - g_zpos[baseid]));
		inoutlist.remove (mylibrary::InOut (-10000.0, (dx + g_xmin[n])));
		inoutlist.remove (mylibrary::InOut ((dx + g_xmax[n]), 10000.0) );
	}
	double t = inoutlist.totallength ();
	if (t <= 0.0)
		return 0.0;
	return t;
}

double EffYlen (int baseid, double ry)
{
	mylibrary::InOutList inoutlist;

	double ymax = g_ymax [baseid];
	double ymin = g_ymin [baseid];
	inoutlist.merge (mylibrary::InOut (ymin, ymax));
	for (int n = 0; n < NUM_PLANES; n++)
	{
		double dy = (ry * (g_zpos[n] - g_zpos[baseid]));
		inoutlist.remove (mylibrary::InOut (-10000.0, (dy + g_ymin[n])));
		inoutlist.remove (mylibrary::InOut ((dy + g_ymax[n]), 10000.0) );
	}

/// Test for dead channels
/// At present, we have dead channels on the Y[baseid] plane.
	if (g_deadch)
	{
		//
		// ch=39
		int nch = 39;
		ymin = double (nch) * g_dysize + g_ymin [baseid];
		ymax = ymin + g_dysize;
		inoutlist.remove (mylibrary::InOut (ymin, ymax));
		//
		// ch=58-63
		nch = 58;
		ymin = double (nch) * g_dysize + g_ymin [baseid];
		nch = 63;
		ymax = double (nch + 1) * g_dysize + g_ymin [baseid];
		inoutlist.remove (mylibrary::InOut (ymin, ymax));
		//
		// ch=105
		nch = 105;
		ymin = double (nch) * g_dysize + g_ymin [baseid];
		ymax = ymin + g_dysize;
		inoutlist.remove (mylibrary::InOut (ymin, ymax));
	}
/// Test END

	double t = inoutlist.totallength ();
	if (t <= 0.0)
		return 0.0;
	return t;
}

void
genacc (double flux0)
{
	setup ();
	calcgeom ();

	for (int ix = 0; ix < NUM_X_PIXELS; ix++)
	{
		double dx = g_dx[ix];
		double rx = dx / g_xdist;
		g_effxlen [ix] = EffXlen (3, rx);
	}

	for (int iy = 0; iy < NUM_Y_PIXELS; iy++)
	{
		double dy = g_dy[iy];
		double ry = dy / g_ydist;
		g_effylen [iy] = EffYlen (2, ry);
	}

	double dxdy = g_dxsize / g_xdist * g_dysize / g_ydist;

	for (int iy = 0; iy < NUM_Y_PIXELS; iy++)
	{
		double dy = g_dy[iy];
		double ry = dy / g_ydist;
		// revert Y
		int iyrev = NUM_Y_PIXELS - 1 - iy;
		for (int ix = 0; ix < NUM_X_PIXELS; ix++)
		{
			double dx = g_dx[ix];
			double rx = dx / g_xdist;
			double cs2t = 1.0 / (1.0 + (rx * rx) + (ry * ry));
			double dsdo = cs2t * cs2t * dxdy * g_effxlen [ix] * g_effylen [iy];
//			double flx = Jokisch::jokisch_integral (g_pcut, fabs (cst));
			acccmbtab[iyrev][ix] = dsdo;
			acccmbfluxtab[iyrev][ix] = acccmbtab[iyrev][ix] * flux0 * cs2t;	// assuming that the flux is propotional to cs2
		}
	}
//@ for (int iy = 0; iy < NUM_Y_PIXELS; iy++)
//@ {
//@ 	std::cout << iy << ",";
//@ 	for (int ix = 0; ix < NUM_X_PIXELS; ix++)
//@ 	{
//@ 		std::cout << ',' << acccmbtab[iy][ix];
//@ 	}
//@ 	std::cout << std::endl;
//@ }
}

int main (int argc, char* argv[])
{
	double binrate = 1.0;	// event rate per bin per hour
	double cycletime = 3600.0;

	double chxsize = 1.0;
	double chysize = 1.0;
	double udist = 100.0;
	double flux0 = 70.0;

	g_xdist = udist;
	g_ydist = udist;
	g_x_xshift = 0.0;
	g_x_yshift = 0.0;
	g_y_xshift = 1.0;
	g_y_yshift = -2.0;
	g_deadch = true;	// dead channel option
	g_accopt = 2;		// acc option

	for (int i = 0; i < IMG_YSIZE; i++)
	{
		for (int j = 0; j < IMG_XSIZE; j++)
		{
			imgbuf[i][j] = 0.0;
			acccmbtab[i][j] = 0.0;
			acccmbfluxtab[i][j] = 0.0;
		}
	}
	double imgmax = 0.0;

	bool listfilegiven = false;
	std::string listfilename;

	int iarg = 1;
	while (iarg < argc)
	{
		std::string sarg(argv[iarg++]);
		if ((sarg == "-rate") || (sarg == "-cycle") || (sarg == "-udist"))
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
			else if (sarg == "-udist")
			{
				udist = dvalue;
				g_xdist = udist;
				g_ydist = udist;
			}
		}
		else if (sarg == "-raw")
			g_accopt = 0;
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

	// generate acceptance (simple version)
	if (g_accopt == 1)
		genacc0 (flux0, chxsize, chysize, udist);
	else if (g_accopt == 2)
		genacc (flux0);


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
									if (g_accopt > 0)
									{
										double acc = acccmbtab[y][x];
										if (acc <= 0.0)
											v = 0.0;
										else
											v = v / acc;
									}
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
