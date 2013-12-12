#include "FiberApplication.h"

//Vrui includes
#include <Geometry/OrthogonalTransformation.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLVertexTemplates.h>
#include <GL/GLContextData.h>
#include <GL/GLGeometryWrappers.h>
#include <GL/GLModels.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/WidgetManager.h>
#include <GLMotif/Blind.h>
#include <GLMotif/Label.h>
#include <GLMotif/Button.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/Menu.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/TextField.h>
#include <GLMotif/RadioBox.h>


#include <algorithm>    // std::find

void FiberApplication::drawArrow(const Vrui::Point& to, Vrui::Scalar radius) const
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

FiberApplication::ObjectDragger::ObjectDragger(Vrui::DraggingTool* sTool, FiberApplication* sApplication)
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
    mainMenuPopup->setTitle("Fiber Application");

    //Create the main menu itself:
    GLMotif::Menu* mainMenu=new GLMotif::Menu("MainMenu",mainMenuPopup,false);

    //Create a button:
    GLMotif::Button* resetNavigationButton=new GLMotif::Button("ResetNavigationButton",mainMenu,"Reset Navigation");

    GLMotif::Button* addSelectionBox=new GLMotif::Button("AddSelectionBox",mainMenu,"New Selection Box");
    addSelectionBox->getSelectCallbacks().add(this,&FiberApplication::OnAddSelectionBoxCallBack);

    //Add a callback to the button:
    resetNavigationButton->getSelectCallbacks().add(this,&FiberApplication::resetNavigationCallback);

    //GLMotif::Button* loadButton=new GLMotif::Button("LoadButton",mainMenu,"Open");
    //loadButton->getSelectCallbacks().add(this,&FiberApplication::OnLoadFiberCallBack);

    /*GLMotif::CascadeButton* orientationButtonCascade=new GLMotif::CascadeButton("orientationButtonCascade",mainMenu,"Orientation");
    orientationButtonCascade->setPopup(createOrientationButtonMenu());

    //Create a toggle button to show the render settings dialog:
    GLMotif::ToggleButton* showPropertiesDialogToggle=new GLMotif::ToggleButton("showPropertiesDialogToggle",mainMenu,"Show Properties Dialog");
    showPropertiesDialogToggle->setToggle(false);
    showPropertiesDialogToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);*/
    GLMotif::ToggleButton* showSelectionBox=new GLMotif::ToggleButton("ActivateSelectionBoxToggle",mainMenu,"Activate Selection Box");
    showSelectionBox->setBorderWidth(0.0f);
    showSelectionBox->setMarginWidth(0.0f);
    showSelectionBox->setHAlignment(GLFont::Left);
    showSelectionBox->setToggle(m_showSelectionBox);
    showSelectionBox->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    //Finish building the main menu:
    mainMenu->manageChild();

    return mainMenuPopup;
}


//function for create the orientation subMenu. This orientation is for see the fiber in the choice direction
//this function are not use for the moment
GLMotif::Popup* FiberApplication::createOrientationButtonMenu(void)
{
    //Create the submenu's top-level shell:
    GLMotif::Popup* orientationButtonMenuPopup=new GLMotif::Popup("orientationButtonMenuPopup",Vrui::getWidgetManager());

    //Create the array of orientation buttons inside the top-level shell:
    GLMotif::SubMenu* orientationButtonMenu=new GLMotif::SubMenu("orientationButtonMenu",orientationButtonMenuPopup,false);

    //these callback are not implemented
    GLMotif::Button* leftButton=new GLMotif::Button("LeftButton",orientationButtonMenu,"orientation to left");
    leftButton->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

    GLMotif::Button* frontButton=new GLMotif::Button("FrontButton",orientationButtonMenu,"orientation to front");
    frontButton->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

    GLMotif::Button* rightButton=new GLMotif::Button("RightButton",orientationButtonMenu,"orientation to right");
    rightButton->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

    GLMotif::Button* behindButton=new GLMotif::Button("BehindButton",orientationButtonMenu,"orientation to behind");
    behindButton->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

    GLMotif::Button* aboveButton=new GLMotif::Button("AboveButton",orientationButtonMenu,"orientation to above");
    aboveButton->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

    GLMotif::Button* belowButton=new GLMotif::Button("BelowButton",orientationButtonMenu,"orientation to below");
    belowButton->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

    /* Calculate the submenu's proper layout: */
    orientationButtonMenu->manageChild();

    /* Return the created top-level shell: */
    return orientationButtonMenuPopup;
}

//Create the properties dialog to change the display of the fiber
//this function are not use for the moment
GLMotif::PopupWindow* FiberApplication::createPropertiesDialog(void)
{
    const GLMotif::StyleSheet& ss=*Vrui::getWidgetManager()->getStyleSheet();

    GLMotif::PopupWindow* propertiesDialogPopup=new GLMotif::PopupWindow("propertiesDialogPopup",Vrui::getWidgetManager(),"Display Settings");
    propertiesDialogPopup->setResizableFlags(true,false);

    GLMotif::RowColumn* propertiesDialog=new GLMotif::RowColumn("propertiesDialog",propertiesDialogPopup,false);
    propertiesDialog->setOrientation(GLMotif::RowColumn::VERTICAL);
    propertiesDialog->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    propertiesDialog->setNumMinorWidgets(2);

    new GLMotif::Label("MinLengthLabel",propertiesDialog,"Min Length");

    GLMotif::Slider* MinLengthSlider=new GLMotif::Slider("MinLengthSlider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
    MinLengthSlider->setValueRange(0.0,1.0,0.001);
    MinLengthSlider->setValue(0);
    MinLengthSlider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);

    new GLMotif::Label("MaxLengthLabel",propertiesDialog,"Max Length");

    GLMotif::Slider* MaxLengthSlider=new GLMotif::Slider("MaxLengthSlider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
    MaxLengthSlider->setValueRange(0.0,1.0,0.001);
    MaxLengthSlider->setValue(0);
    MaxLengthSlider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);

    new GLMotif::Label("SubsamplingLabel",propertiesDialog,"Subsampling");

    GLMotif::Slider* SubsamplingSlider=new GLMotif::Slider("SubsamplingSlider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
    SubsamplingSlider->setValueRange(0.0,1.0,0.001);
    SubsamplingSlider->setValue(0);
    SubsamplingSlider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);

    new GLMotif::Label("ThicknessLabel",propertiesDialog,"Thickness");

    GLMotif::Slider* ThicknessSlider=new GLMotif::Slider("ThicknessSlider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
    ThicknessSlider->setValueRange(0.0,1.0,0.001);
    ThicknessSlider->setValue(0);
    ThicknessSlider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);


    //toggle button
    GLMotif::ToggleButton* showFakeTubeToggle=new GLMotif::ToggleButton("ShowFakeTubeToggle",propertiesDialog,"Show Fake Tube");
    showFakeTubeToggle->setBorderWidth(0.0f);
    showFakeTubeToggle->setMarginWidth(0.0f);
    showFakeTubeToggle->setHAlignment(GLFont::Left);
    showFakeTubeToggle->setToggle(useFakeTube);
    showFakeTubeToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    GLMotif::ToggleButton* showIntersectedFibersToggle=new GLMotif::ToggleButton("ShowIntersectedFibersToggle",propertiesDialog,"Show Intersected Fibers");
    showIntersectedFibersToggle->setBorderWidth(0.0f);
    showIntersectedFibersToggle->setMarginWidth(0.0f);
    showIntersectedFibersToggle->setHAlignment(GLFont::Left);
    showIntersectedFibersToggle->setToggle(useFakeTube);
    showIntersectedFibersToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    GLMotif::ToggleButton* LocalColoringToggle=new GLMotif::ToggleButton("LocalColoringToggle",propertiesDialog,"Local Coloring");
    LocalColoringToggle->setBorderWidth(0.0f);
    LocalColoringToggle->setMarginWidth(0.0f);
    LocalColoringToggle->setHAlignment(GLFont::Left);
    LocalColoringToggle->setToggle(useFakeTube);
    LocalColoringToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    GLMotif::ToggleButton* ColorOverlayToggle=new GLMotif::ToggleButton("ColorOverlayToggle",propertiesDialog,"Color With Overlay");
    ColorOverlayToggle->setBorderWidth(0.0f);
    ColorOverlayToggle->setMarginWidth(0.0f);
    ColorOverlayToggle->setHAlignment(GLFont::Left);
    ColorOverlayToggle->setToggle(useFakeTube);
    ColorOverlayToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    GLMotif::ToggleButton* ColorTransparencyToggle=new GLMotif::ToggleButton("ColorTransparencyToggle",propertiesDialog,"Color Transparency");
    ColorTransparencyToggle->setBorderWidth(0.0f);
    ColorTransparencyToggle->setMarginWidth(0.0f);
    ColorTransparencyToggle->setHAlignment(GLFont::Left);
    ColorTransparencyToggle->setToggle(useFakeTube);
    ColorTransparencyToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    propertiesDialog->manageChild();

    return propertiesDialogPopup;
}


void FiberApplication::resetNavigationCallback(Misc::CallbackData* cbData)
{
    //Reset the Vrui navigation transformation:
    Vrui::Point center = Fibers::Point::origin;
    Vrui::Scalar radius = Geometry::dist(Fibers::Point::origin,mFibers.getBBMax());
    Vrui::setNavigationTransformation(center, radius);

    /*********************************************************************
    Now the coordinate system's origin is in the middle of the
    environment, e.g., in the middle of the CAVE, and one coordinate unit
    is one inch long. The rendered cube will show up exactly 10" big.
    *********************************************************************/
}

void FiberApplication::OnAddSelectionBoxCallBack(Misc::CallbackData* cbData)
{
    Fibers::Point center( 0,0,0);

    Fibers::Point size( 10.0,10.0,10.0);

    m_SelectionBox.push_back(new SelectionBox(center,size,m_showSelectionBox));
}

std::vector<SelectionBox*>& FiberApplication::getSelectionBoxVector()
{
    return m_SelectionBox;
}

void FiberApplication::OnLoadFiberCallBack(Misc::CallbackData* cbData)
{
    if(!inLoadFiber)
    {
        char cCurrentPath[FILENAME_MAX];
        getcwd(cCurrentPath, sizeof(cCurrentPath));

        //Create a file selection dialog to select a palette file:
        GLMotif::FileSelectionDialog* fsDialog=new GLMotif::FileSelectionDialog(Vrui::getWidgetManager(),"Load Palette File...",cCurrentPath,".fib");
        fsDialog->getOKCallbacks().add(this,&FiberApplication::loadFiberOKCallback);
        fsDialog->getCancelCallbacks().add(this,&FiberApplication::loadFiberCancelCallback);
        Vrui::getWidgetManager()->popupPrimaryWidget(fsDialog,Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        inLoadFiber = true;
    }

}

//for show subdialog menu.
//not use for the moment
void FiberApplication::menuToggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
    //Adjust program state based on which toggle button changed state:
    if(strcmp(cbData->toggle->getName(),"showPropertiesDialogToggle")==0)
    {
        if(cbData->set)
        {
            //Open the render dialog at the same position as the main menu:
            Vrui::getWidgetManager()->popupPrimaryWidget(propertiesDialog,Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        }
        else
        {
            //Close the render dialog:
            Vrui::popdownPrimaryWidget(propertiesDialog);
        }
    }
    else if (strcmp(cbData->toggle->getName(), "ActivateSelectionBoxToggle") == 0)
    {
        m_showSelectionBox = cbData->set;
        for(int i=0; i<m_SelectionBox.size();i++)
        {
            m_SelectionBox[i]->setIsActive(m_showSelectionBox);
        }
    }
}

//method for all slider call back
void FiberApplication::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
}

void FiberApplication::loadFiberOKCallback(GLMotif::FileSelectionDialog::OKCallbackData* cbData)
{

    //Load the fiber file:
    std::string fileName = cbData->selectedFileName;

    //Destroy the file selection dialog:
    Vrui::getWidgetManager()->deleteWidget(cbData->fileSelectionDialog);
    //load Fiber
    mFibers.load(fileName);

    mFibers.initializeBuffer();
    inLoadFiber = false;
}


void FiberApplication::loadFiberCancelCallback(GLMotif::FileSelectionDialog::CancelCallbackData* cbData)
{
    //Destroy the file selection dialog:
    Vrui::getWidgetManager()->deleteWidget(cbData->fileSelectionDialog);
    inLoadFiber = false;
}

void FiberApplication::ButtonSelectedCallBack(Misc::CallbackData* cbData)
{

}

FiberApplication::FiberApplication(int& argc,char**& argv,char**& appDefaults)
    :Vrui::Application(argc,argv,appDefaults),
     mainMenu(0),
     propertiesDialog(0),
     m_fileName(""),
     m_showSelectionBox(true)
{
    //TODO create tool at launch if possible

    m_fileName = "fibers.fib";

    processCommandLineArguments(argc,argv);
    //Create the user interface:
    mainMenu=createMainMenu();

    //create Popup Window
    //propertiesDialog = createPropertiesDialog();

    //Install the main menu:
    Vrui::setMainMenu(mainMenu);

    if(m_fileName != "")
    {
        mFibers.load(m_fileName);
    }

    //Set the navigation transformation:
    resetNavigationCallback(0);

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

    // delete propertiesDialog;
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

void FiberApplication::updateSelectedFiber(Fibers* pFiber)
{
    const int fibersCount( pFiber->getLineCount());

    //if the selection box are active, initialize selectedFibers to false else initialize selectedFibers to true
    std::vector<bool> selectedFibers(fibersCount,!m_showSelectionBox);

    //find all fibers that pass in selection boxes
    for(std::vector<SelectionBox*>::iterator it = m_SelectionBox.begin(); it != m_SelectionBox.end(); it++)
    {
        if((*it)->isActive() && mFibers.containsSelectionBox((*it)->getBoundingBox()))
        {
            std::vector<bool> subSelectedFibers = (*it)->getSelectedFibers(pFiber);

            for(int i=0; i< fibersCount; i++)
            {
                selectedFibers[i] = selectedFibers[i] || subSelectedFibers[i];
            }
        }
    }

    pFiber->setSelectedFiber(selectedFibers);
}

void FiberApplication::frame(void)
{
    /*********************************************************************
    This function is called exactly once per frame, no matter how many
    eyes or windows exist. It is the perfect place to change application
    or Vrui state (run simulations, animate models, synchronize with
    background threads, change the navigation transformation, etc.).
    *********************************************************************/
    //update fibers selected if there is at least one selection box.
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

    mFibers.draw();

    for(int i=0; i<m_SelectionBox.size();i++)
    {
        m_SelectionBox[i]->draw();
    }

    //Draw the forward direction:
    /*glColor3f(1.0f,0.0f,0.0f);
    drawArrow(Vrui::Point(6.0,0.0,0.0),arrowRadius);

    //Draw the up direction:
    glColor3f(0.0f,1.0f,0.0f);
    drawArrow(Vrui::Point(0.0,6.0,0.0),arrowRadius);

    //Draw the up direction:
    glColor3f(0.0f,0.0f,1.0f);
    drawArrow(Vrui::Point(0.0,0.0,6.0),arrowRadius);*/

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

    //init glew lib, cannot init in constructor because the application not launch
    GLenum errorCode = glewInit();

    if( GLEW_OK != errorCode )
    {
       return;
    }

    //we cannot initialize buffer before the initialize glew.
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

void FiberApplication::processCommandLineArguments(int& argc, char**& argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (strcasecmp(argv[i] + 1, "file") == 0)
            {
                ++i;
                m_fileName = argv[i];
            }
        }
    }
}

