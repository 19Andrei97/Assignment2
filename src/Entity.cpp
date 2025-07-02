#include "Entity.h"

Entity::Entity(const size_t id, const std::string& tag)
	: m_id(id)
	, m_tag(tag)
{

}

bool Entity::isActive() const
{
	return m_isActive;
}

const size_t Entity::id() const
{
	return m_id;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

void Entity::destroy() 
{
	m_isActive = false;
}