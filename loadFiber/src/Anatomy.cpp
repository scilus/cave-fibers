#include "Anatomy.h"

#include "Nifti/nifti1_io.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//This function template read pixel data and put them in the vector of float.
//It read the file Anatomy per Anatomy. It get the max  and min value of
//pixel data.
//
//aSize Size of Vector of float.
//arMax The Max Value of Pixel Data.
//arMin The Min value of Pixel Data.
//aNbBrick The number of Anatomy in the Volume.
//arFloatDataset The Vector of float.
//arFileName The complete path of file selected.
//////////////////////////////////////////////////////////////////////////

template <typename TypeInput> void _ReadNiftiBuffer(int aSize,float& arMax,
                                    float& arMin, int aNbBrick,
                                    std::vector<float>& arFloatDataset,
                                    const std::string& arFileName)
{
   //read data brick per brick of Anatomy
   for(int BrickIndex=0;BrickIndex<aNbBrick;BrickIndex++)
   {
      //open a volume 3D per Anatomy
      int BList[Y_AXIS] = {BrickIndex};
      nifti_brick_list BrickList;

      //first param: filename
      //second param: number of sub-bricks to read. If the third parameter is
      //not NULL, it specify the size of the list
      //third param: list of sub-bricks to read (can be NULL; if NULL,
      //read complete dataset) if this list is not NULL, it take the indices of
      //brick specify in the list.
      //last param: pointer to empty nifti_brick_list struct
      //this function read per volume 3D. If the number of Bands is 6, the
      //number of brick is 6.
      nifti_image* pFileData = nifti_image_read_bricks(arFileName.c_str(),1,
         BList,&BrickList);

      //put pixel data in a buffer.
      TypeInput* pDataOut = (TypeInput*)BrickList.bricks[X_AXIS];
      int BrickSize = aSize/aNbBrick;

      //Fill the Dataset
      for(int j=0;j<BrickSize;j++)
      {
         arFloatDataset[(BrickSize*BrickIndex)+j]=pDataOut[j];
         arMin = std::min(arMin,arFloatDataset[(BrickSize*BrickIndex)+j]);
         arMax = std::max(arMax,arFloatDataset[(BrickSize*BrickIndex)+j]);
      }
      nifti_image_free(pFileData);
      delete [] pDataOut;
   }
}

Anatomy::Anatomy():
	m_voxelSize(Anatomy::Point(0.0,0.0,0.0)),
	m_isRGB(false),
	m_maxPixelValue(0),
	m_minPixelValue(0),
	m_dims(3,0)
{

}

Anatomy::~Anatomy()
{
}

void Anatomy::draw() const
{
    float   IndexColorR;
    float   IndexColorG;
    float   IndexColorB;
    float   IndexColor;

    float* pixels = new float[m_dims[X_AXIS]*m_dims[Y_AXIS]*3];

    for(int y=0;y<m_dims[Y_AXIS];y++)
    {
        for(int x=0;x<m_dims[X_AXIS];x++)
        {
            //There are 3 color if the volume is in color
            if (m_isRGB)
            {
                //the order of arguments are called differently,
                //depending on the angle of the image
                IndexColorR = getPixelValue(x,y,80,0);
                IndexColorG = getPixelValue(x,y,80,1);
                IndexColorB = getPixelValue(x,y,80,2);
            }
            else
            {
                IndexColor = getPixelValue(x,y,80,0);
                pixels[(3 * m_dims[X_AXIS] * y)+(3 * x)] = IndexColor;
                pixels[(3 * m_dims[X_AXIS] * y)+(3 * x)+1] = IndexColor;
                pixels[(3 * m_dims[X_AXIS] * y)+(3 * x)+2] = IndexColor;
            }
        }
    }

    glDrawPixels(m_dims[X_AXIS],m_dims[Y_AXIS],GL_RGB,GL_FLOAT,pixels);
}

const std::vector<float>& Anatomy::getFloatDataset() const {

	return m_floatDataset;
}

void Anatomy::setFloatDataset(const std::vector<float>& afloatDataset)
{
	m_floatDataset = afloatDataset;
}

bool Anatomy::isRgb() const
{
	return m_isRGB;
}

float Anatomy::getMaxPixelValue() const
{
	return m_maxPixelValue;
}

void Anatomy::setMaxPixelValue(float amaxPixelValue)
{
	m_maxPixelValue = amaxPixelValue;
}

float Anatomy::getMinPixelValue() const
{
	return m_minPixelValue;
}

void Anatomy::setMinPixelValue(float aminPixelValue)
{
	m_minPixelValue = aminPixelValue;
}

const Anatomy::Point& Anatomy::getVoxelSize() const
{
    return m_voxelSize;
}

void Anatomy::setVoxelSize(const Point& avoxelSize)
{
	m_voxelSize = avoxelSize;
}
//////////////////////////////////////////////////////////////////////////
//For obtain the index of one pixel
int Anatomy::getPixelIndex(int ax, int ay, int az) const
{
    //It is different if the volume is in color
    if (m_isRGB)
    {
        return ((3 * m_dims[X_AXIS] * m_dims[Y_AXIS] * az)+(3 * m_dims[X_AXIS] * ay)+(3 * ax));
    }
    else
    {
        return((m_dims[Y_AXIS] * m_dims[X_AXIS] * az)+(m_dims[X_AXIS] * ay)+ax);
    }
}

//////////////////////////////////////////////////////////////////////////

float Anatomy::getPixelValue(int aX, int aY, int aZ, int aColor) const
{
    return m_floatDataset[getPixelIndex(aX,aY,aZ)+aColor];
}

//////////////////////////////////////////////////////////////////////////

float Anatomy::getPixelValue(int aIndex) const
{
    return m_floatDataset[aIndex];
}

//////////////////////////////////////////////////////////////////////////

void Anatomy::setPixelValue(float aValue, int aX, int aY, int aZ,int aColor)
{
    m_floatDataset[getPixelIndex(aX,aY,aZ)+aColor] = aValue;
}

//////////////////////////////////////////////////////////////////////////

void Anatomy::setPixelValue(float aValue, int aIndex)
{
    m_floatDataset[aIndex] = aValue;
}

std::vector<int> Anatomy::getDimensions() const
{
    return m_dims;
}

bool Anatomy::load( const std::string &filename )
{
	std::string extension = filename.substr(filename.find_last_of("."));
	bool LoadingSuccesful = false;

	if(extension == ".gz")
	{
	    std::string baseFileName = filename.substr(0,filename.find_last_of("."));
	    extension = filename.substr(baseFileName.find_last_of("."));
	}

	if(extension == ".nii.gz" || (extension == ".nii")
	  || (extension == ".hdr") || (extension == ".img"))
	{
	    m_minPixelValue = std::numeric_limits< float >::max();
	    m_maxPixelValue = std::numeric_limits< float >::min();

		int bands = 0;
		//open header
		nifti_image* pImage = nifti_image_read(filename.c_str(),0);
		if(!pImage)
		{
			return false;
		}

		//size of the volume
		m_dims[X_AXIS]  = pImage->dim[1];
		m_dims[Y_AXIS]  = pImage->dim[2];
		m_dims[Z_AXIS]  = pImage->dim[3];
		bands           = pImage->dim[4];

		int NbBrick = 1; // the number of volume 3D in the volume.

		//set the number of brick
		for(int i=4; i<=pImage->ndim; i++)
		{
			NbBrick *= pImage->dim[i];
		}

		//Voxel Size
		m_voxelSize[X_AXIS] = pImage->dx;
		m_voxelSize[Y_AXIS] = pImage->dy;
		m_voxelSize[Z_AXIS] = pImage->dz;

		if(bands<=0)
		{
		    bands=1;
		}

		if(m_dims[Z_AXIS]<=0)
		{
		    m_dims[Z_AXIS]=1;
		}

		m_max = Point(ceil(m_dims[X_AXIS]/2),ceil(m_dims[Y_AXIS]/2),ceil(m_dims[Z_AXIS]/2));
		m_min = Point(-ceil(m_dims[X_AXIS]/2),-ceil(m_dims[Y_AXIS]/2),-ceil(m_dims[Z_AXIS]/2));

		//Check what is the type of the file
		if(pImage->datatype==DT_UNSIGNED_CHAR/*2*/)
		{
			m_type=TYPE_UNSIGNED_CHAR;
		}
		else if(pImage->datatype==DT_SIGNED_SHORT/*4*/)
		{
			m_type=TYPE_SHORT;
		}
		else if(pImage->datatype==DT_SIGNED_INT/*8*/)
		{
			m_type=TYPE_INT;
		}
		else if(pImage->datatype== DT_FLOAT/*16*/ )
		{
			m_type=TYPE_FLOAT;
		}
		else if(pImage->datatype==DT_DOUBLE/*64*/)
		{
			m_type=TYPE_DOUBLE;
		}
		else
		{
			m_type=TYPE_VOID;
		}

		if(m_type==TYPE_VOID)
		{
			LoadingSuccesful=false;
		}

		//verif if the volume is in color or not
		if(m_dims[Z_AXIS]==3)
		{
			m_isRGB=true;
		}
		else
		{
			m_isRGB=false;
		}

		nifti_image_free(pImage);

		//The size of the Vector
		int Size  = m_dims[X_AXIS]*m_dims[Y_AXIS]*m_dims[Z_AXIS]*bands;
		m_floatDataset.resize(Size);
		LoadingSuccesful=true;

		//read pixel data
		switch(m_type)
		{
		case TYPE_UNSIGNED_CHAR:
		{
			m_maxPixelValue=255.0;
			_ReadNiftiBuffer<unsigned char>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
		}
		break;
		case TYPE_SHORT:
		{
			_ReadNiftiBuffer<short>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
		}
		break;
		case TYPE_INT:
		{
			_ReadNiftiBuffer<int>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
		}
		break;
		case TYPE_FLOAT:
		{
			_ReadNiftiBuffer<float>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
		}
		break;
		case TYPE_DOUBLE:
		{
			_ReadNiftiBuffer<double>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
		}
		break;
		default:
		{
		    LoadingSuccesful = false;
		}
		break;
		}
	}

	return LoadingSuccesful;
}
