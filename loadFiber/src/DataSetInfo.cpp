#include "DataSetInfo.h"

DataSetInfo::DataSetInfo():
    m_type         ( TYPE_VOID ),
    m_name         (""),
    m_fullPath     (""),
    m_threshold    ( 0.0f ),
    m_show         ( true ),
	m_max(3,0),
	m_min(3,0),
    m_bufferObjects( 0 )
{

}

DataSetInfo::~DataSetInfo()
{
}

std::string DataSetInfo::getName() const
{
	return m_name;
}

std::string DataSetInfo::getPath() const
{
	return m_fullPath;
}

void DataSetInfo::setName(std::string name)
{
	m_name = name;
}

float DataSetInfo::getThreshold() const
{
	return m_threshold;
}
void  DataSetInfo::setThreshold(float value)
{
	m_threshold = value;
}

bool DataSetInfo::toggleShow()
{
	m_show = !m_show;
	return m_show;
}

void DataSetInfo::setShow ( bool i_show   )
{
	m_show = i_show;
}

bool DataSetInfo::getShow() const
{
	return m_show;
};

DataSetInfo::Point DataSetInfo::getBBMax() const
{
	return m_max;
}

DataSetInfo::Point DataSetInfo::getBBMin() const
{
	return m_min;
}

DatasetType DataSetInfo::getType() const
{
	return m_type;
}

void DataSetInfo::setType(DatasetType atype)
{
	m_type = atype;
}

const DataSetInfo::Color& DataSetInfo::getColor() const
{
    return m_color;
}

void DataSetInfo::setColor(const Color& color)
{
    this->m_color = color;
}
