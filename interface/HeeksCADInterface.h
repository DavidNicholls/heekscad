// HeeksCADInterface.h

// include this in your dynamic library to interface with HeeksCAD

#pragma once

class HeeksObj;
class wxFrame;
class wxAuiManager;

class CHeeksCADInterface{
public:
	CHeeksCADInterface(){}
	~CHeeksCADInterface(){}

	virtual double GetTolerance();
	virtual void RefreshProperties();
	virtual void Repaint();
	virtual wxFrame* GetMainFrame();
	virtual wxAuiManager* GetAuiManager();
	virtual void AddToolBarButton(wxToolBar* toolbar, const wxString& title, wxBitmap& bitmap, const wxString& caption, void(*onButtonFunction)(wxCommandEvent&));
	virtual wxString GetExeFolder();
	virtual void AddUndoably(HeeksObj* object);
	virtual const std::list<HeeksObj*>& GetSelection();
};
