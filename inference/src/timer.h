/*!
 * timer.cpp
 *
 * Timer to control real time running speed.
 *
 *  Created on: Jul 9, 2020
 *      Author: Mickey Beurskens
 */


#ifndef INFERENCE_TIMER_H
#define INFERENCE_TIMER_H

#include <chrono>

using namespace std::chrono;
class Timer {
private:
    steady_clock::time_point next;    // Next time to wait until
    steady_clock::time_point prev;    // Previous time
    milliseconds exec_time;  // Execution time for a single loop
public:
    /*!
     * Initialize time that sleeps every exec_milli_sec milliseconds since last called.
     * @param exec_milli_sec Time in milliseconds to wait each loop.
     */
    Timer(int exec_milli_sec);

    /*!
     * Timer sleep.
     */
    void Sleep();

    /*!
     * Reset timer, count again from then onwards.
     */
    void Reset();
};

#endif //INFERENCE_TIMER_H
