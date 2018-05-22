// coinopt.cpp

#include <iostream>
#include <sstream>
#include "coinopt.h"

namespace MUONDAQ
{

Coinopt::Coinopt()
{
	m_mergewidth = 8;
	m_coinwidth = 32;
	m_infilename = "";
	m_outfilename = "";
	m_singledump = false;
	m_mergedump = false;
	m_longdump = false;
	for (int i = 0; i < 256; i++)
		m_toffset [i] = (unsigned int)(0);
}

Coinopt::~Coinopt()
{
}

void
Coinopt::usage (std::ostream& os, const char* pname) const
{
	os << "Usage: "
		<< pname << " [options] inputfilename\n"
		<< " options:\n"
		<< " -merge nsec\tmerge width in nano-sec ["
		<< m_mergewidth << "].\n"
		<< " -width nsec\tcoincidence width in nano-sec ["
		<< m_coinwidth << "].\n"
		<< " -out filename\twrite coincidence data on the specified file.\n"
		<< " -single\toutput including single unit-pair coincidence.\n"
		<< " -longdump\toutput with raw data.\n"
		<< " -mergedump\toutput with merged data.\n"
		<< " -shortdump\toutput without raw/merged data.\n"
		<< " -toff DAQboxId usec\toffset for DAQbox-timecounter in nano-sec.\n"
		<< std::endl;
}

bool
Coinopt::set (int argc, char* argv[])
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
			if ((word == "-width") || (word == "-merge"))
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
				if ((word != "-merge") && (ui == 0))
				{
					std::cerr << "ERROR "
						<< word
						<< " requires positive integer."
						<< std::endl;
					result = false;
					break;
				}
				if (word == "-width")
					m_coinwidth = ui;
				else if (word == "-merge")
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
			else if ((word == "-longdump") || (word == "-long"))
			{
				m_longdump = true;
			}
			else if (word == "-mergedump")
			{
				m_mergedump = true;
			}
			else if ((word == "-singledump") || (word == "-single"))
			{
				m_singledump = true;
			}
			else if ((word == "-brief") || (word == "-shortdump") || (word == "-short"))
			{
				m_longdump = m_mergedump = false;
			}
			else if ((word == "-toff") || (word == "-toffset"))
			{
				if ((ip + 1) >= argc)
				{
					std::cerr << "ERROR "
						<< " -- requires DAQboxid and offset-value."
						<< std::endl;
					result = false;
					break;
				}
				char boxid = *argv[ip++];
				std::stringstream ss (argv[ip++]);
				unsigned int uv;
				if (!(ss >> uv))
				{
					std::cerr << "ERROE "
						<< word
						<< " -- cannot read offset-value."
						<< std::endl;
					result = false;
					break;
				}
				m_toffset [(unsigned int)(boxid) & 255] = uv;
			}
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
