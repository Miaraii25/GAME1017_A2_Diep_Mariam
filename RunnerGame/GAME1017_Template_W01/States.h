#pragma once
#ifndef _STATES_H_
#define _STATES_H_

#include <SDL.h>
#include "Button.h"
#include <vector>
//#include "Enemy.h"
#include "Label.h"
#include "Sprite.h"

#include "SDL_image.h"
#include "SDL_mixer.h"
#include <time.h>

using namespace std;

class State // This is the abstract base class for all specific states.
{
protected:
	State() {}
public:
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();
};

// Pause while playing
class PauseState : public State
{
private:
	Button* m_ResumeBtn;
	Button* m_ExitBtn;
public:
	PauseState();
	void Update();
	void Render();
	void Enter();
	void Exit();
};

// Let's play game
class GameState : public State
{
private:
	Sprite bgArray[2];
	Sprite mgArray[5];
	Sprite fgArray[3];

	SDL_Texture* m_pBGText; // For the bg.
	SDL_Texture* m_pMGText; // For the fg.
	SDL_Texture* m_pFGText; // For the fg.

	SDL_Texture* m_pSprText; // For the player.

	SDL_Texture* m_pObsText; // For the obstacles.

	Label* m_pTimeLabel;
	time_t  start;
	bool jumping;
	const SDL_Rect jumpRect = { 1024,0,128,128 };
	int loseFrames = -1;
	int rollFrames = -1;

	Sprite* m_pTarget;
	Enemy* m_pEnemy;
	Sprite* m_pBackground;
	Player* m_player;


	Mix_Music* m_pMusic;
	vector<Mix_Chunk*> m_vSounds;

	bool m_bENull, // These three flags check if we need to clear the respective vectors of nullptrs.
		m_bPBNull,
		m_bEBNull; //


	int m_iESpawn, // The enemy spawn frame timer properties.
		m_iESpawnMax;
	std::vector<Enemy*> m_vEnemies;
	std::vector<Bullet*> m_vPBullets;
	std::vector<Bullet*> m_vEBullets;
	SDL_Point m_pivot;

	int score = 0;

public:
	GameState();
	void Update();
	void CheckCollision();
	void Render();
	void EnemyUpdate();
	void Enter();
	void Exit();
	void Resume();
public:
	static GameState& Instance();
	bool m_loseGame = false;
	bool m_winGame = false;
	bool m_endGame = false;
};

// Start the game
class TitleState : public State
{
public:
	TitleState();
	void Update();
	void Render();
	void Enter();
	void Exit();
private:
	Button* m_playBtn;
	Button* m_exitBtn;
	Sprite* m_pGameStart;

	Label* m_pStartLabel;
};

// You are defeated
class EndState : public State
{
private:
	Button* m_MenuBtn;
	Sprite* m_pGameover;
	Label* m_pEndLabel;

	bool rendered = false;
public:
	EndState();
	void Update();
	void Render();
	void Enter();
	void Exit();
};

// You are winner
class WinState : public State
{
private:
	Button* m_MenuBtn;
	Sprite* m_pWin;
	Label* m_pWinLabel;
public:
	WinState();
	void Update();
	void Render();
	void Enter();
	void Exit();
};
#endif