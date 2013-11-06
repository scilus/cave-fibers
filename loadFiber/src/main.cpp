#include <stdio.h>
#include <stdlib.h>

#include "FiberApplication.h"

int main(int argc,char* argv[])
{
    /* Create an application object: */
    char** appDefaults=0; // This is an additional parameter no one ever uses
    FiberApplication app(argc,argv,appDefaults);

    /* Run the Vrui main loop: */
    app.run();
    /* Exit to OS: */
    return 0;
}
