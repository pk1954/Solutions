// Shape.h
//
// NNetModel

#pragma once

#include <type_traits>   
#include "d2d1helper.h"
#include "BoolOp.h"
#include "MoreTypes.h"
#include "tHighlightType.h"
#include "NNetParameters.h"
#include "ShapeType.h"
#include "ShapeId.h"

class Shape;
class DrawContext;

using std::is_base_of;
using std::remove_pointer_t;
using std::remove_pointer;
using std::unique_ptr;
using std::wostream;
using std::wstring;

template <typename T> 
concept Shape_t = is_base_of<Shape, remove_pointer_t<T>>::value;

using UPShape    = unique_ptr<Shape>;
using ShapeFuncC = function<void(Shape const &)>;
using ShapeFunc  = function<void(Shape       &)>;
using ShapeCrit  = function<bool(Shape const &)>;

static ShapeCrit const ShapeCritAlwaysTrue { [](auto & s) { return true; } };

class Shape
{
public:
	static void Initialize( Param const & param ) { m_pParameters = & param; }

	static bool const TypeFits( ShapeType const type ) { return true; }  // every shape type is a Shape

	Shape( ShapeType const );
	virtual ~Shape() { }

	virtual void CheckShape() const;
	virtual void Dump() const;

	virtual bool operator==( Shape const & ) const;

	virtual void IncCounter( ) = 0;
	virtual void DecCounter( ) = 0;

	virtual void       DrawExterior  ( DrawContext const &, tHighlightType const ) const = 0;
	virtual void       DrawInterior  ( DrawContext const &, tHighlightType const ) const = 0;
	virtual void       Prepare       ( )                                                 = 0;
	virtual bool const CompStep      ( )                                                 = 0;
	virtual void       Recalc        ( )                                                 = 0;
	virtual void       MoveShape     ( MicroMeterPoint const & )                         = 0;
	virtual bool const IsInRect      ( MicroMeterRect  const & )                   const = 0;
	virtual bool       IsPointInShape( MicroMeterPoint const & )                   const = 0;

	virtual void Clear () { m_mVinputBuffer = 0.0_mV; };

	void Select(tBoolOp const op = tBoolOp::opTrue) { ApplyOp(m_bSelected, op); }

	bool      const IsSelected  () const { return m_bSelected; }
	bool      const IsDefined   () const { return ::IsDefined( m_identifier ); }
	wstring   const GetName     () const { return ShapeType::GetName( m_type.GetValue() ); }
	ShapeType const GetShapeType() const { return m_type; }
	ShapeId   const GetId       () const { return m_identifier; }

	bool const HasType(ShapeType const type) const { return m_type == type; }

	bool const IsPipe       () const { return m_type.IsPipeType        ( ); }
	bool const IsKnot       () const { return m_type.IsKnotType        ( ); }
	bool const IsNeuron     () const { return m_type.IsNeuronType      ( ); }
	bool const IsInputNeuron() const { return m_type.IsInputNeuronType ( ); }
	bool const IsAnyNeuron  () const { return m_type.IsAnyNeuronType   ( ); }
	bool const IsBaseKnot   () const { return m_type.IsBaseKnotType    ( ); }
	bool const IsUndefined  () const { return m_type.IsUndefinedType   ( ); }

	virtual void SetId( ShapeId const id ) { m_identifier = id;	}

	friend wostream & operator<< ( wostream &, Shape const & );

protected:

	mV m_mVinputBuffer { 0._mV };

	inline static Param const * m_pParameters{ nullptr };

	D2D1::ColorF GetExteriorColor( tHighlightType const ) const;
	D2D1::ColorF GetInteriorColor( tHighlightType const ) const;
	D2D1::ColorF GetInteriorColor( mV const ) const;
	D2D1::ColorF GetInteriorColor( ) const { return GetInteriorColor( m_mVinputBuffer ); }

	float GetFillLevel( mV const ) const;
	float GetFillLevel( ) const { return GetFillLevel( m_mVinputBuffer ); };

	void SetType(ShapeType const type) { m_type = type; }

private:

	ShapeType m_type       { ShapeType::Value::undefined };
	bool      m_bSelected  { false };
	ShapeId   m_identifier { };
};

template <Shape_t T> bool HasType( Shape const & shape ) 
{ 
	return remove_pointer<T>::type::TypeFits( shape.GetShapeType() ); 
}
