//
// InstancedVertexShader.hlsl
// Copyright © 2018 Diel Barnes. All rights reserved.
//

// Constant buffer

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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
};

// Entry point

PS_INPUT IVS(IVS_INPUT input)
{
	PS_INPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices
	output.position = mul(input.position, input.worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader
	output.texCoord = input.texCoord;

	// Calculate the normal vector against the world matrix only
	output.normal = mul(input.normal, (float3x3)input.worldMatrix);

	// Normalize the normal vector
	output.normal = normalize(output.normal);

	return output;
}
