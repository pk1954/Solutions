// ClosedConnector.h
//
// NNetModel

#pragma once

#include <vector>
#include "BoolOp.h"
#include "MoreTypes.h"
#include "Nob.h"
#include "NobType.h"

using std::vector;

class Neuron;
class Connector;
class IoNeuronList;

class ClosedConnector: public Nob
{
public:

	static bool    const TypeFits(NobType const type) { return type.IsClosedConnectorType(); }
	static NobType const GetNobType()                 { return NobType::Value::closedConnector; }

	ClosedConnector() :	Nob(NobType::Value::closedConnector) {};
	virtual ~ClosedConnector() {}

	virtual void Check() const;
	virtual void Dump () const;

	virtual MicroMeterPnt const GetPos() const;
	virtual Radian        const GetDir() const;

	virtual void       DrawExterior(DrawContext    const &, tHighlight const) const;
	virtual void       DrawInterior(DrawContext    const &, tHighlight const) const;
	virtual void       Expand      (MicroMeterRect       &)                   const;
	virtual bool const IsIncludedIn(MicroMeterRect const &)                   const;
	virtual bool const Includes    (MicroMeterPnt  const &)                   const;
	virtual void       RotateNob   (MicroMeterPnt  const &, Radian const);
	virtual void       MoveNob     (MicroMeterPnt  const &);
	virtual void       Prepare     ();
	virtual bool const CompStep    ();
	virtual void       Recalc      ();
	virtual void       Clear       ();
	virtual void       Link        (Nob const &, Nob2NobFunc const &);

	virtual NobIoMode const GetIoMode() const { return NobIoMode::internal; }

	virtual bool const IsCompositeNob() { return true; }

	void Push(Neuron * const p) { m_list.push_back(p); }
	Neuron * const Pop();

	void SetParentPointers();
	void ClearParentPointers();

	size_t const Size() const { return m_list.size(); };

	inline static wchar_t const SEPARATOR     { L':' };
	inline static wchar_t const OPEN_BRACKET  { L'{' };
	inline static wchar_t const CLOSE_BRACKET { L'}' };

	vector<Neuron *> const & GetNeurons() const { return m_list; }

private:
	vector<Neuron *> m_list{};
};

ClosedConnector const * Cast2ClosedConnector(Nob const *);
ClosedConnector       * Cast2ClosedConnector(Nob       *);
