#include "timer.hpp"

Timer::Timer(std::ostream& strm) : start(std::chrono::system_clock::now()), strm(strm) {}
Timer::~Timer() {
    strm << (std::chrono::system_clock::now() - start).count() << "ns" << std::endl;
}
