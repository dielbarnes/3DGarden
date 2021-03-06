//
// LightInstancedVertexShader.hlsl
// Copyright � 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
// Texturing and Lighting in DirectX 11 (https://www.3dgep.com/texturing-lighting-directx-11)
// RasterTek Tutorial 37: Instancing (http://www.rastertek.com/dx11tut37.html)
//

// Constant buffers

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

// Input/output

struct IVS_INPUT
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	matrix worldMatrix : WORLDMATRIX;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

// Entry point

PS_INPUT IVS(IVS_INPUT input)
{
	PS_INPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// Calculate the position of the vertex in the world
	float4 worldPosition = mul(input.position, input.worldMatrix);
	output.position = worldPosition;

	// Calculate the position of the vertex against the view and projection matrices
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader
	output.texCoord = input.texCoord;

	// Calculate the normal vector against the world matrix only
	output.normal = mul(input.normal, (float3x3)input.worldMatrix);

	// Normalize the normal vector
	output.normal = normalize(output.normal);

	// Calculate the view direction
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// Normalize the view direction
	output.viewDirection = normalize(output.viewDirection);

	return output;
}
