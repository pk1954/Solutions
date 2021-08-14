// UPNobList.cpp 
//
// NNetModel

#include "stdafx.h"
#include "Knot.h"
#include "Pipe.h"
#include "BaseKnot.h"
#include "Neuron.h"
#include "InputConnector.h"
#include "OutputConnector.h"
#include "ClosedConnector.h"
#include "InputNeuron.h"
#include "OutputNeuron.h"
#include "NobIdList.h"
#include "UPNobList.h"

using std::move;

void UPNobList::Clear()
{ 
	m_list.clear(); 
	m_nobsOfType.fill(0);
}

void UPNobList::SetErrorHandler(NobErrorHandler * const p) 
{ 
	m_pNobErrorHandler = p; 
}

UPNob ShallowCopy(Nob const & nob)  //TODO: simplify! Better 
{
	switch (nob.GetNobType().GetValue())
	{
	case NobType::Value::inputConnector:
		return Copy<InputConnector>(nob);

	case NobType::Value::outputConnector:
		return Copy<OutputConnector>(nob);

	case NobType::Value::closedConnector:
		return Copy<ClosedConnector>(nob);

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
		assert(false);
		return nullptr;
	}
}

bool UPNobList::operator==(UPNobList const & other) const
{
	if (m_list.size() != other.m_list.size())
		return false;
	for (int i = 0; i < m_list.size(); ++i)
	{
		Nob const * pNob      {       m_list[i].get() };
		Nob const * pNobOther { other.m_list[i].get() };
		if ((pNob == nullptr) != (pNobOther == nullptr))
		{
			return false;
		}
		if ((pNob != nullptr) && (pNobOther != nullptr))
		{
			if (*pNob != *pNobOther)
				return false;
		}
	}
	return true;
}

UPNob UPNobList::ExtractNob(NobId const id)	
{
	assert(IsDefined(id));
	assert(IsValidNobId(id));

	UPNob upNob { move(m_list[id.GetValue()]) };
	if (upNob)
		decCounter(id);
	return move(upNob);
}

void UPNobList::SetNob2Slot(UPNob upNob)
{                 
	NobId const id { upNob->GetId() };
	SetNob2Slot(id, move(upNob));
}

void UPNobList::SetNob2Slot(NobId const id, UPNob upNob) 
{
	assert(IsDefined(id));
	assert(IsValidNobId(id));
	assert(IsEmptySlot(id));
	assert(upNob);

	incCounter(upNob);
	m_list[id.GetValue()] = move(upNob);
}

Nob * const UPNobList::ReplaceNob(UPNob upT)
{
	NobId const id { upT->GetId() };

	assert(IsDefined(id));
	assert(IsValidNobId(id));

	incCounter(upT);
	decCounter(id);

	UPNob tmp = move(upT);
	m_list[id.GetValue()].swap(tmp);
	return tmp.release();
}

NobId const UPNobList::Push(UPNob upNob)	
{
	NobId idNewSlot { IdNewSlot() };
	if (upNob)
	{
		upNob->SetId(idNewSlot);
		incCounter(upNob);
	}
	m_list.push_back(move(upNob));
	return idNewSlot;
}

void UPNobList::MoveFrom(UPNobList & src, size_t nrOfItems)
{
	for (size_t i = 0; i < nrOfItems; ++i)
		Push(src.Pop());
}

void UPNobList::copy(const UPNobList & rhs)
{
	rhs.CheckNobList();

	m_list.resize(rhs.m_list.size());

	for (auto const & pNobSrc : rhs.m_list)
	{
		if (pNobSrc)
			SetNob2Slot(pNobSrc->GetId(), ShallowCopy(*pNobSrc));
	}

	for (auto const & pNobDst : m_list)
	{
		if (pNobDst)
			pNobDst->Link
			(
				*rhs.GetAt(pNobDst->GetId()),                        // the src nob used as template for links
				[&](Nob const *pSrc){ return GetAt(pSrc->GetId()); } // how to get dst nob from src nob
		    );
	}

	m_pNobErrorHandler = rhs.m_pNobErrorHandler;
	CheckNobList();
}

UPNobList::~UPNobList()
{
	Clear();
}

UPNobList::UPNobList(const UPNobList & rhs) // copy constructor
{
	copy(rhs);
}

UPNobList & UPNobList::operator= (const UPNobList & rhs) // copy assignment
{
	if (this != &rhs)
	{
		Clear();
		copy(rhs);
	}
	return * this;
}

void UPNobList::CheckNobList() const
{
#ifdef _DEBUG
	for (size_t i = 0; i < m_list.size(); ++i)
	{
		if (m_list[i])
		{
			assert(m_list[i]->GetId().GetValue() == i);
			m_list[i]->Check();
		}
	}
#endif
}

void UPNobList::Dump() const
{
	for (auto const & it : m_list)
		if (it)
			it->Dump();
}

MicroMeterRect const UPNobList::CalcEnclosingRect(SelMode const mode) const
{
	MicroMeterRect rect { MicroMeterRect::ZERO_VAL() };
	for (auto const & it : m_list)
		if (it && ((mode == SelMode::allNobs) || it->IsSelected()))
			it->Expand(rect);
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
		if (pNob && pNob->Includes(pnt))
		{
			while (pNob->HasParentNob())
				pNob = pNob->GetParentNob();
			if (crit(* pNob))
				return pNob->GetId();
		}
	};
	return NobId(NO_NOB);
}

unique_ptr<vector<Nob *>> UPNobList::GetAllSelected()
{
	unique_ptr<vector<Nob *>> upNobs = make_unique<vector<Nob *>>();
	for (auto & it : m_list)
	{
		if (it && it->IsSelected()) 
			upNobs->push_back(it.get());
	}
	return move(upNobs);
}

bool const UPNobList::AnyNobsSelected() const
{
	for (auto const & it : m_list)
		if (it && it->IsSelected())
			return true;
	return false;
}

void UPNobList::CallErrorHandler(NobId const id) const
{
	if (m_pNobErrorHandler)
	{
		(* m_pNobErrorHandler)(id);
	}
}

void UPNobList::Apply2All(NobFuncC const & func) const
{
	for (auto const & it : m_list)
		if (it)
			func(* it.get()); 
}                        

void UPNobList::Apply2All(NobFunc const & func)
{
	for (auto & it : m_list)
		if (it)
			func(* it.get()); 
}                        

bool const UPNobList::Apply2AllB(NobCrit const & func) const
{
	for (auto & it : m_list)
		if (it && func(* it.get()))
			return true;
	return false;
}

void UPNobList::Apply2AllSelected(NobType const type, NobFunc const & func)
{
	for (auto & it : m_list)
		if (it && it->IsSelected() && (it->GetNobType() == type))
			func(* it.get()); 
}

void UPNobList::Apply2AllSelected(NobType const type, NobFuncC const & func) const
{
	for (auto & it : m_list)
		if (it && it->IsSelected() && (it->GetNobType() == type))
			func(* it.get()); 
}

void UPNobList::SelectAllNobs(bool const bOn) 
{ 
	for (auto & it : m_list)
		if (it)
			it->Select(bOn);
}

void UPNobList::Move(MicroMeterPnt const& delta)
{
	for (auto & it : m_list)
		if (it)
			it->MoveNob(delta);
}

unsigned int const UPNobList::CountInSelection(NobType const nobType) const
{
	unsigned int uiNr { 0 };
	Apply2AllSelected(nobType, [&](auto & s) { ++uiNr; });
	return uiNr;
}

unsigned int const UPNobList::GetCounter(NobType const t) const 
{ 
	return counter(t); 
}

unsigned int const UPNobList::GetCounter() const 
{ 
	return accumulate(m_nobsOfType.begin(), m_nobsOfType.end(), 0); 
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
	m_nobsOfType.fill(0);
	for (auto & it : m_list)
		if (it)
		{ 
			NobType    const type  { it->GetNobType() };
			unsigned int const index { static_cast<unsigned int>(type.GetValue()) };
			++m_nobsOfType[index];
		};
}

bool const UPNobList::Contains(Nob const * pNob) const 
{ 
	return Apply2AllB([&](Nob const & nob) { return pNob == &nob; }); 
}

void UPNobList::Reconnect(NobId const id)
{
	if (Nob * pNod { GetAt(id) })
		pNod->Reconnect();
}
