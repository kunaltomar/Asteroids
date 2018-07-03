#include "UFO.h"
#include "Graphics.h"
#include "Maths.h"
#include <algorithm>
#include "Font.h"
#include "Random.h"
#include "System.h"

UFO::UFO() :
	accelerationControl_(0.0f),
	rotationControl_(0.0f),
	debug_(0),
	velocity_(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	forward_(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
	rotation_(0.0f)
{

}

void UFO::SetControlInput(float acceleration,
	float rotation)
{
	//accelerationControl_ = acceleration;
	//rotationControl_ = rotation;
}

void UFO::Update(System *system)
{
	/*
	if (debug_ == nullptr)
	{
		debug_ = system->GetGraphics()->CreateXFont("Arial", 16);
	}
	const float RATE_OF_ROTATION = 0.1f;
	const float MAX_SPEED = 2.0f;
	const float VELOCITY_TWEEN = 0.05f;

	rotation_ = Maths::WrapModulo(rotation_ + rotationControl_ * RATE_OF_ROTATION,
		Maths::TWO_PI);

	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationZ(&rotationMatrix, rotation_);
	D3DXVec3TransformNormal(&forward_, &D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		&rotationMatrix);
	D3DXVec3Normalize(&forward_, &forward_);

	D3DXVECTOR3 idealVelocity;
	D3DXVec3Scale(&idealVelocity, &forward_, accelerationControl_ * MAX_SPEED);
	D3DXVec3Lerp(&velocity_, &velocity_, &idealVelocity, VELOCITY_TWEEN);

	D3DXVECTOR3 newPosition = GetPosition();
	D3DXVec3Add(&newPosition, &newPosition, &velocity_);
	SetPosition(newPosition);*/
}

void UFO::Render(Graphics *graphics) const
{
	struct DummyVert
	{
		float x, y, z;
		D3DCOLOR diffuse;
	};

	DummyVert axis[8] =
	{
		
		{ -15.0f, 0.0f, 0.0f, D3DCOLOR_RGBA(240,128,128,1) },{ 15.0f, 0.0f, 0.0f, D3DCOLOR_RGBA(240,128,128,1) },
		{ 0.0f, 10.0f, 0.0f, D3DCOLOR_RGBA(240,128,128,1) },{ -10.0f, 0.0f, 0.0f,D3DCOLOR_RGBA(240,128,128,1) },
		{ 0.0f, 10.0f, 0.0f, D3DCOLOR_RGBA(240,128,128,1) },{ 10.0f, 0.0f, 0.0f, D3DCOLOR_RGBA(240,128,128,1) },
	};

	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationZ(&rotationMatrix, rotation_);

	D3DXVECTOR3 position = GetPosition();
	D3DXMATRIX translationMatrix;
	D3DXMatrixTranslation(&translationMatrix,
		position.x,
		position.y,
		position.z);

	D3DXMATRIX shipTransform = rotationMatrix * translationMatrix;

	D3DXMATRIX identityMatrix;
	D3DXMatrixIdentity(&identityMatrix);

	DWORD dummyFvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	graphics->SetVertexFormat(dummyFvf);
	graphics->DisableLighting();
	graphics->SetModelMatrix(&shipTransform);
	graphics->DrawImmediate(D3DPT_LINELIST, 4, &axis[0], sizeof(axis[0]));
	graphics->SetModelMatrix(&identityMatrix);
	graphics->EnableLighting();

}

D3DXVECTOR3 UFO::GetForwardVector() const
{
	return forward_;
}

D3DXVECTOR3 UFO::GetVelocity() const
{
	return velocity_;
}

void UFO::Reset()
{
	accelerationControl_ = 0.0f;
	rotationControl_ = 0.0f;

	velocity_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	forward_ = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	rotation_ = 0.0f;

	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		D3DXVECTOR3 position = D3DXVECTOR3(x, y, 0.0f);
	SetPosition(position);
}
