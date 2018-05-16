// accopt.cpp

#include <iostream>
#include <sstream>
#include "accopt.h"

namespace MUPYRAMID
{

Accopt::Accopt()
{
	m_outfilename = "";
	m_order = XYXY;
	m_xygap = 45.0;
	m_unitdist = 1500.0;
}

Accopt::~Accopt()
{
}

void
Accopt::usage (std::ostream& os, const char* pname) const
{
	os << "Usage: "
		<< pname << " [options]\n"
		<< " options:\n"
		<< " -out filename\toutput file name.\n"
		<< " -order (XYXY|XYYX|YXXY|YXYX)\tplane order\n"
		<< " -xygap mm\tXY gap size in mm [" << m_xygap << "].\n"
		<< " -unitdist mm\tdistance between units (center-center) in mm ["
		<< m_unitdist << "].\n"
		<< std::endl;
}

bool
Accopt::set (int argc, char* argv[])
{
	if (argc < 1)
		return false;

	bool result = true;
	int ip = 1;	// arguments start from index = 1
	while (ip < argc)
	{
		if (*argv[ip] == '-')
		{
			std::string word (argv[ip++]);
			if ((word == "-xygap") || (word == "-unitdist"))
			{
				if (ip >= argc)
				{
					std::cerr << "ERROR "
						<< word
						<< " missing value."
						<< std::endl;
					result = false;
					break;
				}
				double dv(0.0);
				std::stringstream ss (argv[ip++]);
				if (!(ss >> dv))
				{
					std::cerr << "ERROR "
						<< word
						<< " requires numeric value."
						<< std::endl;
					result = false;
					break;
				}
				if (word == "-xygap")
					m_xygap = dv;
				else if (word == "-unitdist")
					m_unitdist = dv;
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
		}
		else
		{
			std::cerr << "ERROR "
				<< argv[ip] << " -- Unknown option."
				<< std::endl;
			result = false;
			break;
		}
	}
	return result;
}

}	// MUPYRAMID
