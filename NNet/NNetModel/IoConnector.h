// IoConnector.h
//
// NNetModel

#pragma once

#include <vector>
#include "BoolOp.h"
#include "MoreTypes.h"
#include "NobType.h"
#include "Nob.h"

using std::vector;
using std::unique_ptr;

class DrawContext;
class MicroMeterPosDir;
class IoNeuron;
class Neuron;

class IoConnector: public Nob
{
public:
	static bool const TypeFits(NobType const type) { return type.IsIoConnectorType(); }

	IoConnector(NobType const);

	virtual ~IoConnector() {}

	virtual void Check() const;
	virtual void Dump () const;

	virtual MicroMeterPnt const GetPos() const;

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
	virtual void       Select      (bool const);
	virtual void       SetParentPointers();
	virtual void       ClearParentPointers();

	virtual bool const IsCompositeNob() { return true; }

	void             Push(Neuron * const);
	Neuron * const   Pop();
	Neuron   const & GetElem(size_t const) const;
	size_t   const   Size() const;

	void AlignDirection();

	void Rotate(MicroMeterPnt const &, MicroMeterPnt const &);

	MicroMeterPosDir const GetPosDir() const;
	Radian           const GetDir()    const;

	void SetDir   (Radian           const );
	void SetPos   (MicroMeterPnt    const &);
	void SetPosDir(MicroMeterPosDir const &);

	void Apply2All(function<void(Neuron &)> const & func) const;

	MicroMeterLine const CalcMaxDistLine();
	MicroMeterPnt  const CalcOrthoVector(MicroMeterLine const &);

	friend wostream & operator<< (wostream &, IoConnector const &);

protected:
	vector<Neuron *> m_list {};
};

IoConnector const * Cast2IoConnector(Nob const *);
IoConnector       * Cast2IoConnector(Nob       *);
