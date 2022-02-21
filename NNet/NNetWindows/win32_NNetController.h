// win32_NNetController.h
//
// NNetSimu

#pragma once

class NNetModelReaderInterface;
class NNetModelCommands;
class WinCommands;
class SlowMotionRatio;
class MonitorWindow;
class NNetModelExporter;
class DisplayFunctor;
class ComputeThread;
class NNetAppMenu;
class NNetModel;
class MainWindow;
class WinManager;
class Preferences;
class CommandStack;
class Observable;
class Signal;
class Sound;

using std::wstring;
using std::wcout;

class NNetController
{
public:
	NNetController() = default;

	void Initialize
	(
		NNetModelExporter        * const,
		MainWindow               * const,
		WinManager               * const,
		NNetModelReaderInterface * const,
		NNetModelCommands        * const,
		ComputeThread            * const,
		SlowMotionRatio          * const,
		DisplayFunctor           * const,
		Sound                    * const,
		Preferences              * const,
		CommandStack             * const,
		MonitorWindow            * const
	);

	virtual ~NNetController();

	bool HandleCommand(int const, LPARAM const, MicroMeterPnt const = NP_NULL);

private:

	bool       IsTraceOn  () const { return m_bTrace; }
	wostream & TraceStream()       { return wcout; }

	void triggerSoundDlg    (NobId const);
	bool processUIcommand   (int const, LPARAM const);
	bool processModelCommand(int const, LPARAM const = 0, MicroMeterPnt const = NP_NULL);

	bool                       m_bTrace                 { true };
	bool                       m_bBlockedUI             { false };
	HCURSOR                    m_hCrsrWait              { nullptr };
	Sound                    * m_pSound                 { nullptr };
	NNetModelExporter        * m_pModelExporter         { nullptr };
	MainWindow               * m_pMainWindow            { nullptr };
	WinManager               * m_pWinManager            { nullptr };
	ComputeThread            * m_pComputeThread         { nullptr };
	NNetModelReaderInterface * m_pNMRI                  { nullptr };
	NNetModelCommands        * m_pModelCommands         { nullptr };
	SlowMotionRatio          * m_pSlowMotionRatio       { nullptr };
	DisplayFunctor           * m_pStatusBarDisplay      { nullptr };
	Preferences              * m_pPreferences           { nullptr };
	CommandStack             * m_pCommandStack          { nullptr };
	MonitorWindow            * m_pMonitorWindow         { nullptr };
	Observable               * m_pStaticModelObservable { nullptr };
};				          
