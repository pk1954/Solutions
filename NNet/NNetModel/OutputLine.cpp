// OutputLine.cpp 
//
// NNetModel

#include "stdafx.h"
#include "Resource.h"
#include "DrawContext.h"
#include "OutputLine.h"

OutputLine::OutputLine(MicroMeterPnt const & upCenter)
	: IoLine(upCenter, NobType::Value::outputLine)
{}

OutputLine::OutputLine(BaseKnot const & baseKnot)
	: IoLine(baseKnot, NobType::Value::outputLine)
{
	SetIncoming(baseKnot);
}

void OutputLine::Check() const
{
	BaseKnot::Check();
	assert(!HasOutgoing());
}

bool OutputLine::operator==(Nob const & rhs) const
{
	return this->BaseKnot::operator== (static_cast<OutputLine const &>(rhs));
}

void OutputLine::DrawExterior(DrawContext const & context, tHighlight const type) const
{
	drawPlug(context, 0.8f, 0.8f, GetExteriorColor(type));
}

void OutputLine::DrawInterior(DrawContext const & context, tHighlight const type) const
{
	drawPlug(context, 0.4f, 0.6f, GetInteriorColor(type));
}

bool OutputLine::Includes(MicroMeterPnt const & point) const
{
	MicroMeterPnt const umCenter { GetPos() + GetScaledDirVector() * 0.5f };
	return Distance(point, umCenter) <= GetExtension();
}

void OutputLine::drawPlug
(
	DrawContext  const & context, 
	float        const   M,       // overall width/height                        
	float        const   V,
	D2D1::ColorF const   colF
) const
{
	MicroMeterPnt const umDirVector { GetDirVector() };
	MicroMeterPnt const umCenter    { GetPos() - GetScaledDirVector() * 0.1f };
	MicroMeterPnt const umP         { umCenter - umDirVector * (M * 0.5f - 0.2f) };

	context.DrawLine(umCenter + umDirVector * (V + 0.8f), umP, GetExtension() *  M,          colF);
	context.DrawLine(umCenter + umDirVector *  V,         umP, GetExtension() * (M + 1.2f), colF);
}

void OutputLine::AppendMenuItems(AddMenuFunc const & add) const
{
	add(IDD_ADD_OUTGOING2BASEKNOT);
	IoLine::AppendMenuItems(add);
}