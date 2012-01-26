// Cylinder.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Bolt.h"
#include "../interface/PropertyVertex.h"
#include "../interface/PropertyDouble.h"
#include "../interface/PropertyLength.h"
#include "../interface/PropertyChoice.h"
#include "Gripper.h"
#include "MarkedList.h"
#include "HeeksConfig.h"
#include "HeeksFrame.h"
#include "../interface/HeeksCADInterface.h"

extern CHeeksCADInterface heekscad_interface;


// Mushroom (or Truss) head socket screws.
BoltParams	iso7380[] = {
					{BoltParams::ISO7380, _T("M3 x 0.5"), 3.0, 1.0, 5.7, 1.65, 2.045, 6.0 },
					{BoltParams::ISO7380, _T("M3 x 0.5"), 3.0, 1.0, 5.7, 1.65, 2.045, 8.0 },
					{BoltParams::ISO7380, _T("M3 x 0.5"), 3.0, 1.0, 5.7, 1.65, 2.045, 10.0 },
					{BoltParams::ISO7380, _T("M3 x 0.5"), 3.0, 1.0, 5.7, 1.65, 2.045, 12.0 },

					{BoltParams::ISO7380, _T("M4 x 0.7"), 4.0, 1.4, 7.6, 2.2,  2.560, 8.0 },
					{BoltParams::ISO7380, _T("M4 x 0.7"), 4.0, 1.4, 7.6, 2.2,  2.560, 10.0 },
					{BoltParams::ISO7380, _T("M4 x 0.7"), 4.0, 1.4, 7.6, 2.2,  2.560, 12.0 },
					{BoltParams::ISO7380, _T("M4 x 0.7"), 4.0, 1.4, 7.6, 2.2,  2.560, 16.0 },

					{BoltParams::ISO7380, _T("M5 x 0.8"), 5.0, 1.6, 9.5, 2.75,  3.08, 10.0 },
					{BoltParams::ISO7380, _T("M5 x 0.8"), 5.0, 1.6, 9.5, 2.75,  3.08, 12.0 },
					{BoltParams::ISO7380, _T("M5 x 0.8"), 5.0, 1.6, 9.5, 2.75,  3.08, 16.0 },
					{BoltParams::ISO7380, _T("M5 x 0.8"), 5.0, 1.6, 9.5, 2.75,  3.08, 20.0 },
					{BoltParams::ISO7380, _T("M5 x 0.8"), 5.0, 1.6, 9.5, 2.75,  3.08, 25.0 },
					{BoltParams::ISO7380, _T("M5 x 0.8"), 5.0, 1.6, 9.5, 2.75,  3.08, 30.0 },

					{BoltParams::ISO7380, _T("M6 x 1"), 6.0, 2.0, 10.5, 3.3,  4.095, 10.0 },
					{BoltParams::ISO7380, _T("M6 x 1"), 6.0, 2.0, 10.5, 3.3,  4.095, 12.0 },
					{BoltParams::ISO7380, _T("M6 x 1"), 6.0, 2.0, 10.5, 3.3,  4.095, 16.0 },
					{BoltParams::ISO7380, _T("M6 x 1"), 6.0, 2.0, 10.5, 3.3,  4.095, 20.0 },
					{BoltParams::ISO7380, _T("M6 x 1"), 6.0, 2.0, 10.5, 3.3,  4.095, 25.0 },
					{BoltParams::ISO7380, _T("M6 x 1"), 6.0, 2.0, 10.5, 3.3,  4.095, 30.0 },

					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 10.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 12.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 16.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 20.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 25.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 30.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 35.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 40.0 },

					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 10.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 12.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 16.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 20.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 25.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 30.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 35.0 },
					{BoltParams::ISO7380, _T("M8 x 1.25"), 8.0, 2.5, 14.0, 4.4,  5.095, 40.0 },

					{BoltParams::ISO7380, _T("M10 x 1.5"), 10.0, 3.0, 17.5, 5.5,  6.095, 16.0 },
					{BoltParams::ISO7380, _T("M10 x 1.5"), 10.0, 3.0, 17.5, 5.5,  6.095, 20.0 },
					{BoltParams::ISO7380, _T("M10 x 1.5"), 10.0, 3.0, 17.5, 5.5,  6.095, 25.0 },
					{BoltParams::ISO7380, _T("M10 x 1.5"), 10.0, 3.0, 17.5, 5.5,  6.095, 30.0 },
					{BoltParams::ISO7380, _T("M10 x 1.5"), 10.0, 3.0, 17.5, 5.5,  6.095, 35.0 },
					{BoltParams::ISO7380, _T("M10 x 1.5"), 10.0, 3.0, 17.5, 5.5,  6.095, 40.0 },

					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 16.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 20.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 25.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 30.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 35.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 40.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 45.0 },
					{BoltParams::ISO7380, _T("M12 x 1.75"), 12.0, 3.5, 21.0, 6.6,  8.115, 50.0 },

					{BoltParams::ISO7380, _T("M16 x 2"), 16.0, 4.0, 28.0, 8.8,  10.115, 25.0 },
					{BoltParams::ISO7380, _T("M16 x 2"), 16.0, 4.0, 28.0, 8.8,  10.115, 30.0 },
					{BoltParams::ISO7380, _T("M16 x 2"), 16.0, 4.0, 28.0, 8.8,  10.115, 35.0 },
					{BoltParams::ISO7380, _T("M16 x 2"), 16.0, 4.0, 28.0, 8.8,  10.115, 40.0 },
					{BoltParams::ISO7380, _T("M16 x 2"), 16.0, 4.0, 28.0, 8.8,  10.115, 45.0 },
					{BoltParams::ISO7380, _T("M16 x 2"), 16.0, 4.0, 28.0, 8.8,  10.115, 50.0 }
				};


static TopoDS_Solid MakeBolt(const gp_Ax2& pos, BoltParams parameters)
{
	gp_Ax2 pos2 = pos;
	/*
	if(height<0)
	{
		pos2 = gp_Ax2(pos.Location(), -(pos.Direction()));
		height = fabs(height);
	}
	*/

	return BRepPrimAPI_MakeCylinder(pos2, parameters.m_thread_diameter/2.0, parameters.m_length);
}

CBolt::CBolt(const gp_Ax2& pos, BoltParams parameters, const wxChar* title, const HeeksColor& col, float opacity):CSolid(MakeBolt(pos, parameters), title, col, opacity), m_pos(pos), m_parameters(parameters)
{
    m_pSolid = NULL;
	HeeksConfig config;

	/*
	if ((m_parameters.m_thread_diameter / 2.0) <= 0)
	{
		config.Read(_T("Cylinder_radius"), &(m_parameters.m_thread_diameter / 2.0), 10.0);
	}

	if (m_parameters.m_length == 0)
	{
		config.Read(_T("Cylinder_height"), &m_parameters.m_length, 10.0);
	}
	*/
}

CBolt::CBolt(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity):CSolid(solid, title, col, opacity), m_pos(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), m_parameters(iso7380[0])
{
    m_pSolid = NULL;
}

const wxBitmap &CBolt::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/cyl.png")));
	return *icon;
}

HeeksObj *CBolt::MakeACopy(void)const
{
	return new CBolt(*this);
}

bool CBolt::IsDifferent(HeeksObj* other)
{
	CBolt* bolt = (CBolt*)other;
	if (! (*this == *bolt)) return(true);

	return CShape::IsDifferent(other);
}

void CBolt::MakeTransformedShape(const gp_Trsf &mat)
{
	/*
	m_pos.Transform(mat);
	double scale = gp_Vec(1, 0, 0).Transformed(mat).Magnitude();
	(m_parameters.m_thread_diameter / 2.0) = fabs((m_parameters.m_thread_diameter / 2.0) * scale);
	m_parameters.m_length = fabs(m_parameters.m_length * scale);
	m_shape = MakeCylinder(m_pos, (m_parameters.m_thread_diameter / 2.0), m_parameters.m_length);
	*/
}

static void on_set_iso7380(int offset, HeeksObj *object)
{
	CBolt *pBolt = (CBolt *) object;
	pBolt->m_parameters = iso7380[offset];
	wxGetApp().m_frame->RefreshProperties();
    pBolt->KillGLLists();
    wxGetApp().Repaint();
	wxGetApp().Changed();
}


void CBolt::GetProperties(std::list<Property *> *list)
{
	CoordinateSystem::GetAx2Properties(list, m_pos);

	int choice = 0;
	std::list< wxString > choices;
	for (::size_t i=0; i<sizeof(iso7380)/sizeof(iso7380[0]); i++)
	{
		wxString option;
		option << iso7380[i].m_size << _T(" x ") << iso7380[i].m_length / wxGetApp().m_view_units;
		if (wxGetApp().m_view_units < 25.4)
		{
			option << _T(" mm");
		}
		else
		{
			option << _T(" in");
		}

		choices.push_back(option);
	}
	list->push_back(new PropertyChoice(_("ISO 7380 - Mushroom head cap screw"), choices, choice, this, on_set_iso7380));

	CSolid::GetProperties(list);
}

void CBolt::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	/*
	gp_Pnt o = m_pos.Location();
	gp_Pnt px(o.XYZ() + m_pos.XDirection().XYZ() * (m_parameters.m_thread_diameter / 2.0));
	gp_Dir z_dir = m_pos.XDirection() ^ m_pos.YDirection();
	gp_Pnt pyz(o.XYZ() + m_pos.YDirection().XYZ() * (m_parameters.m_thread_diameter / 2.0) + z_dir.XYZ() * m_parameters.m_length);
	gp_Pnt pmxz(o.XYZ() + m_pos.XDirection().XYZ() * (-(m_parameters.m_thread_diameter / 2.0)) + z_dir.XYZ() * m_parameters.m_length);
	gp_Pnt pz(o.XYZ() + z_dir.XYZ() * m_parameters.m_length);
	list->push_back(GripData(GripperTypeTranslate,o.X(),o.Y(),o.Z(),NULL));
	list->push_back(GripData(GripperTypeObjectScaleXY,px.X(),px.Y(),px.Z(),NULL));
	list->push_back(GripData(GripperTypeRotateObject,pyz.X(),pyz.Y(),pyz.Z(),NULL));
	list->push_back(GripData(GripperTypeRotateObject,pmxz.X(),pmxz.Y(),pmxz.Z(),NULL));
	list->push_back(GripData(GripperTypeObjectScaleZ,pz.X(),pz.Y(),pz.Z(),NULL));
	*/
}

void CBolt::OnApplyProperties()
{
	CBolt* new_object = new CBolt(m_pos, m_parameters, m_title.c_str(), m_color, m_opacity);
	new_object->CopyIDsFrom(this);
	HEEKSOBJ_OWNER->Add(new_object, NULL);
	HEEKSOBJ_OWNER->Remove(this);
	if(wxGetApp().m_marked_list->ObjectMarked(this))
	{
		wxGetApp().m_marked_list->Remove(this,false);
		wxGetApp().m_marked_list->Add(new_object, true);
	}
	wxGetApp().Repaint();
}

int CBolt::GetCentrePoints(double* pos, double* pos2)
{
	gp_Pnt o = m_pos.Location();
	gp_Dir z_dir = m_pos.XDirection() ^ m_pos.YDirection();
	gp_Pnt pz(o.XYZ() + z_dir.XYZ() * m_parameters.m_length);
	extract(o, pos);
	extract(pz, pos2);
	return 2;
}

bool CBolt::GetScaleAboutMatrix(double *m)
{
	gp_Trsf mat = make_matrix(m_pos.Location(), m_pos.XDirection(), m_pos.YDirection());
	extract(mat, m);
	return true;
}

bool CBolt::Stretch(const double *p, const double* shift, void* data)
{
	gp_Pnt vp = make_point(p);
	gp_Vec vshift = make_vector(shift);

	gp_Pnt o = m_pos.Location();
	gp_Pnt px(o.XYZ() + m_pos.XDirection().XYZ() * (m_parameters.m_thread_diameter / 2.0));
	gp_Dir z_dir = m_pos.XDirection() ^ m_pos.YDirection();
	gp_Pnt pz(o.XYZ() + z_dir.XYZ() * m_parameters.m_length);

	bool make_a_new_cylinder = false;

	if(px.IsEqual(vp, wxGetApp().m_geom_tol)){
		px = px.XYZ() + vshift.XYZ();
		double new_x = gp_Vec(px.XYZ()) * gp_Vec(m_pos.XDirection()) - gp_Vec(o.XYZ()) * gp_Vec(m_pos.XDirection());
		double new_y = gp_Vec(px.XYZ()) * gp_Vec(m_pos.YDirection()) - gp_Vec(o.XYZ()) * gp_Vec(m_pos.YDirection());
		make_a_new_cylinder = true;
		m_parameters.m_thread_diameter = sqrt(new_x * new_x + new_y * new_y) * 2.0;
	}
	else if(pz.IsEqual(vp, wxGetApp().m_geom_tol)){
		pz = pz.XYZ() + vshift.XYZ();
		double new_height = gp_Vec(pz.XYZ()) * gp_Vec(z_dir) - gp_Vec(o.XYZ()) * gp_Vec(z_dir);
		if(new_height > 0){
			make_a_new_cylinder = true;
			m_parameters.m_length = new_height;
		}
	}

	if(make_a_new_cylinder)
	{
		CBolt* new_object = new CBolt(m_pos, m_parameters, m_title.c_str(), m_color, m_opacity);
		new_object->CopyIDsFrom(this);
		HEEKSOBJ_OWNER->Add(new_object, NULL);
		HEEKSOBJ_OWNER->Remove(this);
		wxGetApp().m_marked_list->Clear(true);
		wxGetApp().m_marked_list->Add(new_object, true);
	}

	return true;
}

void CBolt::SetXMLElement(TiXmlElement* element)
{
	const gp_Pnt& l = m_pos.Location();
	element->SetDoubleAttribute("lx", l.X());
	element->SetDoubleAttribute("ly", l.Y());
	element->SetDoubleAttribute("lz", l.Z());

	const gp_Dir& d = m_pos.Direction();
	element->SetDoubleAttribute("dx", d.X());
	element->SetDoubleAttribute("dy", d.Y());
	element->SetDoubleAttribute("dz", d.Z());

	const gp_Dir& x = m_pos.XDirection();
	element->SetDoubleAttribute("xx", x.X());
	element->SetDoubleAttribute("xy", x.Y());
	element->SetDoubleAttribute("xz", x.Z());

	element->SetDoubleAttribute("r", (m_parameters.m_thread_diameter / 2.0));
	element->SetDoubleAttribute("h", m_parameters.m_length);

	CSolid::SetXMLElement(element);
}

void CBolt::SetFromXMLElement(TiXmlElement* pElem)
{
	// get the attributes
	double l[3] = {0, 0, 0};
	double d[3] = {0, 0, 1};
	double x[3] = {1, 0, 0};

	for(TiXmlAttribute* a = pElem->FirstAttribute(); a; a = a->Next())
	{
		std::string name(a->Name());
		if(name == "lx")	 {l[0] = a->DoubleValue();}
		else if(name == "ly"){l[1] = a->DoubleValue();}
		else if(name == "lz"){l[2] = a->DoubleValue();}

		else if(name == "dx"){d[0] = a->DoubleValue();}
		else if(name == "dy"){d[1] = a->DoubleValue();}
		else if(name == "dz"){d[2] = a->DoubleValue();}

		else if(name == "xx"){x[0] = a->DoubleValue();}
		else if(name == "xy"){x[1] = a->DoubleValue();}
		else if(name == "xz"){x[2] = a->DoubleValue();}

		else if(name == "r"){m_parameters.m_thread_diameter = a->DoubleValue() * 2.0;}
		else if(name == "h"){m_parameters.m_length = a->DoubleValue();}
	}

	m_pos = gp_Ax2(make_point(l), make_vector(d), make_vector(x));

	CSolid::SetFromXMLElement(pElem);
}


TopoDS_Shape CBolt::GetShape() const
{

    try {
        gp_Ax2 ax2(m_pos);
        BRepPrimAPI_MakeCylinder head( ax2, m_parameters.m_head_diameter / 2.0, m_parameters.m_head_height );

/*
        double radius =
        BRepFilletAPI_MakeChamfer chamfer(head.Shape());
        for (TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next())
        {
            TopoDS_Edge edge( explorer.Current() );
            chamfer.Add(radius, m_topods_edge, TopoDS::Face(face->Face()));
        }


        TopoDS_Shape new_shape = chamfer.Shape();
        wxGetApp().Add(new CSolid(*((TopoDS_Solid*)(&new_shape)), _("Solid with edge chamfer"), *(body->GetColor()), body->GetOpacity()), NULL);
    */



        gp_Pnt location(m_pos.Location());
        location.SetZ(location.Z() + m_parameters.m_head_height);
        ax2.SetLocation(location);
        BRepPrimAPI_MakeCylinder shaft( ax2, m_parameters.m_thread_diameter / 2.0, m_parameters.m_length );

        TopoDS_Shape shape = BRepAlgoAPI_Fuse(head.Shape() , shaft.Shape() );

        // Generate a hexagon solid to subtract from the head.
        int number_of_side_for_polygon = 6;
        double radius = m_parameters.m_flat_width;
        double sideAngle =2.0 * Pi / number_of_side_for_polygon;
        radius = radius/cos((sideAngle/2));
        BRepBuilderAPI_MakeWire wire_maker;
        for(int i = 0; i<number_of_side_for_polygon; i++)
        {
            gp_Pnt p0(-1.0 * (m_parameters.m_flat_width / 2.0), 0.0, 0.0);
            gp_Pnt p1(+1.0 * (m_parameters.m_flat_width / 2.0), 0.0, 0.0);
            gp_Pnt start, end;
            gp_Dir xdir(make_vector(p0, p1));
            gp_Dir zdir(make_vector(gp_Pnt(0.0,0.0,0.0), gp_Pnt(0.0,0.0,1.0)));
            gp_Dir ydir = zdir ^ xdir;
            double angle0 = (sideAngle * i)+(sideAngle/2);
            double angle1 = (sideAngle * (i+1))+(sideAngle/2);
            start = p0.XYZ() + xdir.XYZ() * ( cos(angle0) * radius ) + ydir.XYZ() * ( sin(angle0) * radius );
            if(i == number_of_side_for_polygon - 1) end = start;
            end = p0.XYZ() + xdir.XYZ() * ( cos(angle1) * radius ) + ydir .XYZ()* ( sin(angle1) * radius );

            Handle(Geom_TrimmedCurve) segment = GC_MakeSegment(start, end);
            TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(segment);
            wire_maker.Add(edge);
        }

        // TopoDS_Face face = BRepBuilderAPI_MakeFace(wire_maker.Wire());

        BRepPrimAPI_MakePrism generator( wire_maker.Wire(), gp_Dir(gp_XYZ(0.0,0.0,1.0)));
        generator.Build();
        return(generator.Shape());

        // TopoDS_Shape new_shape = BRepAlgoAPI_Fuse(shape , generator.Shape() );


        return(shape);
   } // End try
   // These are due to poor parameter settings resulting in negative lengths and the like.  I need
   // to work through the various parameters to either ensure they're correct or don't try
   // to construct a shape.
   catch (Standard_ConstructionError)
   {
	   // If we fail to create the solid then it's probably because the parameters of the geometry
	   // don't make sense.  eg: a negative length.  If we return a small sphere then we avoid
	   // the infinite loop that is created by the NULL shape pointer.

	    BRepPrimAPI_MakeSphere ball( gp_Pnt(0.0, 0.0, 0.0), 1.0 );
		return ball.Shape();
   } // End catch
   catch (Standard_DomainError)
   {
	   // If we fail to create the solid then it's probably because the parameters of the geometry
	   // don't make sense.  eg: a negative length.  If we return a small sphere then we avoid
	   // the infinite loop that is created by the NULL shape pointer.

	    BRepPrimAPI_MakeSphere ball( gp_Pnt(0.0, 0.0, 0.0), 1.0 );
		return ball.Shape();
   } // End catch
}


void CBolt::glCommands(bool select, bool marked, bool no_color)
{
    if(!m_pSolid)
    {
        try {
            TopoDS_Shape shape = GetShape();
            m_pSolid = heekscad_interface.NewSolid( *((TopoDS_Solid *) &shape), NULL, HeeksColor(234, 123, 89) );
        } // End try
        catch(Standard_DomainError) { }
        catch(...)  { }
    }

    if(m_pSolid)m_pSolid->glCommands( true, false, true );

} // End glCommands() method



void CBolt::KillGLLists(void)
{
	DeleteSolid();
}

void CBolt::DeleteSolid()
{
	if(m_pSolid)delete m_pSolid;
	m_pSolid = NULL;
}
