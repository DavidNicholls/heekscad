// RuledSurface.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "RuledSurface.h"
#include "Wire.h"
#include "Face.h"
#include "ConversionTools.h"
#include "MarkedList.h"
#include "HeeksConfig.h"
#include "HeeksFrame.h"
#include "../interface/DoubleInput.h"
#include "../interface/PropertyCheck.h"
#include "../interface/HDialogs.h"
#include "../interface/NiceTextCtrl.h"

void PickCreateRuledSurface()
{
	if(wxGetApp().m_marked_list->size() == 0)
	{
		wxGetApp().PickObjects(_("pick some sketches"));
	}

	if(wxGetApp().m_marked_list->size() > 0)
	{
		std::list<TopoDS_Wire> wire_list;

		std::list<HeeksObj*> sketches_to_delete;

		for(std::list<HeeksObj *>::const_iterator It = wxGetApp().m_marked_list->list().begin(); It != wxGetApp().m_marked_list->list().end(); It++)
		{
			HeeksObj* object = *It;
			if(object->GetType() == SketchType)
			{
				std::list<HeeksObj*> list;
				list.push_back(object);
				TopoDS_Wire wire;
				if(ConvertLineArcsToWire2(list, wire))
				{
					wire_list.push_back(wire);
					if(wxGetApp().m_loft_removes_sketches)sketches_to_delete.push_back(object);
				}
			}
		}

		wxGetApp().Remove(sketches_to_delete);

		TopoDS_Shape shape;
		if(CreateRuledSurface(wire_list, shape, true))
		{
			HeeksObj* new_object = CShape::MakeObject(shape, _("Ruled Surface"), SOLID_TYPE_UNKNOWN, HeeksColor(51, 45, 51), 1.0f);
			wxGetApp().Add(new_object, NULL);
			wxGetApp().Repaint();
		}

	}
}


HeeksObj* CreateExtrusionOrRevolution(std::list<HeeksObj*> list, double height_or_angle, bool solid_if_possible, bool revolution_not_extrusion, double taper_angle_for_extrusion, bool add_new_objects, extrude_axis_t *axis)
{
	std::list<TopoDS_Shape> faces_or_wires;

	std::list<HeeksObj*> sketches_or_faces_to_delete;

	for(std::list<HeeksObj *>::const_iterator It = list.begin(); It != list.end(); It++)
	{
		HeeksObj* object = *It;
		switch(object->GetType())
		{
		case SketchType:
		case CircleType:
			{
				if(ConvertSketchToFaceOrWire(object, faces_or_wires, (fabs(taper_angle_for_extrusion) <= 0.0000001) && solid_if_possible))
				{
					if(wxGetApp().m_extrude_removes_sketches)sketches_or_faces_to_delete.push_back(object);
				}
			}
			break;

		case FaceType:
			faces_or_wires.push_back(((CFace*)object)->Face());
			if(wxGetApp().m_extrude_removes_sketches)sketches_or_faces_to_delete.push_back(object);
			break;

		default:
			break;
		}
	}

	wxGetApp().Remove(sketches_or_faces_to_delete);

	std::list<TopoDS_Shape> new_shapes;
	gp_Trsf trsf = wxGetApp().GetDrawMatrix(false);
	if(revolution_not_extrusion)
	{
		CreateRevolutions(faces_or_wires, new_shapes, gp_Ax1(gp_Pnt(0, 0, 0).Transformed(trsf), gp_Vec(1, 0, 0).Transformed(trsf)), height_or_angle);
	}
	else
	{
		gp_Vec vector = gp_Vec(0.0, 0.0, height_or_angle);
		if (axis)
		{
			switch (*axis)
			{
			case extrude_XAxis:
				vector = gp_Vec(height_or_angle, 0.0, 0.0);
				break;

			case extrude_YAxis:
				vector = gp_Vec(0.0, height_or_angle, 0.0);
				break;

			default:
			case extrude_ZAxis:
				vector = gp_Vec(0.0, 0.0, height_or_angle);
				break;
			}
		}

		CreateExtrusions(faces_or_wires, new_shapes, vector, taper_angle_for_extrusion, solid_if_possible);
	}
	HeeksObj* new_object = 0;
	if(new_shapes.size() > 0)
	{
		for(std::list<TopoDS_Shape>::iterator It = new_shapes.begin(); It != new_shapes.end(); It++){
			TopoDS_Shape& shape = *It;
			new_object = CShape::MakeObject(shape, revolution_not_extrusion ? _("Revolved Solid") : _("Extruded Solid"), SOLID_TYPE_UNKNOWN, wxGetApp().current_color, 1.0f);
			if(add_new_objects)
				wxGetApp().Add(new_object, NULL);
			else
				break;
		}
		wxGetApp().Repaint();
	}
	return new_object;
}

HeeksObj* CreatePipeFromProfile(HeeksObj* spine, HeeksObj* profile)
{
	const TopoDS_Wire wire = ((CWire*)spine)->Wire();
	std::list<TopoDS_Shape> faces;
	std::list<HeeksObj*> pipe_shapes;
	if(ConvertSketchToFaceOrWire(profile, faces, true))
	{
		for(std::list<TopoDS_Shape>::iterator It2 = faces.begin(); It2 != faces.end(); It2++)
		{
			TopoDS_Shape& face = *It2;

			try
			{
				// pipe profile algong spine
				BRepOffsetAPI_MakePipe makePipe(wire, face);
				makePipe.Build();
				TopoDS_Shape shape = makePipe.Shape();

				HeeksObj* new_object = CShape::MakeObject(shape, _("Pipe"), SOLID_TYPE_UNKNOWN, wxGetApp().current_color, 1.0f);
				if(new_object)pipe_shapes.push_back(new_object);
			}
			catch (Standard_Failure) {
				Handle_Standard_Failure e = Standard_Failure::Caught();
				wxMessageBox(wxString(_("Error making pipe")) + _T(": ") + Ctt(e->GetMessageString()));
			}
		}
		if(pipe_shapes.size() > 0)
		{
			wxGetApp().CreateUndoPoint();
			for(std::list<HeeksObj*>::iterator It = pipe_shapes.begin(); It != pipe_shapes.end(); It++)
			{
				HeeksObj* object = *It;
				wxGetApp().Add(object, NULL);
			}
			wxGetApp().Remove(profile);
			wxGetApp().Changed();
			return pipe_shapes.front();
		}
	}

	return NULL;
}

HeeksObj* CreateRuledFromSketches(std::list<HeeksObj*> list, bool make_solid)
{
	std::list<TopoDS_Wire> wire_list;
	for(std::list<HeeksObj *>::iterator It = list.begin(); It != list.end(); It++)
	{
		HeeksObj* object = *It;
		if(object->GetType() == SketchType)
		{
			std::list<HeeksObj*> s;
			s.push_back(object);
			TopoDS_Wire wire;
			if(ConvertLineArcsToWire2(s, wire))
			{
				wire_list.push_back(wire);
			}
		}
	}

	TopoDS_Shape shape;
	if(CreateRuledSurface(wire_list, shape, make_solid))
	{
		return CShape::MakeObject(shape, _("Ruled Surface"), SOLID_TYPE_UNKNOWN, HeeksColor(51, 45, 51), 1.0f);
	}
	return NULL;
}

bool InputExtrusionHeight(double &value, bool *extrude_makes_a_solid, double *taper_angle, extrude_axis_t *axis)
{
		HDialog dlg(wxGetApp().m_frame);
		wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
		wxStaticText *static_label = new wxStaticText(&dlg, wxID_ANY, _("Make extrusion"));
		sizerMain->Add( static_label, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, dlg.control_border );

		CLengthCtrl* value_control = new CLengthCtrl(&dlg);
		value_control->SetValue(value);
		dlg.AddLabelAndControl(sizerMain, _("height"), value_control);

		wxCheckBox* solid_check_box = NULL;
		if(extrude_makes_a_solid)
		{
			solid_check_box = new wxCheckBox(&dlg, wxID_ANY, _("Extrude makes a solid"));
			solid_check_box->SetValue(*extrude_makes_a_solid);
			sizerMain->Add( solid_check_box, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, dlg.control_border );
		}

		wxRadioButton *radioBtnX = new wxRadioButton( &dlg, wxID_ANY, wxT("along X axis"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
		sizerMain->Add( radioBtnX, 0, wxALL, dlg.control_border );

		wxRadioButton *radioBtnY = new wxRadioButton( &dlg, wxID_ANY, wxT("along Y axis"), wxDefaultPosition, wxDefaultSize, 0 );
		sizerMain->Add( radioBtnY, 0, wxALL, dlg.control_border );

		wxRadioButton *radioBtnZ = new wxRadioButton( &dlg, wxID_ANY, wxT("along Z axis"), wxDefaultPosition, wxDefaultSize, 0 );
		sizerMain->Add( radioBtnZ, 0, wxALL, dlg.control_border );

        if (axis)
        {
            switch (*axis)
            {
                case extrude_XAxis:
                    radioBtnX->SetValue(true);
                    radioBtnY->SetValue(false);
                    radioBtnZ->SetValue(false);
                    break;

                case extrude_YAxis:
                    radioBtnX->SetValue(false);
                    radioBtnY->SetValue(true);
                    radioBtnZ->SetValue(false);
                    break;

                case extrude_ZAxis:
                    radioBtnX->SetValue(false);
                    radioBtnY->SetValue(false);
                    radioBtnZ->SetValue(true);
                    break;
            }
        }

		CDoubleCtrl* taper_angle_control = NULL;
		if(taper_angle)
		{
			taper_angle_control = new CDoubleCtrl(&dlg);
			taper_angle_control->SetValue(*taper_angle);
			dlg.AddLabelAndControl(sizerMain, _("taper outward angle"), taper_angle_control);
		}

		sizerMain->Add( dlg.MakeOkAndCancel(wxHORIZONTAL), 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, dlg.control_border );
		dlg.SetSizer( sizerMain );
		sizerMain->SetSizeHints(&dlg);
		sizerMain->Fit(&dlg);
		value_control->SetFocus();
		if(dlg.ShowModal() == wxID_OK)
		{
			value = value_control->GetValue();
			if(extrude_makes_a_solid)*extrude_makes_a_solid = solid_check_box->GetValue();
			if(taper_angle)*taper_angle = taper_angle_control->GetValue();
			if(axis)
			{
			    *axis = extrude_ZAxis;  // default.
				if (radioBtnX->GetValue()) *axis = extrude_XAxis;
				if (radioBtnY->GetValue()) *axis = extrude_YAxis;
				if (radioBtnZ->GetValue()) *axis = extrude_ZAxis;
			}
			return true;
		}
		return false;
}

void PickCreateExtrusion()
{
	if(wxGetApp().m_marked_list->size() == 0)
	{
		wxGetApp().PickObjects(_("pick sketches, faces or circles"), MARKING_FILTER_CIRCLE | MARKING_FILTER_SKETCH | MARKING_FILTER_FACE);
	}

	double height;
	double taper_angle;
    extrude_axis_t axis = extrude_ZAxis;
	{
		HeeksConfig config;
		config.Read(_T("ExtrusionHeight"), &height, 10.0);
		config.Read(_T("ExtrusionTaperAngle"), &taper_angle, 0.0);
		config.Read(_T("ExtrusionAxis"), (int *) &axis, extrude_ZAxis);
	}

	if(InputExtrusionHeight(height, &(wxGetApp().m_extrude_to_solid), &taper_angle, &axis))
	{
		{
			HeeksConfig config;
			config.Write(_T("ExtrusionHeight"), height);
			config.Write(_T("ExtrusionTaperAngle"), taper_angle);
			config.Write(_T("ExtrudeToSolid"), wxGetApp().m_extrude_to_solid);
			config.Write(_T("ExtrusionAxis"), int(axis));
		}

		if(wxGetApp().m_marked_list->size() > 0)
		{
			CreateExtrusionOrRevolution(wxGetApp().m_marked_list->list(),height, wxGetApp().m_extrude_to_solid, false, taper_angle, true, &axis);
		}
	}
}

bool InputRevolutionAngle(double &angle, bool *extrude_makes_a_solid)
{
		HDialog dlg(wxGetApp().m_frame);
		wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
		wxStaticText *static_label = new wxStaticText(&dlg, wxID_ANY, _("Input revolution angle"));
		sizerMain->Add( static_label, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, dlg.control_border );

		CDoubleCtrl* value_control = new CDoubleCtrl(&dlg);
		value_control->SetValue(angle);
		dlg.AddLabelAndControl(sizerMain, _("angle"), value_control);

		wxCheckBox* solid_check_box = NULL;
		if(extrude_makes_a_solid)
		{
			solid_check_box = new wxCheckBox(&dlg, wxID_ANY, _("Makes a solid"));
			solid_check_box->SetValue(*extrude_makes_a_solid);
			sizerMain->Add( solid_check_box, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, dlg.control_border );
		}

		sizerMain->Add( dlg.MakeOkAndCancel(wxHORIZONTAL), 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, dlg.control_border );
		dlg.SetSizer( sizerMain );
		sizerMain->SetSizeHints(&dlg);
		sizerMain->Fit(&dlg);
		value_control->SetFocus();
		if(dlg.ShowModal() == wxID_OK)
		{
			angle = value_control->GetValue();
			if(extrude_makes_a_solid)*extrude_makes_a_solid = solid_check_box->GetValue();
			return true;
		}
		return false;
}

void PickCreateRevolution()
{
	if(wxGetApp().m_marked_list->size() == 0)
	{
		wxGetApp().PickObjects(_("pick sketches, faces or circles"), MARKING_FILTER_CIRCLE | MARKING_FILTER_SKETCH | MARKING_FILTER_FACE);
	}

	double angle = 360.0; // to do, this should get written to config file

	if(InputRevolutionAngle(angle, &wxGetApp().m_extrude_to_solid))
	{
		if(wxGetApp().m_marked_list->size() > 0)
		{
			CreateExtrusionOrRevolution(wxGetApp().m_marked_list->list(), angle, wxGetApp().m_extrude_to_solid, true, 0.0, true, NULL);
		}
	}
}

bool CreateRuledSurface(const std::list<TopoDS_Wire> &wire_list, TopoDS_Shape& shape, bool make_solid)
{
	if(wire_list.size() > 0)
	{
			BRepOffsetAPI_ThruSections generator( make_solid ? Standard_True : Standard_False, Standard_False );
			for(std::list<TopoDS_Wire>::const_iterator It = wire_list.begin(); It != wire_list.end(); It++)
			{
				const TopoDS_Wire &wire = *It;
				generator.AddWire(wire);
			}

		try{
			generator.Build();
			shape = generator.Shape();
		}
		catch (Standard_Failure) {
			Handle_Standard_Failure e = Standard_Failure::Caught();
			wxMessageBox(wxString(_("Error making ruled solid")) + _T(": ") + Ctt(e->GetMessageString()));
			return false;
		}
		catch(...)
		{
			wxMessageBox(_("Fatal error making ruled solid"));
			return false;
		}

		return true;
	}
	return false;
}

void CreateExtrusions(const std::list<TopoDS_Shape> &faces_or_wires, std::list<TopoDS_Shape>& new_shapes, const gp_Vec& extrude_vector, double taper_angle, bool solid_if_possible)
{
	try{
		for(std::list<TopoDS_Shape>::const_iterator It = faces_or_wires.begin(); It != faces_or_wires.end(); It++)
		{
			const TopoDS_Shape& face_or_wire = *It;
			if(fabs(taper_angle) > 0.0000001)
			{
				// make an offset face
				double distance = tan(taper_angle * Pi/180) * extrude_vector.Magnitude();
				bool wire = (face_or_wire.ShapeType() == TopAbs_WIRE);
				BRepOffsetAPI_MakeOffset offset;
				if(wire)
					offset = BRepOffsetAPI_MakeOffset(TopoDS::Wire(face_or_wire));
				else
					continue; // can't do CreateRuledSurface on faces yet
                offset.Perform(distance);

				// parallel
				std::list<TopoDS_Wire> wire_list;
				wire_list.push_back(TopoDS::Wire(face_or_wire));
				wire_list.push_back(TopoDS::Wire(offset.Shape()));

				gp_Trsf mat;
				mat.SetTranslation(extrude_vector);
				BRepBuilderAPI_Transform myBRepTransformation(wire_list.back(),mat);
				wire_list.back() = TopoDS::Wire(myBRepTransformation.Shape());

				TopoDS_Shape new_shape;
				if(CreateRuledSurface(wire_list, new_shape, solid_if_possible))
				{
					new_shapes.push_back(new_shape);
				}
            }
			else
			{
				BRepPrimAPI_MakePrism generator( face_or_wire, extrude_vector );
				generator.Build();
				new_shapes.push_back(generator.Shape());
			}
		}
	}
	catch (Standard_Failure) {
		Handle_Standard_Failure e = Standard_Failure::Caught();
		wxMessageBox(wxString(_("Error making extruded solid")) + _T(": ") + Ctt(e->GetMessageString()));
	}
	catch(...)
	{
		wxMessageBox(_("Fatal error making extruded solid"));
	}

}

void CreateRevolutions(const std::list<TopoDS_Shape> &faces_or_wires, std::list<TopoDS_Shape>& new_shapes, const gp_Ax1& axis, double angle)
{
	try{
		for(std::list<TopoDS_Shape>::const_iterator It = faces_or_wires.begin(); It != faces_or_wires.end(); It++)
		{
			const TopoDS_Shape& face_or_wire = *It;
			if(fabs(angle - 360.0) < 0.00001)
			{
				BRepPrimAPI_MakeRevol generator( face_or_wire, axis );
				generator.Build();
				new_shapes.push_back(generator.Shape());
			}
			else
			{
				BRepPrimAPI_MakeRevol generator( face_or_wire, axis, angle * Pi/180 );
				generator.Build();
				new_shapes.push_back(generator.Shape());
			}
		}
	}
	catch (Standard_Failure) {
		Handle_Standard_Failure e = Standard_Failure::Caught();
		wxMessageBox(wxString(_("Error making revolved solid")) + _T(": ") + Ctt(e->GetMessageString()));
	}
	catch(...)
	{
		wxMessageBox(_("Fatal error making revolved solid"));
	}

}


