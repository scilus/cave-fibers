#ifndef FIBERAPPLICATION_H_
#define FIBERAPPLICATION_H_

#include "fiber.h"
#include "SelectionBox.h"

#include <iostream>
#include <stdexcept>
#include <Math/Math.h>

//Vrui includes
#include <GL/GLObject.h>
#include <GLMotif/Slider.h>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/FileSelectionDialog.h>

#include <GLMotif/Menu.h>
#include <GLMotif/TextField.h>

//Vrui includes to use the Vrui interface
#include <Vrui/Vrui.h>
#include <Vrui/Application.h>

#include <Vrui/DraggingToolAdapter.h>

class FiberApplication:public Vrui::Application,public GLObject
{
public:
	typedef DataSetInfo::Point Point;
	typedef SelectionBox::ONTransform ONTransform; // Type for object positions/orientations

    //Embedded classes:
    private:
    struct DataItem:public GLObject::DataItem // Data structure storing OpenGL-dependent application data
    {
        //Elements:
        public:
        GLuint         textureObjectId; // Texture object ID of some texture
        GLuint         displayListId; // Display list ID of some display list

        //Constructors and destructors:
        DataItem(void)
        {
            //Create a texture object to hold a texture:
            glGenTextures(1,&textureObjectId);

            //Create a display list:
            displayListId=glGenLists(1);
        };
        virtual ~DataItem(void)
        {
            //Destroy the texture object:
            glDeleteTextures(1,&textureObjectId);

            //Destroy the display list:
            glDeleteLists(displayListId,1);
        };
    };

    class ObjectDragger:public Vrui::DraggingToolAdapter // Class to drag object
	{
		//Elements:
		private:
    	FiberApplication* application; // Pointer to the application object "owning" this dragger
		bool dragging; // Flag whether the dragged object is valid
		ONTransform dragTransform; // The dragging transformation applied to the dragged object
		SelectionBox* m_selectedBox;

		//Constructors and destructors:
		public:
		ObjectDragger(Vrui::DraggingTool* sTool, FiberApplication* sApplication);

		//Methods:
		virtual void dragStartCallback(Vrui::DraggingTool::DragStartCallbackData* cbData);
		virtual void dragCallback(Vrui::DraggingTool::DragCallbackData* cbData);
		virtual void dragEndCallback(Vrui::DraggingTool::DragEndCallbackData* cbData);
	};

    public:
    std::vector<SelectionBox*>& getSelectionBoxVector();

    //Elements:
    private:

    //Vrui parameters:
    GLMotif::PopupMenu* mainMenu; // The program's main menu
    GLMotif::PopupWindow* propertiesDialog; // The properties settings dialog

    Fibers mFibers;
    bool m_showSelectionBox;

    std::vector<ObjectDragger*> m_objectDragger;
    std::vector<SelectionBox*> m_SelectionBox;

    std::string m_fileName;

    bool useFakeTube;
    bool inLoadFiber;

    //Private methods:
    GLMotif::PopupMenu* createMainMenu(void); // Creates the program's main menu
    GLMotif::PopupWindow* createPropertiesDialog(void); // Creates the Properties settings dialog
    GLMotif::Popup* createOrientationButtonMenu(void);

    void resetNavigationCallback(Misc::CallbackData* cbData); // Method to reset the Vrui navigation transformation to its default

    void OnAddSelectionBoxCallBack(Misc::CallbackData* cbData);
    void OnLoadFiberCallBack(Misc::CallbackData* cbData);      // Method call when the load button are press
    void menuToggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
    void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
    void loadFiberOKCallback(GLMotif::FileSelectionDialog::OKCallbackData* cbData);// Method called when the Ok button are press in FileSelectionDialog
    void loadFiberCancelCallback(GLMotif::FileSelectionDialog::CancelCallbackData* cbData); // Method called when the cancel button are press in FileSelectionDialog
    void ButtonSelectedCallBack(Misc::CallbackData* cbData);

    //Constructors and destructors:
    public:
    FiberApplication(int& argc, char**& argv, char**& appDefaults); // Initializes the Vrui toolkit and the application
    virtual ~FiberApplication(void); // Shuts down the Vrui toolkit

    void drawArrow(const Vrui::Point& to, Vrui::Scalar radius) const;
    void updateSelectedFiber(Fibers* pFiber);

    //Methods from Vrui::Application:
    virtual void toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData* cbData);
    virtual void toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData* cbData);
    virtual void frame(void); // Called exactly once per frame
    virtual void display(GLContextData& contextData) const; // Called for every eye and every window on every frame

    //Methods from GLObject:
    virtual void initContext(GLContextData& contextData) const; // Called once upon creation of each OpenGL context

    void processCommandLineArguments(int& argc, char**& argv);
};


#endif //FIBERAPPLICATION_H_
