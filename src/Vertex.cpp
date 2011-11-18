// Vertex.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "Vertex.h"
#include "Face.h"
#include "Solid.h"
#include "Gripper.h"
#include "DigitizeMode.h"
#include "Drawing.h"
#include "HPoint.h"

CVertex::CVertex(const TopoDS_Vertex &vertex):m_topods_vertex(vertex){
	gp_Pnt pos = BRep_Tool::Pnt(vertex);
	extract(pos, m_point);
}

CVertex::~CVertex(){
}

void CVertex::FindEdges()
{
	CShape* body = GetParentBody();
	if(body)
	{
		for(HeeksObj* object = body->m_edges->GetFirstChild(); object; object = body->m_edges->GetNextChild())
		{
			CEdge* e = (CEdge*)object;
			CVertex* v0 = e->GetVertex0();
			CVertex* v1 = e->GetVertex1();
			if(v0 == this || v1 == this)m_edges.push_back(e);
		}
	}
}

const wxBitmap &CVertex::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/vertex.png")));
	return *icon;
}

void CVertex::glCommands(bool select, bool marked, bool no_color){
	// don't render anything, but put a point for selection
	glRasterPos3dv(m_point);
}

void CVertex::GetGripperPositions(std::list<GripData> *list, bool just_for_endof){
	list->push_back(GripData(GripperTypeTranslate,m_point[0],m_point[1],m_point[2],NULL));
}

CEdge* CVertex::GetFirstEdge()
{
	if (m_edges.size()==0)FindEdges();
	if (m_edges.size()==0) return NULL;
	m_edgeIt = m_edges.begin();
	return *m_edgeIt;
}

CEdge* CVertex::GetNextEdge()
{
	if (m_edges.size()==0 || m_edgeIt==m_edges.end()) return NULL;
	m_edgeIt++;
	if (m_edgeIt==m_edges.end()) return NULL;
	return *m_edgeIt;
}

CShape* CVertex::GetParentBody()
{
#ifdef MULTIPLE_OWNERS
	if(HEEKSOBJ_OWNER == NULL)return NULL;
	if(HEEKSOBJ_OWNER->HEEKSOBJ_OWNER == NULL)return NULL;
	if(HEEKSOBJ_OWNER->HEEKSOBJ_OWNER->GetType() != SolidType)return NULL;
	return (CShape*)(HEEKSOBJ_OWNER->HEEKSOBJ_OWNER);
#else
	if(m_owner == NULL)return NULL;
	if(m_owner->m_owner == NULL)return NULL;
	if(m_owner->m_owner->GetType() != SolidType)return NULL;
	return (CShape*)(m_owner->m_owner);
#endif
}


class ClickVertexLocation:public Tool{
public:
	CVertex *which;

public:
	void Run(){
		wxGetApp().m_digitizing->digitized_point = DigitizedPoint( gp_Pnt(which->m_point[0], which->m_point[1], which->m_point[2]), DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}

		DigitizeMode *pDigitizeMode = dynamic_cast<DigitizeMode *>(wxGetApp().input_mode_object);
		if (pDigitizeMode != NULL)
		{	
			// Tell the DigitizeMode class that we're specifying the
			// location rather than the mouse location over the graphics window.

			pDigitizeMode->DigitizeToLocatedPosition( gp_Pnt(which->m_point[0], which->m_point[1], which->m_point[2]) );
		}
	}
	const wxChar* GetTitle(){return _("Click vertex location");}
	wxString BitmapPath(){return _T("pickpos");}
	const wxChar* GetToolTip(){return _("Click vertex location");}
};
static ClickVertexLocation click_vertex_location;

class OffsetFromPoint:public Tool{
public:
	CVertex *which;

public:
	void Run(){
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		DigitizeMode *pDigitizeMode = dynamic_cast<DigitizeMode *>(wxGetApp().input_mode_object);
		if ((pDrawingMode != NULL) || (pDigitizeMode != NULL))
		{
			gp_Pnt location = HPoint::GetOffset(gp_Pnt(which->m_point[0], which->m_point[1], which->m_point[2]));
		
			if (pDrawingMode != NULL)
			{
				wxGetApp().m_digitizing->digitized_point = DigitizedPoint(location, DigitizeInputType);
				pDrawingMode->AddPoint();
			}

			if (pDigitizeMode != NULL)
			{
				pDigitizeMode->DigitizeToLocatedPosition( location );
			}
		}
	}
	const wxChar* GetTitle(){return _("Offset from vertex location");}
	wxString BitmapPath(){return _T("pickpos");}
	const wxChar* GetToolTip(){return _("Offset from vertex location");}
};
static OffsetFromPoint offset_from_point;


void CVertex::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
	DigitizeMode *pDigitizeMode = dynamic_cast<DigitizeMode *>(wxGetApp().input_mode_object);
	if ((pDrawingMode != NULL) || (pDigitizeMode != NULL))
	{
		click_vertex_location.which = this;
		t_list->push_back(&click_vertex_location);
		
		offset_from_point.which = this;
		t_list->push_back(&offset_from_point);
	}
}

