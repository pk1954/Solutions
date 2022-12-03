// Pipe.ixx
//
// NNetModel

module;

#include <cassert>
#include <utility>
#include <functional>
#include <iostream>
#include <vector>

export module NNetModel:Pipe;

import DrawContext;
import Types;
import SaveCast;
import :tHighlight;
import :NNetParameters;
import :NobType;
import :NobId;
import :Nob;

using std::pair;
using std::vector;
using std::function;
using std::wostream;
using std::unique_ptr;

//class PosNob;    // avoid circular reference
//class Synapse;   // avoid circular reference

class Pipe;

export using PipeFunc = function<void(Pipe&)>;
export using PipeCrit = function<bool(Pipe const&)>;
export using PipePair = pair<unique_ptr<Pipe>, unique_ptr<Pipe>>;

export class Pipe : public Nob
{
public:
	using SegNr = NamedType<size_t, struct segNr_Parameter>;

	Pipe();
	Pipe(Nob * const, Nob * const);   //TODO: Nob --> PosNob
	Pipe(Pipe const &);   // copy constructor

	~Pipe() final = default;

	bool operator==(Nob const&) const;

	void AppendMenuItems(AddMenuFunc const&) const final;

	void Dump() const override;

	static bool TypeFits(NobType const type) { return type.IsPipeType(); }

	void SetPos(MicroMeterPnt const& pos) final { assert(false); }

	void SetStartPnt(Nob * const);   //TODO: Nob --> PosNob
	void SetEndPnt  (Nob * const);   //TODO: Nob --> PosNob

	void AddSynapse        (Nob *);              //TODO: Nob --> Synapse
	void RemoveSynapse     (Nob *);              //TODO: Nob --> Synapse
	bool IsConnectedSynapse(Nob const &) const;  //TODO: Nob --> Synapse
	bool IsConnectedTo     (NobId const) const;

	void Emphasize(bool const) final;
	void Emphasize(bool const, bool const);

	Nob * GetStartNobPtr() { return m_pNobStart; }   //TODO: Nob --> PosNob
	Nob * GetEndNobPtr  () { return m_pNobEnd; }     //TODO: Nob --> PosNob

	Nob const * GetStartNobPtr () const { return m_pNobStart; }   //TODO: Nob --> PosNob
	Nob const * GetEndNobPtr   () const { return m_pNobEnd;   }   //TODO: Nob --> PosNob
	size_t      GetNrOfSegments() const { return m_potential.size(); }

	void SetNrOfSegments(size_t const n) { m_potential.resize(n); }

	NobId         GetStartKnotId() const;
	NobId         GetEndKnotId  () const;
	MicroMeterPnt GetStartPoint () const;
	MicroMeterPnt GetEndPoint   () const;
	MicroMeter    GetLength     () const;
	MicroMeterPnt GetVector     () const;
	MicroMeterPnt GetVector     (float const) const;
	MicroMeter    GetSegLength  () const { return GetLength() / Cast2Float(GetNrOfSegments()); };
	MicroMeter    DistPntToPipe(MicroMeterPnt const&) const;

	void          RotateNob(MicroMeterPnt const&, Radian const) final { /* Pipe dir defined by endpoints */ }
	void          SetDir   (Radian const)                       final { /* Pipe dir defined by endpoints */ };
	void          Reconnect()                                   final { /* nothing to connect */ };

	PipePair Split(Nob &) const;

	Radian        GetDir()        const final { return Vector2Radian(GetVector()); };
	NobIoMode     GetIoMode()     const final { return NobIoMode::internal; }
	mV            GetNextOutput() const final { return m_potential[m_potIndex]; }

	MicroMeterPnt GetPos          ()                                      const final;
	bool          IsIncludedIn    (MicroMeterRect const &)                const final;
	bool          Includes        (MicroMeterPnt  const &)                const final;
	void          Check           ()                                      const final;
	void          DrawExterior    (DrawContext const &, tHighlight const) const final;
	void          DrawInterior    (DrawContext const &, tHighlight const) const final;
	void          Expand          (MicroMeterRect &)                      const final;
	void          MoveNob         (MicroMeterPnt const &)                       final;
	void          Link            (Nob const &, Nob2NobFunc const &)            final;
	void          CollectInput    ()                                            final;
	bool          CompStep        ()                                            final;
	void          PositionChanged ()                                            final;
	void          ClearDynamicData()                                            final;
	void          Select          (bool const)                                  final;

	float PosOnPipe(MicroMeterPnt const&) const;

	SegNr GetSegNr  (float const f)     const { return SegNr(Cast2Int(round(f * Cast2Float(GetNrOfSegments() - 1)))); }
	mV    GetVoltage(SegNr const segNr) const { return m_potential[segNr2index(segNr)]; }
	mV    GetVoltage(float const f)     const { return GetVoltage(GetSegNr(f)); }

	mV    GetVoltageAt(MicroMeterPnt const&) const;

	void  SetVoltage(SegNr const segNr, mV const v) { m_potential[segNr2index(segNr)] = v; }

	void DrawArrows(DrawContext const&, MicroMeter const) const;

	void DislocateEndPoint();
	void DislocateStartPoint();

	void RecalcSynapsePositions();

	friend wostream & operator<< (wostream&, Pipe const&);

	MicroMeterPnt GetSegmentVector() const
	{
		MicroMeterPnt const umVector{ GetEndPoint() - GetStartPoint() };
		MicroMeterPnt const umpSegVec{ umVector / Cast2Float(GetNrOfSegments()) };
		return umpSegVec;
	}

	MicroMeterPnt GetSegmentCenter(SegNr const segNr) const
	{
		return getSegmentPos(segNr, 0.5f);
	}

	MicroMeterPnt GetSegmentStart(SegNr const segNr) const
	{
		return getSegmentPos(segNr, 0.0f);
	}

	MicroMeterPnt GetSegmentEnd(SegNr const segNr) const
	{
		return getSegmentPos(segNr, 1.0f);
	}

	void Apply2AllSegments(auto const& func) const
	{
		for (auto segNr = SegNr(0); segNr.GetValue() < GetNrOfSegments(); ++segNr)
			func(segNr);
	}

	void Apply2AllSynapses(auto const& func) const
	{
		for (auto it : m_synapses)
			func(it);
	}

	void FixSynapses() const;

private:
	Nob      * m_pNobStart { nullptr };  //TODO: Nob --> PosNob
	Nob      * m_pNobEnd   { nullptr };  //TODO: Nob --> PosNob
	size_t     m_potIndex   { 0 };   // index in m_potential if SegNr 0
	vector<mV> m_potential  { };

	vector<Nob*> m_synapses;  //TODO: Nob --> Synapse

	MicroMeterPnt dislocation() const;
	size_t        segNr2index(SegNr const) const;
	void          recalcSegments();
	void          posChangedRecursive(Pipe const&);

	MicroMeterPnt getSegmentPos(SegNr const segNr, float const fPos) const
	{
		MicroMeterPnt const umVector  { GetEndPoint() - GetStartPoint() };
		MicroMeterPnt const umpSegVec { umVector / Cast2Float(GetNrOfSegments()) };
		float         const fPosition { (static_cast<float>(segNr.GetValue()) + fPos) };
		return GetStartPoint() + umpSegVec * fPosition;
	}
};

export Pipe const * Cast2Pipe(Nob const *);
export Pipe       * Cast2Pipe(Nob       *);
export Pipe       * SelectPipe(Nob*, PipePair const&, float);
