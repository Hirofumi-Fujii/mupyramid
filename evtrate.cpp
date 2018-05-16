// evtrate.cpp
// g++ -Wall evtrate.cpp coinrecord.cpp hist1d.cpp hist2d.cpp gzfstream.cpp gzipfilebuf.cpp  ncpng.cpp mytimer.cpp -lz -o evtrate.exe

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
// static const char* cointype[4] = { "01*", "0*2", "*12", "012" };
static const int xdir[NUM_UNITS] = { -1, -1, 1 };

int main (int argc, char* argv[])
{

	int reqtype = 0;

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

	mylibrary::Hist1D histtdiff("tdiff", 0.0, 1000.0, 1000);

	long totalfile = 0;
	double duration = 0.0;
	double totalcoin = 0.0;

	double tsecnow = 0.0;
	double tsecfirst = 0.0;
	double tseclast = 0.0;

	double tulast = 0.0;
	double tnlast = 0.0;

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
				bool firstevent = true;
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
							double tu1 = coinrec.microsec(uid1);
							double tn1 = coinrec.nanosec(uid1);
							double tu2 = coinrec.microsec(uid2);
							double tn2 = coinrec.nanosec(uid2);

							double dx = (double)(x1 - x2) / 10.0;
							double dy = (double)(y1 - y2) / 10.0;

							double tof = (tu1 - tu2) * 1000.0 + (tn1 - tn2);

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
							if (!firstevent)
							{
								double tdiff = (tu1 - tulast) + ((tn1 - tnlast) / 1000.0);
								histtdiff.cumulate (tdiff);
							}
							tulast = tu1;
							tnlast = tn1;
							firstevent = false;
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
	std::ofstream ofcsv ("evtrate.csv");
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
			<< std::endl;
		histtdiff.dump (ofcsv);
		ofcsv.close ();
	}


	return 0;
}
