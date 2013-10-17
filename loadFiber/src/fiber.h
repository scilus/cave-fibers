#pragma once
#include <string>

#include <vector>
#include <sstream>

#include <GL/glew.h>

///////////////////////////////////////////////////////////////////////////
// Enum representing the different type of fiber coloration mode.
///////////////////////////////////////////////////////////////////////////
enum FibersColorationMode
{
    NORMAL_COLOR        = 0,
    CURVATURE_COLOR     = 1,
    TORSION_COLOR       = 2,
    DISTANCE_COLOR      = 3,
    MINDISTANCE_COLOR   = 4,
    CUSTOM_COLOR        = 5,    // This one is used only for the mean fiber. Should be moved.
    CONSTANT_COLOR      = 6
};


class fiber
{
public:
	fiber(void);
	~fiber(void);

	// Fibers loading methods
    bool 	load( const std::string &filename );

    void    updateFibersColors();

    void 	updateLinesShown();

    void 	initializeBuffer() const;

    void 	draw() const;

    float   getFiberLength( const int fiberId ) const;

    void    resetColorArray();

    //getter setters
    int  	getLineCount() const;
    const std::vector< float >&	getPointArray() const;
	const std::vector< float >&	getColorArray() const;
	const std::vector< float >&	getNormalArray() const;

private:
    bool 	loadDmri(const std::string &filename );

	void 	createColorArray( const bool colorsLoadedFromFile );

	float	getPointValue( int  ptIndex );

	int  	getPointsPerLine(const int lineId ) const;
	int  	getStartIndexForLine( const int lineId ) const;

	void    drawFakeTubes();
	void    drawSortedLines();
	void   	drawCrossingFibers();

	void 	setShader();
	void 	releaseShader();

	int						m_countLines;
	int						m_countPoints;
	std::vector< int >		m_linePointers;
	std::vector< float >	m_pointArray;
	std::vector< float >	m_colorArray;
	std::vector< float >	m_normalArray;
	std::vector< int >		m_reverse;
    std::vector< bool >		m_selected;
    std::vector< bool >		m_filtered;
	int						m_type;
	std::string				m_fullPath;
	bool                  	m_fibersInverted;
	bool                  	m_isInitialized;
	bool                 	m_useFakeTubes;
	bool               		m_useTransparency;
	float                 	m_cachedThreshold;
	float       			m_threshold;
	bool            		m_useIntersectedFibers;
	FibersColorationMode  	m_fiberColorationMode;
	GLuint*     			m_bufferObjects;
	bool        			m_showFS;       // Show front sector for meshs.

};

