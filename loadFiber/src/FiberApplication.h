#ifndef FIBERAPPLICATION_H_
#define FIBERAPPLICATION_H_

#include "fiber.h"

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

class FiberApplication:public Vrui::Application,public GLObject
{
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

    //Elements:
    private:

    //Vrui parameters:
    GLMotif::PopupMenu* mainMenu; // The program's main menu
    GLMotif::PopupWindow* propertiesDialog; // The properties settings dialog

    fiber mFibers;

    bool useFakeTube;
    bool inLoadFiber;

    //Private methods:
    GLMotif::PopupMenu* createMainMenu(void); // Creates the program's main menu
    GLMotif::PopupWindow* createPropertiesDialog(void); // Creates the Properties settings dialog
    GLMotif::Popup* createOrientationButtonMenu(void);

    void resetNavigationCallback(Misc::CallbackData* cbData); // Method to reset the Vrui navigation transformation to its default
    void OnLoadFiberCallBack(Misc::CallbackData* cbData);      // Method call when the load button are press
    void menuToggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData);
    void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
    void loadFiberOKCallback(GLMotif::FileSelectionDialog::OKCallbackData* cbData);// Method called when the Ok button are press in FileSelectionDialog
    void loadFiberCancelCallback(GLMotif::FileSelectionDialog::CancelCallbackData* cbData); // Method called when the cancel button are press in FileSelectionDialog
    void ButtonSelectedCallBack(Misc::CallbackData* cbData);

    //Constructors and destructors:
    public:
    FiberApplication(int& argc,char**& argv,char**& appDefaults); // Initializes the Vrui toolkit and the application
    virtual ~FiberApplication(void); // Shuts down the Vrui toolkit

    //Methods from Vrui::Application:
    virtual void frame(void); // Called exactly once per frame
    virtual void display(GLContextData& contextData) const; // Called for every eye and every window on every frame

    //Methods from GLObject:
    virtual void initContext(GLContextData& contextData) const; // Called once upon creation of each OpenGL context
};


#endif //FIBERAPPLICATION_H_
