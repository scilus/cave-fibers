#include "fiber.h"
#include <cmath>


fiber::fiber(void)
:   m_colorArray(),
    m_countLines( 0 ),
    m_countPoints( 0 ),
    m_linePointers(),
    m_pointArray(),
    m_normalArray(),
    m_reverse(),
    m_selected(),
    m_filtered()
{
}


fiber::~fiber(void)
{
}

bool fiber::load( const std::string &filename )
{
    bool res( false );

    //for now, just ".fib" extension are supported. later find a way to support many extention.
    res = loadDmri( filename );

    ///* OcTree points classification */
    //m_pOctree = new Octree( 2, m_pointArray, m_countPoints );
    
    return res;
}

bool fiber::loadDmri( const std::string &filename )
{
    FILE *pFile;
    pFile = fopen( filename.c_str(), "r" );

    if( pFile == NULL ) 
    {   
        return false;
    }

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
                
                curLine[j * 3]  = f1;
                curLine[j * 3 + 1] = f2;
                curLine[j * 3 + 2] = f3;
            }

            m_countPoints += curLine.size() / 3;
            lines.push_back( curLine );
        }
    }

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
    std::vector< std::vector< float > >::iterator it;

    for( it = lines.begin(); it < lines.end(); it++ )
    {
        std::vector< float >::iterator it2;

        for( it2 = ( *it ).begin(); it2 < ( *it ).end(); it2++ )
        {
            m_pointArray[pos++] = *it2;
        }
    }

    createColorArray( false );
    //m_type = FIBERS;
    m_fullPath = filename;
	
    return true;
}

void fiber::createColorArray( const bool colorsLoadedFromFile )
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

int fiber::getLineCount()
{
    return m_countLines;
}

int fiber::getPointsPerLine( const int lineId )
{
    return ( m_linePointers[lineId + 1] - m_linePointers[lineId] );
}
