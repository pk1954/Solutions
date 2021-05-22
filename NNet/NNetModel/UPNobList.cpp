// UPNobList.cpp 
//
// NNetModel

#include "stdafx.h"
#include "Knot.h"
#include "Pipe.h"
#include "BaseKnot.h"
#include "Neuron.h"
#include "Connector.h"
#include "ClosedConnector.h"
#include "InputNeuron.h"
#include "OutputNeuron.h"
#include "NobIdList.h"
#include "UPNobList.h"

using std::move;

void UPNobList::Clear()
{ 
	m_list.clear(); 
	m_nobsOfType.fill( 0 );
}

void UPNobList::SetErrorHandler( NobErrorHandler * const p ) 
{ 
	m_pNobErrorHandler = p; 
}

UPNob ShallowCopy( Nob const & nob )
{
	switch ( nob.GetNobType().GetValue() )
	{
	case NobType::Value::connector:
		return Copy<Connector>(nob);

	case NobType::Value::inputNeuron:
		return Copy<InputNeuron>(nob);

	case NobType::Value::outputNeuron:
		return Copy<OutputNeuron>(nob);

	case NobType::Value::knot:
		return Copy<Knot>(nob);

	case NobType::Value::neuron:
		return Copy<Neuron>(nob);

	case NobType::Value::pipe:
		return Copy<Pipe>(nob);

	default:
		assert( false );
		return nullptr;
	}
}

bool UPNobList::operator==( UPNobList const & other ) const
{
	if ( m_list.size() != other.m_list.size() )
		return false;
	for ( int i = 0; i < m_list.size(); ++i )
	{
		Nob const * pNob      {       m_list[i].get() };
		Nob const * pNobOther { other.m_list[i].get() };
		if ( (pNob == nullptr) != (pNobOther == nullptr) )
		{
			return false;
		}
		else if ( (pNob != nullptr) && (pNobOther != nullptr) )
		{
			if ( *pNob != *pNobOther )
				return false;
		}
	}
	return true;
}

UPNob UPNobList::ExtractNob( NobId const id )	
{
	assert( IsDefined(id) );
	assert( IsValidNobId(id) );

	decCounter(id);

	return move( m_list[id.GetValue()] );
}


void UPNobList::SetNob2Slot( NobId const id, UPNob upNob ) // only for special situations
{                                                                    // read model from script
	assert( IsDefined(id) );
	assert( IsValidNobId(id) );
	assert( IsEmptySlot(id) );
	assert( upNob );

	incCounter(upNob);
	m_list[id.GetValue()] = move(upNob);
}

void UPNobList::SetNob2Slot( UPNob upNob )	 // only for special situations
{                 
	NobId const id { upNob->GetId() };
	SetNob2Slot(id, move(upNob));
}

Nob * const UPNobList::ReplaceNob( NobId const id, UPNob upT )	
{
	assert( IsDefined(id) );
	assert( IsValidNobId( id ) );

	incCounter(upT);
	decCounter(id);

	UPNob tmp = move(upT);
	m_list[id.GetValue()].swap( tmp );
	return tmp.release();
}

void UPNobList::LinkNob
(
	Nob         const & nobSrc, 
	Nob2NobFunc const & dstFromSrc
) const
{
	if ( Nob * pNobDst { dstFromSrc(& nobSrc) } )
		pNobDst->Link(nobSrc, dstFromSrc);
}

NobId const UPNobList::Push( UPNob upNob )	
{
	NobId idNewSlot { IdNewSlot() };
	if ( upNob )
	{
		upNob->SetId( IdNewSlot() );
		incCounter(upNob);
	}
	m_list.push_back( move(upNob) );
	return idNewSlot;
}

void UPNobList::copy( const UPNobList & rhs )
{
	rhs.CheckNobList();

	m_list.resize( Cast2Long(rhs.m_list.size()) );

	for ( auto const & pNobSrc : rhs.m_list )
	{
		if ( pNobSrc )
			SetNob2Slot( pNobSrc->GetId(), ShallowCopy( * pNobSrc ) );
	}

	for ( auto const & pNobSrc : rhs.m_list )
	{
		if ( pNobSrc )
			LinkNob(*pNobSrc, [&](Nob const *pSrc){ return GetAt(pSrc->GetId()); });
	}

	m_pNobErrorHandler = rhs.m_pNobErrorHandler;
}

UPNobList::~UPNobList()
{
	Clear();
}

UPNobList::UPNobList( const UPNobList & rhs ) // copy constructor
{
	copy( rhs );
}

UPNobList & UPNobList::operator= ( const UPNobList & rhs ) // copy assignment
{
	if (this != &rhs)
	{
		Clear();
		copy( rhs );
	}
	return * this;
}

void UPNobList::CheckNobList() const
{
#ifdef _DEBUG
	Apply2All( [&](Nob const & nob) { nob.Check(); } );
#endif
}

void UPNobList::Dump() const
{
	Apply2All( [&](Nob const & nob) { nob.Dump(); } );
}

MicroMeterRect const UPNobList::CalcEnclosingRect( SelMode const mode ) const
{
	MicroMeterRect rect { MicroMeterRect::ZERO_VAL() };
	for ( auto const & pNob : m_list )
		if ( pNob && ((mode == SelMode::allNobs) || pNob->IsSelected()) )
			pNob->Expand( rect );
	return rect;
}

NobId const UPNobList::FindNobAt
( 
	MicroMeterPnt const   pnt, 
	NobCrit       const & crit 
) const
{
	for (size_t i = m_list.size(); i --> 0;)	
	{
		Nob * pNob = m_list[i].get();
		if ( pNob && crit(* pNob) && pNob->Includes(pnt) ) 
			return pNob->GetId();
	};
	return NobId( NO_NOB );
}

bool const UPNobList::AnyNobsSelected() const
{
	return Apply2AllB<Nob>([&](Nob const & s) { return s.IsSelected(); });
}

void UPNobList::CallErrorHandler(NobId const id) const
{
	if ( m_pNobErrorHandler )
	{
		(* m_pNobErrorHandler)( id );
	}
}

void UPNobList::Apply2All( NobFuncC const & func ) const
{
	for ( auto const & it : m_list )
	{ 
		if ( it.get() )
			func( * it.get() ); 
	}
}                        

void UPNobList::Apply2All( NobFunc const & func )
{
	for ( auto & it : m_list )
	{ 
		if ( it.get() )
			func( * it.get() ); 
	}
}                        

bool const UPNobList::Apply2AllB( NobCrit const & func ) const
{
	for ( auto & it : m_list )
	{ 
		if ( it.get() && func( * it.get() ) )
			return true;
	}
	return false;
}

void UPNobList::Apply2AllSelected( NobType const type, NobFunc const & func )
{
	Apply2All( { [&](Nob & s) { if (s.IsSelected() && (s.GetNobType() == type)) func(s); } } );
}

void UPNobList::Apply2AllSelected( NobType const type, NobFuncC const & func ) const
{
	Apply2All( { [&](Nob const & s) { if (s.IsSelected() && (s.GetNobType() == type)) func(s); } } );
}

void UPNobList::SelectAllNobs(bool const bOn) 
{ 
	Apply2All( [&](Nob & s) { s.Select(bOn); } ); 
}

NobIdList UPNobList::Append( UPNobList & list2Append )
{
	NobIdList idList;
	long offset { Size() };
	for ( auto & upNob : list2Append.m_list )
	{
		if ( upNob )
		{
			NobId id { upNob->GetId() + offset };
			upNob->SetId( id );
			idList.Add( id );
		}
		m_list.push_back( move(upNob) );
	}
	return idList;
}

UPNobList UPNobList::ExtractNobs(NobIdList idList)
{
	UPNobList nobList;
	idList.Apply2All([&](NobId const &id){ nobList.Push(ExtractNob(id)); } );
	return nobList;
}

unsigned int const UPNobList::CountInSelection(NobType const nobType) const
{
	unsigned int uiNr { 0 };
	Apply2AllSelected( nobType, [&](auto & s) { ++uiNr; } );
	return uiNr;
}

unsigned int const UPNobList::GetCounter(NobType const t) const 
{ 
	return counter(t); 
}

unsigned int const UPNobList::GetCounter() const 
{ 
	return accumulate( m_nobsOfType.begin(), m_nobsOfType.end(), 0 ); 
}

unsigned int const & UPNobList::counter(NobType const t) const 
{ 
	return m_nobsOfType[static_cast<unsigned int>(t.GetValue())]; 
}

unsigned int & UPNobList::counter(NobType const t)       
{ 
	return const_cast<unsigned int &>(static_cast<const UPNobList&>(*this).counter(t)); 
}

void UPNobList::countNobs()
{
	m_nobsOfType.fill( 0 );
	for ( auto & it : m_list )
	{ 
		NobType    const type  { it->GetNobType() };
		unsigned int const index { static_cast<unsigned int>(type.GetValue()) };
		++m_nobsOfType[index];
	};
}