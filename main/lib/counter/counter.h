#ifndef COUNTER_H
#define COUNTER_H
#include <utility>
#include "esp_timer.h"
class cCount{
    private:
        int _cnt=0;
        int _goal=0;
        int _pvGoal=0;//previous goal
        bool* cFlag;
        bool* nFlag;
        bool _rdy=false;
        void checkGoalChange(int goal);
    public:
    cCount(): cFlag(nullptr), nFlag(nullptr) {};
    void count(int goal,bool &currentFlag,bool &nextFlag);
    void count(int goal);
    int get();
    bool start();
    bool finish();
    template<typename Func, typename... Args>
    void after(Func callback, Args&&... args) {
        if(_rdy) {
            callback(std::forward<Args>(args)...);
        }
    }
    bool currentFlag();
    bool nextFlag();

    void reset();
    ~cCount();
};
class cTime{
    private:
        int64_t _goalTime=0;
        int64_t _lastTime=0;
        int64_t _elapsedTime=0;
        int8_t cntFirst=0;
        bool _rdy=false;
        void checkGoalChange(int64_t goalMillisec);
    public:
        cTime(): _goalTime(0), _lastTime(0), _elapsedTime(0) {};
        void wait(int64_t goalMillisec);
        bool start();
        bool finish();
        int64_t get();
        void reset();
        ~cTime();
};

#endif // COUNTER_H