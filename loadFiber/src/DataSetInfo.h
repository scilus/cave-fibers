#ifndef DATASETINFO_H_
#define DATASETINFO_H_

#include <string>
#include <GL/glew.h>
#include <Geometry/Point.h>

#define X 0
#define Y 1
#define Z 2
#define W 3

//All type of pixel data and the type void if the type is unknown.
enum DatasetType
{
   Type_UNSIGNED_CHAR,
   Type_CHAR,
   Type_UNSIGNED_SHORT,
   Type_SHORT,
   Type_UNSIGNED_INT,
   Type_INT,
   Type_UNSIGNED_LONG,
   Type_LONG,
   Type_FLOAT,
   Type_DOUBLE,
   Type_Fiber,
   Type_VOID,
};

class DataSetInfo
{
public:
	typedef Geometry::Point<float,3> Point;

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
	bool     		toggleShowFS();

	void     setShow( bool i_show);
	void     setShowFS( bool i_show);
	bool     getShow() const;
	bool     getShowFS() const;

	Point 	getBBMax() const;
	Point 	getBBMin() const;

	DatasetType getType() const;

	void setType(DatasetType atype);

protected:
	std::string                 m_name;
	std::string                 m_fullPath;
	bool						m_show;
	bool                        m_showFS;       // Show front sector for meshs.
	DatasetType         		m_type;
	float       				m_threshold;
	GLuint*     				m_bufferObjects;

	Point						m_max;
	Point						m_min;
};

#endif /* DATASETINFO_H_ */
