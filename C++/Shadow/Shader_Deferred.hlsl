
cbuffer Matrices
{
	matrix			g_TransformMatrix;
	matrix			g_ProjMatrix;
	matrix			g_LightViewMatrix;
	matrix			g_LightProjMatrix;
};

cbuffer InverseMatrices
{
	matrix			g_ViewMatrixInverse;
	matrix			g_ProjMatrixInverse;
};

cbuffer Camera
{
	vector			g_vCamPosition;
};

cbuffer Light
{
	vector			g_vLightDir;
	vector			g_vLightPos;
	float			g_fRange;
	vector			g_vLightDiffuse;
	vector			g_vLightAmbient;
	vector			g_vLightSpecular;
};

cbuffer Material
{
	vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
	vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

cbuffer BlurWeight
{
	float TexGapX;
	float TexGapY;
	float Weight0;
	float Weight1;
	float Weight2;
	float Weight3;
	float Weight4;
	float Weight5;
	float Weight6;
	float Weight7;
};

texture2D		g_TargetTexture;
texture2D		g_NormalTexture;
texture2D		g_DepthTexture;
texture2D		g_EmissiveTexture;
texture2D		g_DiffuseTexture;
texture2D		g_LightDepthTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;


sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matTP;

	matTP = mul(g_TransformMatrix, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matTP);

	Out.vTexUV = In.vTexUV;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 600.f;
	float		bias = 0.016f;
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 투영공간상의 위치. */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스상의 위치. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드스페이스상의 위치. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	vector LightViewPosition = mul(vWorldPos, g_LightViewMatrix);
	LightViewPosition = mul(LightViewPosition, g_LightProjMatrix);

	//이게이제 라이트기준으로 본 녀석의 UV좌표.
	float2 WorldToUV;
	WorldToUV.x = (LightViewPosition.x / LightViewPosition.w) / 2.f + 0.5f;
	WorldToUV.y = -(LightViewPosition.y / LightViewPosition.w) / 2.f + 0.5f;


	float WorldDepth = LightViewPosition.z / LightViewPosition.w;
	WorldDepth -= bias;
	float UVDepth = g_LightDepthTexture.Sample(DefaultSampler, WorldToUV).x;

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

	if (WorldDepth <= UVDepth || WorldToUV.x > 1.f || WorldToUV.x < 0.f || WorldToUV.y > 1.f || WorldToUV.y < 0.f)
	{
	}
	else
	{
		Out.vShade *= 0.2f;
	}
	Out.vShade.a = 1.f;

	vector			vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
	vector			vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(vReflect * -1.f, vLook)), 30.f);

	return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 투영공간상의 위치. */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스상의 위치. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드스페이스상의 위치. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fRange - fDistance), 0.f) / g_fRange;

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 1.f;

	vector			vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector			vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(vReflect * -1.f, vLook)), 30.f) * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor = vDiffuse * vShade + vSpecular;


	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLOOMWEIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	float2 TexUV = In.vTexUV;
	TexUV.x -= TexGapX * 7;
	vector		vBlur = float4(0.f, 0.f, 0.f, 0.f);

	vector		vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight7;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight6;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight5;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight4;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight3;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight2;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight1;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight0;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight1;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight2;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight3;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight4;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight5;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight6;
	TexUV.x += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight7;
	TexUV.x += TexGapX;

	Out.vColor = vBlur;

	return Out;
}

PS_OUT PS_MAIN_BLOOMHEIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 TexUV = In.vTexUV;
	TexUV.y -= TexGapY * 7;
	vector		vBlur = float4(0.f, 0.f, 0.f, 0.f);

	vector		vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight7;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight6;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight5;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight4;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight3;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight2;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight1;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight0;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight1;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight2;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight3;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight4;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight5;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight6;
	TexUV.y += TexGapX;
	vDiffuse = g_EmissiveTexture.Sample(DefaultSampler, TexUV);
	vBlur += vDiffuse * Weight7;
	TexUV.y += TexGapX;

	Out.vColor = vBlur;

	return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexUV);
	
	Out.vColor = vEmissive;

	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

BlendState NonBlendState
{
	BlendEnable[0] = false;
};


DepthStencilState NonZTestDepthStencilState
{
	DepthEnable = false;
	DepthWriteMask = all;
	DepthFunc = less;
};

RasterizerState DefaultRasterizerState
{
	FillMode = solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

BlendState LightBlendState
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};


technique11	DefaultTechnique
{
	pass DefaultPass
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Direction
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass BlendRender
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass BloomWeight
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOMWEIGHT();
	}

	pass BloomHeight
	{
		SetBlendState(NonBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOMHEIGHT();
	}

	pass FinalRender
	{
		SetBlendState(LightBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestDepthStencilState, 0);
		SetRasterizerState(DefaultRasterizerState);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

}
