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

using std::vector;

export class IoLine : public PosNob
{
public:
	static bool TypeFits(NobType const type) { return type.IsIoLineType(); }

	IoLine(MicroMeterPnt const & upCenter, NobType const type)
		: PosNob(type),
		  m_circle(upCenter, NEURON_RADIUS)
	{}

	bool CompStep() final { return false; }

	MicroMeter       GetExtension() const final { return m_circle.GetRadius(); }
	MicroMeterPnt    GetPos()       const final { return m_circle.GetPos(); }
	MicroMeterPosDir GetPosDir()    const final;
	Radian           GetDir()       const final;

	void SetPos   (MicroMeterPnt const&)               final;
	void SetDir   (Radian const)                       final;
	void MoveNob  (MicroMeterPnt const&)               final;
	void RotateNob(MicroMeterPnt const&, Radian const) final;

	void Link(Nob const&, Nob2NobFunc const&)          final;

	void SetDirVector(MicroMeterPnt const &p) { SetDir(Vector2Radian(p)); }

	// IoLines can be locked or unlocked
	bool LockDirection();     // locked: m_radDirection is not null, it is displayed with this direction
	bool UnlockDirection();   // unlocked: m_radDirection is null, displayed direction
	bool IsDirLocked() const; //           is computed depending on connected pipe(s)

	MicroMeterPnt    GetScaledDirVector() const;
	MicroMeterPnt    GetDirVector() const;
	MicroMeterPosDir GetRawPosDir() const;

	Pipe       * GetPipe ()       { return m_pPipe; }
	Pipe const * GetPipeC() const { return m_pPipe; }

	void SetPipe(Pipe * pPipe) { m_pPipe = pPipe; }

	void SelectAllConnected(bool const bFirst, bool const bOn) final
	{
		if ((IsSelected() != bOn) || bFirst)
		{
			Nob::Select(bOn);
			m_pPipe->SelectAllConnected(false, bOn);
			if (HasParentNob())
				GetParentNob()->SelectAllConnected(false, bOn);
		}
	}

private:
	MicroMeterPnt determineVector() const;

	MicroMeterCircle m_circle;
	Radian           m_radDirection { Radian::NULL_VAL() };
	Pipe           * m_pPipe        { nullptr };
};

export MicroMeterPnt CalcOrthoVector(vector<IoLine*> const&, MicroMeterLine const&);
