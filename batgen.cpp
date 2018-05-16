// batgen.cpp

#include <iostream>
#include <sstream>

int main (int argc, char* argv[])
{
	int runno1 = 1;
	int runno2 = 900;

	if (argc != 3)
	{
		std::cerr
			<< "Usage: "
			<< argv[0]
			<< " 1st_runn last_runno"
			<< std::endl;
		return (-1);
	}
	std::stringstream ss;
	ss << argv[1] << ' ' << argv[2];
	ss >> runno1;
	ss >> runno2;	
	
	for (int i = runno1; i <= runno2; i++)
	{
		std::cout
			<< "/work/bin/gencoinz "
			<< "/data1/E-hall/detector1/data/"
			<< i
			<< "/out"
			<< i
			<< "_coin_all.dat.gz -out d1coin"
			<< i
			<< ".dat.gz"
			<< std::endl;
	}
	return 0;
}
