//#include "Enemy.h"
////#include "Engine.h"
//#include "CollisionManager.h" // Includes MAMA.
//#include "SoundManager.h"
//#include "States.h"
//#include "Engine.h"
//Enemy::Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, 
//	         int ss, int ns, int nf, SDL_FRect* target)
//	:AnimatedSprite(s, d, r, t, ss, ns, nf), m_pTarget(target)
//{ 
//	m_dx = m_dy = m_accel = m_vel = 0.0;
//	m_velMax = 5.0;
//	m_rotMax = 2.5;
//}
//
//void Enemy::Update()
//{
//	switch (m_state)
//	{
//		case idle:
//			Stop();
//			break;
//		case seeking:
//		{ // {} Needed because of local var.
//			double destAngle = MAMA::AngleBetweenPoints((m_pTarget->y + m_pTarget->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f),
//				(m_pTarget->x + m_pTarget->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f));
//			Move2Stop(destAngle);
//			Move2Full(destAngle);
//			SOMA::PlaySound("engine", 0, 0);
//			break;
//		}
//		case fleeing:
//		{
//			double destAngle = MAMA::AngleBetweenPoints((GetDstP()->y + GetDstP()->h / 2.0f) - (m_pTarget->y + m_pTarget->h / 2.0f),
//				(GetDstP()->x + GetDstP()->w / 2.0f) - (m_pTarget->x + m_pTarget->w / 2.0f));
//			Move2Full(destAngle);
//			SOMA::PlaySound("flee", 0, 1);
//			break;
//		}
//		case arrive:
//		{
//			SOMA::PlaySound("arrive", 0, 2);
//			if (MAMA::Distance((GetDstP()->x + GetDstP()->w / 2.0f), (m_pTarget->x + m_pTarget->w / 2.0f),
//				(GetDstP()->y + GetDstP()->h / 2.0f), (m_pTarget->y + m_pTarget->h / 2.0f)) <= 2.0)
//			{
//				Stop();
//				break;
//			}
//			else
//			{
//				double destAngle = MAMA::AngleBetweenPoints((m_pTarget->y + m_pTarget->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f),
//					(m_pTarget->x + m_pTarget->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f));
//				Move2Stop(destAngle);
//			}
//		}
//		case avoidance:
//		{
//			double destAngle = MAMA::AngleBetweenPoints((m_pTarget->y + m_pTarget->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f),
//				(m_pTarget->x + m_pTarget->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f));
//			Move2Full(destAngle);
//			SOMA::PlaySound("avoid", 0, 3);
//			break;
//		}
//	}
//	// Move the enemy.
//	GetDstP()->x += (float)m_dx;
//	GetDstP()->y += (float)m_dy;
//	if (m_state != idle)
//		Animate();
//	m_src.x = m_src.w * m_sprite;
//}
//
//void Enemy::Render()
//{
//	Sprite::Render();
//}
//
//void Enemy::SetState(int s)
//{ 
//	m_state = static_cast<state>(s); 
//	if (m_state != idle)
//		Start();
//}
//
//void Enemy::Start()
//{
//	m_sprite = 0;
//	m_accel = 0.2;
//
//}
//
//void Enemy::Stop()
//{
//	m_dx = m_dy = 0.0;
//	m_vel = 0;
//	m_frame = 0;
//	m_frameMax = 4;
//	m_sprite = 4;
//	SOMA::StopSound(2);
//}
//
//void Enemy::SetVs(const double angle)
//{
//	double destAngle = MAMA::Rad2Deg(angle) + 90;
//	m_angle += std::min(std::max(MAMA::Angle180(destAngle - m_angle), -m_rotMax), m_rotMax); // Only rotate slightly towards the destination angle.
//	// Now use the new slight rotation to generate dx and dy as normal.
//	m_vel += m_accel;
//	m_vel = std::min(m_vel, m_velMax);
//}
//
//void Enemy::Move2Full(double& angle)
//{
//	double perc = 0.0;
//	for (unsigned i = 0; i < Engine::Instance().GetObs().size(); i++)
//	{
//		Sprite* temp = Engine::Instance().GetObs()[i];
//		double dist = MAMA::Distance((GetDstP()->x + GetDstP()->w / 2.0f), (temp->GetDstP()->x + temp->GetDstP()->w / 2.0f),
//			(GetDstP()->y + GetDstP()->h / 2.0f), (temp->GetDstP()->y + temp->GetDstP()->h/2.0f));
//		perc = 1.0 - std::max(std::min((dist - 50.0) / (125.0 - 50.0), 1.0), 0.0);
//		double fleeAngle = MAMA::AngleBetweenPoints((GetDstP()->y + GetDstP()->h / 2.0f) - (temp->GetDstP()->y + temp->GetDstP()->h / 2.0f),
//			(GetDstP()->x + GetDstP()->w / 2.0f) - (temp->GetDstP()->x + temp->GetDstP()->w / 2.0f));
//		if (perc > 0.0)
//			angle = MAMA::LerpRad(angle, fleeAngle, perc);
//	}
//	SetVs(angle);
//	MAMA::SetDeltas(MAMA::Deg2Rad(m_angle - 90), m_dx, m_dy, m_vel, m_vel); // Try this: m_vel*(1-perc) 
//}
//
//void Enemy::Move2Stop(const double angle)
//{
//	SetVs(angle);
//	double dist = MAMA::Distance((GetDstP()->x + GetDstP()->w / 2.0f), (m_pTarget->x + m_pTarget->w / 2.0f),
//		(GetDstP()->y + GetDstP()->h / 2.0f), (m_pTarget->y + m_pTarget->h / 2.0f));
//	double perc = std::max(std::min(((dist - 2.0) / (200.0 - 2.0)), 1.0), 0.1);
//
//	m_frameMax = 12 - (int)(8.0 * perc);
//
//	MAMA::SetDeltas(MAMA::Deg2Rad(m_angle - 90), m_dx, m_dy, m_vel*perc, m_vel*perc);
//}
