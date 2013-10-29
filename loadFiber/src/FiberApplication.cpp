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

FiberApplication::FiberApplication(int& argc,char**& argv,char**& appDefaults)
    :Vrui::Application(argc,argv,appDefaults),
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
    Vrui::requestUpdate();
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

    /* Set up the animation transformation: */
    glPushMatrix();

    /* Rotate around X, then Y, then Z: */
    glRotate(modelAngles[0],Vrui::Vector(1,0,0));
    glRotate(modelAngles[1],Vrui::Vector(0,1,0));
    glRotate(modelAngles[2],Vrui::Vector(0,0,1));

    /* Call the display list created in the initDisplay() method: */
    glCallList(dataItem->displayListId);

    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    //draw fiber depending of the option choose by the user (It is not implemented for the moment)
    if(mFibers.IsUseFakeTubes())
    {
        mFibers.drawFakeTubes();
    }
    else if(mFibers.IsuseTransparency())
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE );
        glDepthMask( GL_FALSE );
        mFibers.drawSortedLines();
        glPopAttrib();
    }
    else if(mFibers.IsUseIntersectedFibers())
    {
        mFibers.drawCrossingFibers();
    }
    else
    {
        mFibers.drawFiber();
    }

    glPopAttrib();

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

    //init glew lib
    GLenum errorCode = glewInit();

    if( GLEW_OK != errorCode )
    {
       return;
    }

    mFibers.initializeBuffer();

    /* Create context data item and store it in the GLContextData object: */
    DataItem* dataItem=new DataItem;
    contextData.addDataItem(this,dataItem);

    /* Now is also the time to upload all display lists' contents: */
    glNewList(dataItem->displayListId,GL_COMPILE);

    /* Draw a wireframe cube 10" wide (unless a user scaled the navigation transformation): */
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    glPopAttrib();

    /* Finish the display list: */
    glEndList();
}

