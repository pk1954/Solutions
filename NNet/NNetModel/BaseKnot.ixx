// BaseKnot.ixx
//
// NNetModel

module;

#include <type_traits>
#include <vector>
#include <compare>

export module NNetModel:BaseKnot;

import Types;
import DrawContext;
import :Pipe;
import :PipeList;
import :NobType;
import :PosNob;

using std::vector;
using std::is_base_of;
using std::remove_pointer_t;

export class BaseKnot : public PosNob
{
public:

	BaseKnot(MicroMeterPnt const &, NobType const, MicroMeter const);
	BaseKnot(BaseKnot const &) = default;
	~BaseKnot() override = default;

	virtual bool operator==(Nob const &) const;

	virtual BaseKnot & operator*=(float const);
	virtual BaseKnot & operator+=(BaseKnot const &);
	virtual BaseKnot & operator-=(BaseKnot const &);

	void AppendMenuItems(AddMenuFunc const&) const override;

	MicroMeterPnt GetPos()      const final { return m_circle.GetPos(); }
	mV            GetVoltage()  const       { return m_mVinputBuffer; }
	void          SetVoltage(mV const v)    { m_mVinputBuffer = v; }

	void Dump()                                       const override;
	void Check()                                      const override;
	void Reconnect()                                        override;
	void SetPos      (MicroMeterPnt  const &)               override;
	void MoveNob     (MicroMeterPnt  const &)               override;
	bool Includes    (MicroMeterPnt  const &)         const override;
	bool IsIncludedIn(MicroMeterRect const &)         const override;
	void Expand      (MicroMeterRect       &)         const override;
	void RotateNob   (MicroMeterPnt  const &, Radian const) override;
	void Link        (Nob const &, Nob2NobFunc const &)     override;

	static bool TypeFits(NobType const type) { return type.IsBaseKnotType(); }

	MicroMeterCircle GetCircle()    const { return m_circle; }
	MicroMeter       GetExtension() const { return m_circle.GetRadius(); }

	void SetExtension(MicroMeter const um) { m_circle.SetRadius(um); }

	bool IsPrecursorOf(Pipe const &) const;
	bool IsSuccessorOf(Pipe const &) const;

	bool IsPrecursorOf(BaseKnot const &) const;
	bool IsSuccessorOf(BaseKnot const &) const;

	bool IsDirectlyConnectedTo(BaseKnot const &) const;
	bool IsDirectlyConnectedTo(Pipe     const &) const;

	bool HasIncoming() const { return m_inPipes.IsNotEmpty(); }
	bool HasOutgoing() const { return m_outPipes.IsNotEmpty(); }
	bool IsOrphan()    const { return !(HasIncoming() || HasOutgoing()); }

	void AddIncoming(BaseKnot const& src) {	src.Apply2AllInPipes ([this](Pipe& pipe) { AddIncoming(pipe); }); }
	void AddOutgoing(BaseKnot const& src) {	src.Apply2AllOutPipes([this](Pipe& pipe) { AddOutgoing(pipe); }); }

	size_t GetNrOfInConns    () const { return m_inPipes.Size(); }
	size_t GetNrOfOutConns   () const { return m_outPipes.Size(); }
	size_t GetNrOfConnections() const { return m_inPipes.Size() + m_outPipes.Size(); }

	void SetIncoming(size_t const i, Pipe * p) { m_inPipes .Set(i, p); }
	void SetOutgoing(size_t const i, Pipe * p) { m_outPipes.Set(i, p); }

	Pipe       & GetIncoming(size_t const i) { return m_inPipes .Get(i); }
	Pipe       & GetOutgoing(size_t const i) { return m_outPipes.Get(i); }

	Pipe const & GetIncoming(size_t const i) const { return m_inPipes .Get(i); }
	Pipe const & GetOutgoing(size_t const i) const { return m_outPipes.Get(i); }

	Pipe& GetFirstIncoming() { return m_inPipes.GetFirst(); }
	Pipe& GetFirstOutgoing() { return m_outPipes.GetFirst(); }

	Pipe const& GetFirstIncoming() const { return m_inPipes.GetFirst(); }
	Pipe const& GetFirstOutgoing() const { return m_outPipes.GetFirst(); }

	void AddIncoming(Pipe & pipe) { m_inPipes .Add(pipe); }
	void AddOutgoing(Pipe & pipe) { m_outPipes.Add(pipe); }

	void RemoveIncoming(Pipe & pipe) { m_inPipes .Remove(pipe); }
	void RemoveOutgoing(Pipe & pipe) { m_outPipes.Remove(pipe); }

	void ReplaceIncoming(Pipe * const pDel, Pipe * const pAdd) final { m_inPipes .Replace(pDel, pAdd); }
	void ReplaceOutgoing(Pipe * const pDel, Pipe * const pAdd) final { m_outPipes.Replace(pDel, pAdd); }

	void SetIncoming(PipeList const & l) { m_inPipes  = l; }
	void SetOutgoing(PipeList const & l) { m_outPipes = l; }

	void SetIncoming(BaseKnot const & b) { SetIncoming(b.m_inPipes); }
	void SetOutgoing(BaseKnot const & b) { SetOutgoing(b.m_outPipes); }

	void Apply2AllInPipes       (PipeFunc const & f) const final { m_inPipes.Apply2All(f); }
	void Apply2AllOutPipes      (PipeFunc const & f) const final { m_outPipes.Apply2All(f); }
	void Apply2AllConnectedPipes(PipeFunc const & f) const;

	bool Apply2AllInPipesB       (PipeCrit const & c) const final { return m_inPipes .Apply2AllB(c); }
	bool Apply2AllOutPipesB      (PipeCrit const & c) const final { return m_outPipes.Apply2AllB(c); }
	bool Apply2AllConnectedPipesB(PipeCrit const & c) const;

	MicroMeterRect GetRect4Text() const;

	void EvaluateSelectionStatus();

private:
	MicroMeterCircle m_circle;

	PipeList m_inPipes;
	PipeList m_outPipes;
};

export BaseKnot const* Cast2BaseKnot(Nob const*);
export BaseKnot      * Cast2BaseKnot(Nob      *);

export template <typename T>
concept BaseKnot_t = is_base_of<BaseKnot, remove_pointer_t<T>>::value;
