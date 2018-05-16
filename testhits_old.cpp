// testhits.cpp
//
// g++ -Wall testhits.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o testhits

#include <iostream>
#include <string>
#include "gzfstream.h"

static const int MAX_UNITS = 9;
double hitcounts[MAX_UNITS];

int 
main (int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " datafile" << std::endl;
		return (-1);
	}

	// open the input file
	mylibrary::igzfstream ifs (argv[1]);
	if (!ifs )
	{
		std::cerr << "ERROR input file ("
			<< argv[1]
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	// clear the hitcounts
	for (int i = 0; i < MAX_UNITS; i++)
		hitcounts[i] = 0.0;

	std::string sline;
	while (getline (ifs, sline))
	{
		if ((sline.size () > 4) && (sline.substr (0, 3) == "DAT"))
		{
			char c = sline[3];
			int unitno = (int)(c - '1');
			if ((unitno >= 0) && (unitno < MAX_UNITS))
				hitcounts[unitno] += 1.0;
		}
		sline.clear ();
	}

	// dump the hitcounts
	for (int i = 0; i < MAX_UNITS; i++)
	{
		if ( i )
			std::cout << ',';
		std::cout << hitcounts [i];
	}
	std::cout << std::endl;
	return 0;
}
