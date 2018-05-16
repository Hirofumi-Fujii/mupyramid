// cointest4.cpp
// g++ -Wall cointest4.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o cointest4

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"

int main (int argc, char* argv[])
{

	// check the arguments
	if (argc != 2)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " data_filename"
			<< std::endl;
		return (-1);
	}

	// open the input file
	mylibrary::igzfstream ifs (argv[1]);
	if (!ifs)
	{
		// file open error
		std::cerr
			<< "ERROR: data file ("
			<< argv[1]
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	// read file and process the COIA and COIN-records

	// Create CoinRecord objects
	//   coinrec for COIN record and
	//   coiarec for COIA record.

	MUONDAQ::CoinRecord coinrec;
	MUONDAQ::CoinRecord coiarec;

	// Record string is stored in recstr
	std::string recstr;
	while (getline (ifs, recstr))
	{
		if (recstr.size () > 4)
		{
			if (recstr.substr(0,4) == "COIN")
			{
				std::stringstream ss (recstr.substr(4));
				if (coinrec.Read (ss))
				{
					if (
						coinrec.xy1cluster(0, 3) &&
						coinrec.xy1cluster(1, 3) &&
						coinrec.xy1cluster(2, 3) &&
						coiarec.xy1cluster(0, 3) &&
						coiarec.xy1cluster(1, 3) )
					{
						for (int i = 0; i < 3; i++)
							std::cout
								<< ' ' << coinrec.xpos(i)
								<< ' ' << coinrec.ypos(i);
						for (int i = 0; i < 2; i++)
							std::cout
								<< ' ' << coiarec.xpos(i)
								<< ' ' << coiarec.ypos(i);
						std::cout << std::endl;
					}
				}
				// COIA record must be cleared before the next record. 
				coiarec.Clear ();
				coinrec.Clear ();
			}
			else if (recstr.substr(0,4) == "COIA")
			{
				std::stringstream ss (recstr.substr(4));
				coiarec.Read (ss);
			}
			else if (recstr.substr(0,4) == "RUNS")
				std::cout << recstr << std::endl;
			else if (recstr.substr(0,4) == "RUNE")
				std::cout << recstr << std::endl;
		}
	}
	return 0;
}
