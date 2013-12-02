#include "Anatomy.h"

#include "Nifti/nifti1_io.h"

//////////////////////////////////////////////////////////////////////////
//This function template read pixel data and put them in the vector of float.
//It read the file volume3D per volume3D. It get the max  and min value of
//pixel data.
//
//aSize Size of Vector of float.
//arMax The Max Value of Pixel Data.
//arMin The Min value of Pixel Data.
//aNbBrick The number of Volume3D in the Volume.
//arFloatDataset The Vector of float.
//arFileName The complete path of file selected.
//////////////////////////////////////////////////////////////////////////

template <typename TypeInput> void _ReadNiftiBuffer(int aSize,float& arMax,
                                    float& arMin, int aNbBrick,
                                    std::vector<float>& arFloatDataset,
                                    const std::string& arFileName)
{
   //read data brick per brick of Volume3D
   for(int BrickIndex=0;BrickIndex<aNbBrick;BrickIndex++)
   {
      //open a volume 3D per Volume3D
      int BList[1] = {BrickIndex};
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
      TypeInput* pDataOut = (TypeInput*)BrickList.bricks[0];
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
	m_bands(0),
	m_frames(0),
	m_rows(0),
	m_columns(0),
	m_isRGB(false),
	m_maxPixelValue(0),
	m_minPixelValue(0)
{

}

Anatomy::~Anatomy()
{
}

void Anatomy::draw() const
{

}

int Anatomy::getBands() const
{
	return m_bands;
}

int Anatomy::getColumns() const
{
	return m_columns;
}

const std::vector<float>& Anatomy::getFloatDataset() const {

	return m_floatDataset;
}

void Anatomy::setFloatDataset(const std::vector<float>& afloatDataset)
{
	m_floatDataset = afloatDataset;
}

int Anatomy::getFrames() const
{
	return m_frames;
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

int Anatomy::getRows() const
{
	return m_rows;
}

void Anatomy::setVoxelSize(const Point& avoxelSize)
{
	m_voxelSize = avoxelSize;
}

bool Anatomy::load( const std::string &filename )
{
	std::string extension = filename.substr(filename.find_last_of(".") + 1);
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
		m_columns = pImage->dim[1];
		m_rows    = pImage->dim[2];
		m_frames  = pImage->dim[3];
		m_bands   = pImage->dim[4];

		int NbBrick = 1; // the number of volume 3D in the volume.

		//set the number of brick
		for(int i=4; i<=pImage->ndim; i++)
		{
			NbBrick *= pImage->dim[i];
		}

		//Voxel Size
		m_voxelSize[0] = pImage->dx;
		m_voxelSize[1] = pImage->dy;
		m_voxelSize[2] = pImage->dz;

		if(m_bands<=0)
		{
			m_bands=1;
		}

		if(m_frames<=0)
		{
			m_frames=1;
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
		if(m_bands==3)
		{
			m_isRGB=true;
		}
		else
		{
			m_isRGB=false;
		}

		delete pImage;

		//The size of the Vector
		int Size  = m_columns*m_rows*m_frames*m_bands;
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
