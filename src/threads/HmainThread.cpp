#include "HmainThread.hpp"

HmainThread::HmainThread() {
	instance = &HVinstance::initialize(true);
	window = &Hwindow::initialize(1280, 720);
	surface = &HVsurface::initialize(*instance, *window);
}