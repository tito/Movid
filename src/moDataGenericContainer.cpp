/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#include "moDataGenericContainer.h"
#include "moProperty.h"

moDataGenericContainer::moDataGenericContainer() {
}

moDataGenericContainer::~moDataGenericContainer() {
	std::map<std::string, moProperty*>::iterator it;
	for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		delete (it->second);
	this->properties.clear();
}


moDataGenericContainer* moDataGenericContainer::clone(){
	std::map<std::string, moProperty*>::iterator it;
    moDataGenericContainer* clone = new moDataGenericContainer();
    for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		clone->properties[it->first] = new moProperty(it->second->asString());
    return clone;
}

bool moDataGenericContainer::exist(const std::string &name) {
	std::map<std::string, moProperty*>::iterator it;
	it = this->properties.find(name);
	return it == this->properties.end() ? false : true;
}

