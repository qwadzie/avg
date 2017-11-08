
/*!
 *  \file Avg.cpp
 *  \brief Sample average, and average over time.
 *
 */

#include "StdAfx.h"
#include "Avg.h"


//////////////////////////////////////////////////////////////////////////////
/*!
 *  \brief Constructor
 */
//////////////////////////////////////////////////////////////////////////////
CAvgSamples::CAvgSamples() : 
		m_uNumTimes(0),		// Number of items current in times[] array of frame times
		m_uNextTime(0)		// location for next sample entry
{
}

//////////////////////////////////////////////////////////////////////////////
/*! 
 * \brief Accumulate a sample. Return current avg.  If current sample is over limit, reset 
 *        to no sample values.
 *
 * \param [in] DWORD dwSample
 *
 * \return double average
 *
 */
//////////////////////////////////////////////////////////////////////////////
#define TEST_DROPF 1
#if !TEST_DROPF
double CAvgSamples::Accum(DWORD currDTime)
{
	times[m_uNextTime] = currDTime;

	if (currDTime > resetLimit) {		// Reset circular buffer
		m_uNumTimes = 0;
		m_uNextTime = 0;
	}
	else
	{
		if (m_uNumTimes < sizeCBuff)
			m_uNumTimes++;
	}
	m_uNextTime = (++m_uNextTime) % sizeCBuff;
	unsigned accum = 0;
	if (m_uNumTimes>0)
	{
		for (int i = 0; i < m_uNumTimes; i++)
			accum += times[i];
		return (m_uNumTimes * (1000.0)) / accum;		// compute FPS average
	}
	else
	{
		return 0;
	}
}
#else
double CAvgSamples::Accum(DWORD currDTime,DWORD dwDrop)
{
	// Enter current number
	times[m_uNextTime] = currDTime;
	// If time is error, too much time between frames, reset accumulator.
	if (currDTime > resetLimit) {		// Reset circular buffer
		m_uNumTimes = 0;
		m_uNextTime = 0;
	}
	else
	{
		// Save the current frame time, and increment the storage counter:
		m_uNextTime = (++m_uNextTime) % sizeCBuff;
		if (m_uNumTimes < sizeCBuff)
			m_uNumTimes++;
		// For dropped frames, use time of 0, increment time storage counter, for each one.
		for (; dwDrop>0; dwDrop--)
		{
			times[m_uNextTime] = 0;
			m_uNextTime = (++m_uNextTime) % sizeCBuff;
			if (m_uNumTimes < sizeCBuff)
				m_uNumTimes++;
		}
		// We need to average 
	}
	// Perform average of times that have been stored. Convert dropped frames into values half of highest value.
	// this saves dropped frames time so that FPS is not artificially high (due to 0 time) or low (highest value is halved, sharing time)
	while (m_uNumTimes>0)		// iterate until no more zero values (dropped frames)
	{
		unsigned accum = 0;
		int iZero = -1, iHigh = -1;
		int iHighVal = 0;
		for (int i = 0; i < m_uNumTimes; i++)
		{
			accum += times[i];
			if (times[i]==0)
				iZero = i;
			if (times[i] > (iHighVal+1))		// is not 0 or 1
			{
				iHigh = i;
				iHighVal = times[i];
			}
		}
		// If a frame dropping (a zero value) and a high value exists, convert them to share their times.
		if ( (iZero>=0) && (iHigh>=0) )
		{
			times[iHigh] = times[iZero] = (times[iHigh]+1)>>1;
			continue;
		}
		// Else no dropping, compute result and return it:
		if (accum != 0)										// avoid divide by zero
			return (m_uNumTimes * (1000.0)) / accum;		// compute FPS average
		else
			break;
	}
	return 0;
}
#endif


//////////////////////////////////////////////////////////////////////////////
/*! 
 * \brief Accumulate average over time interval; when time interval is passed return a new average result.
 *
 * \param [in] double  Sample
 * \param [in] DWORD timestamp ms
 *
 * \return double average
 *
 */
//////////////////////////////////////////////////////////////////////////////
double CAvgTimeInterval::Accum(double dSample,DWORD dms)
{
	m_dNumAccum++;
	m_dAccum += dSample;

	// if current time < last time displayed, or current time is > last time displayed + time interval...
	if ((dms < m_dwTimeDispFPS) || (dms >= (m_dwTimeDispFPS + timeInterval) ) )		// timeInterval is 1/3 of a second.
	{
		//
		// average over the number of samples received: (this slows down for dropped frames)
		//
		m_dResult = m_dAccum / m_dNumAccum;
		//
		// average over the time interval:
		//
		//m_dResult = (m_dAccum * m_dNumAccum) / 1000.0;

		m_dwTimeDispFPS = dms;							// dms current time, m_dwTimeDispFPS is last time we displayed.
		m_dNumAccum = 0;
		m_dAccum = 0;
	}
	return m_dResult;
}

//
// end of Avg.cpp
//