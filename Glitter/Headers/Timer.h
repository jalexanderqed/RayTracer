#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using std::chrono::high_resolution_clock;

class Timer {
private:
    high_resolution_clock::time_point t_start, t_end;
	std::chrono::microseconds total_time;
public:
	Timer() { }

	void startTimer(void) {
		t_start = high_resolution_clock::now();
	}

	void stopTimer(void) {
		t_end = high_resolution_clock::now();
		total_time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
	}

	double getTime(void) const{
		return total_time.count() / 1000.0;
	}
};


#endif		/* TIMER_H */