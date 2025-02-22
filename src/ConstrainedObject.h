// ConstrainedObject.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once


#include "../interface/HeeksObj.h"
#ifdef MULTIPLE_OWNERS
class Constraint;
#include "Constraint.h"
class HPoint;

class ConstrainedObject: public ObjList{
public:
	std::list<Constraint*> constraints;

	ConstrainedObject();
	~ConstrainedObject(void);

	const ConstrainedObject& operator=(const ConstrainedObject &b);

	virtual void LoadToDoubles();
	virtual void LoadFromDoubles();


	void SetAbsoluteAngleConstraint(EnumAbsoluteAngle angle);
	bool SetPerpendicularConstraint(ConstrainedObject* obj);
	bool SetParallelConstraint(ConstrainedObject* obj);
	bool SetEqualLengthConstraint(ConstrainedObject* obj);
	bool SetColinearConstraint(ConstrainedObject* obj);
	bool SetConcentricConstraint(ConstrainedObject* obj);
	bool SetEqualRadiusConstraint(ConstrainedObject* obj);
	void glCommands(HeeksColor color, gp_Ax1 mid_point);
	bool RemoveExisting(HeeksObj* obj, EnumConstraintType type);
	Constraint* GetExisting(EnumConstraintType type);
	bool HasConstraints();
	void ReloadPointers();
	void SetCoincidentPoint(ConstrainedObject* obj, bool remove);
	bool HasPointConstraint(ConstrainedObject* obj);
	void SetLineLengthConstraint(double length);
	void SetLineVerticalLengthConstraint(double length);
	void SetLineHorizontalLengthConstraint(double length);
	void SetRadiusConstraint(double radius);
	void SetLineLength(double length);
	void SetLineVerticalLength(double length);
	void SetLineHorizontalLength(double length);
	void SetRadius(double radius);
	void SetTangentConstraint(ConstrainedObject* obj);
	void SetPointOnLineConstraint(HPoint* obj);
	void SetPointOnLineMidpointConstraint(HPoint* obj);
	void SetPointOnArcConstraint(HPoint* obj);
	void SetPointOnArcMidpointConstraint(HPoint* obj);
	void SetPointOnCircleConstraint(HPoint* obj);
	void SetPointFixedConstraint();
};
#endif
