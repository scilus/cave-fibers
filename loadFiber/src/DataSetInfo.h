#ifndef DATASETINFO_H_
#define DATASETINFO_H_

#include <string>
#include <GL/glew.h>
#include <Geometry/Point.h>
#include <Geometry/Vector.h>
#include <GL/GLColorTemplates.h>
#include <algorithm>
#include <limits>       // std::numeric_limits

enum AXES
{
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2
};

//All type of pixel data and the type void if the type is unknown.
enum DatasetType
{
   TYPE_UNSIGNED_CHAR,
   TYPE_CHAR,
   TYPE_UNSIGNED_SHORT,
   TYPE_SHORT,
   TYPE_UNSIGNED_INT,
   TYPE_INT,
   TYPE_UNSIGNED_LONG,
   TYPE_LONG,
   TYPE_FLOAT,
   TYPE_DOUBLE,
   TYPE_FIBER,
   TYPE_ISO_SURFACE,
   TYPE_VOID,
};

class DataSetInfo
{
public:
	typedef Geometry::Point<float,3> Point;
	typedef GLColor<GLfloat,4> Color; // Data type for colors
	typedef Geometry::Vector<float,3> Vector; // Compatible vector type

	DataSetInfo();
	virtual ~DataSetInfo();

	virtual void 	draw() const = 0;
	virtual bool 	load( const std::string &filename ) = 0;

	std::string 	getName() const;
	std::string 	getPath() const;
	void     		setName(std::string name);
	void     		setType(int type);
	float    		getThreshold() const;
	void     		setThreshold(float value);

	virtual bool	toggleShow();

	void     setShow( bool i_show);
	bool     getShow() const;

	Point 	getBBMax() const;
	Point 	getBBMin() const;

	DatasetType getType() const;

	void setType(DatasetType atype);
    const Color& getColor() const;
    void setColor(const Color& color);

protected:
	std::string                 m_name;
	std::string                 m_fullPath;
	bool						m_show;
	DatasetType         		m_type;
	float       				m_threshold;
	GLuint*     				m_bufferObjects;

	Color                       m_color;

	Point						m_max;
	Point						m_min;
};

#endif /* DATASETINFO_H_ */
