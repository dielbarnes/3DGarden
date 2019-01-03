//
// Camera.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "Camera.h"

#pragma region Init

Camera::Camera(const XMFLOAT3 position, const float fAspectRatio)
{
	// Set the initial camera position
	m_position = position;
	m_rotation = XMFLOAT3(0, 0, 0);

	// Initialize the projection matrix
	m_mProjection = XMMatrixPerspectiveFovLH( // Build a left-handed perspective projection matrix
						//XM_PIDIV4,		// Pi/4 (45 degrees) field of view angle Y (90 degrees is top-down view)
						XM_PI/2.5,		
						fAspectRatio,	// Aspect ratio of view-space X:Y
						0.1f,			// Near Z (distance to the near clipping plane); must be greater than 0
						1000.0f);		// Far Z (distance to the far clipping plane); must be greater than 0
}

Camera::~Camera()
{
}

#pragma endregion

#pragma region Getters

XMFLOAT3 Camera::GetPosition()
{
	return m_position;
}

XMMATRIX Camera::GetViewMatrix()
{
	return m_mView;
}

XMMATRIX Camera::GetProjectionMatrix()
{
	return m_mProjection;
}

#pragma endregion

#pragma region Move/Rotate

void Camera::MoveForward(const float& fT, const float& fValue)
{
	XMVECTOR temp = XMLoadFloat3(&m_position) + XMMatrixInverse(nullptr, m_mView).r[2] * fT * fValue;
	XMStoreFloat3(&m_position, temp);
}

void Camera::Strafe(const float& fT, const float& fValue)
{
	XMVECTOR temp = XMLoadFloat3(&m_position) + XMMatrixInverse(nullptr, m_mView).r[0] * fT * fValue;
	XMStoreFloat3(&m_position, temp);
}

void Camera::Rotate(const float fDeltaX, const float fDeltaY)
{
	m_rotation.x += fDeltaY;
	m_rotation.y += fDeltaX;
}

#pragma endregion

#pragma region Update

void Camera::Update()
{
	XMVECTOR vPosition = XMLoadFloat3(&m_position); // Position of the camera in the world
	XMVECTOR vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // Where the camera is looking by default
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // The vector that points upwards

	// Apply rotation

	float pitch = m_rotation.x * XM_PI/180; // X axis
	float yaw = m_rotation.y * XM_PI/180; // Y axis
	float roll = m_rotation.z * XM_PI/180; // Z axis
	XMMATRIX mRotation = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	vLook = XMVector3TransformCoord(vLook, mRotation);
	vUp = XMVector3TransformCoord(vUp, mRotation);

	// Apply translation
	vLook = XMVectorAdd(vPosition, vLook);

	// Build view matrix for a left-handed coordinate system
	m_mView = XMMatrixLookAtLH(vPosition, vLook, vUp);
}

#pragma endregion
