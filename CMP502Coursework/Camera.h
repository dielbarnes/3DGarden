//
// Camera.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <directxmath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(const XMFLOAT3 position, const float fAspectRatio);
	~Camera();

	XMFLOAT3 GetPosition();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	void MoveForward(const float& fT, const float& fValue);
	void Strafe(const float& fT, const float& fValue);
	void Rotate(const float fDeltaX, const float fDeltaY);

	void Update();

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_mView;
	XMMATRIX m_mProjection;
};

#endif
