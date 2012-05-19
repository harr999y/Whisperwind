//////////////////////////////////////////////////////////////
//simple Duffuse bump - adapted from the nVidia cgFX samples
//This sample supports Render To Texture and is setup to use
//Mapping channel 3 - it just needs to be uncommented.
//////////////////////////////////////////////////////////////

float4x4 worldMatrix : 		World;	// World or Model matrix
float4x4 WorldIMatrix : 	WorldI;	// World Inverse or Model Inverse matrix
float4x4 mvpMatrix : 		WorldViewProj;	// Model*View*Projection
float4x4 worldViewMatrix : 	WorldView;
float4x4 viewInverseMatrix :	ViewI;

// tweakables

texture diffuseTexture : DiffuseMap< 
	string name = "seafloor.dds"; 
	string UIName = "Diffuse Texture";
//	int Texcoord = 0;
//	int MapChannel = 3;	
	>;
	
texture normalMap : NormalMap < 
	string name = "NMP_Ripples2_512.dds"; 
	string UIName = "Normal Texture";
//	int Texcoord = 0;
//	int MapChannel = 3;	
	>;

float4 ambientColor : Ambient
<
> = { 1.1, 1.1, 1.1, 1.0};


float4 bumpHeight
<
> = { 1.5, 1.0, 0.5, 0.0};


float4 lightDir : Direction <  
	string UIName = "Light Direction"; 
	string Object = "TargetLight";
	int RefID = 0;
	> = {-0.577, -0.577, 0.577,1.0};


struct a2v {
	float4 Position : POSITION; //in object space
	float3 Normal : NORMAL; //in object space
	float2 TexCoord : TEXCOORD0;
	float3 T : TANGENT; //in object space
	float3 B : BINORMAL; //in object space
};

struct v2f {
	float4 Position : POSITION; //in projection space
	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float3 LightVector : TEXCOORD2;
};

struct f2fb {
	float4 col : COLOR;
};

v2f DiffuseBumpVS(a2v IN,
		uniform float4x4 WorldViewProj,
        	uniform float4x4 WorldIMatrix,
		uniform float4 LightDir)
{
	v2f OUT;

	// pass texture coordinates for fetching the diffuse map
	OUT.TexCoord0.xy = IN.TexCoord.xy;

	// pass texture coordinates for fetching the normal map
	OUT.TexCoord1.xy = IN.TexCoord.xy;
#if 1
	// compute the 3x3 tranform from tangent space to object space
	float3x3 objToTangentSpace;
	objToTangentSpace[0] = IN.B;
	objToTangentSpace[1] = IN.T;
	objToTangentSpace[2] = IN.Normal;

	// transform normal from object space to tangent space and pass it as a color
	//OUT.Normal.xyz = 0.5 * mul(IN.Normal,objToTangentSpace) + 0.5.xxx;

    	float4 objectLightDir = mul(LightDir,WorldIMatrix);
	float4 vertnormLightVec = normalize(objectLightDir);
	// transform light vector from object space to tangent space and pass it as a color 
	OUT.LightVector.xyz = 0.5 * mul(objToTangentSpace,vertnormLightVec.xyz ) + 0.5.xxx;
	// transform position to projection space
	OUT.Position = mul(IN.Position,WorldViewProj).xyzw;
#else
        OUT.LightVector = 0;
	OUT.Position = mul(IN.Position,WorldViewProj).xyzw;
#endif
	return OUT;

}

f2fb DiffuseBumpPS(v2f IN,
		uniform sampler2D DiffuseMap,
		uniform sampler2D NormalMap,
              	uniform float4 bumpHeight) 
{
	f2fb OUT;

	//fetch base color
	float4 color = tex2D(DiffuseMap,IN.TexCoord0 );

	//fetch bump normal
	
	float4 bumpNormal = (2 * (tex2D(NormalMap,IN.TexCoord1)-0.5)) * 1.5;

	//expand iterated light vector to [-1,1]
	float3 lightVector = 2 * (IN.LightVector - 0.5 );
	lightVector = normalize(lightVector);

	//compute final color (diffuse + ambient)
	float4 bump = dot(bumpNormal.xyz,lightVector.xyz);
	
	OUT.col = color + bumpNormal;//(color *bump) + 0.1 ;
	OUT.col.a = 1.0;
	return OUT;
}

sampler2D diffuseSampler = sampler_state
{
	Texture = <diffuseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};

sampler2D normalSampler = sampler_state 
{
	Texture = <normalMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};

//////// techniques ////////////////////////////

technique Default
{

	pass p0
	{
		ZEnable = true;
		ZWriteEnable = true;
		CullMode = None;
		VertexShader = compile vs_1_1 DiffuseBumpVS(mvpMatrix,WorldIMatrix,lightDir);
        	PixelShader = compile ps_2_0 DiffuseBumpPS(diffuseSampler,normalSampler,bumpHeight);
	}
}

