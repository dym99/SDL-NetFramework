#include "Scene.h"

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

int spriteSortFunc(const void* a, const void* b) {
	Sprite* spriteA = (Sprite*)a;
	Sprite* spriteB = (Sprite*)b;
	return spriteA->getZOrder() > spriteB->getZOrder();
}

void Scene::render()
{
	for (size_t i = 0; i < m_sprites.size(); ++i) {
		qsort(&m_sprites[0], m_sprites.size(), sizeof(Sprite*), spriteSortFunc);
		m_sprites[i]->render();
	}
}

void Scene::addSprite(Sprite* _sprite)
{
	m_sprites.push_back(_sprite);
}
