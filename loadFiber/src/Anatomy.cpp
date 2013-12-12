#include "Anatomy.h"

#include "Nifti/nifti1_io.h"

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
      int BList[Y] = {BrickIndex};
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
      TypeInput* pDataOut = (TypeInput*)BrickList.bricks[X];
      int BrickSize = aSize/aNbBrick;

      //Fill the Dataset
      for(int j=0;j<BrickSize;j++)
      {
         arFloatDataset[(BrickSize*BrickIndex)+j]=pDataOut[j];
      }

      delete pFileData;
      delete [] pDataOut;
   }
}

Anatomy::Anatomy():
	m_voxelSize(Anatomy::Point(0.0,0.0,0.0)),
	m_isRGB(false),
	m_maxPixelValue(0),
	m_minPixelValue(0),
	m_dims(4,0)
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

    float* pixels = new float[m_dims[X]*m_dims[Y]*3];

    for(int y=0;y<m_dims[Y];y++)
    {
        for(int x=0;x<m_dims[X];x++)
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
                if(IndexColor!=0)
                {
                    int patate = 0;
                }
                pixels[(3 * m_dims[X] * y)+(3 * x)] = IndexColor;
                pixels[(3 * m_dims[X] * y)+(3 * x)+1] = IndexColor;
                pixels[(3 * m_dims[X] * y)+(3 * x)+2] = IndexColor;
            }
        }
    }

    glDrawPixels(m_dims[X],m_dims[Y],GL_RGB,GL_FLOAT,pixels);
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
        return ((3 * m_dims[X] * m_dims[Y] * az)+(3 * m_dims[X] * ay)+(3 * ax));
    }
    else
    {
        return((m_dims[Y] * m_dims[X] * az)+(m_dims[X] * ay)+ax);
    }
}

//////////////////////////////////////////////////////////////////////////

float Anatomy::getPixelValue(int aX, int aY, int aZ, int aColor) const
{
    int patat = getPixelIndex(aX,aY,aZ);
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

	if(extension == ".gz")
	{
	    extension = filename.substr(0,filename.find_last_of("."));
	    extension = filename.substr(extension.find_last_of("."));
	}

	if(extension == ".nii.gz" || (extension == ".nii")
	  || (extension == ".hdr") || (extension == ".img"))
	{
		bool LoadingSuccesful = false;
		//open header
		nifti_image* pImage = nifti_image_read(filename.c_str(),0);
		if(!pImage)
		{
			return false;
		}

		//size of the volume
		m_dims[X] = pImage->dim[Y];
		m_dims[Y]    = pImage->dim[Z];
		m_dims[Z]  = pImage->dim[W];
		m_dims[W]   = pImage->dim[4];

		int NbBrick = 1; // the number of volume 3D in the volume.

		//set the number of brick
		for(int i=4; i<=pImage->ndim; i++)
		{
			NbBrick *= pImage->dim[i];
		}

		//Voxel Size
		m_voxelSize[X] = pImage->dx;
		m_voxelSize[Y] = pImage->dy;
		m_voxelSize[Z] = pImage->dz;

		if(m_dims[W]<=0)
		{
		    m_dims[W]=1;
		}

		if(m_dims[Z]<=0)
		{
		    m_dims[Z]=1;
		}

		//Check what is the type of the file
		if(pImage->datatype==DT_UNSIGNED_CHAR/*2*/)
		{
			m_type=Type_UNSIGNED_CHAR;
		}
		else if(pImage->datatype==DT_SIGNED_SHORT/*4*/)
		{
			m_type=Type_SHORT;
		}
		else if(pImage->datatype==DT_SIGNED_INT/*8*/)
		{
			m_type=Type_INT;
		}
		else if(pImage->datatype== DT_FLOAT/*16*/ )
		{
			m_type=Type_FLOAT;
		}
		else if(pImage->datatype==DT_DOUBLE/*64*/)
		{
			m_type=Type_DOUBLE;
		}
		else
		{
			m_type=Type_VOID;
		}

		if(m_type==Type_VOID)
		{
			LoadingSuccesful=false;
		}

		//verif if the volume is in color or not
		if(m_dims[Z]==3)
		{
			m_isRGB=true;
		}
		else
		{
			m_isRGB=false;
		}

		delete pImage;

		//The size of the Vector
		int Size  = m_dims[X]*m_dims[Y]*m_dims[Z]*m_dims[W];
		m_floatDataset.resize(Size);

		//read pixel data
		switch(m_type)
		{
		case Type_UNSIGNED_CHAR:
		{
			m_maxPixelValue=255.0;
			_ReadNiftiBuffer<unsigned char>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
			LoadingSuccesful=true;
		}
		break;
		case Type_SHORT:
		{
			_ReadNiftiBuffer<short>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
			LoadingSuccesful=true;
		}
		break;
		case Type_INT:
		{
			_ReadNiftiBuffer<int>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
			LoadingSuccesful=true;
		}
		break;
		case Type_FLOAT:
		{
			_ReadNiftiBuffer<float>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
			LoadingSuccesful=true;
		}
		break;
		case Type_DOUBLE:
		{
			_ReadNiftiBuffer<double>(Size,m_maxPixelValue,m_minPixelValue,NbBrick,m_floatDataset,filename);
			LoadingSuccesful=true;
		}
		break;
		default:
		{
			return NULL;
		}
		break;
		}

		return LoadingSuccesful;
	}
	return false;
}
