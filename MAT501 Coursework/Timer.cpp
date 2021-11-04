#include "Timer.h"

using namespace sf;


Clock Timer::m_gameClock;
Clock Timer::m_frameClock;
Time Timer::m_dt;


void Timer::Start()
{
	m_gameClock.restart();
	m_frameClock.restart();
	m_dt = Time::Zero;
}


void Timer::NewFrame()
{
	m_dt = m_frameClock.restart();
}


float Timer::t()
{
	return m_gameClock.getElapsedTime().asSeconds();
}


float Timer::dt()
{
	return m_dt.asSeconds();
}

