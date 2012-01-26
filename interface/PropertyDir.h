// PropertyDir.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined PropertyDir_HEADER
#define PropertyDir_HEADER

#include "PropertyString.h"

class PropertyDir:public PropertyString{
public:
	wxString m_title;

	PropertyDir(const wxChar* t, const wxChar* v, HeeksObj* object, void(*callbackfunc)(const wxChar*, HeeksObj*) = NULL);

	// Property's virtual functions
	int get_property_type(){return DirectoryPropertyType;}
	Property *MakeACopy(void)const;
};

#endif
