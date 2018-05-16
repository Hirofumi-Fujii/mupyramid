// coinimg.cpp
// g++ -Wall coinimg.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp -lz -o coinimg.exe

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "ncpng.h"

static const unsigned int NUM_UNITS = 3;
// static const char* cointype[4] = { "01*", "0*2", "*12", "012" };
static const int xdir[NUM_UNITS] = { -1, -1, 1 };

double hist[200][200];

int main (int argc, char* argv[])
{
	unsigned long numcoin [4] = { 0 };
	unsigned long numcoins[4] = { 0 };
	unsigned long numcoinh[4] = { 0 };
	unsigned long numcoinrec = 0;

	int reqtype = 0;

	for (int i = 0; i < 200; i++)
		for (int j = 0; j < 200; j++)
			hist[i][j] = 0.0;
	double histmax = 0.0;

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

	// read the filename from the list file
	std::string liststr;
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
					<< argv[1]
					<< ") open error."
					<< std::endl;
			}
			else
			{
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
						++numcoinrec;
						std::stringstream ss (recstr);
						std::string recid;
						ss >> recid;
						int numdat[NUM_UNITS] = { 0 };
						int numxhits[NUM_UNITS] = { 0 };
						int numyhits[NUM_UNITS] = { 0 };
						int numxclust[NUM_UNITS] = { 0 };
						int numyclust[NUM_UNITS] = { 0 };
						int xav[NUM_UNITS] = { 0 };
						int yav[NUM_UNITS] = { 0 };
						for (unsigned int u = 0; u < NUM_UNITS; u++)
						{
							int n;
							if (ss >> n)
								numdat[u] = n;
						}
						for (unsigned int u = 0; u < NUM_UNITS; u++)
						{
							int n;
							if (ss >> n)
								numxhits[u] = n;
							if (ss >> n)
								numxclust[u] = n;
							if (ss >> n)
								xav[u] = n;	// Xav
							if (ss >> n)
								numyhits[u] = n;
							if (ss >> n)
								numyclust[u] = n;
							if (ss >> n)
								yav[u] = n;	// Yav
							double d;
							ss >> d;	// microT
							ss >> d;	// nanoT
						}
						if ((numdat[0] > 0) && (numdat[1] > 0))
						{
							numcoin[0] += 1;
							if ((numxclust[0] == 1) && (numyclust[0] == 1)
								&& (numxclust[1] == 1) && (numyclust[1] == 1))
							{
								numcoins[0] += 1;
								if ((numxhits[0] <= 3) && (numyhits[0] <= 3)
									&& (numxhits[1] <= 3) && (numyhits[1] <= 3))
								{
									if (reqtype == 0)
										histit = true;
									numcoinh[0] += 1;
								}
							}
						}
						if ((numdat[0] > 0) && (numdat[2] > 0))
						{
							numcoin[1] += 1;
							if ((numxclust[0] == 1) && (numyclust[0] == 1)
								&& (numxclust[2] == 1) && (numyclust[2] == 1))
							{
								numcoins[1] += 1;
								if ((numxhits[0] <= 3) && (numyhits[0] <= 3)
									&& (numxhits[2] <= 3) && (numyhits[2] <= 3))
								{
									if (reqtype == 1)
										histit = true;
									numcoinh[1] += 1;
								}
							}
						}
						if ((numdat[1] > 0) && (numdat[2] > 0))
						{
							numcoin[2] += 1;
							if ((numxclust[1] == 1) && (numyclust[1] == 1)
								&& (numxclust[2] == 1) && (numyclust[2] == 1))
							{
								numcoins[2] += 1;
								if ((numxhits[1] <= 3) && (numyhits[1] <= 3)
									&& (numxhits[2] <= 3) && (numyhits[2] <= 3))
								{
									if (reqtype == 2)
										histit = true;
									numcoinh[2] += 1;
								}
							}
						}
						if ((numdat[0] > 0) && (numdat[1] > 0) && (numdat[2] > 0))
						{
							numcoin[3] += 1;
							if ((numxclust[0] == 1) && (numyclust[0] == 1)
								&& (numxclust[1] == 1) && (numyclust[1] == 1)
								&& (numxclust[2] == 1) && (numyclust[2] == 1))
							{
								numcoins[3] += 1;
								if ((numxhits[0] <= 3) && (numyhits[0] <= 3)
									&& (numxhits[1] <= 3) && (numyhits[1] <= 3)
									&& (numxhits[2] <= 3) && (numyhits[2] <= 3))
								{
									if (reqtype == 3)
										histit = true;
									numcoinh[3] += 1;
								}
							}
						}
						if (histit)
						{
							x1 = xav[uid1];
							if (xdir[uid1] < 0)
								x1 = 990 - xav[uid1];
							x2 = xav[uid2];
							if (xdir[uid2] < 0)
								x2 = 990 - xav[uid2];
							y1 = yav[uid1];
							y2 = yav[uid2];
							int dx = (x1 - x2) / 10;
							int dy = (y1 - y2) / 10;
							if (dx < -99)
								dx = -99;
							if (dx > 99)
								dx = 99;
							if (dy < -99)
								dy = -99;
							if (dy > 99)
								dy = 99;
							int idx = dx + 99;
							int idy = dy + 99;
							hist[idy][idx] += 1.0;
							if (hist[idy][idx] > histmax)
								histmax = hist[idy][idx];
						}
					}
//					else if (recstr.size() > 4)
//					{
//						if (recstr.substr(0,4) == "RUNS")
//							std::cout << recstr << std::endl;
//						else if (recstr.substr(0,4) == "RUNE")
//							std::cout << recstr << std::endl;
//						else if (recstr.substr(0,4) == "PARA")
//							std::cout << recstr << std::endl;
//					}
				}
			}
		}
		liststr.clear();
	}

	// Show results.
	std::ofstream ofcsv ("coinimg.csv");
	for (int j = 0; j < 200; j++)
	{
		for (int i = 0; i < 200; i++)
		{
			if (i)
				ofcsv << ',';
			ofcsv << hist[j][i];
		}
		ofcsv << std::endl;
	}
	ofcsv.close ();

	// make image
	double histoffs = 0.0;
	double histnorm = 1.0;
	if (histmax > histnorm)
		histnorm = histmax;
	mypnglib::NCPNG pngimg (200, 200, mypnglib::NCPNG::GREYSCALE, 16);
	for (int y = 0; y < 200; y++)
	{
		for (int x = 0; x < 200; x++)
		{
			double v = (hist[y][x] - histoffs) / histnorm;
			if (v > 1.0)
				v = 1.0;
			else if (v < 0.0)
				v = 0.0;
			mypnglib::NCPNG::GREY_PIXEL pixel(v);
			pngimg.put (x, y, pixel);
		}
	}
	std::ofstream ofs ("coinimg.png", std::ios::binary);
	pngimg.write (ofs);

	return 0;
}
