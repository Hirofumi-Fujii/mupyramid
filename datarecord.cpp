// datarecord.cpp

#include <iostream>
#include <sstream>
#include "datarecord.h"

namespace MUONDAQ
{

DataRecord::DataRecord ()
{
}

DataRecord::DataRecord (const std::string& recstr)
{
	// check the size of 'recstr'
	// recordid(4)+space(1)+trignum(1+)+(space(1)+hexdata(2))*datasize(44)
	if (recstr.size() < 138)
		return;
	std::stringstream ss (recstr);
	std::string idword;
	if (!(ss >> idword) || (idword.size() != 4)
		|| (idword.substr(0,3) != "DAT"))
		return;
	unsigned int triggerno;
	if (!(ss >> std::dec >> triggerno))
		return;
	BBTX036MULTI::XYUnitData xydata;
	if (!(ss >> std::hex >> xydata))
		return;
	if (!xydata.ready())
		return;
	m_sourcestr = recstr;
	m_triggerno = triggerno;
	m_xydata = xydata;
}

DataRecord::~DataRecord ()
{
}

}	// namespace MUONDAQ
