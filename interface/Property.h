// Property.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

// Base class for all Properties

#if !defined Property_HEADER
#define Property_HEADER

enum{
	InvalidPropertyType,
	StringPropertyType,
	DoublePropertyType,
	LengthPropertyType,
	IntPropertyType,
	VertexPropertyType,
	ChoicePropertyType,
	ColorPropertyType,
	CheckPropertyType,
	ListOfPropertyType,
	TrsfPropertyType,
	FilePropertyType,
	DirectoryPropertyType
};

#include "Python.h"
#include <vector>

class Property{
public:
	bool m_highlighted;
	HeeksObj* m_object;

	void(*m_selectcallback)(HeeksObj*);

	Property(HeeksObj* object, void(*selectcallback)(HeeksObj*) = NULL, bool highlighted = false):m_highlighted(highlighted), m_object(object), m_selectcallback(selectcallback) {}
	virtual ~Property(){}

	virtual int get_property_type(){return InvalidPropertyType;}
	virtual bool property_editable()const = 0;
	virtual Property *MakeACopy(void)const = 0;
	virtual void CallSetFunction()const = 0;
	virtual const wxChar* GetShortString(void)const{return _("Unknown Property");}

    /**
        Use the Python interpreter to evaluate the 'value' string passed in.  Before
        doing this, however, define all the variable=value pairs found in the properties
        list just in case the 'value' expression uses one of these properties as
        a reference.  eg: if a propertyDouble has 'diameter'='3.4' and the
        'value'='diameter / 2.0' then we need to end up returning '1.7' in a wxString.
     */
	virtual bool Evaluate( const std::list<Property *> properties, wxString value, wxString & evaluated_version ) const
    {
        // For this default method, don't do any interpretation.  Just return the value typed in.
        // Leave the interpretation for the PropertyDouble, PropertyLength and such.
        evaluated_version = value;
        return(true);
    }

    virtual PyObject *PyName() const { return(NULL); }
	virtual PyObject *PyValue() const { return(NULL); }
};

#endif
