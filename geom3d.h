// geom3d.h
#ifndef GEOM3D_H_INCLUDED
#define GEOM3D_H_INCLUDED

#include <cmath>
#include <cfloat>
#include <list>

namespace mylibrary
{

class Vector3D
{
public:
	Vector3D() :
		m_x(0.0), m_y(0.0), m_z(0.0) { return; }
	Vector3D(double x, double y, double z) :
		m_x(x), m_y(y), m_z(z) { return; }
	virtual ~Vector3D() {};
	double x() const { return m_x; }
	double y() const { return m_y; }
	double z() const { return m_z; }
	double norm() const { return sqrt(m_x * m_x + m_y * m_y + m_z * m_z); }
	double normalize();

protected:
	double m_x;
	double m_y;
	double m_z;
};

inline Vector3D operator+ (const Vector3D& v1, const Vector3D& v2)
	{ return Vector3D(
		v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z() ); }
inline Vector3D operator- (const Vector3D& v1, const Vector3D& v2)
	{ return Vector3D(
		v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z() ); }
inline Vector3D operator* (const Vector3D& v1, const Vector3D& v2)
	{ return Vector3D(
			v1.y() * v2.z() - v1.z() * v2.y(),
			v1.z() * v2.x() - v1.x() * v2.z(),
			v1.x() * v2.y() - v1.y() * v2.x()); }
inline Vector3D operator* (const Vector3D& v, double d)
	{ return( Vector3D (v.x() * d, v.y() * d, v.z() * d) ); }
inline Vector3D operator* (double d, const Vector3D& v)
	{ return( Vector3D (d * v.x(), d * v.y(), d * v.z()) ); }
inline double dot(const Vector3D& v1, const Vector3D& v2)
	{ return( v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z()); }

class Line3D
{
public:
	Line3D(const Vector3D& pos, const Vector3D& dir) :
		m_pos(pos), m_dir(dir) { m_dir.normalize(); return; }
	virtual ~Line3D() { return; }
	virtual const Vector3D& pos() const { return m_pos; }
	virtual const Vector3D& dir() const { return m_dir; }

protected:
	Vector3D m_pos;
	Vector3D m_dir;
};

class InOut
{
public:
	InOut() : m_rin(DBL_MAX), m_rout(DBL_MAX) { return; }
	InOut(double rin, double rout) : m_rin(rin), m_rout(rout)
		{ if (rout >= rin) return; m_rin = rout; m_rout = rin; return; }
	virtual ~InOut();
	double length() const { return (m_rout - m_rin); }
	double rin() const { return m_rin; }
	double rout() const { return m_rout;}

protected:
	double m_rin;
	double m_rout;
};

class InOutList
{
public:
	InOutList() : m_numelem(0) { return; }
	virtual ~InOutList();
	const InOutList& add(const InOut& inout);
	const InOutList& add(const InOutList& list);
	const InOutList& remove(const InOut& inout);
	const InOutList& remove(const InOutList& list);
	double totallength() const;
	bool empty() const { return m_list.empty(); }
	int size() const { return m_list.size(); }

protected:
	int m_numelem;
	std::list<InOut> m_list;
};

class Shape3D
{
public:
	Shape3D() { return; }
	virtual ~Shape3D() { return; }
	virtual InOutList hit (const Line3D& line) const
		{ return InOutList(); }
};

}	// namespace mylibrary

#endif	// GEOM3D_H_INCLUDED
