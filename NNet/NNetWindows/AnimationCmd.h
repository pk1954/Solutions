// AnimationCmd.h
//
// NNetWindows

#pragma once

#include "win32_callable.h"
#include "win32_mainWindow.h"

using std::function;

class AnimationCmd
{
public:
    AnimationCmd(MainWindow & win)
        : m_win(win)
    {}

    virtual void Do  (function<void()> const &) = 0;
    virtual void Undo(function<void()> const &) = 0;

    virtual void UpdateUI() { m_win.Notify(false); };

protected:
  
    MainWindow     & m_win;
    function<void()> m_targetReachedFunc { nullptr };
};