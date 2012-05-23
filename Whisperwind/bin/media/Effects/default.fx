texture diffuseTexture;

sampler TestSampler = 
sampler_state
{
	Texture = <diffuseTexture>;
	MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
};

float4x4 proj : WorldViewProj;
float3 ambient : Ambient = { 0.3, 0.3, 0.3 };
float3 diffuse : Diffuse = { 0.3, 0.3, 0.3 };
float3 specular : Specular = { 0.0, 0.0, 0.0 };

float3 lightDir : LightPosition = { 0.0, 100.0, 0.0 };
float3 lightColor : LightColor = { 1.0, 1.0, 1.0 };

void VS(   float4 pos : POSITION0,
				  float3 normal : NORMAL,
				  float2 coord : TEXCOORD0,
				  out float4 oPos : POSITION0,
			      out float2 oCoord : TEXCOORD0,
			      out float4 oColor : COLOR0 )
{
    // Just for test.
	float3 L = normalize(lightDir - pos.xyz);
	float diffuseLight = max(dot(normal, L), 0);
	float3 diffuseColor = diffuse * lightColor * diffuseLight;

	oColor = float4(ambient + diffuseColor + specular, 1.0) * 0.1;

	oCoord = coord;
	
	oPos = mul(pos, proj);
}

void PS( in float2 coord : TEXCOORD0,
				in float4 color : COLOR0,
				out float4 oColor : COLOR0)
{
	oColor = tex2D(TestSampler, coord) + float4(min(color.x, 0.3), min(color.y, 0.3), min(color.z, 0.3), 1.0);
}

technique basic
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}