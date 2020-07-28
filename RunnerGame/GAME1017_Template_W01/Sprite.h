#pragma once
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "SDL.h"
#include <vector>
#include "SDL_mixer.h"
#include "Engine.h"
#include <algorithm> // For min/max.
#include <cmath>	 // For cos/sin.

class Sprite // Inline class.
{
public: // Inherited and public.
	Sprite() {};
	Sprite(SDL_Rect s, SDL_Rect d) :m_src(s), m_dst(d) {}
	Sprite(SDL_Rect s, SDL_Rect d, SDL_Renderer* r, SDL_Texture* t)
		:m_src(s), m_dst(d), m_pRend(r), m_pText(t), m_angle(0.0) {}
	virtual void Render() {	SDL_RenderCopyEx(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, SDL_FLIP_NONE); }
	SDL_Rect* GetSrcP() { return &m_src; }
	SDL_Rect* GetDstP() { return &m_dst; }
	double& GetAngle() { return m_angle; }
	void SetAngle(double a) { m_angle = a; }
	void SetSrcP(SDL_Rect s) { m_src = s; }
protected: // Private BUT inherited.
	double m_angle;
	SDL_Rect m_src;
	SDL_Rect m_dst;
	SDL_Renderer* m_pRend;
	SDL_Texture* m_pText;
private: // Private NOT inherited.
};

class AnimatedSprite : public Sprite// Also inline.
{
public:
	AnimatedSprite(SDL_Rect s, SDL_Rect d, /*SDL_Renderer* r, SDL_Texture* t,*/ int a, int ns, int nf)
		:Sprite(s, d/*, r, t*/), m_spriteMax(ns), m_frameMax(nf) 
	{
		m_angle = a;
	}
	void Animate()
	{
		m_frame++;
		if (m_frame == m_frameMax)
		{
			m_frame = 0;
			m_sprite++;
			if (m_sprite == m_spriteMax)
				m_sprite = 0;
		}
		m_src.x = m_src.w * m_frame;
	}
	void SetMaxSp(int ns) { m_spriteMax = ns; }
	void SetMaxFr(int nf) { m_frameMax = nf; }
protected:
	int m_sprite,		// Which sprite to start on for animation.
		m_spriteMax,	// How many sprites total.
		m_frame = 0,	// Frame counter.
		m_frameMax;		// Number of frames per sprite.

};

class Player : public AnimatedSprite
{
public:
	/////Player(SDL_Rect s, SDL_Rect d) :AnimatedSprite(s, d, 90, 8, 4) {};
	Player(SDL_Rect s, SDL_Rect d) :AnimatedSprite(s, d, 0 ,8,8 ) {};
	// Add more members later.
};

class Bullet : public Sprite
{
private:
	int m_speed;
	bool m_active;
public:
	Bullet(SDL_Rect s, SDL_Rect d, int spd) :Sprite(s, d), m_speed(spd), m_active(true) {}
	void Update()
	{
		m_dst.x += m_speed;
	}
};


class Enemy : public AnimatedSprite
{
private:
	int m_bulletTimer,
		m_timerMax; // Basically fire rate of bullets.
	std::vector<Bullet*>* m_pBulletVec; // Pointer to the enemy bullet vector from Engine.
	Mix_Chunk* m_pPew;
public:
	/////Enemy(SDL_Rect s, SDL_Rect d, std::vector<Bullet*>* bVec, Mix_Chunk* c, int fr = 120) : AnimatedSprite(s, d, -90, 4, 4 ), m_pBulletVec(bVec), m_pPew(c), m_bulletTimer(0), m_timerMax(fr) {}; // Default function params go in prototypes.
	Enemy(SDL_Rect s, SDL_Rect d, std::vector<Bullet*>* bVec, Mix_Chunk* c, int fr = 120) : AnimatedSprite(s, d, 0, 1, 1), m_pBulletVec(bVec), m_pPew(c), m_bulletTimer(0), m_timerMax(fr) {}; // Default function params go in prototypes.
	void Update()
	{
		/////Animate();
		m_dst.x -= 3;
		/////
		/*
		if (m_bulletTimer++ == m_timerMax)
		{
			m_bulletTimer = 0;
			m_pBulletVec->push_back(new Bullet({ 160,100,14,14 }, { m_dst.x,m_dst.y - 28,14,14 }, -10));
			Mix_PlayChannel(-1, m_pPew, 0);
		}
		*/
	}
	// Add more members later.
};

#endif