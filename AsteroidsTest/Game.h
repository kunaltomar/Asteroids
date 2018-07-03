#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <d3dx9math.h>
#include <list>

class OrthoCamera;
class Background;
class Ship;
class Bullet;
class Asteroid;
class Explosion;
class Collision;
class System;
class Graphics;
class GameEntity;

class Font;

class Game
{
public:
	Game();
	~Game();

	void Update(System *system);
	void RenderBackgroundOnly(Graphics *graphics);
	void RenderEverything(Graphics *graphics);

	void InitialiseLevel(int numAsteroids);
	bool IsLevelComplete() const;
	bool IsGameOver() const;

	void DoCollision(GameEntity *a, GameEntity *b);
	void DeleteBullet(Bullet *bullettodelete);
private:
	Game(const Game &);
	void operator=(const Game &);

	typedef std::list<Asteroid *> AsteroidList;
	typedef std::list<Explosion *> ExplosionList;
	typedef std::list<Bullet *> BulletList;

	void SpawnPlayer();
	void DeletePlayer();
	void UpdatePlayerLife();

	void UpdateBulletLife();
	void UpdatePlayer(System *system);
	void UpdateAsteroids(System *system);
	void UpdateExplosion(System *system);
	void UpdateBullet(System *system);
	bool IsBullet(GameEntity *entity) const;
	void WrapEntity(GameEntity *entity) const;

	void DeleteAllAsteroids();
	void DeleteAllExplosions();
	void DeleteAllBullet();

	void SpawnBullet(const D3DXVECTOR3 &position,
		const D3DXVECTOR3 &direction);
	

	void SpawnExplosion(int numAsteroids);
	void SpawnExplosionAt(const D3DXVECTOR3 &position, int size);

	void SpawnAsteroids(int numAsteroids);
	void SpawnAsteroidAt(const D3DXVECTOR3 &position, int size);
	bool IsAsteroid(GameEntity *entity) const;
	void AsteroidHit(Asteroid *asteroid);
	void DeleteAsteroid(Asteroid *asteroid);

	void UpdateCollisions();

	float explosionStartTime;
	bool bClearexplosion ;
	void ClearExplosion(float starttime);
	void Powerup();

	void BulletFireDelay();

	OrthoCamera *camera_;
	bool bCanFirebullet;
	bool startDelay;
	
	float beginfiretm;
	int BulletPatern;

	Background *background_;
	Ship *player_;
	AsteroidList asteroids_;
	ExplosionList explosions_;
	BulletList bulletlist_;
	class UFO* ufo_;

	Collision *collision_;
};

#endif // GAME_H_INCLUDED
