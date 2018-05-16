#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "xyunitdata.h"

int main (int argc, char* argv[])
{
	static const double usec = 1000000.0;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0]
			<< " datafilename" << std::endl;
		return (-1);
	}
	std::ifstream ifs(argv[1]);
	if (!ifs)
	{
		std::cerr << "Error: file " << argv[1]
			<< " open error" << std::endl;
		return (-2);
	}
	unsigned int ndata = 0;
	double t1st = 0.0;
	double tlast = 0.0;
	std::string recline;
	while ( getline (ifs, recline) )
	{
		if (recline.size() > 4)
		{
			std::stringstream ss (recline);
			std::string word;
			if (ss >> word)
			{
				if (word == "DATA")
				{
					unsigned int eventno;
					BBTX036MULTI::XYUnitData xydata;
					ss >> std::dec >> eventno
						 >> std::hex >> xydata;
					if ( xydata.ready() )
					{
						++ndata;
						tlast = xydata.microsec();
						if (ndata == 1)
							t1st = tlast;
					}
				}
			}
		}
	}
	double rate = 0.0;
	if ((ndata > 1) && (tlast > t1st))
		rate = double (ndata - 1) / ((tlast - t1st) / usec);
	std::cout << "Total " << ndata << " events, in "
		<< (t1st / usec) << " - " << (tlast / usec)
		<< " sec., rate = " << rate << " events/sec."
		<< std::endl;
	return 0;
}
