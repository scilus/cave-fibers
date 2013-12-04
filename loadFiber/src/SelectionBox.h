#ifndef SELECTIONBOX_H_
#define SELECTIONBOX_H_

#include <Geometry/Point.h>
#include <Geometry/Ray.h>
#include <Geometry/Box.h>
#include <Geometry/Point.h>
#include <Geometry/Vector.h>
#include <Geometry/Rotation.h>
#include <Geometry/OrthogonalTransformation.h>
#include <vector>

#include "fiber.h"


class SelectionBox {
public:
    typedef Geometry::Vector<float,3> Vector; // Compatible vector type
    typedef Geometry::OrthonormalTransformation<float,3> ONTransform; // Type for object positions/orientations
    typedef Geometry::Rotation<float,3> Rotation; // Rotation type
    typedef Fibers::Point Point;

    SelectionBox(Point aCenter, Point aSize);

    virtual ~SelectionBox();

    void draw() const;

    void update();

    void toggleIsBoxSelected();
    bool isActive() const;

    void move(const Fibers::Point& aTranslation);
    Point getCenter() const;
    Geometry::Box<float,3> getBoundingBox();

    float                               pickBox(Fibers::Point p);
    Geometry::Box<float,3>::HitResult   pickBox(Geometry::Ray<float,3> Ray);
    void                                unPickBox();

    std::vector<bool> getSelectedFibers(Fibers* aFiber);


private:

    // Functions
    void    draw1() const;
    void    draw2() const;
    void    draw3() const;
    void    draw4() const;
    void    draw5() const;
    void    draw6() const;

    bool    insideBox(Point aPoint);


    bool        m_isSelected;
    Point       m_size;
    Point       m_center;


    bool    m_boxMoved;
    bool    m_needUpdate;
    bool    m_boxResized;
    bool    m_isActive;

    Geometry::Box<float,3> m_box;

    // Those variables represent the min/max value in 3D Space of the object.
    float m_minX;
    float m_minY;
    float m_minZ;
    float m_maxX;
    float m_maxY;
    float m_maxZ;

};

#endif /* SELECTIONBOX_H_ */
