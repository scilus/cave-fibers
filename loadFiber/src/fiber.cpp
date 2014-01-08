#include "fiber.h"
#include <cmath>
#include <float.h>
#include <stdio.h>

Fibers::Fibers(void)
:   m_colorArray(),
    m_countLines( 0 ),
    m_countPoints( 0 ),
    m_linePointers(),
    m_pointArray(),
    m_normalArray(),
    m_cfStartOfLine(),
    m_cfPointsPerLine(),
    m_reverse(),
    m_selected(),
    m_filtered(),
    m_fibersInverted( false ),
    m_isInitialized( false ),
    m_useFakeTubes( false ),
    m_useTransparency( false ),
    m_useIntersectedFibers(false),
    m_fiberColorationMode( NORMAL_COLOR ),
    m_cachedThreshold( 0.0f )
{
    m_bufferObjects = new GLuint[3];
}

Fibers::~Fibers(void)
{
    m_linePointers.clear();
    m_reverse.clear();
    m_pointArray.clear();
    m_normalArray.clear();
    m_colorArray.clear();

    glDeleteBuffers( 3, m_bufferObjects );
}

bool Fibers::load( const std::string &filename )
{
    bool res( false );

    //for now, just ".fib" extension are supported. later find a way to support many extention.
    res = loadDmri( filename );
    
    if(res)
    {
        updateLinesShown();
    }

    return res;
}

void Fibers::updateFibersColors()
{
    if( m_fiberColorationMode == NORMAL_COLOR )
    {
        resetColorArray();
    }
}

void Fibers::resetLinesShown()
{
    m_selected.assign( m_countLines, false );
}

void Fibers::updateLinesShown()
{
    m_selected.assign( m_countLines, true );

    if( m_fibersInverted )
    {
        for( int k = 0; k < m_countLines; ++k )
        {
            m_selected[k] = !m_selected[k];
        }
    }
}

void Fibers::initializeBuffer() const
{
    if( m_isInitialized)
    {
        return;
    }

    glGenBuffers( 3, m_bufferObjects );
    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * m_countPoints * 3, &m_pointArray[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * m_countPoints * 3, &m_colorArray[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[2] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * m_countPoints * 3, &m_normalArray[0], GL_STATIC_DRAW );
}

void Fibers::initDraw()
{
    m_isInitialized = true;

    setShader();

    if( m_cachedThreshold != m_threshold )
    {
        updateFibersColors();
        m_cachedThreshold = m_threshold;
    }

    if(m_useIntersectedFibers)
    {
        findCrossingFibers();
    }
}

void Fibers::draw() const
{
    //draw fiber depending of the option choose by the user (It is not implemented for the moment)
    if(isUseFakeTubes())
    {
        drawFakeTubes();
    }
    else if(isUseTransparency())
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE );
        glDepthMask( GL_FALSE );
        drawSortedLines();
        glPopAttrib();
    }
    else if(isUseIntersectedFibers())
    {
        drawCrossingFibers();
    }
    else
    {
        drawFiber();
    }
}

void Fibers::drawFiber() const
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[0] );
    glVertexPointer( 3, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[1] );
    glColorPointer( 3, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[2] );
    glNormalPointer( GL_FLOAT, 0, 0 );

    for( int i = 0; i < m_countLines; ++i )
    {
        if( ( m_selected[i]) && !m_filtered[i] )
        {
            glDrawArrays( GL_LINE_STRIP, getStartIndexForLine( i ), getPointsPerLine( i ) );
        }
    }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );

    //releaseShader();
}

void Fibers::invertFibers()
{
    m_fibersInverted = !m_fibersInverted;
}

bool Fibers::loadDmri( const std::string &filename )
{
    FILE *pFile;
    pFile = fopen( filename.c_str(), "r" );

    if( pFile == NULL ) 
    {   
        return false;
    }

    //delete the previous fibers dataset before load a new one.
    resetFiber();

    char *pS1 = new char[10];
    char *pS2 = new char[10];
    char *pS3 = new char[10];
    char *pS4 = new char[10];
    float f1, f2, f3, f4, f5;
    int res;
    
    // the header
    res = fscanf( pFile, "%f %s", &f1, pS1 );
    res = fscanf( pFile, "%f %s %s %s %s", &f1, pS1, pS2, pS3, pS4 );
    res = fscanf( pFile, "%f", &f1 );
    res = fscanf( pFile, "%f %f %f %f %f", &f1, &f2, &f3, &f4, &f5 );
    res = fscanf( pFile, "%f %f %f %f %f", &f1, &f2, &f3, &f4, &f5 );
    res = fscanf( pFile, "%f %f %f %f %f", &f1, &f2, &f3, &f4, &f5 );
    res = fscanf( pFile, "%s %s", pS1, pS2 );
    m_countLines = std::atof(pS1);
    
    std::vector< std::vector< float > > lines;
    m_countPoints = 0;
    float back, front;
    std::stringstream ss;

    float minX = std::numeric_limits< float >::max();
    float maxX = std::numeric_limits< float >::min();
    float minY = std::numeric_limits< float >::max();
    float maxY = std::numeric_limits< float >::min();
    float minZ = std::numeric_limits< float >::max();
    float maxZ = std::numeric_limits< float >::min();
    float meanX = 0;
    float meanY = 0;
    float meanZ = 0;

    for( int i = 0; i < m_countLines; i++ )
    {
        res = fscanf( pFile, "%s %s %s", pS1, pS2, pS3 );

        ss << pS1;
        ss >> back;
        ss.clear();
        ss << pS2;
        ss >> front;
        ss.clear();
        
        int nbpoints = back + front;

        if( back != 0 && front != 0 )
        {
            nbpoints--;
        }

        if( nbpoints > 0 )
        {
            std::vector< float > curLine;
            curLine.resize( nbpoints * 3 );
            //back
            for( int j = back - 1; j >= 0; j-- )
            {
                res = fscanf( pFile, "%s %s %s %s", pS1, pS2, pS3, pS4 );
              
                ss << pS1;
                ss >> f1;
                ss.clear();

                ss << pS2;
                ss >> f2;
                ss.clear();
                
                ss << pS3;
                ss >> f3;
                ss.clear();
                
                minX = std::min(minX,f1);
                maxX = std::max(maxX,f1);
                minY = std::min(minY,f2);
                maxY = std::max(maxY,f2);
                minZ = std::min(minZ,f3);
                maxZ = std::max(maxZ,f3);

                curLine[j * 3]  = f1;
                curLine[j * 3 + 1] = f2;
                curLine[j * 3 + 2] = f3;
            }

            if( back != 0 && front != 0 )
            {
                //repeated pts
                res = fscanf( pFile, "%s %s %s %s", pS1, pS2, pS3, pS4 );
            }

            //front
            for( int j = back; j < nbpoints; j++ )
            {
                res = fscanf( pFile, "%s %s %s %s", pS1, pS2, pS3, pS4 );
                
                ss << pS1;
                ss >> f1;
                ss.clear();

                ss << pS2;
                ss >> f2;
                ss.clear();
                
                ss << pS3;
                ss >> f3;
                ss.clear();
                
                minX = std::min(minX,f1);
                maxX = std::max(maxX,f1);
                minY = std::min(minY,f2);
                maxY = std::max(maxY,f2);
                minZ = std::min(minZ,f3);
                maxZ = std::max(maxZ,f3);

                curLine[j * 3]  = f1;
                curLine[j * 3 + 1] = f2;
                curLine[j * 3 + 2] = f3;
            }

            m_countPoints += curLine.size() / 3;
            lines.push_back( curLine );
        }
    }

    meanX = (maxX+minX)/2.0;
    meanY = (maxY+minY)/2.0;
    meanZ = (maxZ+minZ)/2.0;

    fclose( pFile );
    
    delete[] pS1;
    delete[] pS2;
    delete[] pS3;
    delete[] pS4;
    pS1 = NULL;
    pS2 = NULL;
    pS3 = NULL;
    pS4 = NULL;
    
    //set all the data in the right format for the navigator
    m_countLines = lines.size();
    m_pointArray.max_size();
    m_linePointers.resize( m_countLines + 1 );
    m_pointArray.resize( m_countPoints * 3 );
    m_linePointers[m_countLines] = m_countPoints;
    m_reverse.resize( m_countPoints );
    m_selected.resize( m_countLines, false );
    m_filtered.resize( m_countLines, false );
    m_linePointers[0] = 0;

    for( int i = 0; i < m_countLines; ++i )
    {
        m_linePointers[i + 1] = m_linePointers[i] + lines[i].size() / 3;
    }

    int lineCounter = 0;

    for( int i = 0; i < m_countPoints; ++i )
    {
        if( i == m_linePointers[lineCounter + 1] )
        {
            ++lineCounter;
        }

        m_reverse[i] = lineCounter;
    }

    unsigned int pos = 0;
    unsigned int index = 0;
    float        mean = meanX;
    std::vector< std::vector< float > >::iterator it;

    for( it = lines.begin(); it < lines.end(); it++ )
    {
        std::vector< float >::iterator it2;
        index = 0;

        for( it2 = ( *it ).begin(); it2 < ( *it ).end(); it2++ )
        {
            mean = meanX;
            if(index%3 == 1)
            {
                mean = meanY;
            }
            else if(index%3 == 2)
            {
                mean = meanZ;
            }
            m_pointArray[pos++] = *it2 - mean;
            index++;
        }
    }

    m_max[0] = maxX - meanX;
    m_max[1] = maxY - meanY;
    m_max[2] = maxZ - meanZ;

    m_min[0] = minX - meanX;
    m_min[1] = minY - meanY;
    m_min[2] = minZ - meanZ;

    m_box = Geometry::Box<float,3>(m_min,m_max);

    createColorArray( false );
    //m_type = FIBERS;
    m_fullPath = filename;
    m_name = filename;

    const size_t last_slash_idx = m_fullPath.rfind('\\');
    if (std::string::npos != last_slash_idx)
    {
        m_name = m_fullPath.substr(last_slash_idx, m_fullPath.size());
    }

    return true;
}

void Fibers::createColorArray(const bool colorsLoadedFromFile)
{
    if( !colorsLoadedFromFile )
    {
        m_colorArray.clear();
        m_colorArray.resize( m_countPoints * 3 );
    }

    m_normalArray.clear();
    m_normalArray.resize( m_countPoints * 3 );
    int   pc = 0;
    
    float x1, x2, y1, y2, z1, z2 = 0.0f;
    float r, g, b, rr, gg, bb    = 0.0f;
    float lastX, lastY, lastZ          = 0.0f;

    for( int i = 0; i < getLineCount(); ++i )
    {
        x1 = m_pointArray[pc];
        y1 = m_pointArray[pc + 1];
        z1 = m_pointArray[pc + 2];
        x2 = m_pointArray[pc + getPointsPerLine( i ) * 3 - 3];
        y2 = m_pointArray[pc + getPointsPerLine( i ) * 3 - 2];
        z2 = m_pointArray[pc + getPointsPerLine( i ) * 3 - 1];
        r = ( x1 ) - ( x2 );
        g = ( y1 ) - ( y2 );
        b = ( z1 ) - ( z2 );

        if( r < 0.0 )
        {
            r *= -1.0;
        }

        if( g < 0.0 )
        {
            g *= -1.0;
        }

        if( b < 0.0 )
        {
            b *= -1.0;
        }

        float norm = sqrt( r * r + g * g + b * b );
        r *= 1.0 / norm;
        g *= 1.0 / norm;
        b *= 1.0 / norm;
        
        lastX = m_pointArray[pc]     + ( m_pointArray[pc]     - m_pointArray[pc + 3] );
        lastY = m_pointArray[pc + 1] + ( m_pointArray[pc + 1] - m_pointArray[pc + 4] );
        lastZ = m_pointArray[pc + 2] + ( m_pointArray[pc + 2] - m_pointArray[pc + 5] );

        for( int j = 0; j < getPointsPerLine( i ); ++j )
        {
            rr = lastX - m_pointArray[pc];
            gg = lastY - m_pointArray[pc + 1];
            bb = lastZ - m_pointArray[pc + 2];
            lastX = m_pointArray[pc];
            lastY = m_pointArray[pc + 1];
            lastZ = m_pointArray[pc + 2];

            if( rr < 0.0 )
            {
                rr *= -1.0;
            }

            if( gg < 0.0 )
            {
                gg *= -1.0;
            }

            if( bb < 0.0 )
            {
                bb *= -1.0;
            }

            float norm = sqrt( rr * rr + gg * gg + bb * bb );
            rr *= 1.0 / norm;
            gg *= 1.0 / norm;
            bb *= 1.0 / norm;
            m_normalArray[pc]     = rr;
            m_normalArray[pc + 1] = gg;
            m_normalArray[pc + 2] = bb;

            if( ! colorsLoadedFromFile )
            {
                m_colorArray[pc]     = r;
                m_colorArray[pc + 1] = g;
                m_colorArray[pc + 2] = b;
            }

            pc += 3;
        }
    }
}

//method not tested
void Fibers::resetColorArray()
{
    float *pColorData( NULL );
    float *pColorData2( &m_colorArray[0] );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[1] );
    pColorData = ( float * ) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_WRITE );

    int pc = 0;
    float r, g, b, x1, x2, y1, y2, z1, z2, lastX, lastY, lastZ;

    for( int i = 0; i < getLineCount(); ++i )
    {
        x1 = m_pointArray[pc];
        y1 = m_pointArray[pc + 1];
        z1 = m_pointArray[pc + 2];
        x2 = m_pointArray[pc + getPointsPerLine( i ) * 3 - 3];
        y2 = m_pointArray[pc + getPointsPerLine( i ) * 3 - 2];
        z2 = m_pointArray[pc + getPointsPerLine( i ) * 3 - 1];
        r = ( x1 ) - ( x2 );
        g = ( y1 ) - ( y2 );
        b = ( z1 ) - ( z2 );

        if( r < 0.0 )
        {
            r *= -1.0;
        }

        if( g < 0.0 )
        {
            g *= -1.0;
        }

        if( b < 0.0 )
        {
            b *= -1.0;
        }

        float norm = sqrt( r * r + g * g + b * b );
        r *= 1.0 / norm;
        g *= 1.0 / norm;
        b *= 1.0 / norm;

        lastX = m_pointArray[pc] + ( m_pointArray[pc] - m_pointArray[pc + 3] );
        lastY = m_pointArray[pc + 1] + ( m_pointArray[pc + 1] - m_pointArray[pc + 4] );
        lastZ = m_pointArray[pc + 2] + ( m_pointArray[pc + 2] - m_pointArray[pc + 5] );

        for( int j = 0; j < getPointsPerLine( i ); ++j )
        {
            pColorData[pc] = r;
            pColorData[pc + 1] = g;
            pColorData[pc + 2] = b;
            pColorData2[pc] = r;
            pColorData2[pc + 1] = g;
            pColorData2[pc + 2] = b;
            pc += 3;
        }
    }

    glUnmapBuffer( GL_ARRAY_BUFFER );

    m_fiberColorationMode = NORMAL_COLOR;
}

//method not tested
void Fibers::drawCrossingFibers() const
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[0] );
    glVertexPointer( 3, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[1] );
    glColorPointer( 3, GL_FLOAT, 0, 0 );


    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[2] );
    glNormalPointer( GL_FLOAT, 0, 0 );

    for( unsigned int i = 0; i < m_cfStartOfLine.size(); ++i )
    {
        if ( 1 < m_cfPointsPerLine[i] )
        {
            glDrawArrays( GL_LINE_STRIP, m_cfStartOfLine[i], m_cfPointsPerLine[i] );
        }
    }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
}

void Fibers::findCrossingFibers()
{
    /*if (   m_cfDrawDirty
        || m_xDrawn != SceneManager::getInstance()->getSliceX()
        || m_yDrawn != SceneManager::getInstance()->getSliceY()
        || m_zDrawn != SceneManager::getInstance()->getSliceZ()
        || m_axialShown    != SceneManager::getInstance()->isAxialDisplayed()
        || m_coronalShown  != SceneManager::getInstance()->isCoronalDisplayed()
        || m_sagittalShown != SceneManager::getInstance()->isSagittalDisplayed() )
    {
        m_xDrawn = SceneManager::getInstance()->getSliceX();
        m_yDrawn = SceneManager::getInstance()->getSliceY();
        m_zDrawn = SceneManager::getInstance()->getSliceZ();
        m_axialShown    = SceneManager::getInstance()->isAxialDisplayed();
        m_coronalShown  = SceneManager::getInstance()->isCoronalDisplayed();
        m_sagittalShown = SceneManager::getInstance()->isSagittalDisplayed();

        float xVoxSize = DatasetManager::getInstance()->getVoxelX();
        float yVoxSize = DatasetManager::getInstance()->getVoxelY();
        float zVoxSize = DatasetManager::getInstance()->getVoxelZ();

        m_cfDrawDirty = true;

        // Determine X, Y and Z range
        const float xMin( (m_xDrawn + 0.5f) * xVoxSize - m_thickness );
        const float xMax( (m_xDrawn + 0.5f) * xVoxSize + m_thickness );
        const float yMin( (m_yDrawn + 0.5f) * yVoxSize - m_thickness );
        const float yMax( (m_yDrawn + 0.5f) * yVoxSize + m_thickness );
        const float zMin( (m_zDrawn + 0.5f) * zVoxSize - m_thickness );
        const float zMax( (m_zDrawn + 0.5f) * zVoxSize + m_thickness );

        bool lineStarted(false);

        m_cfStartOfLine.clear();
        m_cfPointsPerLine.clear();

        unsigned int index( 0 );
        unsigned int point( 0 );
        for ( unsigned int line( 0 ); line < static_cast<unsigned int>(m_countLines); ++line )
        {
            if ( m_selected[line] && !m_filtered[line] )
            {
                for ( unsigned int i( 0 ); i < static_cast<unsigned int>(getPointsPerLine(line)); ++i, ++point, index += 3 )
                {
                    if ( m_sagittalShown && xMin <= m_pointArray[index] && xMax >= m_pointArray[index] )
                    {
                        if ( !lineStarted )
                        {
                            m_cfStartOfLine.push_back(point);
                            m_cfPointsPerLine.push_back(0);
                            lineStarted = true;
                        }
                        ++m_cfPointsPerLine.back();
                    }
                    else if ( m_coronalShown && yMin <= m_pointArray[index + 1] && yMax >= m_pointArray[index + 1] )
                    {
                        if ( !lineStarted )
                        {
                            m_cfStartOfLine.push_back(point);
                            m_cfPointsPerLine.push_back(0);
                            lineStarted = true;
                        }
                        ++m_cfPointsPerLine.back();
                    }
                    else if ( m_axialShown && zMin <= m_pointArray[index + 2] && zMax >= m_pointArray[index + 2] )
                    {
                        if ( !lineStarted )
                        {
                            m_cfStartOfLine.push_back(point);
                            m_cfPointsPerLine.push_back(0);
                            lineStarted = true;
                        }
                        ++m_cfPointsPerLine.back();
                    }
                    else
                    {
                        lineStarted = false;
                    }
                }
                lineStarted = false;
            }
            else
            {
                point += getPointsPerLine(line);
                index += getPointsPerLine(line) * 3;
            }
        }
    }*/
}

int Fibers::getLineCount() const
{
    return m_countLines;
}

int Fibers::getPointCount()
{
    return m_countPoints;
}

bool Fibers::isSelected( int  fiberId )
{
    return m_selected[fiberId];
}

const std::vector< float >& Fibers::getPointArray() const
{
    return m_pointArray;
}

const std::vector< float >& Fibers::getColorArray() const
{
    return m_colorArray;
}

const std::vector< float >& Fibers::getNormalArray() const
{
    return m_normalArray;
}

std::vector< int > Fibers::getReverseIdx() const
{
    return m_reverse;
}

const bool& Fibers::isUseFakeTubes() const
{
    return m_useFakeTubes;
}

const bool& Fibers::isUseTransparency() const
{
    return m_useTransparency;
}

const bool& Fibers::isUseIntersectedFibers() const
{
    return m_useIntersectedFibers;
}

float Fibers::getPointValue( int ptIndex )
{
    return m_pointArray[ptIndex];
}

int Fibers::getPointsPerLine( const int lineId ) const
{
    return ( m_linePointers[lineId + 1] - m_linePointers[lineId] );
}

int Fibers::getStartIndexForLine( const int lineId ) const
{
    return m_linePointers[lineId];
}

bool Fibers::containsSelectionBox(Geometry::Box<float,3>  aBox)
{
    if(m_box.contains(aBox))
    {
        return true;
    }
    return false;
}

void Fibers::setSelectedFiber(const std::vector<bool>& aSelectedFiber)
{
    m_selected = aSelectedFiber;
}

void Fibers::setShader()
{
    /*DatasetInfo *pDsInfo = (DatasetInfo*) this;

    if( m_useFakeTubes )
    {
        ShaderHelper::getInstance()->getFakeTubesShader()->bind();
        ShaderHelper::getInstance()->getFakeTubesShader()->setUniInt  ( "globalColor", getShowFS() );
        ShaderHelper::getInstance()->getFakeTubesShader()->setUniFloat( "dimX", (float)MyApp::frame->m_pMainGL->GetSize().x );
        ShaderHelper::getInstance()->getFakeTubesShader()->setUniFloat( "dimY", (float)MyApp::frame->m_pMainGL->GetSize().y );
        ShaderHelper::getInstance()->getFakeTubesShader()->setUniFloat( "thickness", GLfloat( 3.175 ) );
    }
    else if( SceneManager::getInstance()->isFibersGeomShaderActive() && m_useIntersectedFibers )
    {
        // Determine X, Y and Z range
        int curSliceX = SceneManager::getInstance()->getSliceX();
        int curSliceY = SceneManager::getInstance()->getSliceY();
        int curSliceZ = SceneManager::getInstance()->getSliceZ();

        float xVoxSize = DatasetManager::getInstance()->getVoxelX();
        float yVoxSize = DatasetManager::getInstance()->getVoxelY();
        float zVoxSize = DatasetManager::getInstance()->getVoxelZ();

        const float xMin( ( curSliceX + 0.5f ) * xVoxSize - m_thickness );
        const float xMax( ( curSliceX + 0.5f ) * xVoxSize + m_thickness );
        const float yMin( ( curSliceY + 0.5f ) * yVoxSize - m_thickness );
        const float yMax( ( curSliceY + 0.5f ) * yVoxSize + m_thickness );
        const float zMin( ( curSliceZ + 0.5f ) * zVoxSize - m_thickness );
        const float zMax( ( curSliceZ + 0.5f ) * zVoxSize + m_thickness );

        ShaderHelper::getInstance()->getCrossingFibersShader()->bind();

        ShaderHelper::getInstance()->getCrossingFibersShader()->setUniFloat("xMin", SceneManager::getInstance()->isSagittalDisplayed() ? xMin : 0 );
        ShaderHelper::getInstance()->getCrossingFibersShader()->setUniFloat("xMax", SceneManager::getInstance()->isSagittalDisplayed() ? xMax : 0 );

        ShaderHelper::getInstance()->getCrossingFibersShader()->setUniFloat("yMin", SceneManager::getInstance()->isCoronalDisplayed() ? yMin : 0 );
        ShaderHelper::getInstance()->getCrossingFibersShader()->setUniFloat("yMax", SceneManager::getInstance()->isCoronalDisplayed() ? yMax : 0 );

        ShaderHelper::getInstance()->getCrossingFibersShader()->setUniFloat("zMin", SceneManager::getInstance()->isAxialDisplayed() ? zMin : 0 );
        ShaderHelper::getInstance()->getCrossingFibersShader()->setUniFloat("zMax", SceneManager::getInstance()->isAxialDisplayed() ? zMax : 0 );
    }
    else if ( !m_useTex )
    {
        ShaderHelper::getInstance()->getFibersShader()->bind();
        ShaderHelper::getInstance()->setFiberShaderVars();
        ShaderHelper::getInstance()->getFibersShader()->setUniInt( "useTex", !pDsInfo->getUseTex() );
//         ShaderHelper::getInstance()->getFibersShader()->setUniInt( "useColorMap", SceneManager::getInstance()->getColorMap() );
        ShaderHelper::getInstance()->getFibersShader()->setUniInt( "useOverlay", pDsInfo->getShowFS() );
    }*/
}

void Fibers::releaseShader()
{
    /*if( m_useFakeTubes )
    {
        ShaderHelper::getInstance()->getFakeTubesShader()->release();
    }
    else if( SceneManager::getInstance()->isFibersGeomShaderActive() && m_useIntersectedFibers )
    {
        ShaderHelper::getInstance()->getCrossingFibersShader()->release();
    }
    else if( !m_useTex )
    {
        ShaderHelper::getInstance()->getFibersShader()->release();
    }*/
}

void Fibers::resetFiber()
{
    //clear all vector
    m_linePointers.clear();
    m_reverse.clear();
    m_pointArray.clear();
    m_normalArray.clear();
    m_colorArray.clear();
    m_cfStartOfLine.clear();
    m_cfPointsPerLine.clear();
    m_selected.clear();
    m_filtered.clear();

    m_countLines = 0;
    m_countPoints = 0;
    m_isInitialized = false;
    m_fibersInverted = false;
    m_isInitialized = false;
    m_useFakeTubes = false;
    m_useTransparency = false;
    m_useIntersectedFibers = false;
    m_threshold = 0.0f;
    m_fiberColorationMode = NORMAL_COLOR;
    m_cachedThreshold = 0.0f;

    //m_bufferObjects = new GLuint[3];
}

//method not tested
void Fibers::drawFakeTubes() const
{
    /*if( ! m_normalsPositive )
    {
        switchNormals( false );
    }*/

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if ( m_useIntersectedFibers )
    {
        for( unsigned int i = 0; i < m_cfStartOfLine.size(); ++i )
        {
            if ( 3 < m_cfPointsPerLine[i] )
            {
                int index = m_cfStartOfLine[i] * 3;
                glBegin( GL_QUAD_STRIP );

                for( unsigned int k = 0; k < m_cfPointsPerLine[i]; ++k, index += 3 )
                {
                    glNormal3f( m_normalArray[index], m_normalArray[index + 1], m_normalArray[index + 2] );
                    glColor3f( m_colorArray[index],  m_colorArray[index + 1],  m_colorArray[index + 2] );
                    glTexCoord2f( -1.0f, 0.0f );
                    glVertex3f( m_pointArray[index], m_pointArray[index + 1], m_pointArray[index + 2] );
                    glTexCoord2f( 1.0f, 0.0f );
                    glVertex3f( m_pointArray[index], m_pointArray[index + 1], m_pointArray[index + 2] );
                }

                glEnd();
            }
        }
    }
    else
    {
        for( int i = 0; i < m_countLines; ++i )
        {
            if( m_selected[i] && !m_filtered[i] )
            {
                int idx = getStartIndexForLine( i ) * 3;
                glBegin( GL_QUAD_STRIP );

                for( int k = 0; k < getPointsPerLine( i ); ++k )
                {
                    glNormal3f( m_normalArray[idx], m_normalArray[idx + 1], m_normalArray[idx + 2] );
                    glColor3f( m_colorArray[idx],  m_colorArray[idx + 1],  m_colorArray[idx + 2] );
                    glTexCoord2f( -1.0f, 0.0f );
                    glVertex3f( m_pointArray[idx], m_pointArray[idx + 1], m_pointArray[idx + 2] );
                    glTexCoord2f( 1.0f, 0.0f );
                    glVertex3f( m_pointArray[idx], m_pointArray[idx + 1], m_pointArray[idx + 2] );
                    idx += 3;
                }

                glEnd();
            }
        }
    }
}

//method not tested
void Fibers::drawSortedLines() const
{
    // Only sort those lines we see.
    unsigned int *pSnippletSort = NULL;
    unsigned int *pLineIds      = NULL;

    int nbSnipplets = 0;

    // Estimate memory required for arrays.
    for( int i = 0; i < m_countLines; ++i )
    {
        if( m_selected[i] && !m_filtered[i] )
        {
            nbSnipplets += getPointsPerLine( i ) - 1;
        }
    }

    pSnippletSort = new unsigned int[nbSnipplets + 1]; // +1 just to be sure because of fancy problems with some sort functions.
    pLineIds      = new unsigned int[nbSnipplets * 2];
    // Build data structure for sorting.
    int snp = 0;

    for( int i = 0; i < m_countLines; ++i )
    {
        if( !( m_selected[i] && !m_filtered[i] ) )
        {
            continue;
        }

        const unsigned int p = getPointsPerLine( i );

        // TODO: update pLineIds and pSnippletSort size only when fiber selection changes.
        for( unsigned int k = 0; k < p - 1; ++k )
        {
            pLineIds[snp << 1] = getStartIndexForLine( i ) + k;
            pLineIds[( snp << 1 ) + 1] = getStartIndexForLine( i ) + k + 1;
            pSnippletSort[snp] = snp;
            snp++;
        }
    }

    GLfloat projMatrix[16];
    glGetFloatv( GL_PROJECTION_MATRIX, projMatrix );

    // Compute z values of lines (in our case: starting points only).
    std::vector< float > zVals( nbSnipplets );

    for( int i = 0; i < nbSnipplets; ++i )
    {
        const int id = pLineIds[i << 1] * 3;
        zVals[i] = ( m_pointArray[id + 0] * projMatrix[2] + m_pointArray[id + 1] * projMatrix[6]
                      + m_pointArray[id + 2] * projMatrix[10] + projMatrix[14] ) / ( m_pointArray[id + 0] * projMatrix[3]
                              + m_pointArray[id + 1] * projMatrix[7] + m_pointArray[id + 2] * projMatrix[11] + projMatrix[15] );
    }

    //sort( &pSnippletSort[0], &pSnippletSort[nbSnipplets], IndirectComp< vector< float > > ( zVals ) );

    float *pColors  = NULL;
    float *pNormals = NULL;

    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[1] );
    pColors = ( float * ) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_ONLY );
    glUnmapBuffer( GL_ARRAY_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, m_bufferObjects[2] );
    pNormals = ( float * ) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_ONLY );
    glUnmapBuffer( GL_ARRAY_BUFFER );

    /*if( SceneManager::getInstance()->isPointMode() )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }*/

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glBegin( GL_LINES );

    if( m_fiberColorationMode == MINDISTANCE_COLOR )
    {
        int i = 0;

        for( int c = 0; c < nbSnipplets; ++c )
        {
            i = c;
            int idx  = pLineIds[pSnippletSort[i] << 1];
            int idx3 = idx * 3;
            int id2  = pLineIds[( pSnippletSort[i] << 1 ) + 1];
            int id23 = id2 * 3;
            //glColor4f(  pColors[idx3 + 0],       pColors[idx3 + 1],       pColors[idx3 + 2],   m_localizedAlpha[idx] * m_alpha );
            glNormal3f( pNormals[idx3 + 0],      pNormals[idx3 + 1],      pNormals[idx3 + 2] );
            glVertex3f( m_pointArray[idx3 + 0],  m_pointArray[idx3 + 1],  m_pointArray[idx3 + 2] );
            //glColor4f(  pColors[id23 + 0],       pColors[id23 + 1],       pColors[id23 + 2],   m_localizedAlpha[id2] * m_alpha );
            glNormal3f( pNormals[id23 + 0],      pNormals[id23 + 1],      pNormals[id23 + 2] );
            glVertex3f( m_pointArray[id23 + 0],  m_pointArray[id23 + 1],  m_pointArray[id23 + 2] );
        }
    }
    else
    {
        int i = 0;

        for( int c = 0; c < nbSnipplets; ++c )
        {
            i = c;
            int idx  = pLineIds[pSnippletSort[i] << 1];
            int idx3 = idx * 3;
            int id2  = pLineIds[( pSnippletSort[i] << 1 ) + 1];
            int id23 = id2 * 3;
            //glColor4f(  pColors[idx3 + 0],       pColors[idx3 + 1],       pColors[idx3 + 2],   m_alpha );
            glNormal3f( pNormals[idx3 + 0],      pNormals[idx3 + 1],      pNormals[idx3 + 2] );
            glVertex3f( m_pointArray[idx3 + 0],  m_pointArray[idx3 + 1],  m_pointArray[idx3 + 2] );
            //glColor4f(  pColors[id23 + 0],       pColors[id23 + 1],       pColors[id23 + 2],   m_alpha );
            glNormal3f( pNormals[id23 + 0],      pNormals[id23 + 1],      pNormals[id23 + 2] );
            glVertex3f( m_pointArray[id23 + 0],  m_pointArray[id23 + 1],  m_pointArray[id23 + 2] );
        }
    }

    glEnd();
    glDisable( GL_BLEND );

    delete[] pSnippletSort;
    delete[] pLineIds;
}
