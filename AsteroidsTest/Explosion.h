#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#include "GameEntity.h"

class Explosion : public GameEntity
{
public:
	Explosion(const D3DXVECTOR3 &position,
		const D3DXVECTOR3 &velocity,
		int size);

	void Update(System *system);
	void Render(Graphics *graphics) const;

	D3DXVECTOR3 GetVelocity() const;
	int GetSize() const;
private:
	D3DXVECTOR3 velocity_;
	D3DXVECTOR3 axis_;
	float angle_;
	float angularSpeed_;
	int size_;
};

#endif // EXPLOSION_H_INCLUDED
