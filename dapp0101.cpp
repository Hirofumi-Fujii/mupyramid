#include <iostream>
#include <fstream>
#include <string>
int main (int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0]
			<< " datafilename" << std::endl;
		return (-1);
	}
	std::ifstream ifs(argv[1]);
	if (!ifs)
	{
		std::cerr << "Error: file " << argv[1]
			<< " open error" << std::endl;
		return (-2);
	}
	unsigned int ndata = 0;
	std::string recline;
	while ( getline (ifs, recline) )
	{
		if ((recline.size() > 4)
			&& (recline.substr (0, 4) == "DATA") )
			++ndata;
	}
	std::cout << "Total " << ndata << " data records"
		<< std::endl;
	return 0;
}
