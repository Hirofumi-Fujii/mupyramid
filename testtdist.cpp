// testhits.cpp
//
// g++ -Wall testhits.cpp sglrate.cpp xyunitdata.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o testhits

#include <iostream>
#include <string>
#include "sglrate.h"
#include "gzfstream.h"

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

	MUONDAQ::SingleRate sglrate;
	sglrate.Cumulate ( ifs );
	sglrate.CSVwrite ( std::cout );
	return 0;
}
