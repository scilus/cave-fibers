#ifndef SELECTIONBOX_H_
#define SELECTIONBOX_H_

#include <Geometry/Point.h>
#include <Geometry/Ray.h>
#include <Geometry/Box.h>
#include <Geometry/Point.h>
#include <Geometry/Vector.h>
#include <Geometry/Rotation.h>
#include <Geometry/OrthogonalTransformation.h>


class SelectionBox {
public:
	typedef Geometry::Point<float,3> Point;
	typedef Geometry::Vector<float,3> Vector; // Compatible vector type
	typedef Geometry::OrthonormalTransformation<float,3> ONTransform; // Type for object positions/orientations
	typedef Geometry::Rotation<float,3> Rotation; // Rotation type

	SelectionBox(Point aCenter, Point aSize);
	SelectionBox();
	virtual ~SelectionBox();

	void draw() const;

	void update();

	void toggleIsBoxSelected();
	bool isActive() const;

	void mouve(const Point& aTranslation);
	Point getCenter() const;

	Geometry::Box<float,3>::HitResult pickBox(Geometry::Ray<float,3> Ray);



private:

	// Functions
	void     draw1() const;
	void     draw2() const;
	void     draw3() const;
	void     draw4() const;
	void     draw5() const;
	void     draw6() const;


	bool 	m_isSelected;
	Point  	m_size;
	Point  	m_center;


	bool    m_boxMoved;
	bool    m_boxResized;
	bool    m_isActive;

	Geometry::Box<float,3> m_box;

	// Those variables represent the min/max value in pixel of the object.
	float m_minX;
	float m_minY;
	float m_minZ;
	float m_maxX;
	float m_maxY;
	float m_maxZ;

};

#endif /* SELECTIONBOX_H_ */
