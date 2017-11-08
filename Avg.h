

/*!
 *  \file Avg.h
 *  \brief CAvgSamples provide average of samples.  CAvgTimeInterval provide average over time interval.
 *
 */

#ifndef Avg_H
#define Avg_H


//////////////////////////////////////////////////////////////////////////////
/*!
 * \brief Average a set number of timestamp samples, numbers below 1 second (1000 ms), for FPS.
 *        compute return FPS average.  Reset averaging and return 0 if difference between samples is too large; 1000.
 *
 *        CAvgSamples averages for a number of samples.
*
* FPS algorithm update for variable frame rates on different machines.
*CAvgSamples::Accum(CurrDTS,DropCnt)
*    param - difference between current and previous frame time stamps,
*    param -  count of # of frames dropped. once dropped frames are accounted for (counted) this variable is set to 0.
*    
*    Create an average of up to the last 12 diffs, return it.
*    Using drop: For each drop, add a 'diff' value. The value of the added diff is half of the largest diff in the list, and that value
*    is also halved. This distributes the value of the dropped frame so that it does not artifically increase the resulting FPS
*    value (by being counted as a '0' length frame, rather it's counted as half of the largest length frame and the largest length
*    frame is cut by half).
*    
*CAvgTimeInterval::Accum(dSample,dms)
*    param- dSample is the value returned from above, the average of the diffs accumulated, a running avg of last 12 frames.
*    param - dms current milisecond.
*        Average all dSample values in a time period, currently is 1/3 second, so the value is updated every 1/3 second.
*        Each 1/3 second the average is re-started accumulating.
*        the screen value and thus this value is updated as frequently as 50 times a second)
*        
 */
//////////////////////////////////////////////////////////////////////////////
class CAvgSamples
{
static const int sizeCBuff = 12;
static const DWORD resetLimit = 1000;
public:

	CAvgSamples();
	double Accum(DWORD currDTime,DWORD dwDrop = 0);
private:
	LONGLONG times[sizeCBuff];		//!< array of samples: frame times
	unsigned	m_uNumTimes;		//!< Number of items current in times[] array of frame times
	unsigned	m_uNextTime;		//!< next array index to store into
};
//////////////////////////////////////////////////////////////////////////////
/*!
 * \brief Average all samples in a fixed time interval
 *
 */
//////////////////////////////////////////////////////////////////////////////
class CAvgTimeInterval
{

static const DWORD timeInterval = 330;
public:

	CAvgTimeInterval() : m_dResult(0), m_dNumAccum(0), m_dAccum(0), m_dwTimeDispFPS(0) {}
	double	Accum(double dSample,DWORD dwTS);
private:
	double		m_dNumAccum;		//!< Number of items accumulated
	double		m_dAccum;			//!< Accumulator
	double		m_dResult;			//!< last result
	DWORD		m_dwTimeDispFPS;	//!< Start of interval last time was displayed
};

#endif		//Avg_H
