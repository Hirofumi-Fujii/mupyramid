// coin3fold.cpp
// g++ -Wall coin3fold.cpp coinrecord.cpp hist2d.cpp gzfstream.cpp gzipfilebuf.cpp mytimer.cpp -lz -o coin3fold.exe

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"
#include "hist2d.h"
#include "mytimer.h"

static const unsigned int NUM_UNITS = 3;
static const int xdir[NUM_UNITS] = { -1, -1,  1 };
static const int ydir[NUM_UNITS] = { -1, -1, -1 };


int main (int argc, char* argv[])
{
	double bins = 50.0;

	// check the arguments
	if (argc < 2)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " list_filename [bins]"
			<< std::endl;
		return (-1);
	}
	if (argc == 3)
	{
		std::stringstream ss(argv[2]);
		double v;
		if ((ss >> v) && (v > 0.0) && (v <= 100.0))
			bins = v;
	}

	double bmin = 50.0 - (bins / 2.0) - 0.001;
	double bmax = 50.0 + (bins / 2.0) + 0.001;

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

	static const int num2dhist = 9;
	mylibrary::Hist2D histxy[num2dhist] = {
		mylibrary::Hist2D ("Unit1_xy", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit2_xy", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit3_xy", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit1_xy_F", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit2_xy_F", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit3_xy_F", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit1_xy_B", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit2_xy_B", -0.5, 99.5, 100, -0.5, 99.5, 100),
		mylibrary::Hist2D ("Unit3_xy_B", -0.5, 99.5, 100, -0.5, 99.5, 100),
	};

	long totalfile = 0;
	double totalcoin = 0.0;

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
				// read file and count the data-records
				std::string recstr;
				while (getline (ifs, recstr))
				{
					if ((recstr.size() > 4) &&
						(recstr.substr(0,4) == "COIN"))
					{
						bool histit = false;
						double x1 = -1.0;
						double x2 = -1.0;
						double x3 = -1.0;
						double y1 = -1.0;
						double y2 = -1.0;
						double y3 = -1.0;
						
						std::stringstream ss (recstr);
						std::string recid;
						ss >> recid;
						MUONDAQ::CoinRecord coinrec;
						if ((ss >> coinrec))
						{
							totalcoin += 1.0;
							if (coinrec.xy1cluster(0) && coinrec.xy1cluster(2) &&
								(coinrec.XYUnit(0).m_numxhits <= 3) &&
								(coinrec.XYUnit(0).m_numyhits <= 3) &&
								(coinrec.XYUnit(2).m_numxhits <= 3) &&
								(coinrec.XYUnit(2).m_numyhits <= 3))
								histit = true;
						}
						if (histit)
						{
							x1 = double(coinrec.xpos(0, xdir[0])) / 10.0;
							x3 = double(coinrec.xpos(2, xdir[2])) / 10.0;
							y1 = double(coinrec.ypos(0, ydir[0])) / 10.0;
							y3 = double(coinrec.ypos(2, ydir[2])) / 10.0;

							if ((x1 > bmin) && (x1 < bmax) &&
								(y1 > bmin) && (y1 < bmax) &&
								(x3 > bmin) && (x3 < bmax) &&
								(y3 > bmin) && (y3 < bmax) )
							{
								histxy[0].cumulate(x1, y1);
								histxy[2].cumulate(x3, y3);
								if ((coinrec.XYUnit(1).m_numxhits > 0) &&
									(coinrec.XYUnit(1).m_numyhits > 0))
								{
									x2 = double(coinrec.xpos(1, xdir[1])) / 10.0;
									y2 = double(coinrec.ypos(1, ydir[1])) / 10.0;
									histxy[1].cumulate(x2, y2);
								}
								double dy = y1 + 50.0 - y2;
								if (dy > 4.5)
								{
									histxy[3].cumulate(x1, y1);
									histxy[5].cumulate(x3, y3);
									if ((coinrec.XYUnit(1).m_numxhits > 0) &&
										(coinrec.XYUnit(1).m_numyhits > 0))
									{
										x2 = double(coinrec.xpos(1, xdir[1])) / 10.0;
										y2 = double(coinrec.ypos(1, ydir[1])) / 10.0;
										histxy[4].cumulate(x2, y2);	
									}
								}
								if (dy < -4.5)
								{
									histxy[6].cumulate(x1, y1);
									histxy[8].cumulate(x3, y3);
									if ((coinrec.XYUnit(1).m_numxhits > 0) &&
										(coinrec.XYUnit(1).m_numyhits > 0))
									{
										x2 = double(coinrec.xpos(1, xdir[1])) / 10.0;
										y2 = double(coinrec.ypos(1, ydir[1])) / 10.0;
										histxy[7].cumulate(x2, y2);	
									}
								}
							}
						}
					}
				}
			}
		}
		liststr.clear();
	}
	mytimer.stop ();

	// Show results.
	std::ofstream ofcsv ("coin3fold.csv");
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
			<< ",\"bins\","
			<< bins
			<< std::endl;
		for (int i = 0; i < num2dhist; i++)
			histxy[i].dump (ofcsv);
		ofcsv.close ();
	}

	return 0;
}
