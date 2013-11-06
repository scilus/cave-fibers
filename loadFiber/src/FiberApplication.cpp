#include "FiberApplication.h"

//Vrui includes
#include <Geometry/OrthogonalTransformation.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLVertexTemplates.h>
#include <GL/GLContextData.h>
#include <GL/GLGeometryWrappers.h>
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


GLMotif::PopupMenu* FiberApplication::createMainMenu(void)
{
    //Create a popup shell to hold the main menu:
    GLMotif::PopupMenu* mainMenuPopup=new GLMotif::PopupMenu("MainMenuPopup",Vrui::getWidgetManager());
    mainMenuPopup->setTitle("Fiber Application");

    //Create the main menu itself:
    GLMotif::Menu* mainMenu=new GLMotif::Menu("MainMenu",mainMenuPopup,false);

    //Create a button:
    GLMotif::Button* resetNavigationButton=new GLMotif::Button("ResetNavigationButton",mainMenu,"Reset Navigation");

    //Add a callback to the button:
    resetNavigationButton->getSelectCallbacks().add(this,&FiberApplication::resetNavigationCallback);

    GLMotif::Button* loadButton=new GLMotif::Button("LoadButton",mainMenu,"Open");
    loadButton->getSelectCallbacks().add(this,&FiberApplication::OnLoadFiberCallBack);

    GLMotif::Button* addSelectionBox=new GLMotif::Button("AddSelectionBox",mainMenu,"New Selection Box");
    addSelectionBox->getSelectCallbacks().add(this,&FiberApplication::OnAddSelectionBoxCallBack);

    GLMotif::CascadeButton* orientationButtonCascade=new GLMotif::CascadeButton("orientationButtonCascade",mainMenu,"Orientation");
    orientationButtonCascade->setPopup(createOrientationButtonMenu());

	//Create a toggle button to show the render settings dialog:
	GLMotif::ToggleButton* showPropertiesDialogToggle=new GLMotif::ToggleButton("showPropertiesDialogToggle",mainMenu,"Show Properties Dialog");
	showPropertiesDialogToggle->setToggle(false);
	showPropertiesDialogToggle->getValueChangedCallbacks().add(this,&FiberApplication::menuToggleSelectCallback);

    //Finish building the main menu:
    mainMenu->manageChild();

    return mainMenuPopup;
}

GLMotif::Popup* FiberApplication::createOrientationButtonMenu(void)
{
	//Create the submenu's top-level shell:
	GLMotif::Popup* orientationButtonMenuPopup=new GLMotif::Popup("orientationButtonMenuPopup",Vrui::getWidgetManager());

	//Create the array of rorientation buttons inside the top-level shell:
	GLMotif::SubMenu* orientationButtonMenu=new GLMotif::SubMenu("orientationButtonMenu",orientationButtonMenuPopup,false);

	//these callback are not implemented
	GLMotif::Button* left=new GLMotif::Button("LeftButton",orientationButtonMenu,"orientation to left");
	left->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

	GLMotif::Button* front=new GLMotif::Button("FrontButton",orientationButtonMenu,"orientation to front");
	front->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

	GLMotif::Button* right=new GLMotif::Button("RightButton",orientationButtonMenu,"orientation to right");
	right->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

	GLMotif::Button* behind=new GLMotif::Button("BehindButton",orientationButtonMenu,"orientation to behind");
	behind->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

	GLMotif::Button* above=new GLMotif::Button("AboveButton",orientationButtonMenu,"orientation to above");
	above->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

	GLMotif::Button* below=new GLMotif::Button("BelowButton",orientationButtonMenu,"orientation to below");
	below->getSelectCallbacks().add(this,&FiberApplication::ButtonSelectedCallBack);

	/* Calculate the submenu's proper layout: */
	orientationButtonMenu->manageChild();

	/* Return the created top-level shell: */
	return orientationButtonMenuPopup;
}

GLMotif::PopupWindow* FiberApplication::createPropertiesDialog(void)
{
	const GLMotif::StyleSheet& ss=*Vrui::getWidgetManager()->getStyleSheet();

	GLMotif::PopupWindow* propertiesDialogPopup=new GLMotif::PopupWindow("propertiesDialogPopup",Vrui::getWidgetManager(),"Display Settings");
	propertiesDialogPopup->setResizableFlags(true,false);

	GLMotif::RowColumn* propertiesDialog=new GLMotif::RowColumn("propertiesDialog",propertiesDialogPopup,false);
	propertiesDialog->setOrientation(GLMotif::RowColumn::VERTICAL);
	propertiesDialog->setPacking(GLMotif::RowColumn::PACK_TIGHT);
	propertiesDialog->setNumMinorWidgets(2);

	new GLMotif::Label("MinLenghtLabel",propertiesDialog,"Min Lenght");

	GLMotif::Slider* MinLenghtSlider=new GLMotif::Slider("MinLenghtSlider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
	MinLenghtSlider->setValueRange(0.0,1.0,0.001);
	MinLenghtSlider->setValue(0);
	MinLenghtSlider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);

	new GLMotif::Label("MaxLenghtLabel",propertiesDialog,"Max Lenght");

	GLMotif::Slider* MaxLenghtSlider=new GLMotif::Slider("MaxLenghtSlider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
	MaxLenghtSlider->setValueRange(0.0,1.0,0.001);
	MaxLenghtSlider->setValue(0);
	MaxLenghtSlider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);

	new GLMotif::Label("SubsamplingLabel",propertiesDialog,"Subsampling");

	GLMotif::Slider* Subsamplinglider=new GLMotif::Slider("Subsamplinglider",propertiesDialog,GLMotif::Slider::HORIZONTAL,ss.fontHeight*5.0f);
	Subsamplinglider->setValueRange(0.0,1.0,0.001);
	Subsamplinglider->setValue(0);
	Subsamplinglider->getValueChangedCallbacks().add(this,&FiberApplication::sliderCallback);

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
}

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

	mFibers.updateLinesShown();

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

void FiberApplication::OnAddSelectionBoxCallBack(Misc::CallbackData* cbData)
{

}

FiberApplication::FiberApplication(int& argc,char**& argv,char**& appDefaults)
    :Vrui::Application(argc,argv,appDefaults),
     mainMenu(0),
     propertiesDialog(0)
{
    //Create the user interface:
    mainMenu=createMainMenu();

    //create Popup Window
    propertiesDialog = createPropertiesDialog();

    //Install the main menu:
    Vrui::setMainMenu(mainMenu);

    //Set the navigation transformation:
    resetNavigationCallback(0);
}

FiberApplication::~FiberApplication(void)
{
    //Destroy the user interface:
    delete mainMenu;
    delete propertiesDialog;
}

void FiberApplication::frame(void)
{
    /*********************************************************************
    This function is called exactly once per frame, no matter how many
    eyes or windows exist. It is the perfect place to change application
    or Vrui state (run simulations, animate models, synchronize with
    background threads, change the navigation transformation, etc.).
    *********************************************************************/

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

    //Save OpenGL state:
    glPushAttrib(GL_ENABLE_BIT);

    //Set up the animation transformation:
    glPushMatrix();

    //Call the display list created in the initDisplay() method:
    glCallList(dataItem->displayListId);

    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    mFibers.draw();

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

