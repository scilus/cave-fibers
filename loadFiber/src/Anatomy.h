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
	const std::vector<float>& 	getFloatDataset() const;
	void 						setFloatDataset(const std::vector<float>& aFloatDataset);
	bool 						isRgb() const;
	float 						getMaxPixelValue() const;
	void 						setMaxPixelValue(float aMaxPixelValue);
	float 						getMinPixelValue() const;
	void 						setMinPixelValue(float aMinPixelValue);
	const Point& 				getVoxelSize() const;
	void 						setVoxelSize(const Point& aVoxelSize);

    //aX The value of slice in x
    //aY The value of slice in y
    //aZ The value of slice in z
    int            getPixelIndex(int aX, int aY, int aZ) const;

    //This function is used to find a specific value int a dataset.
    //
    //aX The value of slice in x
    //aY The value of slice in y
    //aZ The value of slice in z
    //aColor For color image, it specify the color desired. 0 for Red
    //1 for Green and 2 for Blue.

    float          getPixelValue(int aX, int aY, int aZ, int aColor=0) const;

    //This function is used to find a specific value in a dataset.
    //
    //aIndex The index of the value in the vector
    float          getPixelValue(int aIndex) const;

    //This function is used to find set a specific value in dataset
    //
    //aValue The new value
    //aX The value of slice in x
    //aY The value of slice in y
    //aZ The value of slice in z
    //aColor For color image, it specify the color desired. 0 for Red,
    //1 for Green and 2 for Blue.
    void           setPixelValue(float aValue, int aX, int aY, int aZ,
                                 int aColor=0);

    //This function is used to find set a specific value in dataset
    //
    //aValue The new value
    //aIndex The index of the value in the vector
    void           setPixelValue(float aValue, int aIndex);

    //get the Dimension of the Volume.
    std::vector<int>  getDimensions() const;

private:
	Point 				m_voxelSize;
    std::vector<int>    m_dims;
    bool				m_isRGB;
    std::vector<float>  m_floatDataset;

    float          		m_maxPixelValue;
	float          		m_minPixelValue;

};

#endif /* ANATOMY_H_ */
