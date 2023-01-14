// IoLine.ixx
//
// NNetModel

module;

#include <vector>

export module NNetModel:IoLine;

import Types;
import :NNetParameters;
import :MicroMeterPosDir;
import :NobType;
import :PosNob;
import :Nob;

export class IoLine : public PosNob
{
public:
	static bool TypeFits(NobType const type) { return type.IsIoLineType(); }

	IoLine(MicroMeterPnt const & upCenter, NobType const type)
	  : PosNob(type),
		m_umPosition(upCenter)
	{}

	void Check() const;

	bool CompStep() final { return false; }

	MicroMeter    GetExtension() const final { return NEURON_RADIUS; }
	MicroMeterPnt GetPos()       const final { return m_umPosition; }
	Radian        GetDir()       const final;

	void Recalc     ()                                   final;
	void SetPosNoFix(MicroMeterPnt const&)               final;
	void SetDir     (Radian const)                       final;
	void MoveNob    (MicroMeterPnt const&)               final;
	void RotateNob  (MicroMeterPnt const&, Radian const) final;
	void Link       (Nob const&, Nob2NobFunc const&)     final;

	void SetDirVector(MicroMeterPnt const &p) { SetDir(Vector2Radian(p)); }

	enum class State
	{
		standard,
		connected,
		locked
	};

	// IoLines are in one of three states:
	//
	// 1: standard - direction is determined by vector of m_pPipe
	//             HasParentNob() is false
	//             m_radDirection is Radian::NULL_VAL 
	// 2: connected - IoLine is in IoConnector. Direction is determined by vector of IoConnector
	//             HasParentNob() is true
	//             ParentNob points to IoConnector 
	//             m_radDirection is Radian::NULL_VAL 
	// 3: locked - direction is explicitely defined in m_radDirection. Only during animations.
	//             HasParentNob() is false
	//             m_radDirection is not Radian::NULL_VAL 

	State GetState() const
	{
		if (HasParentNob())
			return State::connected;
		if (m_radDirection.IsNull())
			return State::standard;
		else
			return State::locked;
	}

	void SetState(State const state, Nob* const p = nullptr)
	{
		switch (state)
		{
		case State::connected:
			m_pIoConnector = p;
			m_radDirection.Set2Null();
			break;
		case State::standard:
			StandardDirection();
			break;
		case State::locked:
			LockDirection();
			break;
		}
	}

	void StandardDirection()             
	{ 
		SetDir(Radian::NULL_VAL());
	}

	void Connect2IoConnector(Nob* const p) 
	{ 
		m_pIoConnector = p; 
		m_radDirection.Set2Null();
	}

	void LockDirection()
	{ 
		SetDir(GetDir()); 
	}

	bool IsDirLocked() const { return m_radDirection.IsNotNull(); }

	MicroMeterPnt GetDirVector() const;

	Pipe       * GetPipe ()       { return m_pPipe; }
	Pipe const * GetPipeC() const { return m_pPipe; }

	void SetPipe(Pipe * pPipe) { m_pPipe = pPipe; }

	void SelectAllConnected(bool const) final;
	void DirectionDirty    ()           final;

	bool  HasParentNob() const final { return m_pIoConnector != nullptr; }
	Nob * GetParentNob() const final { return m_pIoConnector; }

private:
	Radian getDirection() const;

	Pipe        * m_pPipe        { nullptr };
	Nob         * m_pIoConnector { nullptr };
	MicroMeterPnt m_umPosition;
	Radian        m_radDirection { Radian::NULL_VAL() };
};
