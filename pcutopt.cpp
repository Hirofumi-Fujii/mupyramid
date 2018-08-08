// pcutopt.cpp

#include <iostream>
#include <sstream>
#include "pcutopt.h"

namespace MUPYRAMID
{

Pcutopt::Pcutopt()
{
	m_trxfilename = "";
	m_trxfilename_given = false;
	m_cosfilename = "";
	m_cosfilename_given = false;
	m_rngfilename = "";
	m_rngfilename_given = false;
	m_pcut0 = 0.0;
	m_pcut0_given = false;
}

Pcutopt::~Pcutopt()
{
}

void
Pcutopt::usage (std::ostream& os, const char* pname) const
{
	os << "Usage: "
		<< pname << " [options] trxfilename cosfilename rngfilename\n"
		<< " options:\n"
		<< " -pcut0 value   Give the detector cutoff momentum in GeV/c.\n"
		<< std::endl;
}

bool
Pcutopt::set (int argc, char* argv[])
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
			if ((word == "-pcut") || (word == "-pcut0"))
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
				if ((word == "-pcut") || (word == "-pcut0"))
				{
					m_pcut0 = dv;
					m_pcut0_given = true;
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
		else
		{
			if (!m_trxfilename_given)
			{
				m_trxfilename = std::string (argv[ip++]);
				m_trxfilename_given = true;
			}
			else if (!m_cosfilename_given)
			{
				m_cosfilename = std::string (argv[ip++]);
				m_cosfilename_given = true;
			}
			else if (!m_rngfilename_given)
			{
				m_rngfilename = std::string (argv[ip++]);
				m_rngfilename_given = true;
			}
			else
			{
				std::cerr << "ERROR"
					<< "trx- ,cos- and rng-filenames have been already given."
					<< std::endl;
				result = false;
				break;
			}
		}
	}
	if ((!m_trxfilename_given) || (!m_cosfilename_given))
		result = false;
	return result;
}

}	// MUPYRAMID
