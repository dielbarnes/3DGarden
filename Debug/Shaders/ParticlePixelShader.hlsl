//
// ParticlePixelShader.hlsl
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 39: Particle Systems (http://www.rastertek.com/dx11tut39.html)
//

Texture2D shaderTexture;
SamplerState samplerState;

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate
	float4 textureColor = shaderTexture.Sample(samplerState, input.texCoord);

	// Multiply the texture color and the particle color to get the final pixel color
	return textureColor * input.color;
}
