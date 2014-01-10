// TriangleMesh.cpp
#include "TriangleMesh.h"

#include <algorithm>
using std::for_each;

#include <string>

/*namespace
{
    class TriangleTensorCalculator
    {
    public:
        TriangleTensorCalculator( TriangleMesh *pTriMesh, float columns, float rows, float frames )
        :   m_pTriMesh( pTriMesh ),
            m_columns( columns ),
            m_frames( frames ),
            m_rows( rows )
        {
        }

        int operator()( const int i )
        {
            using std::min;
            using std::max;

            TriangleMesh::Vector p = m_pTriMesh->getTriangleCenter( i );
            int x = min( (int)(m_columns - 1), max( 0, (int)( p[0] + 0.5 ) ) );
            int y = min( (int)(m_rows    - 1), max( 0, (int)( p[1] + 0.5 ) ) );
            int z = min( (int)(m_frames  - 1), max( 0, (int)( p[2] + 0.5 ) ) );
            return x + y * m_columns + z * m_columns * m_rows;
        }
    private:
        TriangleTensorCalculator();

    private:
        TriangleMesh *m_pTriMesh;
        const float m_columns;
        const float m_frames;
        const float m_rows;
    };
}*/


// Construction
TriangleMesh::TriangleMesh()
:   m_numVerts( 0 ),
    m_numTris( 0 ),
    //m_isCleaned( false ),
    m_vertNormalsCalculated( false ),
    //m_neighborsCalculated( false ),
    //m_triangleTensorsCalculated( false )//,
    m_defaultColor( 200, 200, 200, 255 )
{
}

TriangleMesh::~TriangleMesh ()
{
    clearMesh();
}

void TriangleMesh::clearMesh()
{
    m_vertices.clear();
    m_vertNormals.clear();
    m_vertColors.clear();
    m_vIsInTriangle.clear();

    m_triangles.clear();
    m_triangleTensor.clear();
    m_triangleColor.clear();
    m_neighbors.clear();
    m_triNormals.clear();

    m_numVerts     = 0;
    m_numTris      = 0;

    m_vertNormalsCalculated = false;
}

void TriangleMesh::fastAddVert(const Point newVert)
{
    m_vertices[m_numVerts] = newVert ;
    ++m_numVerts;
}

void TriangleMesh::resizeVerts(const int size)
{
    m_vertices.resize(size);
    m_vIsInTriangle.resize(size);
}

void TriangleMesh::fastAddTriangle(const int vertA, const int vertB, const int vertC)
{
    Triangle t = {{vertA, vertB, vertC}};
    m_triangles[m_numTris] = t;
    m_triNormals[m_numTris] = calcTriangleNormal(t);
    m_vIsInTriangle[vertA].push_back(m_numTris);
    m_vIsInTriangle[vertB].push_back(m_numTris);
    m_vIsInTriangle[vertC].push_back(m_numTris);
    ++m_numTris;
}

void TriangleMesh::resizeTriangles(const int size)
{
    m_triangles.resize(size);
    m_triNormals.resize(size);
    m_triangleTensor.resize(size,0);
    m_triangleColor.resize(size, m_defaultColor);
    std::vector<int> v(3,-1);
    m_neighbors.resize(size, v);
}

TriangleMesh::Vector TriangleMesh::calcTriangleNormal(const Triangle t)
{
    Vector v1 = m_vertices[t.pointID[1]] - m_vertices[t.pointID[0]];
    Vector v2 = m_vertices[t.pointID[2]] - m_vertices[t.pointID[0]];

    Vector tempNormal = Vector(v1[1]*v2[2]-v1[2]*v2[1],v1[2]*v2[0]-v1[0]*v2[2],v1[0]*v2[1]-v1[1]*v2[0]);
    tempNormal.normalize();
    return tempNormal;
}

TriangleMesh::Vector TriangleMesh::calcTriangleNormal(const int triNum)
{

    Vector v1 = m_vertices[m_triangles[triNum].pointID[1]] - m_vertices[m_triangles[triNum].pointID[0]];
    Vector v2 = m_vertices[m_triangles[triNum].pointID[2]] - m_vertices[m_triangles[triNum].pointID[0]];

    Vector tempNormal = Vector(v1[1]*v2[2]-v1[2]*v2[1],v1[2]*v2[0]-v1[0]*v2[2],v1[0]*v2[1]-v1[1]*v2[0]);
    tempNormal.normalize();
    return tempNormal;
}

TriangleMesh::Point TriangleMesh::getVertex (const int triNum, int pos)
{
    if (pos < 0 || pos > 2) pos = 0;
    return m_vertices[m_triangles[triNum].pointID[pos]];
}

TriangleMesh::Vector TriangleMesh::getVertNormal(const int vertNum)
{
    if ( !m_vertNormalsCalculated )
    {
        calcVertNormals();
    }
    return m_vertNormals[vertNum];
}

TriangleMesh::Vector TriangleMesh::calcVertNormal(const int vertNum)
{
    Vector sum(0,0,0);

    for(size_t i = 0 ; i < m_vIsInTriangle[vertNum].size() ; ++i)
    {
        sum = sum + m_triNormals[m_vIsInTriangle[vertNum][i]];
    }
    sum.normalize();
    return sum;
}

void TriangleMesh::setVertColor( const int vertNum, const Color color)
{
    m_vertColors[vertNum] = color;
}

TriangleMesh::Color TriangleMesh::getVertColor(const int vertNum)
{
    return m_vertColors[vertNum];
}

void TriangleMesh::calcVertNormals()
{
    m_vertNormals.clear();
    m_vertNormals.resize(m_numVerts);
    for ( int i = 0 ; i < m_numVerts ; ++i)
    {
        m_vertNormals[i] = calcVertNormal(i);
    }

    m_vertNormalsCalculated = true;
}

void TriangleMesh::flipNormals()
{
    for(int i = 0; i < getNumVertices(); ++i)
    {
        m_vertNormals[i] = m_vertNormals[i]*=-1;
    }
}

int TriangleMesh::getNumVertices()
{
    return m_numVerts;
}

int TriangleMesh::getNumTriangles()
{
    return m_numTris;
}

TriangleMesh::Point TriangleMesh::getVertex(const int vertNum)
{
    return m_vertices[vertNum];
}

TriangleMesh::Vector TriangleMesh::getNormal(const int triNum)
{
    return m_triNormals[triNum];
}

Triangle TriangleMesh::getTriangle(const int triNum)
{
    return m_triangles[triNum];
}

TriangleMesh::Color TriangleMesh::getTriangleColor(const int triNum)
{
    return m_triangleColor[triNum];
}

std::vector<unsigned int> TriangleMesh::getStar(const int vertNum)
{
    return m_vIsInTriangle[vertNum];
}

std::vector<TriangleMesh::Point> TriangleMesh::getVerts()
{
    return m_vertices;
}

void TriangleMesh::setVertex(const unsigned int vertNum, const Point nPos)
{
    m_vertices[vertNum] = nPos;
}
