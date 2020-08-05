#include <iostream>
#include "Button.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "StateManager.h"
#include <SDL.h>

Button::Button(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t)
	: Sprite(src, dst, r, t), m_state(STATE_UP) {}

bool Button::MouseCollision()
{
	const int mx = EVMA::GetMousePos().x;
	const int my = EVMA::GetMousePos().y;
	return (mx < (m_dst.x + m_dst.w) && mx > m_dst.x &&
		    my < (m_dst.y + m_dst.h) && my > m_dst.y);
}

int Button::Update()
{
	bool col = MouseCollision();
	switch (m_state)
	{
	case STATE_UP:
	/*	if (col)   
			m_state = STATE_OVER;*/
		if (col && EVMA::MousePressed(1)) // 1 = left, 2 = middle, 3 = right
			Execute();
		break;
	case STATE_OVER:
		if (!col)
			m_state = STATE_UP;
		if (col && EVMA::MousePressed(1)) // 1 = left, 2 = middle, 3 = right
			m_state = STATE_DOWN;
		break;
	case STATE_DOWN:
		if (EVMA::MouseReleased(1))
		{
			if (col)
			{
				m_state = STATE_OVER;
				// Execute new "callback".
				Execute();
				return 1;
			}
			else 
				m_state = STATE_UP;
		}
		break;
	}
	return 0;
}

void Button::Render()
{
	m_src.x = m_src.w * (int)m_state;
	SDL_RenderCopy(m_pRend, m_pText, &m_src, &m_dst);
}
// Create button subclasses and their overridden Execute methods below...
PlayButton::PlayButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t) :Button(src, dst, r, t) {}
void PlayButton::Execute()
{
	STMA::ChangeState(new GameState);
}

ResumeButton::ResumeButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t) :Button(src, dst, r, t) {}
void ResumeButton::Execute()
{
	STMA::GetStates().pop_back();
}

ExitButton::ExitButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t) :Button(src, dst, r, t) {}
void ExitButton::Execute()
{
	Engine::Instance().Running() = false;
}

MenuButton::MenuButton(SDL_Rect src, SDL_Rect dst, SDL_Renderer* r, SDL_Texture* t) :Button(src, dst, r, t) {}
void MenuButton::Execute()
{
	STMA::ChangeState(new TitleState);
}
