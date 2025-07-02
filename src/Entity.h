#pragma once

#include "Components.h"
#include <memory>
#include <string>

class Entity
{
	friend class EntityManager;

	bool		m_isActive = true;
	size_t		m_id = 0;
	std::string m_tag = "default";

	// Constructor and Destructor
	Entity(const size_t id, const std::string& tag = "default");

public:

	// Conponent Pointers
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape>		cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput>		cInput;
	std::shared_ptr<CScore>		cScore;
	std::shared_ptr<CLifespan>	cLifespan;

	// Private memeber access functions
	bool isActive() const;
	const size_t id() const;
	const std::string& tag() const;
	void destroy();

};
