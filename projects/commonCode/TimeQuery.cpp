/** @file TimeQuery.h
*  @brief wrapper for the time_query in openGL
*
*	@author Bartlomiej Filipek 
*	@date May 2012
*/

#include "commonCode.h"

#include "Init.h"
#include "TimeQuery.h"

///////////////////////////////////////////////////////////////////////////////
TimerQuery::TimerQuery() 
{ 
    mQuery     = 0; 
    resetTime();
}

///////////////////////////////////////////////////////////////////////////////
TimerQuery::~TimerQuery()
{ 
    deleteQuery(); 
}

///////////////////////////////////////////////////////////////////////////////
void TimerQuery::init() 
{ 
    //if (!GLEW_ARB_timer_query)
    //{
    //	LOG_ERROR("GL_ARB_timer_query not available!");
    //	return;
    //}
    deleteQuery(); 
    resetTime(); 
    glGenQueries(1, &mQuery); 
}

///////////////////////////////////////////////////////////////////////////////
void TimerQuery::updateResults(WaitOption wait)
{
    // query time results
    int stopTimerAvailable = (wait == WaitOption::WaitForResults ? 0 : 1);
    while (!stopTimerAvailable)
    {
        glGetQueryObjectiv(mQuery, GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }

    GLuint64 t;
    glGetQueryObjectui64v(mQuery, GL_QUERY_RESULT, &t);
    mWholeTime += t;
    mCounter++;

    double msec = (double)t/1000000.0;
    mTime = 0.25*(msec + mTimes[0] + mTimes[1] + mTimes[2]);
    mTimes[2] = mTimes[1];
    mTimes[1] = mTimes[0];
    mTimes[0] = msec;
}



///////////////////////////////////////////////////////////////////////////////
void TimerQuery::deleteQuery() 
{ 
    if (mQuery > 0) 
        glDeleteQueries(1, &mQuery); 
    mQuery = 0; 
}