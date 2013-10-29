/*
 * FiberApplication.h
 *
 *  Created on: Oct 9, 2013
 *      Author: belp2210
 */

#ifndef FIBERAPPLICATION_H_
#define FIBERAPPLICATION_H_

#include "fiber.h"

#include <iostream>
#include <stdexcept>
#include <Math/Math.h>
#include <GL/GLObject.h>
#include <Vrui/Vrui.h>
#include <Vrui/Application.h>

class FiberApplication:public Vrui::Application,public GLObject
{
    /* Embedded classes: */
    private:
    struct DataItem:public GLObject::DataItem // Data structure storing OpenGL-dependent application data
    {
        //Elements:
        public:
        GLuint 	textureObjectId; // Texture object ID of some texture
        GLuint 	displayListId; // Display list ID of some display list
        GLuint* bufferObjects;

        //Constructors and destructors:
        DataItem(void)
        {
            /* Create a texture object to hold a texture: */
            glGenTextures(1,&textureObjectId);

            /* Create a display list: */
            displayListId=glGenLists(1);

            //create buffer
            bufferObjects = new GLuint[3];
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
    Vrui::Scalar modelAngles[3]; // Euler angles to animate the model in degrees
    Vrui::Scalar rotationSpeeds[3]; // Rotation speeds around the Euler axes in degrees/s

    //Vrui parameters:
    GLMotif::PopupMenu* mainMenu; // The program's main menu

    fiber mFibers;

    //Private methods:
    GLMotif::PopupMenu* createMainMenu(void); // Creates the program's main menu
    void resetNavigationCallback(Misc::CallbackData* cbData); // Method to reset the Vrui navigation transformation to its default

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


#endif /* FIBERAPPLICATION_H_ */
