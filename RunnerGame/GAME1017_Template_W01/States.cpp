
#include "States.h"
#include "Engine.h"
#include "Button.h"
#include<iostream>
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "TextureManager.h"
#include "EventManager.h"
#include "CollisionManager.h"
#include "FontManager.h"
#include "DebugManager.h"
#include <ctime>
#include "Utilities.h"
#include "SoundManager.h"
#include <chrono>
#include <thread>
#define WIDTH 1024
#define HEIGHT 768
#define BGSCROLL 2 // Could these scroll/speed values be handled in the class? Yes. Consider it!
#define MGSCROLL 4
#define FGSCROLL 8
#define PSPEED 6
// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

// begin PauseState
PauseState::PauseState()
{
}

void PauseState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_R))
		//STMA::PopState(); 
		STMA::GetStates().pop_back();
	if (m_ResumeBtn->Update() == 1)
		return;
	if (m_ExitBtn->Update() == 1)
		return;

}

void PauseState::Render()
{
	std::cout << "Rendering Pause..." << std::endl;
	STMA::GetStates().front()->Render(); // invoke Render of GameState
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 255, 128);
	SDL_Rect rect = { 256, 128, 512, 512 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	m_ResumeBtn->Render();
	m_ExitBtn->Render();
	State::Render();
}

void PauseState::Enter()
{
	std::cout << "Entering Pause..." << std::endl;
	m_ResumeBtn = new ResumeButton({ 0,0,480,480 }, { 360,230,300,300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("resume"));
	m_ExitBtn = new ExitButton({ 0,0,480,480 }, { 360,430,300,300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("exit"));

}

void PauseState::Exit()
{
	std::cout << "Exiting Pause..." << std::endl;
}
// end PauseState

// begin gamestate
GameState::GameState() :m_iESpawn(0), m_iESpawnMax(60), m_pivot({ 0,0 })
{
	m_bEBNull = m_bENull = m_bPBNull = false; // Setting all to false.
	Engine::Instance().getCanShoot() = true;
}

void GameState::Enter()
{
	int width = 1024, height = 768;
	std::cout << "Entering GameState..." << std::endl;
	srand((unsigned)time(NULL));

	/////m_pBGText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/background.png");
	/////m_pSprText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/sprites.png");
	m_pBGText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Backgrounds.png");
	m_pSprText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Player.png");
	m_pObsText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Obstacles.png");

	bgArray[0] = Sprite({ 0,0,1024,768 }, { 0, 0, 1024, 768 });
	bgArray[1] = Sprite({ 0,0,1024,768 }, { 1024, 0, 1024, 768 });

	mgArray[0] = Sprite({ 1024,0,256,512 }, { 0, 0, 256, 512 });
	mgArray[1] = Sprite({ 1024,0,256,512 }, { 256, 0, 256, 512 });
	mgArray[2] = Sprite({ 1024,0,256,512 }, { 512, 0, 256, 512 });
	mgArray[3] = Sprite({ 1024,0,256,512 }, { 768, 0, 256, 512 });
	mgArray[4] = Sprite({ 1024,0,256,512 }, { 1024, 0, 256, 512 });

	fgArray[0] = Sprite({ 1024,512,512,256 }, { 0, 512, 512, 256 });
	fgArray[1] = Sprite({ 1024,512,512,256 }, { 512, 512, 512, 256 });
	fgArray[2] = Sprite({ 1024,512,512,256 }, { 1024, 512, 512, 256 });

	/////m_player = new Player({ 0,0,94,100 }, { 256,384 - 50,94,100 });
	m_player = new Player({ 0,0,128,128 }, { 256,384,128,128 });

	m_pMusic = Mix_LoadMUS("Aud/game.mp3"); // Load the music track.
	Mix_PlayMusic(m_pMusic, -1);
	m_vSounds.reserve(3); // Optional but good practice.
	m_vSounds.push_back(Mix_LoadWAV("Aud/enemy.wav"));
	m_vSounds.push_back(Mix_LoadWAV("Aud/laser.wav"));
	m_vSounds.push_back(Mix_LoadWAV("Aud/explode.wav"));

	// Elapsed time initialization.
	SDL_Color white = { 255, 255, 255, 0 };
	m_pTimeLabel = new Label("UI", 20, 20, "Elapsed time: 0", white);
	start = time(NULL);
	jumping = false;
	Engine::Instance().setJumpTime(-1);
}
void GameState::Update()
{
	int timeDif;
	std::chrono::high_resolution_clock Clock;

	if (EVMA::KeyPressed(SDL_SCANCODE_P))
		STMA::PushState(new PauseState());
	if (EVMA::KeyPressed(SDL_SCANCODE_X))
		STMA::PushState(new EndState());


	if (m_loseGame && m_endGame)
	{
		std::chrono::milliseconds dura(1500);
		std::this_thread::sleep_for(dura);
		Mix_HaltMusic();
		STMA::PushState(new EndState());
	}

	if (m_winGame)
	{
		Mix_HaltMusic();
		STMA::PushState(new WinState());
	}

	// Scroll the backgrounds. Check if they need to snap back.
	for (int i = 0; i < 2; i++)
		bgArray[i].GetDstP()->x -= BGSCROLL;
	if (bgArray[1].GetDstP()->x <= 0)
	{
		bgArray[0].GetDstP()->x = 0;
		bgArray[1].GetDstP()->x = 1024;
	}

	for (int i = 0; i < 5; i++)
		mgArray[i].GetDstP()->x -= MGSCROLL;
	if (mgArray[1].GetDstP()->x <= 0)
	{
		mgArray[0].GetDstP()->x = 0;
		mgArray[1].GetDstP()->x = 256;
		mgArray[2].GetDstP()->x = 512;
		mgArray[3].GetDstP()->x = 768;
		mgArray[4].GetDstP()->x = 1024;
	}

	for (int i = 0; i < 3; i++)
		fgArray[i].GetDstP()->x -= FGSCROLL;
	if (fgArray[1].GetDstP()->x <= 0)
	{
		fgArray[0].GetDstP()->x = 0;
		fgArray[1].GetDstP()->x = 512;
		fgArray[2].GetDstP()->x = 1024;
	}


	if (jumping) {
		timeDif = time(NULL) - (Engine::Instance().getJumpTime());
		if (timeDif >= 2) {
			m_player->GetDstP()->y += 128;
			jumping = false;
			Engine::Instance().setJumpTime(-1);
			Engine::Instance().getCanShoot() = true;
			cout << "Player y = " + to_string(m_player->GetDstP()->y) << endl;
		}
	}
	else {
		// Player animation/movement.
		m_player->Animate(); // Oh! We're telling the player to animate itself. This is good! Hint hint.
	}
	//m_player->Animate();
	
	if (EVMA::KeyHeld(SDL_SCANCODE_A) && m_player->GetDstP()->x > m_player->GetDstP()->h)
		m_player->GetDstP()->x -= PSPEED;

	if (EVMA::KeyHeld(SDL_SCANCODE_D) && m_player->GetDstP()->x < WIDTH / 2)
		m_player->GetDstP()->x += PSPEED;

	if (EVMA::KeyHeld(SDL_SCANCODE_S)) 
	{
		if (!Engine::Instance().Rolling()) {
			Engine::Instance().Rolling() = true;
			rollFrames = 0;

		}
		else {
			rollFrames++;
			if (rollFrames > 3) {
				rollFrames = 0;
			}
		}
		m_player->SetSrcP({ 0 + (128 * rollFrames),128,128,128 });
		m_player->SetMaxSp(4);
		m_player->SetMaxFr(4);
		cout << "Rolling..." << endl;
	}


	if (EVMA::KeyPressed(SDL_SCANCODE_SPACE) && Engine::Instance().getCanShoot()) {
		/////
		/*
		Engine::Instance().getCanShoot() = false;
		//SOMA::PlaySound("laser", 0, 0);
		m_vPBullets.push_back(new Bullet({ 376,0,10,100 }, { m_player->GetDstP()->x + 85,m_player->GetDstP()->y + 42,10,100 }, 30));
		*/
		Engine::Instance().getCanShoot() = false;
		m_player->GetDstP()->y -= 128;
		jumping = true;
		Engine::Instance().setJumpTime(time(NULL));
		m_player->SetSrcP({ 1024,0,128,128 });
		cout << "Player y = " + to_string(m_player->GetDstP()->y) << endl;
		Mix_PlayChannel(-1, m_vSounds[1], 0);
	}

	if (!m_loseGame) {
		EnemyUpdate();
		CheckCollision();
	}

}

void GameState::CheckCollision()
{
	int player_w, player_h;
	int enemy_w, enemy_h;
	bool collided = false;

	if (Engine::Instance().Rolling()) {
		player_w = 64;
		player_h = 64;
	}
	else {
		player_w = 64;
		player_h = 128;
	}

	/////SDL_Rect p = { m_player->GetDstP()->x - 100, m_player->GetDstP()->y, 100, 94 };
	SDL_Rect p = { m_player->GetDstP()->x + 32, m_player->GetDstP()->y, player_w, player_h };
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		/////SDL_Rect e = { m_vEnemies[i]->GetDstP()->x, m_vEnemies[i]->GetDstP()->y - 40, 56, 40 };
		if (m_vEnemies[i]->GetDstP()->h == 448) {
			enemy_w = m_vEnemies[i]->GetDstP()->h;
			enemy_h = m_vEnemies[i]->GetDstP()->w;
		}
		else {
			enemy_w = m_vEnemies[i]->GetDstP()->w;
			enemy_h = m_vEnemies[i]->GetDstP()->h;
		}

		if (m_vEnemies[i]->GetDstP()->h == 448) {
			if (!Engine::Instance().Rolling()) {
				if ((m_player->GetDstP()->x >= m_vEnemies[i]->GetDstP()->x - 448 && m_player->GetDstP()->x <= m_vEnemies[i]->GetDstP()->x + 0) ||
					(m_player->GetDstP()->x + 128 >= m_vEnemies[i]->GetDstP()->x - 448 && m_player->GetDstP()->x + 128 <= m_vEnemies[i]->GetDstP()->x + 0)) {
					collided = true;
				}
			}
		}
		else {
			SDL_Rect e = { m_vEnemies[i]->GetDstP()->x - 128, m_vEnemies[i]->GetDstP()->y, enemy_w, enemy_h };
			if (SDL_HasIntersection(&p, &e)) {
				collided = true;
			}
		}

		if (collided)
		{
			cout << "Player x0 = " + to_string(m_player->GetDstP()->x) << endl;
			cout << "Player x1 = " + to_string(m_player->GetDstP()->x + player_w) << endl;
			cout << "Player y0 = " + to_string(m_player->GetDstP()->y) << endl;
			cout << "Player y1 = " + to_string(m_player->GetDstP()->y + player_h) << endl;
			cout << "Enemy x0 = " + to_string(m_vEnemies[i]->GetDstP()->x) << endl;
			cout << "Enemy x1 = " + to_string(m_vEnemies[i]->GetDstP()->x + enemy_w) << endl;
			cout << "Enemy y0 = " + to_string(m_vEnemies[i]->GetDstP()->y) << endl;
			cout << "Enemy y1 = " + to_string(m_vEnemies[i]->GetDstP()->y + enemy_h) << endl;
			// Game over!
			cout << "Player goes boom!" << endl;
			//SOMA::PlaySound("explode", 0, 1);
			Mix_PlayChannel(-1, m_vSounds[2], 0);

			m_loseGame = true;
			m_player->SetMaxFr(5);
			m_player->SetMaxSp(5);
			break;
		}
	}
	// Player bullets vs. Enemies.
	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		SDL_Rect b = { m_vPBullets[i]->GetDstP()->x - 100, m_vPBullets[i]->GetDstP()->y, 100, 10 };
		for (int j = 0; j < (int)m_vEnemies.size(); j++)
		{
			if (m_vEnemies[j] == nullptr) continue;
			/////SDL_Rect e = { m_vEnemies[j]->GetDstP()->x, m_vEnemies[j]->GetDstP()->y - 40, 56, 40 };
			SDL_Rect e = { m_vEnemies[j]->GetDstP()->x, m_vEnemies[j]->GetDstP()->y - 128, 128, 128 };
			if (SDL_HasIntersection(&b, &e))
			{
				//SOMA::PlaySound("explode", 0, 1);
				Mix_PlayChannel(-1, m_vSounds[2], 0);
				delete m_vEnemies[j];
				m_vEnemies[j] = nullptr;
				delete m_vPBullets[i];
				m_vPBullets[i] = nullptr;
				m_bENull = true;
				m_bPBNull = true;
				score++;
				cout << "Score: " << score << endl;
				if (score == 5)
				{
					m_winGame = true;
				}
				break;
			}
		}
	}
	if (m_bENull) CleanVector<Enemy*>(m_vEnemies, m_bENull);
	if (m_bPBNull) CleanVector<Bullet*>(m_vPBullets, m_bPBNull);
	// Enemy bullets vs. player.
	for (int i = 0; i < (int)m_vEBullets.size(); i++)
	{
		if (SDL_HasIntersection(&p, m_vEBullets[i]->GetDstP()))
		{
			// Game over!
			cout << "Player goes boom!" << endl;

			Mix_PlayChannel(-1, m_vSounds[2], 0);
			delete m_vEBullets[i];
			m_vEBullets[i] = nullptr;
			CleanVector<Bullet*>(m_vEBullets, m_bEBNull);
			m_loseGame = true;
			m_player->SetMaxSp(5);
			m_player->SetMaxFr(5);

			break;
		}
	}
}


void GameState::Render()
{

	//m_pBackground->Render();

		// Enemy animation/movement.
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		m_vEnemies[i]->Update(); // Oh, again! We're telling the enemies to update themselves. Good good!
		/////if (m_vEnemies[i]->GetDstP()->x < -56)
		if (m_vEnemies[i]->GetDstP()->x < -128)
		{
			delete m_vEnemies[i];
			m_vEnemies[i] = nullptr;
			m_bENull = true;
		}
	}
	if (m_bENull) CleanVector<Enemy*>(m_vEnemies, m_bENull); // Better to have a logic check (if) than a function call all the time!

	// Render stuff. Background first.
	for (int i = 0; i < 2; i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, bgArray[i].GetSrcP(), bgArray[i].GetDstP());

	for (int i = 0; i < 5; i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, mgArray[i].GetSrcP(), mgArray[i].GetDstP());

	for (int i = 0; i < 3; i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGText, fgArray[i].GetSrcP(), fgArray[i].GetDstP());

	// Player.
	/*if (jumping) {
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pSprText, &jumpRect, m_player->GetDstP(), m_player->GetAngle(), &m_pivot, SDL_FLIP_NONE);
	}
	else {*/
	if (m_loseGame) {
		loseFrames++;
		if (loseFrames > 4) {
			m_endGame = true;
			m_player->SetSrcP({ 1024,128,128,128 });
		}
		else {
			m_player->SetSrcP({ 512 + (128 * loseFrames),128,128,128 });
			std::chrono::milliseconds dura(200);
			std::this_thread::sleep_for(dura);
		}
	}

	else {
		if (!Engine::Instance().Rolling() && rollFrames > -1) {
			m_player->SetSrcP({ 0,0,128,128 });
			m_player->SetMaxSp(8);
			m_player->SetMaxFr(8);
			rollFrames = -1;
		}
	}
	if (!m_loseGame && m_player->GetSrcP()->x > 128 * 7) {
		cout << "Player frame x =  " + to_string(m_player->GetSrcP()->x) << endl;
		/*
		m_player->SetSrcP({ 0,0,128,128 });
		m_player->SetMaxSp(8);
		m_player->SetMaxFr(8);
		*/
	}
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pSprText, m_player->GetSrcP(), m_player->GetDstP(), m_player->GetAngle(), &m_pivot, SDL_FLIP_NONE);
	/////}


	// Player bullets.	
	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pSprText, m_vPBullets[i]->GetSrcP(), m_vPBullets[i]->GetDstP(), 90, &m_pivot, SDL_FLIP_NONE);
		//m_vPBullets[i]->Render();

	}
	// Enemies.
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		/////SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pSprText, m_vEnemies[i]->GetSrcP(), m_vEnemies[i]->GetDstP(), -90, &m_pivot, SDL_FLIP_NONE);
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pObsText, m_vEnemies[i]->GetSrcP(), m_vEnemies[i]->GetDstP(), 90, &m_pivot, SDL_FLIP_NONE);
		//m_vEnemies[i]->Render();

	}
	// Enemy bullets.
	for (int i = 0; i < (int)m_vEBullets.size(); i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pSprText, m_vEBullets[i]->GetSrcP(), m_vEBullets[i]->GetDstP());
	//m_vEBullets[i]->Render();

/////m_pTimeLabel->SetText("Elapsed time: " + to_string(start));
	m_pTimeLabel->Render();
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();
	SDL_RenderPresent(Engine::Instance().GetRenderer());
	m_pTimeLabel->SetText(("Elapsed time: " + to_string(time(NULL) - start) + "").c_str());
}

void GameState::EnemyUpdate()
{
	int enemy_x, enemy_y;
	int enemy_w, enemy_h;
	int enemy_pos_y;

	int rnd = rand() % 2;

	// Update enemy spawns.
	if (m_iESpawn++ == m_iESpawnMax)
	{
		if (rnd == 0) {
			enemy_x = 128;
			enemy_y = 128;
			enemy_w = 128;
			enemy_h = 128;
			enemy_pos_y = 384;

		}
		else {
			enemy_x = 0;
			enemy_y = 0;
			enemy_w = 128;
			enemy_h = 448;
			enemy_pos_y = 320;
		}
		/////m_vEnemies.push_back(new Enemy({ 0,100,40,56 }, { WIDTH,56 + rand() % (HEIGHT - 114),40,56 }, &m_vEBullets, m_vSounds[0], 30 + rand() % 91)); // Randomizing enemy bullet spawn to every 30-120 frames.
		m_vEnemies.push_back(new Enemy({ enemy_x,enemy_y,enemy_w,enemy_h }, { WIDTH + enemy_h,enemy_pos_y,enemy_w,enemy_h }, &m_vEBullets, m_vSounds[0], 30 + rand() % 91)); // Randomizing enemy bullet spawn to every 30-120 frames.
		m_iESpawn = 0;
		m_iESpawnMax = (160 * (rnd + 1)) + rand() % 481;
		cout << "Spawn Max = " + to_string(m_iESpawnMax) << endl;
	}
	// Update the bullets. Player's first.
	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		m_vPBullets[i]->Update();
		if (m_vPBullets[i]->GetDstP()->x > WIDTH)
		{
			delete m_vPBullets[i];
			m_vPBullets[i] = nullptr;
			m_bPBNull = true;
		}
	}
	if (m_bPBNull) CleanVector<Bullet*>(m_vPBullets, m_bPBNull);

	// Now enemy bullets. Is Update() getting a little long?
	for (int i = 0; i < (int)m_vEBullets.size(); i++)
	{
		m_vEBullets[i]->Update();
		if (m_vEBullets[i]->GetDstP()->x < -10)
		{
			delete m_vEBullets[i];
			m_vEBullets[i] = nullptr;
			m_bEBNull = true;
		}
	}
	if (m_bEBNull) CleanVector<Bullet*>(m_vEBullets, m_bEBNull);
}


void GameState::Exit()
{
	delete m_player;
	m_player = nullptr;
	for (int i = 0; i < (int)m_vSounds.size(); i++)
		Mix_FreeChunk(m_vSounds[i]);
	m_vSounds.clear();
	//Mix_FreeMusic(m_pMusic);
	//Mix_CloseAudio();
}

void GameState::Resume() {}
// end title state

// begin title state
TitleState::TitleState() {}

void TitleState::Enter()
{
	SDL_Color white = { 0, 255, 255, 0 };
	m_pStartLabel = new Label("UI", 320, 40, "RUNNER GAME", white);
	m_playBtn = new PlayButton({ 0,0,480,480 }, { 360,230,300,300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("start"));
	m_exitBtn = new ExitButton({ 0,0,480,480 }, { 360,430,300,300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("exit"));
	m_pGameStart = new Sprite({ 0,0, 1024, 768 }, { 0,0,1024, 768 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("StartScene"));
	SOMA::Load("Aud/SpaceSprinkles.mp3", "SpaceStart", SOUND_MUSIC);
	SOMA::PlayMusic("SpaceStart", -1, 3000);

}
void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
	if (m_exitBtn->Update() == 1)
		return;


}

void TitleState::Render()
{

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 115, 90, 90);
	SDL_RenderClear(Engine::Instance().GetRenderer());


	m_pGameStart->Render();
	m_pStartLabel->Render();

	m_playBtn->Render();
	m_exitBtn->Render();
	State::Render();

}


void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
}

GameState& GameState::Instance()
{
	static GameState instance;
	return instance;
}
// end title state


// begin end-state
EndState::EndState() {}

void EndState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_N))
		STMA::ChangeState(new TitleState());
	if (m_MenuBtn->Update() == 1)
		return;
}

void EndState::Render()
{
	// Tile-specific rendering. Write to back buffer
	if (!rendered) {
		std::cout << "Rendering EndState..." << std::endl;
		rendered = true;
	}
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	m_pGameover->Render();
	m_pEndLabel->Render();
	m_MenuBtn->Render();
	State::Render(); // RenderPresent
}

void EndState::Enter()
{
	std::cout << "Enter EndState" << std::endl;

	SDL_Color red = { 255, 0, 0, 255 };
	m_MenuBtn = new MenuButton({ 0,0,480,480 }, { 360, 250,300,300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("menu"));
	m_pGameover = new Sprite({ 0,0,1024,768 }, { 0,0,1024,768 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("EndScene"));
	m_pEndLabel = new Label("UI", 210, 40, "YOU ARE DEFEATED", red);

	SOMA::Load("Aud/space.mp3", "SpaceEnd", SOUND_MUSIC);
	SOMA::PlayMusic("SpaceEnd", -1, 3000);

}

void EndState::Exit()
{
	std::cout << "Exiting EndState..." << std::endl;
}
// End end-state


// Begin win-state
WinState::WinState() {}

void WinState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_N))
		STMA::ChangeState(new TitleState());
	if (m_MenuBtn->Update() == 1)
		return;
}

void WinState::Render()
{
	std::cout << "Rendering WinState..." << std::endl;
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	m_pWin->Render();
	m_pWinLabel->Render();
	m_MenuBtn->Render();
	State::Render(); // RenderPresent
}

void WinState::Enter()
{
	std::cout << "Enter WinState" << std::endl;

	SDL_Color yellow = { 255, 255, 0, 0 };
	m_MenuBtn = new MenuButton({ 0,0,480,480 }, { 360, 250,300,300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("menu"));
	m_pWin = new Sprite({ 0,0,1024,768 }, { 0,0,1024,768 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("WinScene"));
	m_pWinLabel = new Label("UI", 226, 40, "YOU ARE WINNER", yellow);

	SOMA::Load("Aud/crazy-space.wav", "Winner", SOUND_MUSIC);
	SOMA::PlayMusic("Winner", -1, 3000);
}

void WinState::Exit()
{
	std::cout << "Exiting WinState..." << std::endl;
}
