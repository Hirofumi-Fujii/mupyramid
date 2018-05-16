// mergeopt.cpp

#include <iostream>
#include <sstream>
#include "mergeopt.h"

namespace MUONDAQ
{

Mergeopt::Mergeopt()
{
	m_mergewidth = 24;
	m_infilename = "";
	m_outfilename = "";
}

Mergeopt::~Mergeopt()
{
}

void
Mergeopt::usage (std::ostream& os, const char* pname) const
{
	os << "Usage: "
		<< pname << " [options] inputfilename\n"
		<< " options:\n"
		<< " -fixedwidth\tfixed merged width\n"
		<< " -merge nsec\tmerge width in nano-sec ["
		<< m_mergewidth << "].\n"
		<< " -out filename\twrite coincidence data on the specified file.\n"
		<< std::endl;
}

bool
Mergeopt::set (int argc, char* argv[])
{
	if (argc < 2)
		return false;

	int ninput = 0;
	bool result = true;
	int ip = 1;	// arguments start from index = 1
	while (ip < argc)
	{
		if (*argv[ip] == '-')
		{
			std::string word (argv[ip++]);
			if (word == "-merge")
			{
				if (ip >= argc)
				{
					std::cerr << "ERROR "
						<< word
						<< " requires positive integer."
						<< std::endl;
					result = false;
					break;
				}
				unsigned int ui(0);
				std::stringstream ss (argv[ip++]);
				if (!(ss >> ui))
				{
					std::cerr << "ERROR "
						<< word
						<< " requires positive integer."
						<< std::endl;
					result = false;
					break;
				}
				if (ui == 0)
				{
					std::cerr << "ERROR "
						<< word
						<< " requires positive integer."
						<< std::endl;
					result = false;
					break;
				}
				m_mergewidth = ui;
			}
			else if ((word == "-o") || (word == "-out"))
			{
				if (ip >= argc)
				{
					std::cerr << "ERROR "
						<< word
						<< " requires filename."
						<< std::endl;
					result = false;
					break;
				}
				m_outfilename = std::string (argv[ip++]);
			}
			else if ((word == "-fixedwidth") || (word == "-fixed"))
				m_fixedwidth = true;
			else if ((word == "-variablewidth") || (word == "-variable"))
				m_fixedwidth = false;
			else
			{
				std::cerr << "ERROR "
					<< word
					<< " -- no such option."
					<< std::endl;
				result = false;
				break;
			}
		}
		else
		{
			if (ninput != 0)
			{
				std::cerr << "ERROR multiple input files"
					<< std::endl;
				result = false;
				break;
			}
			m_infilename = argv[ip++];
			++ninput;
		}
	}
	if (ninput == 0)
	{
		std::cerr << "ERROR you must specify the input filename"
			<< std::endl;
		result = false;
	}
	return result;
}

}	// MUONDAQ
