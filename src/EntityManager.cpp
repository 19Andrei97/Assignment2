#include "EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::update()
{	
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	m_entitiesToAdd.clear();

	removeDeadEntities(m_entities);

	for (auto& [tag, entityVec] : m_entityMap) 
	{

		removeDeadEntities(entityVec);
	
	}
}

/// MANAGING ENTITIES //////////////////////////////////////////////////////////////

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const std::shared_ptr<Entity>& e) {
				return !e->m_isActive;
			}),
		vec.end()
	);
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(++m_totalEntities, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

/// GETTING ENTITIES //////////////////////////////////////////////////////////////

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	static EntityVec emptyVec; // restituito se il tag non esiste

	auto it = m_entityMap.find(tag);
	if (it != m_entityMap.end()) {
		return it->second;
	}
	return emptyVec;
}