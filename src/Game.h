#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

struct WindowConfig { unsigned int width, height; int frameLimit, fullScreen; };
struct FontConfig	{ std::string fontFile; int size{ 24 }, r{ 0 }, g{ 0 }, b{ 0 }; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;		float S; };
struct EnemyConfig	{ int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;	float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;		float S; };

class Game
{
	sf::RenderWindow			m_window;
	EntityManager				m_entities;
	sf::Font					m_font;
	std::unique_ptr<sf::Text>	m_text;
	WindowConfig				m_windowConfig;
	FontConfig					m_fontConfig;
	PlayerConfig				m_playerConfig;
	EnemyConfig					m_enemyConfig;
	BulletConfig				m_bulletConfig;
	int							m_score = 0;
	int							m_currentFrame = 0;
	int							m_lastSpecialWeaponTime = 0;
	bool						m_specialOn = false;
	int							m_lastEnemySpawnTime = 0;
	bool						m_paused = false;	// Game is paused check
	bool						m_running = true;	// Game is running check

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);
	void setPaused();

	void sMovement();
	void sUserInput();
	void sLifeSpan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 & mousePos);
	void spawnSpecialWeapon();

public:

	Game(const std::string& config);

	void run();
	
};