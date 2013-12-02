#ifndef FIBER_H_
#define FIBER_H_

#include <string>

#include <vector>
#include <sstream>

#include <Geometry/Box.h>
#include "DataSetInfo.h"

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

class Fibers:public DataSetInfo
{
public:
	typedef DataSetInfo::Point Point;

    Fibers(void);
    ~Fibers(void);

    // Fibers loading methods
    virtual bool load( const std::string &filename );
    virtual void draw() const;

    void    updateFibersColors();

    void    resetLinesShown();
    void    updateLinesShown();

    void    initializeBuffer() const;

    void    drawFiber() const;
    void    drawFakeTubes() const;
    void    drawSortedLines() const;
    void    drawCrossingFibers() const;

    void     invertFibers();

    float   getFiberLength( const int fiberId ) const;

    void    resetColorArray();

    void    initDraw();

    void    findCrossingFibers();

    //getter setters
    int     getLineCount() const;
    int     getPointCount();
    bool    isSelected( int  fiberId );

    const std::vector< float >&    getPointArray() const;
    const std::vector< float >&    getColorArray() const;
    const std::vector< float >&    getNormalArray() const;
    std::vector< int > getReverseIdx() const;

    const bool& isUseFakeTubes() const;
    const bool& isUseTransparency() const;
    const bool& isUseIntersectedFibers() const;

    bool containsSelectionBox(Geometry::Box<float,3>  aBox);

    void setSelectedFiber(const std::vector<bool>& aSelectedFiber);

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
    bool                        m_fibersInverted;
    bool                        m_isInitialized;
    bool                        m_useFakeTubes;
    bool                        m_useTransparency;
    float                       m_cachedThreshold;
    bool                        m_useIntersectedFibers;
    FibersColorationMode        m_fiberColorationMode;

    Geometry::Box<float,3>  m_box;

};

#endif /* FIBER_H_ */
