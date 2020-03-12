#pragma once

#include "Texture.h"
#include "IBehaviour.h"

#include <glm/glm.hpp>
#include <vector>

class Sprite
{
public:
	Sprite(Texture* m_texture, const glm::vec2& position, const glm::vec2& dimensions);
	~Sprite();

	void setTexture(Texture* _texture);
	Texture* getTexture() const;

	void setAngle(const float& _angle);
	float getAngle() const;

	void setPosition(const glm::vec2& _pos);
	glm::vec2 getPosition() const;

	void setDimensions(const glm::vec2& _dim);
	glm::vec2 getDimensions() const;

	void setFlip(const SDL_RendererFlip& _flip);
	SDL_RendererFlip getFlip() const;

	void setZOrder(const float& _zOrder);
	float getZOrder() const;

	void addBehaviour(IBehaviour* _behaviour);

	template<typename T>
	T* getBehaviour();

	void init();
	void update();
	void render();
private:
	Texture* m_texture;
	glm::vec2 m_pos, m_dim;
	float m_angle;
	SDL_RendererFlip m_flip;

	float m_zOrder;

	std::vector<IBehaviour*> m_behaviours;
};

template<typename T>
inline T* Sprite::getBehaviour()
{
	for (int i = 0; i < m_behaviours.size(); ++i) {
		T* temp;
		temp = dynamic_cast<T*>(m_behaviours[i]);
		return temp;
	}
}
