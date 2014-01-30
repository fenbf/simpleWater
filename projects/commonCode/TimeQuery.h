/** @file TimeQuery.h
*  @brief wrapper for the time_query in openGL
*
*	@author Bartlomiej Filipek 
*	@date May 2012
*/


/** simple wrapper for the GL_TIME_QUERY from OpenGL */
class TimerQuery
{
public:
    enum class WaitOption 
    { 
        WaitForResults, 
        DoNotWaitForResults 
    };

private:
    GLuint   mQuery;
    GLuint64 mWholeTime;
    GLuint   mCounter;
    double   mTimes[3];
    double   mTime;
public:
    TimerQuery();
    ~TimerQuery();

    /// inits the query, deletes the query if it is created
    void init();

    /// call it before the code you want to measure
    inline void begin();

    /// call it just after the code you want to measure
    inline void end();

    /// call it after end() or at the end of a frame
    void updateResults(WaitOption wait);

    /// resets all the time data (does not delete the query object!)
    inline void resetTime();

    /// @return average time of the whole tests, call it usually at the end of app, in miliseconds
    inline double getAverageTime() const;

    /// @return average time from four tests, updated in updateResults(), in miliseconds 
    inline double getTime() const;
private:
    void deleteQuery();
};

///////////////////////////////////////////////////////////////////////////////
// inline:

///////////////////////////////////////////////////////////////////////////////
void TimerQuery::begin()
{
    glBeginQuery(GL_TIME_ELAPSED, mQuery);
}

///////////////////////////////////////////////////////////////////////////////
void TimerQuery::end()
{
    glEndQuery(GL_TIME_ELAPSED);
}

///////////////////////////////////////////////////////////////////////////////
inline void TimerQuery::resetTime() 
{ 
    mWholeTime = 0; 
    mCounter   = 0; 
    mTimes[0] = mTimes[1] = mTimes[2] = 0.0;
    mTime = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
inline double TimerQuery::getAverageTime() const	
{
    double avg = mWholeTime/(double)mCounter;
    avg /= 1000000.0;
    return avg;
}

///////////////////////////////////////////////////////////////////////////////
inline double TimerQuery::getTime() const 
{ 
    return mTime; 
}