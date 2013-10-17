/*
 * FiberApplication.cpp
 *
 *  Created on: Oct 9, 2013
 *      Author: belp2210
 */

#include "FiberApplication.h"

#include <Geometry/OrthogonalTransformation.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLVertexTemplates.h>
#include <GL/GLContextData.h>
#include <GL/GLGeometryWrappers.h>
#include <GLMotif/Button.h>
#include <GLMotif/Menu.h>
#include <GLMotif/PopupMenu.h>

GLMotif::PopupMenu* FiberApplication::createMainMenu(void)
{
    /* Create a popup shell to hold the main menu: */
    GLMotif::PopupMenu* mainMenuPopup=new GLMotif::PopupMenu("MainMenuPopup",Vrui::getWidgetManager());
    mainMenuPopup->setTitle("Vrui Demonstration");

    /* Create the main menu itself: */
    GLMotif::Menu* mainMenu=new GLMotif::Menu("MainMenu",mainMenuPopup,false);

    /* Create a button: */
    GLMotif::Button* resetNavigationButton=new GLMotif::Button("ResetNavigationButton",mainMenu,"Reset Navigation");

    /* Add a callback to the button: */
    resetNavigationButton->getSelectCallbacks().add(this,&FiberApplication::resetNavigationCallback);

    /* Finish building the main menu: */
    mainMenu->manageChild();

    return mainMenuPopup;
}

void FiberApplication::resetNavigationCallback(Misc::CallbackData* cbData)
{
    /* Reset the Vrui navigation transformation: */
    Vrui::NavTransform t=Vrui::NavTransform::identity;
    t*=Vrui::NavTransform::translateFromOriginTo(Vrui::getDisplayCenter());
    t*=Vrui::NavTransform::scale(Vrui::getInchFactor());
    Vrui::setNavigationTransformation(t);

    /*********************************************************************
    Now the coordinate system's origin is in the middle of the
    environment, e.g., in the middle of the CAVE, and one coordinate unit
    is one inch long. The rendered cube will show up exactly 10" big.
    *********************************************************************/
}

FiberApplication::FiberApplication(int& argc,char**& argv)
    :Vrui::Application(argc,argv),
     mainMenu(0)
    {
    /* Initialize the animation parameters: */
    for(int i=0;i<3;++i)
    {
        modelAngles[i]=Vrui::Scalar(0);
    }
    rotationSpeeds[0]=Vrui::Scalar(0);
    rotationSpeeds[1]=Vrui::Scalar(0);
    rotationSpeeds[2]=Vrui::Scalar(0);

    /* Create the user interface: */
    mainMenu=createMainMenu();

    /* Install the main menu: */
    Vrui::setMainMenu(mainMenu);

    /* Set the navigation transformation: */
    resetNavigationCallback(0);

    std::string fileName = "fibers.fib";
    //load Fiber
    mFibers.load(fileName);

    mFibers.updateLinesShown();
}

FiberApplication::~FiberApplication(void)
{
    /* Destroy the user interface: */
    delete mainMenu;
}

void FiberApplication::frame(void)
{
    /*********************************************************************
    This function is called exactly once per frame, no matter how many
    eyes or windows exist. It is the perfect place to change application
    or Vrui state (run simulations, animate models, synchronize with
    background threads, change the navigation transformation, etc.).
    *********************************************************************/

    /* Get the time since the last frame: */
    double frameTime=Vrui::getCurrentFrameTime();

    /* Change the model angles: */
    for(int i=0;i<3;++i)
    {
        modelAngles[i]+=rotationSpeeds[i]*frameTime;
        modelAngles[i]=Math::mod(modelAngles[i],Vrui::Scalar(360));
    }

    /* Request another rendering cycle to show the animation: */
    Vrui::scheduleUpdate(Vrui::getApplicationTime()+1.0/125.0); // Aim for 125 FPS
}

void FiberApplication::display(GLContextData& contextData) const
{
    /*********************************************************************
    This method is called once for every eye in every window on every
    frame. It must not change application or Vrui state, as it is called
    an unspecified number of times, and might be called from parallel
    background threads. It also must not clear the screen or initialize
    the OpenGL transformation matrices. When this method is called, Vrui
    will already have rendered its own state (menus etc.) and have set up
    the transformation matrices so that all rendering in this method
    happens in navigation (i.e., model) coordinates.
    *********************************************************************/

    /* Get the OpenGL-dependent application data from the GLContextData object: */
    DataItem* dataItem=contextData.retrieveDataItem<DataItem>(this);

    /* Save OpenGL state: */
    glPushAttrib(GL_ENABLE_BIT);

    /* Enable texturing and use the texture uploaded in the initDisplay() method: */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,dataItem->textureObjectId);

    /* Render some textured objects here... */
    // ...

    /* Disable texture mapping and protect the texture object (important!): */
    glBindTexture(GL_TEXTURE_2D,0);
    glDisable(GL_TEXTURE_2D);

    /* Set up the animation transformation: */
    glPushMatrix();

    /* Rotate around X, then Y, then Z: */
    glRotate(modelAngles[0],Vrui::Vector(1,0,0));
    glRotate(modelAngles[1],Vrui::Vector(0,1,0));
    glRotate(modelAngles[2],Vrui::Vector(0,0,1));

    /* Call the display list created in the initDisplay() method: */
    glCallList(dataItem->displayListId);

    /* Go back to navigation coordinates: */
    glPopMatrix();

    /* Restore OpenGL state: */
    glPopAttrib();
}

void FiberApplication::initContext(GLContextData& contextData) const
{
    /*********************************************************************
    For classes derived from GLObject, this method is called for each
    newly-created object, once per every OpenGL context created by Vrui.
    This method must not change application or Vrui state, but only create
    OpenGL-dependent application data and store them in the GLContextData
    object for retrieval in the display method.
    *********************************************************************/

	 GLenum errorCode = glewInit();

	if( GLEW_OK != errorCode )
	{
	   return;
	}

    /* Create context data item and store it in the GLContextData object: */
    DataItem* dataItem=new DataItem;
    contextData.addDataItem(this,dataItem);

    /* Now is the time to upload all needed texture data: */
    glBindTexture(GL_TEXTURE_2D,dataItem->textureObjectId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    GLfloat texImage[4][4]=
    {
        {0.0f, 0.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 0.0f, 0.0f}
    };
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,4,4,0,GL_LUMINANCE,GL_FLOAT,texImage);
    glBindTexture(GL_TEXTURE_2D,0);

    /* Now is also the time to upload all display lists' contents: */
	glNewList(dataItem->displayListId,GL_COMPILE);

	/* Draw a wireframe cube 10" wide (unless a user scaled the navigation transformation): */
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);

	mFibers.initializeBuffer();

	mFibers.draw();
	glPopAttrib();


	/* Finish the display list: */
	glEndList();

    /*glGenBuffers( 3, dataItem->bufferObjects );
	glBindBuffer( GL_ARRAY_BUFFER, dataItem->bufferObjects[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * mFibers.getLineCount() * 3, &mFibers.getPointArray()[0], GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, dataItem->bufferObjects[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * mFibers.getLineCount() * 3, &mFibers.getColorArray()[0], GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, dataItem->bufferObjects[2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * mFibers.getLineCount() * 3, &mFibers.getNormalArray()[0], GL_STATIC_DRAW );*/
}

