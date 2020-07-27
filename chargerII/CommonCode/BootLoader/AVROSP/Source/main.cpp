/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.cpp
* Compiler          : Dev-C++ 4.9.8.0 - http://bloodshed.net/dev/
* Revision          : $Revision: 1.8 $
* Date              : $Date: Friday, July 09, 2004 06:35:28 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Target platform   : Win32
*
* AppNote           : AVR911 - AVR Open-source Programmer
*
* Description       : AVROSP main entry function.
*
* 
****************************************************************************/
#include "JobInfo.hpp"
#include <vector>
#include <string>

using namespace std;


int main(int argc, char *argv[])
{
	JobInfo j;

	try
	{
		j.parseCommandline( argc, argv );
		j.doJob();
	}
	catch( ErrorMsg * e )
	{
		cout << endl << "An error occurred:" << endl;
		cout << "  [" << e->What() << "]" << endl;
		
		delete e;
	}
  	
	return 0;
}

