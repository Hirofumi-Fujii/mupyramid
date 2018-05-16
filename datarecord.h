// datarecord.h

#ifndef DATARECORD_H_INCLUDED
#define DATARECORD_H_INCLUDED

#include <string>
#include "xyunitdata.h"

namespace MUONDAQ
{

class DataRecord
{
public:
	DataRecord ();
	DataRecord (const std::string& recstr);
	virtual ~DataRecord();
	bool ready () const { return m_xydata.ready (); }
	const BBTX036MULTI::XYUnitClock clockcount () const
		{ return m_xydata.clockcount (); }
	const BBTX036MULTI::XYUnitData xyunitdata () const
		{ return m_xydata; }
	const std::string sourcestr () const { return m_sourcestr; }

protected:
	std::string m_sourcestr;
	std::string m_idword;
	unsigned int m_triggerno;
	BBTX036MULTI::XYUnitData m_xydata;
};

}	// namespace MUONDAQ

#endif	// DATARECORD_H_INCLUDED
