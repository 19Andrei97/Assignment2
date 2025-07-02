#include "Game.h"
#include "Random.h"
#include <iostream>
#include <fstream>

// GAME FLOW ////////////////////////////////////////////////////////////

Game::Game(const std::string & config)
{
	init(config);
}

void Game::init(const std::string & path)
{
	std::ifstream fin(path);

	std::string label;

	// Window
	fin >> label >> m_windowConfig.width >> m_windowConfig.height >> m_windowConfig.frameLimit >> m_windowConfig.fullScreen;

	// Font
	fin >> label >> m_fontConfig.fontFile >> m_fontConfig.size
		>> m_fontConfig.r >> m_fontConfig.g >> m_fontConfig.b;
		
	// Player
	fin >> label >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
		>> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT
		>> m_playerConfig.V;

	// Enemy
	fin >> label >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB
		>> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;

	// Bullet
	fin >> label >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
		>> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT
		>> m_bulletConfig.V >> m_bulletConfig.L;

	// set up window configuration

	sf::State state;

	if (m_windowConfig.fullScreen == 1)
		state = sf::State::Fullscreen;
	else
		state = sf::State::Windowed;

	m_window.create(sf::VideoMode({m_windowConfig.width, m_windowConfig.height }), "Assignment 2", state);
	m_window.setFramerateLimit(m_windowConfig.frameLimit);


	// TEXT AND FONT
	if (!m_font.openFromFile(m_fontConfig.fontFile)) {
		std::cerr << "Could not load font!\n";
	}

	m_text = std::make_unique<sf::Text>(m_font);
	m_text->setFont(m_font);
	m_text->setCharacterSize(m_fontConfig.size);
	m_text->setFillColor(sf::Color(m_fontConfig.r, m_fontConfig.g, m_fontConfig.b));
	m_text->setPosition({ 20.f, 20.f });

	spawnPlayer();
}

void Game::run()
{

	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifeSpan();
		}

		if (m_specialOn)
		{
			spawnSpecialWeapon();
		}

		sUserInput();
		sRender();

		++m_currentFrame;
	}

}

void Game::setPaused()
{
	m_paused = !m_paused;

	m_player->cInput->up = false;
	m_player->cInput->down = false;
	m_player->cInput->left = false;
	m_player->cInput->right = false;
}

// SPAWNS ////////////////////////////////////////////////////////////

void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");

	float mx{ m_window.getSize().x / 2.f };
	float my{ m_window.getSize().y / 2.f };

	entity -> cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);

	entity -> cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
	 
	entity -> cInput = std::make_shared<CInput>();

	entity -> cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	m_player = entity;
}

void Game::spawnEnemy()
{
	auto entity = m_entities.addEntity("enemy");

	int speed = Random::get(static_cast<int>(m_enemyConfig.SMIN), static_cast<int>(m_enemyConfig.SMAX));
	int vertices = Random::get(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

	int ex = Random::get((0 + m_enemyConfig.SR), (m_window.getSize().x - m_enemyConfig.SR));
	int ey = Random::get((0 + m_enemyConfig.SR), (m_window.getSize().y - m_enemyConfig.SR));

	int r = Random::get(0, 150);
	int g = Random::get(0, 150);
	int b = Random::get(0, 150);

	int direction = Random::get(0, 360);

	float radians = direction * (3.14159265f / 180.f);
	float dx = std::cos(radians) * speed;
	float dy = std::sin(radians) * speed;

	entity -> cTransform = std::make_shared<CTransform>(
		Vec2(static_cast<float>(ex), static_cast<float>(ey)),
		Vec2(dx, dy),
		static_cast<float>(direction)
	);

	entity -> cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, sf::Color(r, g, b), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
	
	entity -> cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	
	entity ->cScore = std::make_shared<CScore>(100);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	size_t direction_radius{ 360 / e->cShape->circle.getPointCount() };
	int direction_temp{0};

	for (size_t i = 0; i < e->cShape->circle.getPointCount() ; i++)
	{
		auto child = m_entities.addEntity("small_enemy");

		int speed = Random::get(static_cast<int>(m_enemyConfig.SMIN), static_cast<int>(m_enemyConfig.SMAX));
		
		direction_temp += direction_radius;

		float radians = direction_temp * (3.14159265f / 180.f);
		float dx = std::cos(radians) * speed;
		float dy = std::sin(radians) * speed;

		child->cTransform = std::make_shared<CTransform>(
			e->cTransform->pos,
			Vec2(dx, dy),
			static_cast<float>(direction_temp)
		);

		child->cShape = std::make_shared<CShape>(
			m_enemyConfig.SR / 2, 
			e->cShape->circle.getPointCount(), 
			e->cShape->circle.getFillColor(),
			e->cShape->circle.getOutlineColor(),
			m_enemyConfig.OT
		);

		child->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR / 2);

		child->cLifespan = std::make_shared<CLifespan>(150);

		child->cScore = std::make_shared<CScore>(e->cScore->score * 2);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	if (target.x < 0 || target.y < 0 ||
		target.x > m_window.getSize().x ||
		target.y > m_window.getSize().y)
	{
		return;
	}

	auto entity_bullet = m_entities.addEntity("bullet");

	Vec2 direction = target - entity->cTransform->pos;
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	Vec2 normalized = direction / length;
	Vec2 velocity = normalized * m_bulletConfig.S;

	entity_bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);

	entity_bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

	entity_bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	entity_bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon()
{
	if (m_specialOn && m_currentFrame % 30 == 0)
	{
		if (m_entities.getEntities("enemy").size() > 0)
		{
			const auto e = m_entities.getEntities("enemy")[0];
			e->destroy();

			m_score += e->cScore->score;
		}
		else if (m_entities.getEntities("small_enemy").size() > 0)
		{
			const auto e = m_entities.getEntities("small_enemy")[0];
			e->destroy();

			m_score += e->cScore->score;
		}
		else
		{
			m_paused = false;
			m_specialOn = false;

			m_lastSpecialWeaponTime = m_currentFrame;
		}

		return;
	}

	if (m_currentFrame - m_lastSpecialWeaponTime > 300)
	{
		m_paused = true;
		m_specialOn = true;
	}
	else
		std::cout << "Frames left: " << m_currentFrame - m_lastSpecialWeaponTime << "\n";
}

// SYSTEMS ////////////////////////////////////////////////////////////

void Game::sMovement()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e->tag() == "player")
		{

			// UP AND DOWN
			if (m_player->cInput->up && (0.0 + m_playerConfig.SR) < m_player->cTransform->pos.y)
			{
				m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
			}
			else if (m_player->cInput->down && (m_window.getSize().y - m_playerConfig.SR) > m_player->cTransform->pos.y)
			{
				m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
			}

			// RIGHT AND LEFT
			if (m_player->cInput->right && (m_window.getSize().x - m_playerConfig.SR) > m_player->cTransform->pos.x)
			{
				m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
			}
			else if (m_player->cInput->left && (0.0 + m_playerConfig.SR) < m_player->cTransform->pos.x)
			{
				m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
			}

		}
		else if (e->tag() == "enemy" || e->tag() == "small_enemy")
		{
			if (e->cTransform->pos.y < (0.0 + m_enemyConfig.SR))
			{
				e->cTransform->velocity.y *= -1.f;
			} else if (e->cTransform->pos.y > (m_window.getSize().y - m_enemyConfig.SR))
			{
				e->cTransform->velocity.y *= -1.f;
			}

			if (e->cTransform->pos.x < (0.0 + m_enemyConfig.SR))
			{
				e->cTransform->velocity.x *= -1.f;
			}
			else if (e->cTransform->pos.x > (m_window.getSize().x - m_enemyConfig.SR))
			{
				e->cTransform->velocity.x *= -1.f;
			}

			e->cTransform->pos.x += e->cTransform->velocity.x;
			e->cTransform->pos.y += e->cTransform->velocity.y;
		}
		else if (e->tag() == "bullet")
		{
			e->cTransform->pos.x += e->cTransform->velocity.x;
			e->cTransform->pos.y += e->cTransform->velocity.y;
		}

	}

}

void Game::sLifeSpan()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan)
		{
			e->cLifespan->remaining -= 3;

			if (e->cLifespan->remaining <= 0)
			{
				e->destroy();
				continue;
			}

			float ratio = e->cLifespan->remaining / static_cast<float>(e->cLifespan->total);

			e->cShape->circle.setFillColor(sf::Color(
				e->cShape->circle.getFillColor().r, 
				e->cShape->circle.getFillColor().g, 
				e->cShape->circle.getFillColor().b, 
				ratio * 255
			));
			e->cShape->circle.setOutlineColor(sf::Color(
				e->cShape->circle.getOutlineColor().r, 
				e->cShape->circle.getOutlineColor().g, 
				e->cShape->circle.getOutlineColor().b, 
				ratio * 255
			));
		}
	}
}

void Game::sCollision()
{
	for (auto& e : m_entities.getEntities())
	{
		if (!e->cScore)
			continue;

		for (auto& c : m_entities.getEntities())
		{
			if (c->cScore)
				continue;

			float dist{ e->cTransform->pos.dist(c->cTransform->pos) };
			float rSum{ e->cCollision->radius + c->cCollision->radius };

			if (dist < rSum)
			{
				if (c->tag() == "bullet")
				{
					if (e->tag() == "enemy")
						spawnSmallEnemies(e);

					c->destroy();
					e->destroy();

					m_score += e->cScore->score;
				}
				else if (c->tag() == "player")
				{
					if (m_score - 500 < 0)
						m_score = 0;
					else
						m_score -= 500;

					e->destroy();

					// SET PLAYER TO CENTER
					float mx{ m_window.getSize().x / 2.f };
					float my{ m_window.getSize().y / 2.f };

					c->cTransform->pos = { mx , my };
				}
			}
		}
	}
}

void Game::sEnemySpawner()
{
	if(m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
		spawnEnemy();
}

void Game::sRender()
{
	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		e->cShape->circle.setPosition(sf::Vector2f(e->cTransform->pos.x, e->cTransform->pos.y));

		if (!m_paused)
		{
			e->cTransform->angle += 1.f;
			e->cShape->circle.setRotation(sf::degrees(e->cTransform->angle));
		}

		m_window.draw(e->cShape->circle);
	}

	if (m_paused)
	{
		m_text->setString("PAUSED");
		m_window.draw(*m_text);
	}
	else
	{
		m_text->setString("Points: " + std::to_string(m_score));
		m_window.draw(*m_text);
	}

	m_window.display();
}

void Game::sUserInput()
{
	// TODO handle user input here MOUSE

	while (const std::optional event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			m_running = false;
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{

			
			if (keyPressed->code == sf::Keyboard::Key::P)
			{
				setPaused();
			}

			if (!m_paused)
			{
				switch (keyPressed->code)
				{
				case sf::Keyboard::Key::W:
					m_player->cInput->up = true;
					break;
				case sf::Keyboard::Key::S:
					m_player->cInput->down = true;
					break;
				case sf::Keyboard::Key::A:
					m_player->cInput->left = true;
					break;
				case sf::Keyboard::Key::D:
					m_player->cInput->right = true;
					break;

				default: break;
				}
			}
		}

		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			if (!m_paused)
			{
				switch (keyReleased->code)
				{
				case sf::Keyboard::Key::W:
					m_player->cInput->up = false;
					break;
				case sf::Keyboard::Key::S:
					m_player->cInput->down = false;
					break;
				case sf::Keyboard::Key::A:
					m_player->cInput->left = false;
					break;
				case sf::Keyboard::Key::D:
					m_player->cInput->right = false;
					break;

				default: break;
				}
			}
		}

		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
		{
			if (!m_paused)
			{
				switch (mousePressed->button)
				{
				case sf::Mouse::Button::Left:
					spawnBullet(m_player, Vec2(static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y)));
					break;

				case sf::Mouse::Button::Right:
					spawnSpecialWeapon();
					break;

				default: break;
				}
			}
		}
	}

}