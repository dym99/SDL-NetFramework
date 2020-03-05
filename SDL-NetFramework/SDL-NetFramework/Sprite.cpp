#include "Sprite.h"

Sprite::Sprite(Texture* _texture, const glm::vec2& _position, const glm::vec2& _dimensions)
{
	m_texture = _texture;
	m_pos = _position;
	m_dim = _dimensions;
	m_angle = 0.0f;
	m_flip = SDL_FLIP_NONE;
	m_zOrder = 0.0f;
	m_behaviours = std::vector<IBehaviour*>();
}

Sprite::~Sprite()
{
	m_texture = nullptr;
	m_pos = { 0,0 };
	m_dim = { 0,0 };
}

void Sprite::setTexture(Texture* _texture)
{
	m_texture = _texture;
}

Texture* Sprite::getTexture() const
{
	return m_texture;
}

void Sprite::setAngle(const float& _angle)
{
	m_angle = _angle;
}

float Sprite::getAngle() const
{
	return m_angle;
}

void Sprite::setPosition(const glm::vec2& _pos)
{
	m_pos = _pos;
}

glm::vec2 Sprite::getPosition() const
{
	return m_pos;
}

void Sprite::setDimensions(const glm::vec2& _dim)
{
	m_dim = _dim;
}

glm::vec2 Sprite::getDimensions() const
{
	return m_dim;
}

void Sprite::setFlip(const SDL_RendererFlip& _flip)
{
	m_flip = _flip;
}

SDL_RendererFlip Sprite::getFlip() const
{
	return m_flip;
}

void Sprite::setZOrder(const float &_zOrder)
{
	m_zOrder = _zOrder;
}

float Sprite::getZOrder() const
{
	return m_zOrder;
}

void Sprite::addBehaviour(IBehaviour* _behaviour)
{
	m_behaviours.push_back(_behaviour);
	_behaviour->setSprite(this);
}

void Sprite::init()
{
	for (size_t i = 0; i < m_behaviours.size(); ++i) {
		m_behaviours[i]->init();
	}
}

void Sprite::update()
{
	for (size_t i = 0; i < m_behaviours.size(); ++i) {
		m_behaviours[i]->update();
	}
}

void Sprite::render()
{
	m_texture->render({ static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_dim.x), static_cast<int>(m_dim.y) }, m_angle, { static_cast<int>(m_dim.x * 0.5f), static_cast<int>(m_dim.y * 0.5f) }, m_flip);
}
