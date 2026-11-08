#include "PCH.h"
#include "Timer.h"

Timer::Timer() noexcept :
    m_last{Clock::now()}
{
}

void Timer::Tick() noexcept
{
	const TimePoint now = Clock::now();
	const Duration unscaledDelta = std::chrono::duration_cast<Duration>(now - m_last);
	m_last = now;
	const bool bPaused = m_bPaused;
	const Duration scaledDelta = bPaused ? Duration::zero() : Duration{unscaledDelta.count() * m_timeScale};

	++m_timeInfo.frameIndex;
	m_timeInfo.unscaledTime += unscaledDelta;
	m_timeInfo.scaledTime += scaledDelta;
	m_timeInfo.unscaledDelta = unscaledDelta;
	m_timeInfo.timeScale = m_timeScale;
	m_timeInfo.scaledDelta = scaledDelta;
	m_timeInfo.bPaused = bPaused;
}

double Timer::ToUnit(Duration d, TimeUnit u) noexcept
{
	switch (u)
	{
		case TimeUnit::Seconds:
			return d.count();
		case TimeUnit::Milliseconds:
			return d.count() * 1e3;
		case TimeUnit::Microseconds:
			return d.count() * 1e6;
		case TimeUnit::Nanoseconds:
			return d.count() * 1e9;
	}
	return d.count();
}

double Timer::GetDelta(TimeDomain domain, TimeUnit unit) const noexcept
{
	if (domain == TimeDomain::Unscaled && m_timeInfo.frameIndex == 0)
	{
		return ToUnit(Duration{1.0 / 60.0}, unit);
	}
	const Duration delta = (domain == TimeDomain::Scaled) ? m_timeInfo.scaledDelta : m_timeInfo.unscaledDelta;
	return ToUnit(delta, unit);
}

double Timer::GetTotalTime(TimeDomain domain, TimeUnit unit) const noexcept
{
	const Duration total = (domain == TimeDomain::Scaled) ? m_timeInfo.scaledTime : m_timeInfo.unscaledTime;
	return ToUnit(total, unit);
}
