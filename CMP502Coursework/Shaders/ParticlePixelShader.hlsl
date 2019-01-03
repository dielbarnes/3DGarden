Texture2D shaderTexture;
SamplerState samplerState;

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location
	float4 textureColor = shaderTexture.Sample(samplerState, input.texCoord);

	// Combine the texture color and the particle color to get the final color result
	return textureColor * input.color;
}
