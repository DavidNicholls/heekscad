
#pragma once

typedef struct BoltParams
{
public:
	typedef enum
	{
		ISO7380 = 0,	// ISO Socket Button Head screw.
	} eHead_t;

	eHead_t	m_head;
	wxString m_size;
	double	m_thread_diameter;
	double	m_thread_distance;
	double	m_head_diameter;
	double	m_head_height;
	double	m_flat_width;
	double	m_length;
} BoltParams;

// Cylinder.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#ifndef BOLT_CLASS_DEFINITION
#define BOLT_CLASS_DEFINITION

#include <TopoDS_Shape.hxx>
#include "Solid.h"

class CBolt: public CSolid{
protected:
	// CShape's virtual functions
	void MakeTransformedShape(const gp_Trsf &mat);
	wxString StretchedName() { return _("Stretched Bolt"); }

public:
	gp_Ax2 m_pos;
	BoltParams m_parameters;
	HeeksObj *m_pSolid;

	CBolt(const gp_Ax2& pos, BoltParams parameters, const wxChar* title, const HeeksColor& col, float opacity);
	CBolt(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity);

	// HeeksObj's virtual functions
	bool IsDifferent(HeeksObj* other);
	const wxChar* GetTypeString(void)const{return _("Bolt");}
	const wxBitmap &GetIcon();
	HeeksObj *MakeACopy(void)const;
	void GetProperties(std::list<Property *> *list);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void OnApplyProperties();
	int GetCentrePoints(double* pos, double* pos2);
	bool GetScaleAboutMatrix(double *m);
	bool Stretch(const double *p, const double* shift, void* data);
	bool DescendForUndo(){return false;}
	void glCommands(bool select, bool marked, bool no_color);

	// CShape's virtual functions
	void SetXMLElement(TiXmlElement* element);
	void SetFromXMLElement(TiXmlElement* pElem);

	// CSolid's virtual functions
	SolidTypeEnum GetSolidType(){return SOLID_TYPE_BOLT;}

	TopoDS_Shape GetShape() const;
	void KillGLLists(void);
    void DeleteSolid();
};

#endif // BOLT_CLASS_DEFINITION
