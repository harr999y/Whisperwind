
texture tex : TEXTURE;

sampler TestSampler = 
sampler_state
{
	Texture = <tex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

//float4 preColor;

void VS( float4 pos : POSITION,
				//float4 color : TEXCOORD1,
				float2 coord : TEXCOORD0,
				out float4 oPos : POSITION,
               //out float4 oColor : TEXCOORD1,
			   out float2 oCoord : TEXCOORD0)
{
	//oColor = color;
	oPos = pos;
	oCoord = coord;
}

void PS(  //float4 color : TEXCOORD1,
				float2 coord : TEXCOORD0,
				out float4 oColor : COLOR0)
{
	oColor = tex2D(TestSampler, coord);//color + preColor * 0.1;
}

technique Test
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}