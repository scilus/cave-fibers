#include "SelectionBox.h"

#include <GL/glew.h>
#include <stdio.h>
#include <Geometry/Vector.h>
#include <Geometry/Rotation.h>
#include <algorithm>    // std::max

#define X 0
#define Y 1
#define Z 2

SelectionBox::SelectionBox(Point aCenter, Point aSize):
m_isSelected(false),
m_size(aSize),
m_center(aCenter),
m_boxMoved(false),
m_boxResized(false),
m_isActive(true)
{
	update();
}

SelectionBox::SelectionBox():
m_isSelected(false),
m_size(Point(0.0,0.0,0.0)),
m_center(Point(0.0,0.0,0.0)),
m_boxMoved(false),
m_boxResized(false),
m_isActive(true)
{
	update();
}

SelectionBox::~SelectionBox()
{

}

void SelectionBox::update()
{
	// Update the min/max position in x, y and z of the object.
	m_minX = m_center[X] - ( m_size[X] * 0.5f);
	m_maxX = m_center[X] + ( m_size[X] * 0.5f);
	m_minY = m_center[Y] - ( m_size[Y] * 0.5f);
	m_maxY = m_center[Y] + ( m_size[Y] * 0.5f);
	m_minZ = m_center[Z] - ( m_size[Z] * 0.5f);
	m_maxZ = m_center[Z] + ( m_size[Z] * 0.5f);

	m_box = Geometry::Box<float,3>(Point(m_minX,m_minY,m_minZ),Point(m_maxX,m_maxY,m_maxZ));
}

void SelectionBox::draw() const
{
	if( !m_isActive )
	{
		return;
	}

	GLfloat color[] = { 0.0f, 1.0f, 1.0f, 1.0f };

	glDepthMask( GL_FALSE );
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glColor4f( color[0], color[1], color[2], color[3] );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glBegin( GL_QUADS );
		glVertex3f( m_minX, m_minY, m_maxZ );
		glVertex3f( m_minX, m_maxY, m_maxZ );
		glVertex3f( m_minX, m_maxY, m_minZ );
		glVertex3f( m_minX, m_minY, m_minZ );

		glVertex3f( m_maxX, m_minY, m_minZ );
		glVertex3f( m_maxX, m_maxY, m_minZ );
		glVertex3f( m_maxX, m_maxY, m_maxZ );
		glVertex3f( m_maxX, m_minY, m_maxZ );

		glVertex3f( m_minX, m_minY, m_minZ );
		glVertex3f( m_maxX, m_minY, m_minZ );
		glVertex3f( m_maxX, m_minY, m_maxZ );
		glVertex3f( m_minX, m_minY, m_maxZ );

		glVertex3f( m_minX, m_maxY, m_maxZ );
		glVertex3f( m_maxX, m_maxY, m_maxZ );
		glVertex3f( m_maxX, m_maxY, m_minZ );
		glVertex3f( m_minX, m_maxY, m_minZ );

		glVertex3f( m_minX, m_minY, m_minZ );
		glVertex3f( m_minX, m_maxY, m_minZ );
		glVertex3f( m_maxX, m_maxY, m_minZ );
		glVertex3f( m_maxX, m_minY, m_minZ );

		glVertex3f( m_maxX, m_minY, m_maxZ );
		glVertex3f( m_maxX, m_maxY, m_maxZ );
		glVertex3f( m_minX, m_maxY, m_maxZ );
		glVertex3f( m_minX, m_minY, m_maxZ );
	glEnd();

	glPopAttrib();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glBegin( GL_QUADS );

		draw2();
		draw1();
		draw4();
		draw3();
		draw6();
		draw5();

	glEnd();

	glDepthMask( GL_TRUE );
}

void SelectionBox::draw1()const
{
    glVertex3f( m_maxX, m_minY, m_minZ );
    glVertex3f( m_maxX, m_maxY, m_minZ );
    glVertex3f( m_maxX, m_maxY, m_maxZ );
    glVertex3f( m_maxX, m_minY, m_maxZ );
}

void SelectionBox::draw2()const
{
	glVertex3f( m_minX, m_minY, m_maxZ );
	glVertex3f( m_minX, m_maxY, m_maxZ );
	glVertex3f( m_minX, m_maxY, m_minZ );
	glVertex3f( m_minX, m_minY, m_minZ );
}

void SelectionBox::draw3()const
{
	glVertex3f( m_minX, m_maxY, m_maxZ );
	glVertex3f( m_maxX, m_maxY, m_maxZ );
	glVertex3f( m_maxX, m_maxY, m_minZ );
	glVertex3f( m_minX, m_maxY, m_minZ );
}

void SelectionBox::draw4()const
{
    glVertex3f( m_minX, m_minY, m_minZ );
    glVertex3f( m_maxX, m_minY, m_minZ );
    glVertex3f( m_maxX, m_minY, m_maxZ );
    glVertex3f( m_minX, m_minY, m_maxZ );
}

void SelectionBox::draw5()const
{
	glVertex3f( m_maxX, m_minY, m_maxZ );
	glVertex3f( m_maxX, m_maxY, m_maxZ );
	glVertex3f( m_minX, m_maxY, m_maxZ );
	glVertex3f( m_minX, m_minY, m_maxZ );
}

void SelectionBox::draw6()const
{
    glVertex3f( m_minX, m_minY, m_minZ );
    glVertex3f( m_minX, m_maxY, m_minZ );
    glVertex3f( m_maxX, m_maxY, m_minZ );
    glVertex3f( m_maxX, m_minY, m_minZ );
}

void SelectionBox::toggleIsBoxSelected()
{
	m_isSelected = ! m_isSelected;
}

bool SelectionBox::isActive() const
{
	return m_isActive;
}

float SelectionBox::pickBox(SelectionBox::Point p)
{
	float dist = Math::Constants<float>::max;
	//compute the size of selectionBox
	float limitsMax = Geometry::sqrDist(m_center,SelectionBox::Point(m_maxX,m_maxY,m_maxZ));
	if(!m_isSelected && m_isActive)
	{
		//check if the Point p are in the box
		float dist2 = Geometry::sqrDist(p,m_center);
		if(dist2 < limitsMax)
		{
			dist = dist2;
		}
	}
	return dist;
}

Geometry::Box<float,3>::HitResult SelectionBox::pickBox(Geometry::Ray<float,3> Ray)
{
	Geometry::Box<float,3>::HitResult hr;
	//Intersect the ray with a boundingBox around the selection box
	if(!m_isSelected && m_isActive)
	{
		hr=m_box.intersectRay(Ray);
	}
	return hr;
}

void SelectionBox::unPickBox()
{
	m_boxMoved = false;
}

void SelectionBox::move(const Point& aTranslation)
{
	m_boxMoved = true;
	m_center = aTranslation;

	update();
}

SelectionBox::Point SelectionBox::getCenter() const
{
	return m_center;
}


