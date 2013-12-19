#ifndef ISOSURFACE_H_
#define ISOSURFACE_H_

#include "DataSetInfo.h"
#include "Anatomy.h"
#include "TriangleMesh.h"

#include <map>

struct POINT3DID
{
    unsigned int newID;
    DataSetInfo::Point point;
};

struct TRIANGLE
{
    unsigned int pointID[3];
};

class IsoSurface : public DataSetInfo
{
public:
    typedef DataSetInfo::Point Point;
    typedef DataSetInfo::Vector Vector; // Compatible vector type
    typedef std::map<unsigned int, POINT3DID> ID2POINT3DID;

    IsoSurface(Anatomy* pAnatomy , bool IsoSurfaceFiltered);
    virtual ~IsoSurface();

    virtual void                draw() const;
    virtual bool                load( const std::string &filename );

    // Deletes the isosurface.
    void                        DeleteSurface();

    // Generates the isosurface from the scalar field contained in the
    // buffer ptScalarField[].
    void                        GenerateSurface(float tIsoLevel);

    // Returns true if a valid surface has been generated.
    bool                        IsSurfaceValid();

private:
    // Calculates the intersection point of the isosurface with an
    // edge.
    POINT3DID       CalculateIntersection(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo);

    // Interpolates between two grid points to produce the point at which
    // the isosurface intersects an edge.
    POINT3DID       Interpolate(float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2);

    // Returns the edge ID.
    int             GetEdgeID(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo);

    // Returns the vertex ID.
    unsigned int    GetVertexID(unsigned int nX, unsigned int nY, unsigned int nZ);

    // Renames vertices and triangles so that they can be accessed more
    // efficiently.
    void            RenameVerticesAndTriangles();

    void            generateGeometry();

    GLuint                      m_GLuint;

    // Lookup tables used in the construction of the isosurface.
    static const unsigned int   m_edgeTable[256];
    static const int            m_triTable[256][16];

    Point                       m_voxelSize;
    std::vector<int>            m_dims;

    // No. of cells in x, y, and z directions.
    unsigned int                m_nCellsX;
    unsigned int                m_nCellsY;
    unsigned int                m_nCellsZ;

    // Cell length in x, y, and z directions.
    float                       m_fCellLengthX;
    float                       m_fCellLengthY;
    float                       m_fCellLengthZ;

    // The number of vertices which make up the isosurface.
    unsigned int                m_nVertices;

    // The number of triangles which make up the isosurface.
    unsigned int                m_nTriangles;

    // The number of normals.
    unsigned int                m_nNormals;

    // The isosurface value.
    float m_tIsoLevel;

    // Indicates whether a valid surface is present.
    bool                        m_bValidSurface;

    bool                        m_positionsCalculated;

    // The buffer holding the scalar field.
    std::vector<float>          m_ptScalarField;

    float                       m_maxPixelValue;

    // List of POINT3Ds which form the isosurface.
    ID2POINT3DID                m_i2pt3idVertices;

    // List of TRIANGLES which form the triangulation of the isosurface.
    std::vector<TRIANGLE>       m_trivecTriangles;

    // Trianglemesh
    TriangleMesh                *m_tMesh;
};

#endif /* ISOSURFACE_H_ */
