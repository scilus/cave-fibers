#ifndef ANATOMY_H_
#define ANATOMY_H_

#include <vector>
#include "DataSetInfo.h"

class Anatomy : public DataSetInfo
{
public:
	typedef DataSetInfo::Point Point;

	Anatomy();
	virtual ~Anatomy();

	virtual void 				draw() const;
	virtual bool 				load( const std::string &filename );
	int 						getBands() const;
	int 						getColumns() const;
	const std::vector<float>& 	getFloatDataset() const;
	void 						setFloatDataset(const std::vector<float>& aFloatDataset);
	int 						getFrames() const;
	bool 						isRgb() const;
	float 						getMaxPixelValue() const;
	void 						setMaxPixelValue(float aMaxPixelValue);
	float 						getMinPixelValue() const;
	void 						setMinPixelValue(float aMinPixelValue);
	int 						getRows() const;
	const Point& 				getVoxelSize() const;
	void 						setVoxelSize(const Point& aVoxelSize);

private:
	Point 				m_voxelSize;
    int   				m_bands;
    int   				m_frames;
    int   				m_rows;
    int   				m_columns;
    bool				m_isRGB;
    std::vector<float>  m_floatDataset;

    float          		m_maxPixelValue;
	float          		m_minPixelValue;

};

#endif /* ANATOMY_H_ */
