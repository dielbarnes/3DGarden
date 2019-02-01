//
// SkyDomePixelShader.hlsl
// Copyright © 2018 Diel Barnes. All rights reserved.
//

// Color

cbuffer ColorBuffer
{
	float4 apexColor;
	float4 centerColor;
};

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 domePosition : TEXCOORD0;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
	// Determine the position of the pixel on the sky dome
	float height = input.domePosition.y;

	// The value ranges from -1.0f to +1.0f so change it to only positive values
	if (height < 0.0f)
	{
		height = 0.0f;
	}

	// Determine the gradient color by interpolating between the apex and the center based on the height of the pixel
	return lerp(centerColor, apexColor, height);
}
