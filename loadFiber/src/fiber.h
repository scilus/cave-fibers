#pragma once
#include <string>

#include <vector>
#include <sstream>


class fiber
{
public:
	fiber(void);
	~fiber(void);

	// Fibers loading methods
    bool load( const std::string &filename );

private:
    bool loadDmri(const std::string &filename );
	void createColorArray( const bool colorsLoadedFromFile );
	int  getLineCount();
	int  getPointsPerLine(const int lineId );

	int						m_countLines;
	int						m_countPoints;
	std::vector< int >		m_linePointers;
	std::vector< float >	m_pointArray;
	std::vector< int >		m_reverse;
    std::vector< bool >		m_selected;
    std::vector< bool >		m_filtered;
	int						m_type;
	std::string				m_fullPath;
	std::vector< float >	m_colorArray;
	std::vector< float >	m_normalArray;
};

