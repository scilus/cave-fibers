#include "FiberApplication.h"

#include <Geometry/OrthogonalTransformation.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLVertexTemplates.h>
#include <GL/GLContextData.h>
#include <GL/GLGeometryWrappers.h>
#include <GL/GLModels.h>
#include <GLMotif/Button.h>
#include <GLMotif/Menu.h>
#include <GLMotif/PopupMenu.h>

#include <algorithm>    // std::find

void FiberApplication::drawArrow(const Vrui::Point& to,Vrui::Scalar radius) const
{
        Vrui::Scalar tipHeight=radius*Vrui::Scalar(6.0);
        Vrui::Scalar shaftLength=Geometry::dist(Vrui::Point::origin,to)-tipHeight;

        glPushMatrix();
        glTranslate(Vrui::Point::origin-Vrui::Point::origin);
        glRotate(Vrui::Rotation::rotateFromTo(Vrui::Vector(0,0,1),to-Vrui::Point::origin));
        glTranslate(Vrui::Vector(0,0,Math::div2(shaftLength)));
        glDrawCylinder(radius,shaftLength,24);
        glTranslate(Vrui::Vector(0,0,Math::div2(shaftLength)+tipHeight*Vrui::Scalar(0.25)));
        glDrawCone(radius*Vrui::Scalar(2),tipHeight,24);
        glPopMatrix();
}

FiberApplication::ObjectDragger::ObjectDragger(Vrui::DraggingTool* sTool,FiberApplication* sApplication)
    :Vrui::DraggingToolAdapter(sTool),
     application(sApplication),
     dragging(false),
     m_selectedBox(NULL)
{
}

void FiberApplication::ObjectDragger::dragStartCallback(Vrui::DraggingTool::DragStartCallbackData* cbData)
{
    if(application->m_SelectionBox.size()>0)
    {
        bool isHit = false;
        std::vector<SelectionBox*> selectionBox = application->getSelectionBoxVector();
        float minLambda=Math::Constants<float>::max;
        //Find the picked object:
        if(cbData->rayBased)
        {
            for(int i=0; i<selectionBox.size();i++)
            {
                Geometry::Box<float,3>::HitResult hr = selectionBox[i]->pickBox(cbData->ray);

                if(hr.isValid() && hr.getParameter()<minLambda)
                {
                    isHit = true;
                    m_selectedBox=selectionBox[i];
                    minLambda=hr.getParameter();
                }
            }
        }
        else
        {
        	for(int i=0; i<selectionBox.size();i++)
			{
				float dist = selectionBox[i]->pickBox(cbData->startTransformation.getOrigin());

				if(dist<minLambda)
				{
					isHit = true;
					m_selectedBox=selectionBox[i];
					minLambda=dist;
				}
			}
        }

        if(isHit)
        {
            dragging =true;
            //Calculate the initial transformation from the dragger to the dragged object:
            dragTransform=ONTransform(cbData->startTransformation.getTranslation(),cbData->startTransformation.getRotation());
            dragTransform.doInvert();
            dragTransform*= ONTransform(m_selectedBox->getCenter() - Geometry::Point<float,3>::origin,SelectionBox::Rotation::identity);
            m_selectedBox->toggleIsBoxSelected();
        }
    }
}

void FiberApplication::ObjectDragger::dragCallback(Vrui::DraggingTool::DragCallbackData* cbData)
{
    if(dragging)
    {
        // Apply the dragging transformation to the dragged atom:
        ONTransform transform=ONTransform(cbData->currentTransformation.getTranslation(),cbData->currentTransformation.getRotation());
        transform*=dragTransform;

        m_selectedBox->move(transform.getOrigin());
    }
}

void FiberApplication::ObjectDragger::dragEndCallback(Vrui::DraggingTool::DragEndCallbackData* cbData)
{
    if(dragging)
    {
        dragging=false;
        m_selectedBox->toggleIsBoxSelected();
        m_selectedBox->unPickBox();
    }
}

GLMotif::PopupMenu* FiberApplication::createMainMenu(void)
{
    //Create a popup shell to hold the main menu:
    GLMotif::PopupMenu* mainMenuPopup=new GLMotif::PopupMenu("MainMenuPopup",Vrui::getWidgetManager());
    mainMenuPopup->setTitle("Vrui Demonstration");

    //Create the main menu itself:
    GLMotif::Menu* mainMenu=new GLMotif::Menu("MainMenu",mainMenuPopup,false);

    //Create a button:
    GLMotif::Button* resetNavigationButton=new GLMotif::Button("ResetNavigationButton",mainMenu,"Reset Navigation");

    GLMotif::Button* addSelectionBox=new GLMotif::Button("AddSelectionBox",mainMenu,"New Selection Box");
    addSelectionBox->getSelectCallbacks().add(this,&FiberApplication::OnAddSelectionBoxCallBack);

    //Add a callback to the button:
    resetNavigationButton->getSelectCallbacks().add(this,&FiberApplication::resetNavigationCallback);

    //Finish building the main menu:
    mainMenu->manageChild();

    return mainMenuPopup;
}

void FiberApplication::resetNavigationCallback(Misc::CallbackData* cbData)
{
    //Reset the Vrui navigation transformation:
    Vrui::Point center = Point::origin;
    Vrui::Scalar radius = Geometry::dist(Point::origin,mFibers.getBBMax());
    Vrui::setNavigationTransformation(center, radius);

    /*********************************************************************
    Now the coordinate system's origin is in the middle of the
    environment, e.g., in the middle of the CAVE, and one coordinate unit
    is one inch long. The rendered cube will show up exactly 10" big.
    *********************************************************************/
}

void FiberApplication::OnAddSelectionBoxCallBack(Misc::CallbackData* cbData)
{
    Point center( 0,0,0);

    Point size( 10.0,10.0,10.0);

    m_SelectionBox.push_back(new SelectionBox(center,size));
}

std::vector<SelectionBox*>& FiberApplication::getSelectionBoxVector()
{
    return m_SelectionBox;
}

FiberApplication::FiberApplication(int& argc,char**& argv,char**& appDefaults)
    :Vrui::Application(argc,argv,appDefaults),
     mainMenu(0)
{
    //TODO create tool at launch if possible

    //Create the user interface:
    mainMenu=createMainMenu();

    //Install the main menu:
    Vrui::setMainMenu(mainMenu);

    std::string fileName = "fibers.fib";
    //load Fiber
    mFibers.load(fileName);

    //Set the navigation transformation:
    resetNavigationCallback(0);

    mFibers.updateLinesShown();

    //Tell Vrui to run in a continuous frame sequence:
    Vrui::updateContinuously();
}

FiberApplication::~FiberApplication(void)
{
    //Destroy the user interface:
    delete mainMenu;

    //Delete all object draggers:
    for(std::vector<ObjectDragger*>::iterator adIt=m_objectDragger.begin();adIt!=m_objectDragger.end();++adIt)
    {
        delete *adIt;
    }

    //Delete all selectionBox
    for(std::vector<SelectionBox*>::iterator adIt=m_SelectionBox.begin();adIt!=m_SelectionBox.end();++adIt)
    {
        delete *adIt;
    }

}

void FiberApplication::toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData* cbData)
{
    //Check if the new tool is a dragging tool:
    Vrui::DraggingTool* tool=dynamic_cast<Vrui::DraggingTool*>(cbData->tool);
    if(tool!=0)
    {
        //Create an atom dragger object and associate it with the new tool:
        ObjectDragger* newDragger=new ObjectDragger(tool,this);

        //Add new dragger to list:
        m_objectDragger.push_back(newDragger);
    }
}

void FiberApplication::toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData* cbData)
{
    //Check if the to-be-destroyed tool is a dragging tool:
    Vrui::DraggingTool* tool=dynamic_cast<Vrui::DraggingTool*>(cbData->tool);
    if(tool!=0)
    {
        //Find the atom dragger associated with the tool in the list:
        std::vector<ObjectDragger*>::iterator adIt;
        for(adIt=m_objectDragger.begin();adIt!=m_objectDragger.end()&&(*adIt)->getTool()!=tool;++adIt)
            ;
        if(adIt!=m_objectDragger.end())
        {
            //Remove the atom dragger:
            delete *adIt;
            m_objectDragger.erase(adIt);
        }
    }
}

void FiberApplication::updateSelectedFiber(Fibers* aFiber)
{
	const int fibersCount( aFiber->getLineCount());

	std::vector<bool> selectedFibers(fibersCount,false);

	std::vector<bool> subSelectedFibers;

	//fiind all fibers that pass in selection boxes
	for(std::vector<SelectionBox*>::iterator it = m_SelectionBox.begin(); it != m_SelectionBox.end(); it++)
	{
		if((*it)->isActive())
		{
			subSelectedFibers = (*it)->getSelectedFiber(aFiber);

			if(subSelectedFibers.size() == fibersCount)
			{
				for(int i=0; i< fibersCount; i++)
				{
					selectedFibers[i] = selectedFibers[i] || subSelectedFibers[i];
				}
			}
		}
	}

	aFiber->setSelectedFiber(selectedFibers);
}

void FiberApplication::frame(void)
{
    /*********************************************************************
    This function is called exactly once per frame, no matter how many
    eyes or windows exist. It is the perfect place to change application
    or Vrui state (run simulations, animate models, synchronize with
    background threads, change the navigation transformation, etc.).
    *********************************************************************/
	//update fibers selected if there are a selection box
	if(m_SelectionBox.size() !=0)
	{
		updateSelectedFiber(&mFibers);
	}
    //Get the time since the last frame:
    double frameTime=Vrui::getCurrentFrameTime();

    //Request another rendering cycle to show the animation:
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

    //Get the OpenGL-dependent application data from the GLContextData object:
    DataItem* dataItem=contextData.retrieveDataItem<DataItem>(this);

    Vrui::Scalar arrowRadius=Vrui::getInchFactor()*Vrui::Scalar(0.125);

    //Save OpenGL state:
    glPushAttrib(GL_ENABLE_BIT);

    //Set up the animation transformation:
    glPushMatrix();

    //Call the display list created in the initDisplay() method:
    glCallList(dataItem->displayListId);

    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    //draw fiber depending of the option choose by the user (It is not implemented for the moment)
    if(mFibers.isUseFakeTubes())
    {
        mFibers.drawFakeTubes();
    }
    else if(mFibers.isUseTransparency())
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE );
        glDepthMask( GL_FALSE );
        mFibers.drawSortedLines();
        glPopAttrib();
    }
    else if(mFibers.isUseIntersectedFibers())
    {
        mFibers.drawCrossingFibers();
    }
    else
    {
        mFibers.drawFiber();
    }

    for(int i=0; i<m_SelectionBox.size();i++)
    {
        m_SelectionBox[i]->draw();
    }

    //Draw the forward direction:
    glColor3f(1.0f,0.0f,0.0f);
    drawArrow(Vrui::Point(6.0,0.0,0.0),arrowRadius);

    //Draw the up direction:
    glColor3f(0.0f,1.0f,0.0f);
    drawArrow(Vrui::Point(0.0,6.0,0.0),arrowRadius);

    //Draw the up direction:
    glColor3f(0.0f,0.0f,1.0f);
    drawArrow(Vrui::Point(0.0,0.0,6.0),arrowRadius);

    glPopAttrib();

    //Go back to navigation coordinates:
    glPopMatrix();

    //Restore OpenGL state:
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

    //Create context data item and store it in the GLContextData object:
    DataItem* dataItem=new DataItem;
    contextData.addDataItem(this,dataItem);

    //Now is also the time to upload all display lists' contents:
    glNewList(dataItem->displayListId,GL_COMPILE);

    //Draw a wireframe cube 10" wide (unless a user scaled the navigation transformation):
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    glPopAttrib();

    //Finish the display list:
    glEndList();
}

