#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <cassert>
#include <cmath>
#include <csignal>
#include <iomanip>
#include <iostream>


#define FORMAT(i, max) std::setw(1+(std::streamsize)log10(max)) << i
#ifdef _MSC_VER
#define SLEEP_IN_MS(ms) Sleep(ms)
#else
//#define SLEEP_IN_MS(ms) usleep(ms)
#define SLEEP_IN_MS(ms) { struct timespec t; t.tv_sec=ms/1000; t.tv_nsec=(ms-t.tv_sec*1000)*1000000; nanosleep(&t, NULL); };
#endif


#define TEST_MAX_SIZE (1<<12)
#define TEST_TIMEOUT_IN_US 800000
#define TEST_MAX_TIME_IN_S 100


namespace Tests {
	void ASSERT(bool condition) {
		if (!condition) {
			std::cerr << "TEST FAILED" << std::endl;
			std::raise(SIGABRT);
		}
	}

	//class Test {
	//public:
	void Test(const std::string &name) {
		std::cout << std::endl << "[ ***** " << name.c_str() << " ***** ]" << std::endl;
	}

	//TODO test result in destructor?
	//};

	namespace Util {
		int dummy(int a) {
			return a;
		}
		int dummyPrint(int a) {
			std::cout << "a = " << a << std::endl;
			return a;
		}

		int compute(int a) {
			int64_t n = (int64_t)1 << a;
			if (a <= 0) {
				return 1;
			}
			uint64_t res = n;
			while (--n > 1) {
				res *= n;
			}
			return (int)res;
		}

		void sleepInMs(int ms) {
			//std::cout << "sleepInMs(" << ms << ") in thread " << std::this_thread::get_id() << std::endl;
			SLEEP_IN_MS(ms);
		}

		int log2(int i) {
			int res = 0;
			while (i >>= 1) {
				++res;
			}
			return res;
		}

		bool isPow2(int i) {
			return (i == 0) || (i - (1 << (int)log2(i)) == 0);
		}

		class Profiler {
		public:
			Profiler(const std::string &name) : name(name) {
#ifdef _MSC_VER
				QueryPerformanceCounter(&startTime);
#else
				gettimeofday(&startTime, NULL);
#endif
			}

			~Profiler() {
				std::cout << "[" << name.c_str() << "] " << FORMAT(elapsedInUs(), TEST_MAX_TIME_IN_S*unit) << " us" << std::endl;
			}

			uint64_t elapsedInUs() {
#ifdef _MSC_VER
				LARGE_INTEGER stopTime;
				QueryPerformanceCounter(&stopTime);
				LARGE_INTEGER countsPerSecond;
				QueryPerformanceFrequency(&countsPerSecond);
				return (uint64_t)((unit * (stopTime.QuadPart - startTime.QuadPart)) / countsPerSecond.QuadPart);
#else
				struct timeval stopTime;
				gettimeofday(&stopTime, NULL);
				return ((uint64_t)stopTime.tv_sec * 1000000 + stopTime.tv_usec) - ((uint64_t)startTime.tv_sec * 1000000 + startTime.tv_usec);
#endif
			}

		private:
			std::string name;
#ifdef _MSC_VER
			LARGE_INTEGER startTime;
#else
			struct timeval startTime;
#endif
			const int unit = 1000000;
		};
	}
}