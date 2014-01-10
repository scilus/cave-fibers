#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H


#include <vector>
#include "DataSetInfo.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

struct Triangle 
{
    unsigned int pointID[3];
};

class TriangleMesh 
{
public:
    typedef DataSetInfo::Point Point;
    typedef DataSetInfo::Color Color; // Data type for colors
    typedef DataSetInfo::Vector Vector; // Compatible vector type
    // Constructor / Destructor
    TriangleMesh();
    ~TriangleMesh();

    // Functions
    void fastAddVert( const Point newVert );

    void fastAddTriangle( const int vertA, const int vertB, const int vertC );

    void clearMesh();

    void reserveVerts    ( const int size );
    void reserveTriangles( const int size );
    void resizeVerts     ( const int size );
    void resizeTriangles ( const int size );
    int  getNumVertices();
    int  getNumTriangles();

    Point       getVertex       ( const int vertNum );
    Point       getVertex       ( const int triNum, int pos );
    Vector      getNormal       ( const int triNum  );
    Vector      getVertNormal   ( const int vertNum );
    Color       getVertColor    ( const int vertNum );
    Triangle    getTriangle     ( const int triNum  );
    Color       getTriangleColor( const int triNum  );

    std::vector< unsigned int > getStar( const int vertNum );
    std::vector< Point >       getVerts();

    void setVertex( const unsigned int vertNum, const Point nPos );
    void setVertColor( const int vertNum, const Color color );

    void   flipNormals();
        

private:
    // Functions
    Vector calcTriangleNormal( const Triangle );
    Vector calcTriangleNormal( const int triNum );
    Vector calcVertNormal( const int vertNum );
    void   calcVertNormals();

private:
    // Variables
    std::vector< Point >                        m_vertices;
    std::vector< Vector >                       m_vertNormals;
    std::vector< Color >                        m_vertColors;
    std::vector< std::vector< unsigned int > >  m_vIsInTriangle;

    std::vector< Triangle >                     m_triangles;
    std::vector< Vector >                       m_triNormals;
    std::vector< int >                          m_triangleTensor;
    std::vector< Color >                        m_triangleColor;
    std::vector< std::vector< int > >           m_neighbors;

    int    m_numVerts;
    int    m_numTris;

    bool m_vertNormalsCalculated;

    Color m_defaultColor;
};

#endif /* TRIANGLEMESH_H */
