#pragma once
#include <chrono>
#include <mutex>

#include "../JuceLibraryCode/JuceHeader.h"

class TimerTextBox : public TextEditor, private Timer
{

	std::chrono::time_point<std::chrono::system_clock> tp;
	String& m_update;
	std::mutex& m_mtx;

public:

	TimerTextBox(String& update, std::mutex& mtx)
		: TextEditor{ String() }, m_mtx{ mtx }, m_update{ update }
	{
		tp = std::chrono::system_clock::now();
	}

	void set_text(const String& src)
	{
		while (!m_mtx.try_lock()) {}
		m_update = src;
		m_mtx.unlock();
	}

	virtual void visibilityChanged()
	{
		if (isVisible())
			startTimer(100);
		else
			stopTimer();
	}

private:

	virtual void timerCallback() override
	{
		if (std::chrono::system_clock::now() - tp >= std::chrono::milliseconds(100))
		{
			while (!m_mtx.try_lock()) {}
			this->setText(m_update);
			m_mtx.unlock();
		}
	}


	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimerTextBox)
};
