/*!
 * timer.cpp
 *
 * Timer to control real time running speed.
 *
 *  Created on: Jul 9, 2020
 *      Author: Mickey Beurskens
 */


#include <thread>
#include <chrono>
#include <iostream>
#include "timer.h"

using namespace std::chrono;
Timer::Timer(int exec_milli_sec)
{
    prev = steady_clock::now();
    exec_time = milliseconds(exec_milli_sec);
    next = prev + exec_time;
}

void Timer::Sleep()
{
    if (steady_clock::now() > next) {
        std::cout << "WARNING: The set execution time for an inference loop has been exceeded!\n";
    }

    std::this_thread::sleep_until(next);
    Reset();
}

void Timer::Reset()
{
    prev = steady_clock::now();
    next = prev + exec_time;
}

