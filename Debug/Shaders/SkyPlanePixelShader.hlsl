//
// SkyPlanePixelShader.hlsl
// Copyright © 2018 Diel Barnes. All rights reserved.
//

Texture2D shaderTexture1;
Texture2D shaderTexture2;
SamplerState samplerState;

// Constant buffer

cbuffer CloudsBuffer
{
	float texture1TranslationX;
	float texture1TranslationZ;
	float texture2TranslationX;
	float texture2TranslationZ;
	float brightness;
	float3 padding;
};

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
	float2 sampleCoordinate;

	// Translate where the pixel color from the first cloud texture is sampled
	sampleCoordinate.x = input.texCoord.x + texture1TranslationX;
	sampleCoordinate.y = input.texCoord.y + texture1TranslationZ;

	// Sample the pixel color from the first cloud texture using the sampler at this texture coordinate
	float4 textureColor1 = shaderTexture1.Sample(samplerState, sampleCoordinate);

	// Translate where the pixel color from the second cloud texture is sampled
	sampleCoordinate.x = input.texCoord.x + texture2TranslationX;
	sampleCoordinate.y = input.texCoord.y + texture2TranslationZ;

	// Sample the pixel color from the second cloud texture using the sampler at this texture coordinate
	float4 textureColor2 = shaderTexture2.Sample(samplerState, sampleCoordinate);

	// Combine the two cloud textures evenly
	float4 outputColor = lerp(textureColor1, textureColor2, 0.5f);

	// Reduce the brightness of the combined cloud textures
	outputColor = outputColor * brightness;

	return outputColor;
}
