
texture tex;

sampler TestSampler = 
sampler_state
{
	Texture = <tex>;
	MipFilter = None;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

float4 preColor : COLOR3;
float4x4 proj : PROJ;

void VS( float4 pos : POSITION0,
				//float4 color : TEXCOORD1,
				float2 coord : TEXCOORD0,
				out float4 oPos : POSITION0,
               //out float4 oColor : TEXCOORD1,
			   out float2 oCoord : TEXCOORD0)
{
	//oColor = color;
	oPos = mul(pos, proj);
	oCoord = coord;
}

void PS(  //float4 color : TEXCOORD1,
				float2 coord : TEXCOORD0,
				out float4 oColor : COLOR0)
{
	oColor = tex2D(TestSampler, coord);// + preColor * 0.03;
}

technique Test
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}