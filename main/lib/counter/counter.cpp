/**
 * @file counter.cpp
 * @brief Implementation of the cCount class for counting and flag management.
 * This class provides methods to wait for a specified goal count,
 * check readiness, and manage current and next flags.
 * @version 1.0.0
 * @date 2025-10-02
 * @author Eubry Gomez Ramirez
 */
#include "counter.h"

void cCount::checkGoalChange(int goal){
    if(_pvGoal!=goal){
        _cnt=-1;
        _goal=goal-1;
        _pvGoal=goal;
    }
}
bool cCount::start(){
    return (_cnt==0)?true:false;//returns true when counter resets
}
void cCount::count(int goal, bool &currentFlag, bool &nextFlag){
    checkGoalChange(goal);
    if(_cnt<_goal){
        _cnt++;
        cFlag = &currentFlag;
        nFlag = &nextFlag;
        _rdy=false;
        return;
    }
    _cnt=0;
    _rdy=true;
    *cFlag=false;
    *nFlag=true;
}
void cCount::count(int goal){
    checkGoalChange(goal);
    if(_cnt<_goal){
        _rdy=false;
        _cnt++;
        return;
    }
    _cnt=0;
    _rdy=true;
}
bool cCount::finish(){return _rdy;}
int cCount::get(){return _cnt;}
bool cCount::currentFlag(){return cFlag ? *cFlag : false;}
bool cCount::nextFlag(){return nFlag ? *nFlag : false;}
void cCount::reset(){_cnt=0;}
cCount::~cCount(){
    cFlag = nullptr;
    nFlag = nullptr;
    _cnt=0;
    _rdy=false;
}
//--------------Time Counter Implementation----------------
void cTime::checkGoalChange(int64_t goalMicrosec){{
    if(_goalTime!=goalMicrosec){
        _goalTime=goalMicrosec;
        reset();
    }
}}
void cTime::wait(int64_t goalMicrosec){
    checkGoalChange(goalMicrosec);
    int64_t currentTime=esp_timer_get_time();
    _elapsedTime= (currentTime - _lastTime);//Calculate elapsed time in microseconds
    _rdy=false;
    if(cntFirst<2){
        cntFirst++;
    }
    if(_elapsedTime>=_goalTime){
        _rdy=true;
        _elapsedTime=0;
        _lastTime=currentTime;
    }
}
bool cTime::start(){
    return (cntFirst==1)?true:((_elapsedTime==0)?true:false);//returns true when timer resets
}
bool cTime::finish(){return _rdy;}
int64_t cTime::get(){
    return _elapsedTime;
}
void cTime::reset(){
    _elapsedTime=0;
    cntFirst=0;
    _lastTime=esp_timer_get_time();
}
cTime::~cTime(){
    _goalTime=0;
    _lastTime=0;
    _elapsedTime=0;
    cntFirst=0;
}