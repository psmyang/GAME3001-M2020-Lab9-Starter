#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();

	if (m_bDebugMode)
	{
		Util::DrawLine(m_pPlayer->getTransform()->position, m_pPlaneSprite->getTransform()->position);
	
		Util::DrawRect(m_pPlayer->getTransform()->position - glm::vec2(m_pPlayer->getWidth() * 0.5f, m_pPlayer->getHeight() * 0.5f),
			m_pPlayer->getWidth(), m_pPlayer->getHeight());

		Util::DrawRect(m_pPlaneSprite->getTransform()->position - glm::vec2(m_pPlaneSprite->getWidth() * 0.5f, m_pPlaneSprite->getHeight() * 0.5f),
			m_pPlaneSprite->getWidth(), m_pPlaneSprite->getHeight());

		Util::DrawRect(m_pObstacle->getTransform()->position - glm::vec2(m_pObstacle->getWidth() * 0.5f, m_pObstacle->getHeight() * 0.5f),
			m_pObstacle->getWidth(), m_pObstacle->getHeight());
	}
}

void PlayScene::update()
{
	updateDisplayList();

	CollisionManager::LOSCheck(m_pPlayer, m_pPlaneSprite, m_pObstacle);

	CollisionManager::AABBCheck(m_pPlayer, m_pPlaneSprite);

	CollisionManager::AABBCheck(m_pPlayer, m_pObstacle);
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	// handle player movement with GameController
	if (SDL_NumJoysticks() > 0)
	{
		if (EventManager::Instance().getGameController(0) != nullptr)
		{
			const auto deadZone = 10000;
			if (EventManager::Instance().getGameController(0)->LEFT_STICK_X > deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
				m_playerFacingRight = true;

				m_pPlayer->getRigidBody()->velocity = glm::vec2(5.0f, 0.0f);
				m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
				m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
			}
			else if (EventManager::Instance().getGameController(0)->LEFT_STICK_X < -deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
				m_playerFacingRight = false;

				m_pPlayer->getRigidBody()->velocity = glm::vec2(-5.0f, 0.0f);
				m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
				m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
			}
			else
			{
				if (m_playerFacingRight)
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
				}
				else
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
				}
			}
		}
	}


	// handle player movement if no Game Controllers found
	if (SDL_NumJoysticks() < 1)
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
			m_playerFacingRight = false;

			m_pPlayer->getRigidBody()->velocity = glm::vec2(-5.0f, 0.0f);
			m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
			m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
			m_playerFacingRight = true;

			m_pPlayer->getRigidBody()->velocity = glm::vec2(5.0f, 0.0f);
			m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
			m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
		}
		else
		{
			if (m_playerFacingRight)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
			}
			else
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
			}
		}
	}
	
	//H KEY Section
	if (!m_bDebugKeys[H_KEY])
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_H))
		{
			//toggles Debug Mode
			m_bDebugMode = !m_bDebugMode;

			m_bDebugKeys[H_KEY] = true;

			if (m_bDebugMode)
			{
				std::cout << "DEBUG Mode on" << std::endl;
			}
			else
			{
				std::cout << "DEBUG Mode off" << std::endl;
			}
		}
	}

	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_H))
	{
		m_bDebugKeys[H_KEY] = false;
	}

	//K KEY Section
	if (!m_bDebugKeys[K_KEY])
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_K))
		{
			std::cout << "DEBUG: Enemies taking damage!" << std::endl;

			m_bDebugKeys[K_KEY] = true;

			
		}
	}

	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_K))
	{
		m_bDebugKeys[K_KEY] = false;
	}

	//P KEY Section
	if (!m_bDebugKeys[P_KEY])
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_P))
		{
			//toggles Debug Mode
			m_bPatrolMode = !m_bPatrolMode;

			m_bDebugKeys[P_KEY] = true;

			if (m_bPatrolMode)
			{
				std::cout << "DEBUG: Patro Mode on" << std::endl;
			}
			else
			{
				std::cout << "DEBUG: Patro Mode off" << std::endl;
			}
		}
	}

	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_P))
	{
		m_bDebugKeys[P_KEY] = false;
	}




	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance()->changeSceneState(END_SCENE);
	}
}

void PlayScene::start()
{
	m_bDebugMode = false;
	m_bPatrolMode = false;
	
	// Plane Sprite
	m_pPlaneSprite = new Plane();
	addChild(m_pPlaneSprite);

	// Player Sprite
	m_pPlayer = new Player();
	addChild(m_pPlayer);
	m_playerFacingRight = true;

	// Obstacle Texture
	m_pObstacle = new Obstacle();
	addChild(m_pObstacle);
	
}
