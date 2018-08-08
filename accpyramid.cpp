// accpyramid.cpp
// g++ -Wall accpyramid.cpp inoutlist.cpp hist2d.cpp ncpng.cpp jokisch.cpp -o accpyramid
//
// 2017-04-13 option '-yxshift' and '-yyshift' are added
// 2017-02-03 option '-udist' is added
//
// dead channels (count from 0)
//   Unit0 X (none)
//   Unit0 Y 39, 58-63, 105
//   Unit1 X (none)
//   Unit1 Y (none)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "inoutlist.h"
#include "hist2d.h"
#include "jokisch.h"

static const int NUM_UNITS = 2;
static const int NUM_X_CHANNELS = 120;
static const int NUM_Y_CHANNELS = 120;

static const int NUM_PLANES = (NUM_UNITS * 2);
static const int NUM_X_PIXELS = (NUM_X_CHANNELS * 2 - 1);
static const int NUM_Y_PIXELS = (NUM_Y_CHANNELS * 2 - 1);

// dead channel option
bool g_deadch;

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

double g_pcut;

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

int main (int argc, char* argv[])
{
	g_pcut = 0.0;	// pcut value in GeV/c
	g_xdist = 150.0;
	g_ydist = 150.0;
	g_x_xshift = 0.0;
	g_x_yshift = 0.0;
	g_y_xshift = 1.0;
	g_y_yshift = -2.0;
	g_deadch = true;	// dead channel option

	int iarg = 1;
	while (iarg < argc)
	{
		std::string sword (argv[iarg++]);
		if ((sword == "-pcut") || (sword == "-udist")
			|| (sword == "-yxshift") || (sword == "-yyshift") )
		{
			if (iarg < argc)
			{
				std::stringstream ss;
				ss << argv[iarg++];
				double dv;
				if (ss >> dv)
				{
					if (sword == "-pcut")
						g_pcut = dv;
					else if (sword == "-udist")
						g_xdist = g_ydist = dv;
					else if (sword == "-yxshift")
						g_y_xshift = dv;
					else if (sword == "-yyshift")
						g_y_yshift = dv;
				}
				else
				{
					std::cerr << "ERROR: numerial value read error."
						<< std::endl;
					return (-1);
				}
			}
			else
			{
				std::cerr << "ERROR: numerial value must be given."
					<< std::endl;
				return (-1);
			}
		}
		else if (sword == "-nodead")
			g_deadch = false;
		else if (sword == "-dead")
			g_deadch = true;
		else
		{
			std::cerr << "ERROR: no such option (" << sword << ")."
				<< std::endl;
			std::cerr << "Usage: " << argv[0] << " [options]"
				<< "\n options"
				<< "\n\t -pcut p    (cut-off momentum in GeV/c)"
				<< "\n\t -udist d   (unit distance in cm)"
				<< "\n\t -yxshift d (shift in x-dir of the Y-plane)"
				<< "\n\t -yyshift d (shift in y-dir of the Y-pnane)"
				<< "\n\t -ndead     (disable dead-channel correction)"
				<< "\n\t -dead      (enable dead-channel correction)"
				<< std::endl;
			return (-1);
		}
	}

	setup ();
	calcgeom ();

	// Histograms
	static const int numhist2d = 4;
	mylibrary::Hist2D hist2d [numhist2d] =
	{
		mylibrary::Hist2D ("dsdo sr*cm^2",
				- (g_xsize - (g_dxsize * 0.5)), g_xsize - (g_dxsize * 0.5), NUM_X_PIXELS,
				- (g_ysize - (g_dysize * 0.5)), g_ysize - (g_dysize * 0.5), NUM_Y_PIXELS ),
		mylibrary::Hist2D ("cos(theta)",
				- (g_xsize - (g_dxsize * 0.5)), g_xsize - (g_dxsize * 0.5), NUM_X_PIXELS,
				- (g_ysize - (g_dysize * 0.5)), g_ysize - (g_dysize * 0.5), NUM_Y_PIXELS ),
		mylibrary::Hist2D ("Jokisch flux (1/(s*sr*m^2)",
				- (g_xsize - (g_dxsize * 0.5)), g_xsize - (g_dxsize * 0.5), NUM_X_PIXELS,
				- (g_ysize - (g_dysize * 0.5)), g_ysize - (g_dysize * 0.5), NUM_Y_PIXELS ),
		mylibrary::Hist2D ("flux*acc(1/s)",
				- (g_xsize - (g_dxsize * 0.5)), g_xsize - (g_dxsize * 0.5), NUM_X_PIXELS,
				- (g_ysize - (g_dysize * 0.5)), g_ysize - (g_dysize * 0.5), NUM_Y_PIXELS ),
	};

	for (int n = 0; n < NUM_PLANES; n++)
	{
		std::cout << n
			<< ',' << g_xpos[n]
			<< ',' << g_ypos[n]
			<< ',' << g_zpos[n]
			<< ',' << g_xmin[n]
			<< ',' << g_xmax[n]
			<< ',' << g_ymin[n]
			<< ',' << g_ymax[n]
			<< std::endl;
	}

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
		for (int ix = 0; ix < NUM_X_PIXELS; ix++)
		{
			double dx = g_dx[ix];
			double rx = dx / g_xdist;
			double cs2t = 1.0 / (1.0 + (rx * rx) + (ry * ry));
			double cst = sqrt (cs2t);
			double dsdo = cs2t * cs2t * dxdy * g_effxlen [ix] * g_effylen [iy];
			double flx = Jokisch::jokisch_integral (g_pcut, fabs (cst));
			hist2d [0].cumulate (g_ddx[ix], g_ddy[iy], dsdo);
			hist2d [1].cumulate (g_ddx[ix], g_ddy[iy], cst);
			hist2d [2].cumulate (g_ddx[ix], g_ddy[iy], flx);
			hist2d [3].cumulate (g_ddx[ix], g_ddy[iy], (flx * dsdo * 0.0001));
		}
	}


	for (int nh = 0; nh < numhist2d; nh++)
	{
		std::stringstream ss;
		ss << "accpyramid-" << nh << ".csv";
		std::string ofcsvnam;
		ss >> ofcsvnam;
		std::ofstream ofcsv (ofcsvnam.c_str());
		if (ofcsv)
		{
			ofcsv << "\"";
			for (int i = 0; i < argc; i++)
				ofcsv << argv[i] << ' ';
			ofcsv << "\"";
			ofcsv << ",deadch," << g_deadch
				<< std::endl;
			ofcsv << "Pyramid-system"
				<< ",xsize," << g_xsize
				<< ",ysize," << g_ysize
				<< ",xchannels," << NUM_X_CHANNELS
				<< ",ychannels," << NUM_Y_CHANNELS
				<< ",xdiff," << g_xdiff
				<< ",ydiff," << g_ydiff
				<< ",xdist," << g_xdist
				<< ",ydist," << g_ydist
				<< ",x_xshift," << g_x_xshift
				<< ",x_yshift," << g_x_yshift
				<< ",y_xshift," << g_y_xshift
				<< ",y_yshift," << g_y_yshift
				<< ",pcut(GeV/c)," << g_pcut
				<< std::endl;
			hist2d[nh].CSVdump (ofcsv);
		}
		else
			std::cerr << "ERROR: output file (" << ofcsvnam << ") open error."
				<< std::endl;
	}

	return 0;
}
