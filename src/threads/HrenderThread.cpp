#include "HrenderThread.hpp"

HrenderThread::HrenderThread() {
	instance = &HVinstance::initialize(true);
	window = &Hwindow::initialize(1280, 720);
	surface = &HVsurface::initialize(*instance, *window);
}