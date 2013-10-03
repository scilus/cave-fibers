#include <stdio.h>
#include <iostream> 
#include <stdlib.h>

#include "fiber.h"

int main( int argc, const char* argv[] )
{
	std::string fileName = "fibers.fib";
	//load file fiber
	fiber myFiber;

	myFiber.load(fileName);

	//finnish that thusday poutine time
}