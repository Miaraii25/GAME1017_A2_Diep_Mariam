//#pragma once
//#ifndef _ENEMY_H_
//#define _ENEMY_H_
//
//#include "Sprite.h"
//
//enum state { idle, seeking, fleeing, arrive, avoidance };
//
//class Enemy : public AnimatedSprite
//{
//public:
//	Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, 
//		  int ss, int ns, int nf, SDL_FRect* target = nullptr);
//	void Update();
//	void Render();
//	state GetState() { return m_state; }
//	void SetState(int s);
//	void Start();
//private:
//	double m_dx, m_dy,
//		m_accel,
//		m_vel,
//		m_velMax,
//		m_rotMax;
//	state m_state;
//	SDL_FRect* m_pTarget;
//private:
//	void Stop();
//	void SetVs(const double angle);
//	void Move2Full(double& angle);
//	void Move2Stop(const double angle);
//};
//
//#endif