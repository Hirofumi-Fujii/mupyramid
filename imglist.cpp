// imglist.cpp

#include <iostream>
#include <sstream>
#include <string>

int main (int argc, char* argv[])
{

	int runno1 = 1;
	int runno2 = 900;

	if (argc != 5)
	{
		std::cerr
			<< "Usage: "
			<< argv[0]
			<< " prefix 1st_runn last_runno postfix"
			<< std::endl;
		return (-1);
	}

	std::stringstream ss;
	ss << argv[2] << ' ' << argv[3];
	ss >> runno1;
	ss >> runno2;	
	
	for (int i = runno1; i <= runno2; i++)
	{
		std::cout
			<< argv[1]
			<< i
			<< argv[4]
			<< std::endl;
	}
	return 0;
}
