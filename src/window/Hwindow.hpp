#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "memory/Hstack.hpp"
#include "Horizon.hpp"

#include <windows.h>

class Hwindow {
	HWND 	  _hwnd;
	HINSTANCE _h_instance;
	WTEXT 	  _class_name;
	WTEXT 	  _title;
	uint32_t  _width;
	uint32_t  _height;
	bool 	  _running;

	static LRESULT CALLBACK WindowProc(HWND hwnd, 
									   UINT u_msg, 
									   WPARAM w_param, 
									   LPARAM l_param);

	Hwindow(HWND hwnd, 
			HINSTANCE h_instance, 
			WTEXT class_name, 
			WTEXT title, 
			uint32_t width, 
			uint32_t height) : _hwnd(hwnd), 
							   _h_instance(h_instance), 
							   _class_name(class_name), 
							   _title(title), 
							   _width(width), 
							   _height(height), 
							   _running(true) {}

public:
	static Hwindow* initialize(WTEXT title, uint32_t width, uint32_t height);
	
	void show() const;
	void hide() const;
	bool is_running() const;
	void process_messages();

	HWND getHandle() const { return _hwnd; }
	uint32_t getWidth() const { return _width; }
	uint32_t getHeight() const { return _height; }

	Hwindow() : _hwnd(nullptr), 
			    _h_instance(nullptr), 
			    _class_name(nullptr), 
			    _title(nullptr), 
			    _width(0), 
			    _height(0), 
			    _running(false) {}

   ~Hwindow() {
		if (this->_hwnd != nullptr) DestroyWindow(this->_hwnd);
        
        if (this->_class_name != nullptr) UnregisterClass(this->_class_name, this->_h_instance);
	}

    Hwindow(const Hwindow&) = delete;
    Hwindow& operator=(const Hwindow&) = delete;

	Hwindow(Hwindow&& other) noexcept : _hwnd(other._hwnd),
										_h_instance(other._h_instance),
										_class_name(other._class_name),
										_title(other._title),
										_width(other._width),
										_height(other._height),
										_running(other._running) {
		other._hwnd = nullptr;
		other._h_instance = nullptr;
		other._class_name = nullptr;
		other._title = nullptr;
		other._width = 0;
		other._height = 0;
		other._running = false;
	}

	Hwindow& operator=(Hwindow&& other) noexcept {
		if (this != &other) return *this;
        
        this->_hwnd = other._hwnd;
        this->_h_instance = other._h_instance;
        this->_class_name = other._class_name;
        this->_title = other._title;
        this->_width = other._width;
        this->_height = other._height;
        this->_running = other._running;

        other._hwnd = nullptr;
        other._h_instance = nullptr;
        other._class_name = nullptr;
        other._title = nullptr;
        other._width = 0;
        other._height = 0;
        other._running = false;

        return *this;
	}
};

#endif // WINDOW_HPP