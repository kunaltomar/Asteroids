#include "Game.h"
#include "System.h"
#include "OrthoCamera.h"
#include "Background.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Explosion.h"
#include "Keyboard.h"
#include "Random.h"
#include "Maths.h"
#include "Bullet.h"
#include "Collision.h"
#include <dos.h>
#include <time.h>
#include <algorithm>
#include "Font.h"
#include <stdio.h>
#include "Graphics.h"
#include "D3dx9math.h"
#include "UFO.h"

Game::Game() :
	camera_(0),
	background_(0),
	player_(0),
	ufo_(0),
	collision_(0)
{
	camera_ = new OrthoCamera();
	camera_->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	camera_->SetFrustum(800.0f, 600.0f, -100.0f, 100.0f);
	background_ = new Background(800.0f, 600.0f);
	collision_ = new Collision();
	bCanFirebullet = true;
	startDelay = false;
	bClearexplosion = false;
	beginfiretm = 0.f;
	BulletPatern = 1;

}

Game::~Game()
{
	delete camera_;
	delete background_;
	delete player_;
	DeleteAllBullet();
	DeleteAllAsteroids();
	DeleteAllExplosions();
	delete collision_;
}

void Game::Update(System *system)
{
	UpdatePlayer(system);
	UpdateAsteroids(system);
	UpdateBullet(system);
	UpdateCollisions();
	UpdateBulletLife();
	UpdateExplosion(system);
	if (bClearexplosion == true)
	{
		ClearExplosion(explosionStartTime);
	}
	if (ufo_)
	{
		ufo_->Update(system);
	}
	BulletFireDelay();
}

void Game::RenderBackgroundOnly(Graphics *graphics)
{
	camera_->SetAsView(graphics);
	background_->Render(graphics);
	
}

void Game::RenderEverything(Graphics *graphics)
{
	camera_->SetAsView(graphics);
	background_->Render(graphics);
	if (ufo_)
	{
		ufo_->Render(graphics);
	}
	if (player_)
	{
		player_->Render(graphics);
	}

	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		(*asteroidIt)->Render(graphics);
	}

	for (BulletList::const_iterator bulletIt = bulletlist_.begin(),
		end = bulletlist_.end();
		bulletIt != end;
		++bulletIt) 
	{
		(*bulletIt)->Render(graphics);
	}

	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
		++explosionIt)
	{
		(*explosionIt)->Render(graphics);
	}
}

void Game::InitialiseLevel(int numAsteroids)
{
		
	DeleteAllAsteroids();
	DeleteAllExplosions();
	
	SpawnPlayer();
	SpawnAsteroids(numAsteroids);
	
	ufo_ = new UFO();
	ufo_->Reset();
	ufo_->EnableCollisions(collision_, 10.0f);
}

bool Game::IsLevelComplete() const
{
	return (asteroids_.empty() && explosions_.empty());
}

bool Game::IsGameOver() const
{
	return (player_ == 0 && explosions_.empty());
}

void Game::DoCollision(GameEntity *a, GameEntity *b)
{
	Ship *player = static_cast<Ship *>(a == player_ ? a : (b == player_ ? b : 0));
	Bullet *bullet = static_cast<Bullet *>(IsBullet(a) ? a : (IsBullet(b) ? b : 0));
	Asteroid *asteroid = static_cast<Asteroid *>(IsAsteroid(a) ? a : (IsAsteroid(b) ? b : 0));
	UFO *ufo = static_cast<UFO*>(a == ufo_ ? a : (b == ufo_ ? b : 0));

 	if (player && asteroid)
	{
 		AsteroidHit(asteroid);
		UpdatePlayerLife();
	}

	if (bullet && asteroid)
	{
		asteroid->DisableCollisions();
		AsteroidHit(asteroid);
		
		bullet->DisableCollisions();
		DeleteBullet(bullet);
	}
	if (player && ufo)
	{
		ufo->DisableCollisions();
		Powerup();
	}

}

void Game::SpawnPlayer()
{
	DeletePlayer();
	player_ = new Ship();
	player_->EnableCollisions(collision_, 10.0f);
	ufo_ = new UFO();
	WrapEntity(ufo_);
}

void Game::DeletePlayer()
{	
	delete player_;
	player_ = 0;
}

void Game::UpdatePlayerLife()
{
	if (player_)
	{
		player_->Life = player_->Life - 1;
		if (player_->Life < 1)
		{
			DeletePlayer();
		}
		else
		{
			SpawnExplosion(10);
			player_->Reset();
			bClearexplosion = true;
			explosionStartTime = clock();
			ClearExplosion(explosionStartTime);
		}
	}
}

void Game::UpdateBulletLife()
{
	BulletList::const_iterator bulletIt = bulletlist_.begin();
	while (bulletIt != bulletlist_.end()) {
		
		if (((clock() - (*bulletIt)->begintime) / CLOCKS_PER_SEC) > 1.5f) {
			delete *bulletIt;
			bulletIt = bulletlist_.erase(bulletIt);
		}
		else
			bulletIt++;
	}
}

void Game::UpdatePlayer(System *system)
{
	if (player_ == 0)
		return;

	Keyboard *keyboard = system->GetKeyboard();
	
	float acceleration = 0.0f;
	if (keyboard->IsKeyHeld(VK_UP) || keyboard->IsKeyHeld('W'))
	{
		acceleration = 1.0f;
	}
	else if (keyboard->IsKeyHeld(VK_DOWN) || keyboard->IsKeyHeld('S'))
	{
		acceleration = -1.0f;
	}

	float rotation = 0.0f;
	if (keyboard->IsKeyHeld(VK_RIGHT) || keyboard->IsKeyHeld('D'))
	{
		rotation = -1.0f;
	}
	else if (keyboard->IsKeyHeld(VK_LEFT) || keyboard->IsKeyHeld('A'))
	{
		rotation = 1.0f;
	}

	player_->SetControlInput(acceleration, rotation);
	player_->Update(system);
	WrapEntity(player_);

	if (keyboard->IsKeyPressed(VK_SPACE))
	{
				
		if (BulletPatern == 1)
		{
			if (bCanFirebullet)
			{
				beginfiretm = clock();
				D3DXVECTOR3 playerForward = player_->GetForwardVector();
				D3DXVECTOR3 bulletPosition = player_->GetPosition() + playerForward * 10.0f;
				SpawnBullet(bulletPosition, playerForward);
				startDelay = true;
				
			}
		}
		if (BulletPatern == 2)
		{
			if (bCanFirebullet)
			{
				beginfiretm = clock();
				D3DXVECTOR3 playerForward = player_->GetForwardVector();
				D3DXVECTOR3 bulletPosition = player_->GetPosition() + playerForward * 10.0f;
				SpawnBullet(bulletPosition, playerForward);

				D3DXMATRIX  rotation;
				D3DXMatrixRotationZ(&rotation, 45.0f);
				D3DXVec3TransformNormal(&playerForward, &playerForward, &rotation);
				SpawnBullet(bulletPosition, playerForward);

				D3DXMatrixRotationZ(&rotation, -90.0f);
				D3DXVec3TransformNormal(&playerForward, &playerForward, &rotation);
				SpawnBullet(bulletPosition, playerForward);
				startDelay = true;
			}
		}
		if (BulletPatern == 3)
		{
			if (bCanFirebullet)
			{
				beginfiretm = clock();
				D3DXMATRIX  rotation;
				D3DXVECTOR3 playerForward = player_->GetForwardVector();
				D3DXVECTOR3 bulletPosition = player_->GetPosition() + playerForward * 10.0f;
				SpawnBullet(bulletPosition, playerForward);

				for (int i = 0; i < 5; i++)
				{
					D3DXMatrixRotationZ(&rotation, 45.0f);
					D3DXVec3TransformNormal(&playerForward, &playerForward, &rotation);
					SpawnBullet(bulletPosition, playerForward);
				}
				startDelay = true;
			}
		}

	}
	if (keyboard->IsKeyPressed(VK_F1))
	{
		BulletPatern = 1;
	}
	if (keyboard->IsKeyPressed(VK_F2))
	{
		BulletPatern = 2;
	}
	if (keyboard->IsKeyPressed(VK_F3))
	{
		BulletPatern = 3;
	}
}

void Game::UpdateAsteroids(System *system)
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
	++asteroidIt)
	{
		(*asteroidIt)->Update(system);
		WrapEntity(*asteroidIt);
	}
}

void Game::UpdateExplosion(System *system)
{
	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
		++explosionIt)
	{
		(*explosionIt)->Update(system);
		WrapEntity(*explosionIt);
	}
}
void Game::UpdateBullet(System *system)
{
	for (BulletList::const_iterator bulletIt = bulletlist_.begin(),
		end = bulletlist_.end();
		bulletIt != end;
		++bulletIt)
	{
		(*bulletIt)->Update(system);
		WrapEntity(*bulletIt);
	}
}

bool Game::IsBullet(GameEntity * entity) const
{

	return (std::find(bulletlist_.begin(),
		bulletlist_.end(), entity) != bulletlist_.end());
}

void Game::WrapEntity(GameEntity *entity) const
{
	D3DXVECTOR3 entityPosition = entity->GetPosition();
	entityPosition.x = Maths::WrapModulo(entityPosition.x, -400.0f, 400.0f);
	entityPosition.y = Maths::WrapModulo(entityPosition.y, -300.0f, 300.0f);
	entity->SetPosition(entityPosition);
}


void Game::DeleteAllAsteroids()
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		(*asteroidIt)->DisableCollisions();
		delete (*asteroidIt);
	}

	asteroids_.clear();
}

void Game::DeleteAllExplosions()
{
	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
		++explosionIt)
	{
		delete (*explosionIt);
	}

	explosions_.clear();
}

void Game::DeleteAllBullet()
{
	for (BulletList::const_iterator bulletIt = bulletlist_.begin(),
		end = bulletlist_.end();
		bulletIt != end;
		++bulletIt)
	{
		(*bulletIt)->DisableCollisions();
		delete (*bulletIt);
	}

	bulletlist_.clear();
}

void Game::SpawnBullet(const D3DXVECTOR3 &position,
	const D3DXVECTOR3 &direction)
{

	Bullet* bullet_ = new Bullet(position, direction);
	bullet_->EnableCollisions(collision_, 3.0f);
	bullet_->begintime = clock();
	bulletlist_.push_back(bullet_);


}

void Game::BulletFireDelay()
{
	if (startDelay)
	{
		float firedelay = (clock() - beginfiretm) / CLOCKS_PER_SEC;
		if (firedelay > 1.5f)				//// Fire Delay  /////////////////
		{
			bCanFirebullet = true;

		}
		else
		{
			bCanFirebullet = false;
		}
		
	}

}

void Game::DeleteBullet(Bullet *bullettodelete)
{
	//bullettodelete->DisableCollisions();
	bulletlist_.remove(bullettodelete);
	delete bullettodelete;
}

void Game::SpawnAsteroids(int numAsteroids)
{
	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	for (int i = 0; i < numAsteroids; i++)
	{
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		D3DXVECTOR3 position = D3DXVECTOR3(x, y, 0.0f);
		SpawnAsteroidAt(position, 3);
	}
}
void Game::SpawnExplosion(int numAsteroids)
{
	float halfWidth = 10.0f * 0.8f;
	float halfHeight = 10.0f * 0.8f;
	for (int i = 0; i < numAsteroids; i++)
	{
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		D3DXVECTOR3 position = player_->GetPosition();
		SpawnExplosionAt(position, 1);
	}

}
void Game::SpawnExplosionAt(const D3DXVECTOR3 &position, int size)
{
	const float MAX_ASTEROID_SPEED = 1.0f;

	float ex_angle = Random::GetFloat(Maths::TWO_PI);
	D3DXMATRIX ex_randomRotation;
	D3DXMatrixRotationZ(&ex_randomRotation, ex_angle);
	D3DXVECTOR3 ex_velocity = D3DXVECTOR3(0.0f, Random::GetFloat(MAX_ASTEROID_SPEED), 0.0f);
	D3DXVec3TransformNormal(&ex_velocity, &ex_velocity, &ex_randomRotation);

	Explosion *explosion = new Explosion(position, ex_velocity, size);
	explosions_.push_back(explosion);
}

void Game::SpawnAsteroidAt(const D3DXVECTOR3 &position, int size)
{
	const float MAX_ASTEROID_SPEED = 1.0f;

	float angle = Random::GetFloat(Maths::TWO_PI);
	D3DXMATRIX randomRotation;
	D3DXMatrixRotationZ(&randomRotation, angle);
	D3DXVECTOR3 velocity = D3DXVECTOR3(0.0f, Random::GetFloat(MAX_ASTEROID_SPEED), 0.0f);
	D3DXVec3TransformNormal(&velocity, &velocity, &randomRotation);

	Asteroid *asteroid = new Asteroid(position, velocity, size);
	asteroid->EnableCollisions(collision_, size * 5.0f);
	asteroids_.push_back(asteroid);
}

bool Game::IsAsteroid(GameEntity *entity) const
{
	return (std::find(asteroids_.begin(),
		asteroids_.end(), entity) != asteroids_.end()); 
}

void Game::AsteroidHit(Asteroid *asteroid)
{
	int oldSize = asteroid->GetSize();
	if (oldSize > 1)
	{
		int smallerSize = oldSize -1;
		D3DXVECTOR3 position = asteroid->GetPosition();
		SpawnAsteroidAt(position, smallerSize);
		SpawnAsteroidAt(position, smallerSize);
	}
	//asteroid->DisableCollisions();
	DeleteAsteroid(asteroid);
}

void Game::DeleteAsteroid(Asteroid *asteroid)
{
	asteroids_.remove(asteroid);
	delete asteroid;
}

void Game::UpdateCollisions()
{
	collision_->DoCollisions(this);
}

void Game::ClearExplosion(float starttime)
{
	float delay = (clock() - starttime) / CLOCKS_PER_SEC;
	if (delay > 1.0f)
	{
		DeleteAllExplosions();
		bClearexplosion = false;
	}
}

void Game::Powerup()
{
	if (BulletPatern>0 && BulletPatern< 3)
		BulletPatern += 1;
	else
	{
		BulletPatern = 3;
	}
	delete ufo_;
	ufo_ = 0;
}



