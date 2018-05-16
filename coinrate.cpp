// coinrate.cpp
// g++ -Wall coinrate.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp mytimer.cpp -lz -o coinrate

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"
#include "mytimer.h"

static const unsigned int NUM_UNITS = 3;

static const char* monstr[] =
{
	"Jan","Feb","Mar","Apr","May","Jun",
	"Jul","Aug","Sep","Oct","Nov","Dec"
};

time_t
UnixTime (const std::string& timestr)
{
	if (timestr.size() < 24)
		return (time_t)(-1);

	struct tm tmtmp;

	// Month
	int m = -1;
	for (int i = 0; i < 12; i++)
	{
		if (timestr.substr(4,3) == monstr[i])
		{
			m = i;
			break;
		}
	}
	if (m < 0)
		return (time_t)(-1);
	tmtmp.tm_mon = m;

	int ibuf[4];

	// Day
	if (timestr[ 8] == ' ')
		ibuf[0] = 0;
	else
		ibuf[0] = (int)(timestr[ 8] - '0');
	ibuf[1] = (int)(timestr[ 9] - '0');
	tmtmp.tm_mday = ibuf[0] * 10 + ibuf[1];

	// Hour
	if (timestr[11] == ' ')
		ibuf[0] = 0;
	else
		ibuf[0] = (int)(timestr[11] - '0');
	ibuf[1] = (int)(timestr[12] - '0');
	tmtmp.tm_hour = ibuf[0] * 10 + ibuf[1];

	// Minuit
	if (timestr[14] == ' ')
		ibuf[0] = 0;
	else
		ibuf[0] = (int)(timestr[14] - '0');
	ibuf[1] = (int)(timestr[15] - '0');
	tmtmp.tm_min = ibuf[0] * 10 + ibuf[1];

	// Sec
	if (timestr[17] == ' ')
		ibuf[0] = 0;
	else
		ibuf[0] = (int)(timestr[17] - '0');
	ibuf[1] = (int)(timestr[18] - '0');
	tmtmp.tm_sec = ibuf[0] * 10 + ibuf[1];
	
	// Year
	ibuf[0] = (int)(timestr[20] - '0');
	ibuf[1] = (int)(timestr[21] - '0');
	ibuf[2] = (int)(timestr[22] - '0');
	ibuf[3] = (int)(timestr[23] - '0');
	tmtmp.tm_year = ibuf[0] * 1000 + ibuf[1] * 100 + ibuf[2] * 10 + ibuf[3] - 1900;

	// Other members
	tmtmp.tm_wday = 0;	// will be set by mktime
	tmtmp.tm_yday = 0;	// will be set by mktime
	tmtmp.tm_isdst = 0;	// will be set by mktime

	return mktime (&tmtmp);
}

int main (int argc, char* argv[])
{
	double coin_all[4];
	double coin_scl[4];

	char sepchar = ',';

	// check the arguments
	if (argc < 2)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " list_filename"
			<< std::endl;
		return (-1);
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

	long totalfile = 0;
	double duration = 0.0;
	double totalcoin = 0.0;

	double tsecnow = 0.0;
	double tsecfirst = 0.0;
	double tseclast = 0.0;

	int detsys = 0;
	int ntwin = -1;
	int ntemp1 = -1;
	int ntemp2 = -1;
	int ntemp3 = -1;
	int ntemp4 = -1;

	mylibrary::MyTimer mytimer;

	// read the filename from the list file
	std::string liststr;
	mytimer.start ();
	std::cout << "\"Filename\""
		<< sepchar << "\"RunStart\""
		<< sepchar << "\"RunEnd\""
		<< sepchar << "\"Detector\""
		<< sepchar << "\"Duration(sec)\""
		<< sepchar << "\"T(1st)\""
		<< sepchar << "\"T(last)\""
		<< sepchar << "\"Twin\""
		<< sepchar << "\"Temp1.x10C\""
		<< sepchar << "\"Temp2.x10C\""
		<< sepchar << "\"Temp3.x10C\""
		<< sepchar << "\"Temp4.x10C\""
		<< sepchar << "\"U0&1(all)\""
		<< sepchar << "\"U0&1(sglC)\""
		<< sepchar << "\"U0&2(all)\""
		<< sepchar << "\"U0&2(sglC)\""
		<< sepchar << "\"U1&2(all)\""
		<< sepchar << "\"U1&2(sglC)\""
		<< sepchar << "\"U0&2/U0&U1&U2(all)\""
		<< sepchar << "\"U0&2/U0&u1&u2(sglC)\""
		<< sepchar << "\"RunStart(UnixTime)\""
		<< sepchar << "\"RunEnd(UnixTime)\""
		<< std::endl;

	while (getline (ifl, liststr))
	{
		std::string datfnam;
		std::string runsrec;
		std::string runerec;

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
				runsrec = "";
				runerec = "";
				bool newrun = true;
				detsys = 0;
				tsecnow = 0.0;
				tsecfirst = 0.0;
				tseclast = 0.0;
				ntwin = -1;
				ntemp1 = -1;
				ntemp2 = -1;
				ntemp3 = -1;
				ntemp4 = -1;
				for (int i = 0; i < 4; i++)
				{
					coin_all[i] = 0.0;
					coin_scl[i] = 0.0;
				}
				// read file and count the data-records
				std::string recstr;
				while (getline (ifs, recstr))
				{
					if ((recstr.size() > 4) &&
						(recstr.substr(0,4) == "COIN"))
					{
						std::stringstream ss (recstr.substr(4));
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
							if ((coinrec.numdat(0) > 0) &&
								(coinrec.numdat(1) > 0))
							{
								coin_all[0] += 1.0;
								if (coinrec.xy1cluster(0) &&
									coinrec.xy1cluster(1))
									coin_scl[0] += 1.0;
							}
							if ((coinrec.numdat(0) > 0) &&
								(coinrec.numdat(2) > 0))
							{
								coin_all[1] += 1.0;
								if (coinrec.xy1cluster(0) &&
									coinrec.xy1cluster(2))
									coin_scl[1] += 1.0;
							}
							if ((coinrec.numdat(1) > 0) &&
								(coinrec.numdat(2) > 0))
							{
								coin_all[2] += 1.0;
								if (coinrec.xy1cluster(1) &&
									coinrec.xy1cluster(2))
									coin_scl[2] += 1.0;
							}
							if ((coinrec.numdat(0) > 0) &&
								(coinrec.numdat(1) > 0) &&
								(coinrec.numdat(2) > 0))
							{
								coin_all[3] += 1.0;
								if (coinrec.xy1cluster(0) &&
									coinrec.xy1cluster(1) &&
									coinrec.xy1cluster(2) )
									coin_scl[3] += 1.0;
							}
						}
					}
					else if ((recstr.size() > 4) &&
							(recstr.substr(0,4) == "PARA"))
					{
						std::stringstream ss (recstr.substr(4));
						int n;
						if ((ss >> n))
							ntwin = n;
						if ((ss >> n))
							ntemp1 = n;
						if ((ss >> n))
							ntemp2 = n;
						if ((ss >> n))
							ntemp3 = n;
						if ((ss >> n))
							ntemp4 = n;
					}
					else if ((recstr.size() > 4) &&
							(recstr.substr(0,3) == "TWI"))
					{
						char c = recstr[3];
						if ((c == '1') || (c == '2') || (c == '3'))
							detsys = 1;
						else if ((c == '4') || (c == '5') || (c == '6'))
							detsys = 2;
						else if ((c == '7') || (c == '8') || (c == '9'))
							detsys = 3;
					}
					else if ((recstr.size() > 4) &&
							(recstr.substr(0,4) == "RUNS"))
					{
						runsrec = recstr.substr(5);
					}
					else if ((recstr.size() > 4) &&
							(recstr.substr(0,4) == "RUNE"))
					{
						runerec = recstr.substr(5);
					}
				}
				duration = tseclast - tsecfirst;
				if (duration > 0.0)
				{
					std::cout << '\"' << datfnam << '\"'
						<< sepchar << '\"' << runsrec << '\"'
						<< sepchar << '\"' << runerec << '\"'
						<< sepchar << detsys
						<< sepchar << duration
						<< sepchar << tsecfirst
						<< sepchar << tseclast
						<< sepchar << ntwin
						<< sepchar << ntemp1
						<< sepchar << ntemp2
						<< sepchar << ntemp3
						<< sepchar << ntemp4;
					for (int i = 0; i < 4; i++)
					{
						std::cout
							<< sepchar << coin_all[i]
							<< sepchar << coin_scl[i];
					}
					std::cout
						<< sepchar << UnixTime(runsrec)
						<< sepchar << UnixTime(runerec);
					std::cout << std::endl;
				}
				else
				{
					std::cerr
						<< "ERROR: data file ("
						<< datfnam
						<< ") zero duration"
						<< std::endl;
				}
			}
		}
		liststr.clear();
	}
	mytimer.stop ();

	return 0;
}
