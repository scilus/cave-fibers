#include <stdio.h>
#include <stdlib.h>

#include "FiberApplication.h"

int main(int argc,char* argv[])
{
	/* Create an application object: */
	FiberApplication app(argc,argv);

	/* Run the Vrui main loop: */
	app.run();
	/* Exit to OS: */
	return 0;
}
