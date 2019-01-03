cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	// Store the texture coordinates for the pixel shader
	output.texCoord = input.texCoord;

	// Store the particle color for the pixel shader
	output.color = input.color;

	return output;
}
