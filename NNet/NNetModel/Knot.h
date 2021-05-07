// Knot.h
//
// NNetModel

#pragma once

#include "Geometry.h"
#include "MoreTypes.h"
#include "tHighlightType.h"
#include "BaseKnot.h"
#include "Pipe.h"

class DrawContext;
class PixelCoordsFp;
class NNetModel;

using std::unique_ptr;
using std::make_unique;

class Knot : public BaseKnot
{
public:
	Knot( MicroMeterPoint const center )
		: BaseKnot( center, NobType::Value::knot, PIPE_WIDTH / 2 )
	{}

	virtual ~Knot() {}

	void Check() const;

	static bool      const TypeFits(NobType const type) { return type.IsKnotType(); }
	static NobType const GetNobType() { return NobType::Value::knot; }

	mV const GetNextOutput() const { return m_mVinputBuffer; }

	virtual bool const CompStep() { return false; }

	virtual void DrawExterior(DrawContext const &, tHighlight const) const;
	virtual void DrawInterior(DrawContext const &, tHighlight const) const;
	virtual void Select      (bool const, bool const);
	virtual void Recalc      () { };

	virtual void DrawNeuronText(PixelCoordsFp const &) const {};

private: 
};
