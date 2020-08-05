#pragma once
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Sprite.h"
#include <SDL.h>


// Button will be an example of the command design pattern.
class Button : public Sprite
{
public:
	int Update();
	void Render();
	SDL_FRect getDst();
protected: // Private but inherited!
	Button(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t);
	SDL_FRect pos;
	enum state { STATE_UP, STATE_OVER, STATE_DOWN } m_state;
	bool MouseCollision();
	virtual void Execute() = 0; // Pure virtual method, meaning Button is now an abstract class (interface)!
};

// List button subclasses here...
class PlayButton : public Button
{
private:
	void Execute();
public:
	PlayButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t);
};

class ResumeButton : public Button
{
private:
	void Execute();
public:
	ResumeButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t);
};


class MenuButton : public Button
{
private:
	void Execute();
public:
	MenuButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t);
};

class ExitButton : public Button
{
private:
	void Execute();
public:
	ExitButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t);
};
#endif