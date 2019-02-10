//
// SkyPlaneVertexShader.hlsl
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 11: Bitmap Clouds (http://www.rastertek.com/tertut11.html)
//

// Constant buffer

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Input/output

struct VS_INPUT
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

// Entry point

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix).xyww;

	// Store the texture coordinates for the pixel shader
	output.texCoord = input.texCoord;

	return output;
}
