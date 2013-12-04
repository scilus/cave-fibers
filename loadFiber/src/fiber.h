#ifndef FIBER_H_
#define FIBER_H_

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
    DISTANCE_COLOR      = 1,
    MINDISTANCE_COLOR   = 2,
    CUSTOM_COLOR        = 3,    // This one is used only for the mean fiber. Should be moved.
    CONSTANT_COLOR      = 4
};


class fiber
{
public:
    fiber(void);
    ~fiber(void);

    // Fibers loading methods
    bool    load( const std::string &filename );

    void    updateFibersColors();

    void    updateLinesShown();

    void    initializeBuffer();

    void    draw() const;

    void    drawFiber() const;
    void    drawFakeTubes() const;
    void    drawSortedLines() const;
    void    drawCrossingFibers() const;

    float   getFiberLength( const int fiberId ) const;

    void    resetColorArray();

    void    initDraw();

    void    findCrossingFibers();

    //getter setters
    int     getLineCount() const;

    const std::vector< float >&    getPointArray() const;
    const std::vector< float >&    getColorArray() const;
    const std::vector< float >&    getNormalArray() const;

    const bool& isUseFakeTubes() const;
    const bool& isUseTransparency() const;
    const bool& isUseIntersectedFibers() const;

private:
    bool    loadDmri(const std::string &filename );

    void    createColorArray( const bool colorsLoadedFromFile );

    float   getPointValue( int  ptIndex );

    int     getPointsPerLine(const int lineId ) const;
    int     getStartIndexForLine( const int lineId ) const;

    void    setShader();
    void    releaseShader();
    void    resetFiber();

    int                         m_countLines;
    int                         m_countPoints;
    std::vector< int >          m_linePointers;
    std::vector< float >        m_pointArray;
    std::vector< float >        m_colorArray;
    std::vector< float >        m_normalArray;
    std::vector< int >          m_reverse;
    std::vector< bool >         m_selected;
    std::vector< bool >         m_filtered;
    std::vector< unsigned int > m_cfStartOfLine;
    std::vector< unsigned int > m_cfPointsPerLine;
    int                         m_type;
    std::string                 m_fullPath;
    bool                        m_fibersInverted;
    bool                        m_isInitialized;
    bool                        m_useFakeTubes;
    bool                        m_useTransparency;
    float                       m_cachedThreshold;
    float                       m_threshold;
    bool                        m_useIntersectedFibers;
    FibersColorationMode        m_fiberColorationMode;
    GLuint*                     m_bufferObjects;
    bool                        m_showFS;       // Show front sector for meshs.

};

#endif /* FIBER_H_ */
