#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <windows.h>

class Hwindow {
	static Hstack<uint32_t, Hwindow> windows;

	HWND hwnd;
	HINSTANCE hInstance;
	WTEXT className;
	WTEXT title;
	uint32_t width;
	uint32_t height;
	bool running;

	static LRESULT CALLBACK WindowProc(HWND hwnd, 
									   UINT uMsg, 
									   WPARAM wParam, 
									   LPARAM lParam);

	Hwindow(HWND hwnd, 
			HINSTANCE hInstance, 
			WTEXT className, 
			WTEXT title, 
			uint32_t width, 
			uint32_t height) : hwnd(hwnd), 
							   hInstance(hInstance), 
							   className(className), 
							   title(title), 
							   width(width), 
							   height(height), 
							   running(true) {}
public:
	static Hwindow& initialize(uint32_t width, uint32_t height);
	
	void show();
	void hide();
	bool isRunning() const;
	void processMessages();

	HWND getHandle() const { return hwnd; }
	uint32_t getWidth() const { return width; }
	uint32_t getHeight() const { return height; }

	Hwindow() : hwnd(nullptr), 
			    hInstance(nullptr), 
			    className(nullptr), 
			    title(nullptr), 
			    width(0), 
			    height(0), 
			    running(false) {}

   ~Hwindow() {
		if (this->hwnd) {
			DestroyWindow(this->hwnd);
		}
		if (this->className) {
			UnregisterClass(this->className, this->hInstance);
		}
	}

	Hwindow(Hwindow&& other) noexcept : hwnd(other.hwnd),
										hInstance(other.hInstance),
										className(other.className),
										title(other.title),
										width(other.width),
										height(other.height),
										running(other.running) {
		other.hwnd = nullptr;
		other.hInstance = nullptr;
		other.className = nullptr;
		other.title = nullptr;
		other.width = 0;
		other.height = 0;
		other.running = false;
	}

	Hwindow& operator=(Hwindow&& other) noexcept {
		if (this != &other) {
			if (this->hwnd) {
				DestroyWindow(this->hwnd);
			} if (this->className) {
				UnregisterClass(this->className, this->hInstance);
			}

			this->hwnd = other.hwnd;
			this->hInstance = other.hInstance;
			this->className = other.className;
			this->title = other.title;
			this->width = other.width;
			this->height = other.height;
			this->running = other.running;

			other.hwnd = nullptr;
			other.hInstance = nullptr;
			other.className = nullptr;
			other.title = nullptr;
			other.width = 0;
			other.height = 0;
			other.running = false;
		}
		
		return *this;
	}

    Hwindow(const Hwindow&) = delete;
    Hwindow& operator=(const Hwindow&) = delete;
};

#endif // WINDOW_HPP