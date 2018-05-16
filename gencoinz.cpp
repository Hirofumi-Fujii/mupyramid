// gencoinz.cpp
// g++ -Wall gencoinz.cpp coinopt.cpp coinmake.cpp databank.cpp datarecord.cpp xyunitdata.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o gencoinz.exe

#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include "coinopt.h"
#include "coinmake.h"
#include "gzfstream.h"

int main (int argc, char* argv[])
{
	MUONDAQ::Coinopt opt;
	if (!opt.set (argc, argv))
	{
		opt.usage (std::cout, argv[0]);
		return (-1);
	}

	// open the input file
	mylibrary::igzfstream ifs (opt.m_infilename.c_str());
	if (!ifs )
	{
		std::cerr << "ERROR input file ("
			<< opt.m_infilename
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	// Dump the information
	MUONDAQ::Coinmake coinmake (opt);

	if (opt.m_outfilename != "")
	{
		mylibrary::ogzfstream ofs (opt.m_outfilename.c_str());
		ofs << "INFO";
		for (int i = 0; i < argc; i++)
			ofs << ' ' << argv[i];
		ofs << std::endl;
		if (!ofs)
		{
			std::cerr << "ERROR output file ("
				<< opt.m_outfilename
				<< ") open error."
				<< std::endl;
			return (-1);
		}
		coinmake.make (ifs, ofs);
	}
	else
	{
		std::cout << "INFO";
		for (int i = 0; i < argc; i++)
			std::cout << ' ' << argv[i];
		std::cout << std::endl;
		coinmake.make (ifs, std::cout);
	}
	return 0;
}
