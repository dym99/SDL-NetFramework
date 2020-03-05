#include "Scene.h"
#include "Debug.h"
#include <algorithm>
Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::update()
{
	for (size_t i = 0; i < m_sprites.size(); ++i) {
		m_sprites[i]->update();
	}
}

bool spriteSortFunc(Sprite* a, Sprite* b) {
	DEBUG_LOG("HALP %f, %f\n", a->getZOrder(), b->getZOrder());
	return (a->getZOrder() > b->getZOrder());
}

void Scene::render()
{
	std::sort(m_sprites.begin(), m_sprites.end(), spriteSortFunc);
	for (size_t i = 0; i < m_sprites.size(); ++i) {
		m_sprites[i]->render();
	}
}

void Scene::addSprite(Sprite* _sprite)
{
	m_sprites.push_back(_sprite);
}
