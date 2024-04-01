
#include "Shader_Defines.hpp"

cbuffer Matrices 
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
	matrix			g_LightViewMatrix;
	matrix			g_LightProjMatrix;
};

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_EmissiveTexture;
//texture2D		g_ORMTexture;
//texture2D		g_MTexture;


sampler DefaultSampler = sampler_state
{
	/*minfilter = linear;
	mipfilter = linear;
	magfilter = linaer*/
	AddressU = wrap;
	AddressV = wrap;
	Filter = min_mag_mip_linear;
	
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

	return Out;
}


VS_OUT VS_UVFIX(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV.x = In.vTexUV.x * 30.f;
	Out.vTexUV.y = In.vTexUV.y * 30.f;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vEmissive : SV_TARGET3;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 600.0f, 0.f, 0.f);
	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.1f)
		discard;
	return Out;
}

PS_OUT PS_MAIN_NORMAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 600.0f, 0.f, 0.f);
	if (Out.vDiffuse.a < 0.1f)
		discard;
	return Out;
}

PS_OUT PS_MAIN_NORMALEMISSIVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vEmissive.r > 0.f)
	{
		Out.vDiffuse = Out.vEmissive;
	}
	else if (Out.vEmissive.b > 0.f)
	{
		Out.vDiffuse.rgba = float4(0.953f, 0.467f, 0.306f, 1.f);
	}
	else
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	}

	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);

	if (Out.vDiffuse.a < 0.2f)
		discard;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 600.0f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_NONCULLEFFECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vEmissive = Out.vDiffuse;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	if (Out.vDiffuse.a < 0.2f)
		discard;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 600.0f, 0.f, 0.f);

	return Out;

}



struct VS_LIGHTDEPTH_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_LIGHTDEPTH_OUT VS_LIGHTDEPTH(VS_IN In)
{
	VS_LIGHTDEPTH_OUT		Out = (VS_LIGHTDEPTH_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_LightViewMatrix);
	matWVP = mul(matWV, g_LightProjMatrix);



	Out.vTexUV = In.vTexUV;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_LIGHTDEPTH_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_LIGHTDEPTH_OUT
{
	float4		vDepth : SV_TARGET0;
};

PS_LIGHTDEPTH_OUT PS_LIGHTDEPTH(PS_LIGHTDEPTH_IN In)
{
	PS_LIGHTDEPTH_OUT		Out = (PS_LIGHTDEPTH_OUT)0;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);

	return Out;
}

technique11	DefaultTechnique
{
	pass DefaultPass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Normal
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	pass NormalEmissive
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMALEMISSIVE();
	}

	pass NormalUVFix
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);
		VertexShader = compile vs_5_0 VS_UVFIX();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NoneCullEffect
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NonCullRasterizerState);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NONCULLEFFECT();
	}

	pass LightDepth
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetRasterizerState(NonCullRasterizerState);
		VertexShader = compile vs_5_0 VS_LIGHTDEPTH();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LIGHTDEPTH();
	}
}
