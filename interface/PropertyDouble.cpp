// PropertyDouble.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"

#include "PropertyDouble.h"
#include <wx/textdlg.h>

extern CHeeksCADInterface heekscad_interface;

PropertyDouble::PropertyDouble(const wxChar* t, double initial_value, HeeksObj* object, void(*callbackfunc)(double, HeeksObj*), void(*selectcallback)(HeeksObj*)):Property(object, selectcallback){
	m_initial_value = initial_value;
	m_callbackfunc = callbackfunc;
	m_callbackfuncidx = 0;
	title = wxString(t);
	has_index = false;
}

PropertyDouble::PropertyDouble(const wxChar* t, double initial_value, HeeksObj* object, void(*callbackfunc)(double, HeeksObj*, int), int index, void(*selectcallback)(HeeksObj*)):Property(object, selectcallback)
{
	m_initial_value = initial_value;
	m_callbackfuncidx = callbackfunc;
	m_callbackfunc = 0;
	m_object = object;
	title = wxString(t);
	has_index = true;
	m_index = index;
}

PropertyDouble::~PropertyDouble(){
}

const wxChar* PropertyDouble::GetShortString(void)const{
	return title.c_str();
}

Property *PropertyDouble::MakeACopy(void)const{
	PropertyDouble* new_object = new PropertyDouble(*this);
	return new_object;
}

void PropertyDouble::CallSetFunction() const
{
	if(m_callbackfunc)(*m_callbackfunc)(m_initial_value, m_object);
	if(m_callbackfuncidx)(*m_callbackfuncidx)(m_initial_value, m_object,m_index);
}


static std::pair<wxString::size_type, wxString::size_type> next_possible_number(const wxString & value, wxString::size_type offset )
{
	std::pair<wxString::size_type, wxString::size_type> results = std::make_pair(wxString::npos, wxString::npos);

	for ( ; offset < value.length(); offset++)
	{
		if ((value[offset] == '-') ||
			(value[offset] == '+') ||
			(value[offset] == '.') ||
			((value[offset] >= '0') &&
			 (value[offset] <= '9')))
		{
			if (results.first == wxString::npos)
			{
				results.first = offset;
			}
			else
			{
				// We already have the starting point.  Just keep stepping through the number.
			}
		}
		else
		{
			if (results.first != wxString::npos)
			{
				results.second = offset-1;
				return(results);
			}
		}
	}

	if (results.first != wxString::npos)
	{
		results.second = value.length() - 1;
		return(results);
	}

	return(results);
}





/**
    Use the Python interpreter to evaluate the 'value' string passed in.  Before
    doing this, however, define all the variable=value pairs found in the properties
    list just in case the 'value' expression uses one of these properties as
    a reference.  eg: if a propertyDouble has 'diameter'='3.4' and the
    'value'='diameter / 2.0' then we need to end up returning '1.7' in a wxString.
 */
/* static */ bool PropertyDouble::EvaluateWithPython( const std::list<Property *> *pProperties, wxString value, wxString & evaluated_version )
{
    // wxString evaluated_version;
    bool return_status = false;

    // Py_Initialize() and Py_Finalize() should only occur once per process.  Do it in the main application framework instead.
    // Py_Initialize();

    PyObject *pModule = PyImport_ImportModule("math");

    if (pModule != NULL)
    {
        PyObject *pDictionary = PyModule_GetDict(pModule);

        if (pDictionary != NULL)
        {
			if (pProperties != NULL)
			{
				// Add the variable=value commands to this dictionary just in case the user wants
				// to use one of the other parameters within their expression.  eg: 'diameter/2.0'
				for (std::list<Property *>::const_iterator itProperty = pProperties->begin(); itProperty != pProperties->end(); itProperty++)
				{
					PyObject *pName = (*itProperty)->PyName();
					PyObject *pValue = (*itProperty)->PyValue();

					if ((pName != NULL) && (pValue != NULL))
					{
						PyDict_SetItem(pDictionary, pName, pValue);
					}

					Py_XDECREF(pName); pName=NULL;
					Py_XDECREF(pValue); pValue=NULL;
				}
			}

            // Add some artificial variables just to see if it all works (and it does).  One day
            // I will add variables for 'last point entered' so that subsequent points can be
            // offset from them.

#ifdef HEEKSCAD
            if (wxGetApp().m_digitizing != NULL)
            {
				double pos[3];
                heekscad_interface.GetReferencePoint(pos);

                wxString last_digitized;
                last_digitized << _T("dx=") << pos[0] << _T("\n");
                last_digitized << _T("dy=") << pos[1] << _T("\n");
                last_digitized << _T("dz=") << pos[2] << _T("\n");
                // wxMessageBox( last_digitized );

                PyObject *pFirstResult = PyRun_String(Ttc(last_digitized), Py_file_input, pDictionary, pDictionary);
                if (pFirstResult == NULL)
                {
                    wxMessageBox(_("Could not add digitize points"));
                }
                else
                {
                    Py_XDECREF(pFirstResult);
                    pFirstResult = NULL;
                }
            }
#endif // HEEKSCAD


#ifdef HEEKSCAD
	#define VIEW_UNITS (wxGetApp().m_view_units)
#else
	#define VIEW_UNITS (heeksCAD->GetViewUnits())
#endif

			wxString interpreted_value(value);

			// Run through the value looking for the various units (inch/mm) and
			// add the modification required to convert it into mm.
			//
			// eg: '1/8 inch' -> '((1/8) * 25.4)'  (assuming we're using mm)
			// eg: '1/8 inch' -> '((1/8) * 1.0)'  (assuming we're using inches)
			// eg: '1/8 mm' -> '((1/8) / 25.4)'  (assuming we're using inches)

			typedef std::list< std::pair<wxString, wxString> >  Patterns_t;
			Patterns_t patterns;

			if (VIEW_UNITS == 1.0)
			{
				// We're using mm.
				patterns.push_back( std::make_pair(_("Inches"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("inches"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("INCHES"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("Inch"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("INCH"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("inch"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("In"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("IN"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("in"), _T(" * 25.4")));
				patterns.push_back( std::make_pair(_("mm"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("MM"), _T(" * 1.0")));
			}
			else
			{
				// We're using inches.
				patterns.push_back( std::make_pair(_("Inches"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("inches"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("INCHES"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("Inch"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("INCH"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("inch"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("In"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("IN"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("in"), _T(" * 1.0")));
				patterns.push_back( std::make_pair(_("mm"), _T(" / 25.4")));
				patterns.push_back( std::make_pair(_("MM"), _T(" / 25.4")));
			}

			// NOTE: The patterns list must be arranged from most verbose to least verbose so that
			// a substitution of 'in' does not occur before the substitution of 'inches'
			patterns.sort();
			patterns.reverse();

			for (Patterns_t::iterator itPattern = patterns.begin(); itPattern != patterns.end(); itPattern++)
			{
				int offset = -1;
				while ((offset = interpreted_value.Find(itPattern->first)) >= 0)
				{
					interpreted_value.insert(0,_T("(("));
					wxString replacement;
					replacement << _T(")") << itPattern->second << _T(")");
					interpreted_value.replace(offset+2, itPattern->first.length(), replacement);
				}
			}

			// Now step through the value and change all integer values (3) into floating point
			// values (3.0) so that values such as '1/8' will be interpreted as '1.0/8.0' to force floating
			// point arithmetic rather than integer arithmetic.

			// eg: '((1/8) * 25.4)' -> '((1.0/8.0) * 25.4)'

			for (wxString::size_type i = 0; i <= interpreted_value.length();)
			{
				std::pair<wxString::size_type, wxString::size_type> pointers = next_possible_number(interpreted_value, i);
				if ((pointers.first != wxString::npos) && (pointers.second != wxString::npos))
				{
					wxString replacement(interpreted_value.substr(0, pointers.first));
					wxString a(interpreted_value.substr(pointers.first, pointers.second - pointers.first + 1));
					if (a.Find('.') < 0)
					{
						a << _T(".0");
					}

					replacement << a;

					i = replacement.length();

					if (pointers.second < interpreted_value.length())
					{
						replacement.append(interpreted_value.substr(pointers.second+1));
					}

					interpreted_value.assign(replacement);
				}
				else
				{
					break;
				}
			}

			// Remove any leading or trailing spaces.
			while ((interpreted_value.size() > 0) && (interpreted_value[0] == ' '))
			{
			    interpreted_value.erase(0,1);
			}

			while ((interpreted_value.size() > 0) && (interpreted_value[interpreted_value.size()-1] == ' '))
			{
			    interpreted_value.erase(interpreted_value.size()-1,1);
			}

            PyObject *pResult = PyRun_String(Ttc(interpreted_value), Py_eval_input, pDictionary, pDictionary);
            if (pResult != NULL)
            {
                double d;
                if (PyArg_Parse(pResult, "d", &d))
                {
                    evaluated_version << d;
                    return_status = true;
                    Py_XDECREF(pResult);
                    pResult = NULL;
                }
                else
                {
                    wxMessageBox(_("Value does not evaluate to a floating point number"));
                    Py_XDECREF(pResult);
                    pResult = NULL;
                    return_status = false;
                }
            }
            else
            {
                wxString message;
                message << _("Could not evaluate ") << interpreted_value << _(" using Python");
                wxMessageBox(message);
                return_status = false;
            }

            // Do NOT release the dictionary - I still don't know why.
            // Py_XDECREF(pDictionary);
            // pDictionary = NULL;
        }
        else
        {
            wxMessageBox(_("Could not load dictionary from 'math' module in Python interpreter"));
            return_status = false;
        }

        Py_XDECREF(pModule);
        pModule = NULL;
    }
    else
    {
        wxMessageBox(_("Could not load 'math' module in Python interpreter"));
        return_status = false;
    }

    // Py_Initialize() and Py_Finalize() should only occur once per process.  Do it in the main application framework instead.
    // Py_Finalize();

    // return(evaluated_version);
    return(return_status);
}

/* virtual */ bool PropertyDouble::Evaluate( const std::list<Property *> properties, wxString value, wxString & evaluated_version ) const
{
	return(EvaluateWithPython( &properties, value, evaluated_version ));
}


/* virtual */ PyObject *PropertyDouble::PyName() const
{
    return(PyString_FromString(Ttc(GetShortString())));
}

/* virtual */ PyObject *PropertyDouble::PyValue() const
{
    return(PyFloat_FromDouble(m_initial_value));
}

