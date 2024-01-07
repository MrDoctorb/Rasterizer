#pragma once

#include <SDL.h>
#include <stdexcept>

#include "Vec.h"
#include "Color.h"

class CanvasBase
{
public:
	CanvasBase(const char* window_title, size_t width, size_t height)
		: _width(width), _height(height)
	{
		//Tell SDL we want to do stuff with the screen
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			//If it fails, throw an error
			throw std::runtime_error(std::string("Could not initialize SDL2: ") + SDL_GetError());
		}

		//What is the window name, where to spawn it (x,y), width, height, how should the window start
		_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			static_cast<int>(width), static_cast<int>(height), SDL_WINDOW_SHOWN);

		//If there is no pointer we failed
		if (_window == nullptr)
		{
			throw std::runtime_error(std::string("Could not create window: ") + SDL_GetError());
		}

		//What window to use, index of the device (-1 means auto pick graphics card), 
		//the flags we need to be supported,
		_renderer = SDL_CreateRenderer(_window, -1, 0);

		if (_renderer == nullptr)
		{
			throw std::runtime_error(std::string("Could not create renderer: ") + SDL_GetError());
		}

	}

	//Here's how to remove constructors
	//Can't copy or assign to itself or the pointers
	CanvasBase(CanvasBase const&) = delete;
	CanvasBase operator = (CanvasBase const&) = delete;

	CanvasBase(CanvasBase const&&) = delete;
	CanvasBase operator = (CanvasBase const&&) = delete;


	virtual ~CanvasBase()
	{
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
	}

	//We are referencing the point and color instead of copying
	//const makes sure we don't change those guys
	void put_pixel(const Vec2i& pt, const Color& color) const
	{
		SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);

		//Doin math so that the center of the screen is 0,0
		SDL_RenderDrawPoint(_renderer,
			(static_cast<int>(_width) / 2) + pt.x,
			(static_cast<int>(_height) / 2) - pt.y);
	}

	//Clears out our buffer
	virtual void clear()
	{
		Color _color = Color::zane_brown;
		SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, 255);
		SDL_RenderClear(_renderer);
	}

	//moves the buffer to screen
	void present() const
	{
		SDL_RenderPresent(_renderer);
	}

protected:
	const size_t _width;
	const size_t _height;

private:
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;


};