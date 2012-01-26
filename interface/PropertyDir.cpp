// PropertyDir.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"

#include "PropertyDir.h"

PropertyDir::PropertyDir(const wxChar* t, const wxChar* v, HeeksObj* object, void(*callbackfunc)(const wxChar*, HeeksObj*)):PropertyString(t, v, object, callbackfunc)
{
}

Property *PropertyDir::MakeACopy(void)const{
	PropertyDir* new_object = new PropertyDir(*this);
	return new_object;
}
