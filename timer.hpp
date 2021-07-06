#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <chrono>
#include <iostream>

class Timer {
public:
    Timer(std::ostream& strm);
    ~Timer();
private:
    const std::chrono::system_clock::time_point start;
    std::ostream& strm;
};

#endif //_TIMER_HPP_
