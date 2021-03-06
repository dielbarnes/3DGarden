//
// LightPixelShader.hlsl
// Copyright � 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
//

Texture2D shaderTexture;
SamplerState samplerState;

// Constant buffer

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate
	float4 textureColor = shaderTexture.Sample(samplerState, input.texCoord);

	// Set the default output color to the ambient light value for all pixels
	float4 outputColor = ambientColor;

	// Invert the light direction for calculations
	// Calculate the amount of light on this pixel
	float lightIntensity = saturate(dot(input.normal, -lightDirection));

	if (lightIntensity > 0.0f)
	{
		// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity
		outputColor += (diffuseColor * lightIntensity);
	}

	// Saturate the final light color
	outputColor = saturate(outputColor);

	// Multiply the texture color and the final diffuse color to get the final pixel color
	outputColor = outputColor * textureColor;

	return outputColor;
}
