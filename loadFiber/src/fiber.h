#pragma once
#include <string>

#include <vector>
#include <sstream>

#define MIN(i,j)  ((i)<(j)?(i):(j))
#define MAX(i,j)  ((i)>(j)?(i):(j))

enum DatasetType 
{
    BOT_INITIALIZED = 0,
    MESH,
    VECTORS,
    TENSORS,
    ODFS,
    MAXIMAS,
    FIBERS,
    SURFACE,
    ISO_SURFACE,
    NOT_INITIALIZED,
    FIBERSGROUP
};


class fiber
{
public:
	fiber(void);
	~fiber(void);

	// Fibers loading methods
    bool load( const std::string &filename );

private:
	bool loadVTK(const std::string &filename );

	bool loadTRK(const std::string &filename );
    bool loadCamino(const std::string &filename );
    bool loadMRtrix(const std::string &filename );
    bool loadPTK(const std::string &filename );
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

