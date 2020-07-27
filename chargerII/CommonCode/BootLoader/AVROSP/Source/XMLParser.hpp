/*****************************************************************************
*
* Atmel Corporation
*
* File              : XMLParser.hpp
* Compiler          : Dev-C++ 4.9.8.0 - http://bloodshed.net/dev/
* Revision          : $Revision: 1.8 $
* Date              : $Date: Friday, July 09, 2004 06:35:32 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Target platform   : Win32
*
* AppNote           : AVR911 - AVR Open-source Programmer
*
* Description       : A simple XML DOM-like parser. It builds a complete tree from
*                     the XML file. IT supports <section/> tags, but not tag attributes.
*
* 
****************************************************************************/
#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

using namespace std;

#include "ErrorMsg.hpp"
#include "Utility.hpp"
#include <iostream>
#include <fstream>
#include <list>

class XMLAbstractNode; // Preliminary definitions.
class XMLTree;
class XMLNode;


/* Main XML file class. Contains search methods and entire XML tree */
class XMLFile
{
protected:
	XMLTree * root; // The root node, either a subtree or an ordinary node.

	void removeComments( string & txt ); // Remove comment tags.
 	void removeAttributes( string & txt ); // Remove attributes from tags.
  	void readFile( const string & _filename ); // Read XML file.
  	void parseFragment( string & fragment, XMLTree * parent );
	
public:
	/* Constructors */
	XMLFile( const string & _filename );
	
	/* Destructor */
	~XMLFile();
	
	/* Methods */
 	bool exists( const string & path ); // Checks if node exists.
 	const string & getValue( const string & path ); // Get node value.
 	
 	void print();
};

#endif

