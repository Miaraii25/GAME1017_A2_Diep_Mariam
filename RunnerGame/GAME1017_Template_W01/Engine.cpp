#include "Engine.h"
#include "CollisionManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "StateManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include <iostream>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
using namespace std;

Engine::Engine():m_running(false){ cout << "Engine class constructed!" << endl; }

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game..." << endl;
	// initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			//m_pBGText = IMG_LoadTexture(m_pRenderer, "Img/background.png");
			//m_pSprText = IMG_LoadTexture(m_pRenderer, "Img/sprites.png");
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				EVMA::Init();
				//SOMA::Init();
				TEMA::Init();
				if (Mix_Init(MIX_INIT_MP3) != 0) // Mixer init success.
				{
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048); // Good for most games.
					Mix_AllocateChannels(16);

				}
				else return false;
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	// Example specific initialization.
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // Call this before any textures are created.

	//FOMA::RegisterFont("Img/LTYPE.TTF", "UI", 44);
	FOMA::RegisterFont("Font/Consolas.TTF", "UI", 64);
	TEMA::RegisterTexture("Img/PlayerShip.png", "player");
	TEMA::RegisterTexture("Img/Enemies.png", "enemy");
	//TEMA::RegisterTexture("Img/gameover.png", "gameover");
	TEMA::RegisterTexture("Img/bgStart.png", "StartScene");
	TEMA::RegisterTexture("Img/bgEnd.png", "EndScene");
	TEMA::RegisterTexture("Img/bgWin.png", "WinScene");
	//TEMA::RegisterTexture("Img/background.png", "background");
	//TEMA::RegisterTexture("Img/sprites.png", "sprites");

	// Final engine initialization calls.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	TEMA::RegisterTexture("Img/menu.png", "menu");
	TEMA::RegisterTexture("Img/resume.png", "resume");
	TEMA::RegisterTexture("Img/start.png", "start");
	TEMA::RegisterTexture("Img/exit.png", "exit");
	STMA::ChangeState(new TitleState);

	m_running = true; // Everything is okay, start the engine.
	cout << "Engine Init success!" << endl;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	EVMA::HandleEvents();
}

void Engine::Update()
{
	STMA::Update();
}

void Engine::Render() 
{
	SDL_SetRenderDrawColor(m_pRenderer, 230, 230, 100, 100);
	SDL_RenderClear(m_pRenderer); // Clear the screen with the draw color.
	// Draw anew.
	STMA::Render();
	SDL_RenderPresent(m_pRenderer);
}

void Engine::Clean()
{
	cout << "Cleaning game." << endl;
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	DEMA::Quit();
	EVMA::Quit();
	FOMA::Quit();
	SOMA::Quit();
	STMA::Quit();
	TEMA::Quit();
}

int Engine::Run()
{
	if (m_running) 
		return -1; 
	if (Init("GAME1017 Engine Template", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (m_running) // Main engine loop.
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_running)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; 
	return instance;
}

SDL_Renderer* Engine::GetRenderer() { return m_pRenderer; }
bool& Engine::Running() { return m_running; }

bool& Engine::getCanShoot()
{
	return m_bCanShoot;
}

bool& Engine::Rolling() { return m_rolling; }


//std::vector<Sprite*>& Engine::GetObs() { return m_obstacles; }

time_t& Engine::getJumpTime()
{
	return jumpTime;
}

void Engine::setJumpTime(time_t p_jumpTime)
{
	jumpTime = p_jumpTime;
}
