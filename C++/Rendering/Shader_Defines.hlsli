cbuffer		LightMat
{
	float4				g_WorldLightPosition;
	float4x4			g_LightViewMatrix;
	float4x4			g_LightProjMatrix;
};

cbuffer		MaterialDesc
{
	vector		g_vMtrlDiffuse = (vector)1.f;
	vector		g_vMtrlAmbient = (vector)1.f;
	vector		g_vMtrlSpecular = (vector)1.f;
	vector		g_vMtrlEmissive = (vector)0.f;
	float		g_fMtrlPower = 10.f;
	int			g_iMtrColorIdx = 0;
}

texture2D g_EffectTexture;
texture2D g_EffectTexture2;

cbuffer DissolveColor
{
    vector g_vDissolveColorNow = (vector) 1.f;
    vector g_vDissolveColorNext = vector(1.f, 0.f, 0.f, 1.f);
    
    //블루 계열
    vector g_vDissolveColorBlue1 = vector(0.294f, 0.f, 0.509f, 1.f);
    vector g_vDissolveColorBlue2 = vector(0.117f, 0.564f, 1.f, 1.f);
};

cbuffer Alpha
{
    float g_fAlpha = 1.f;
};

cbuffer GammaCorrection
{
    bool g_bLinearSpace = true;
    bool g_bToMonitor = true;
};

cbuffer Camera
{
	vector			g_vCamPosition;
	float			g_fFar = 300.f;
};

cbuffer Light
{
	vector			g_vLightDiffuse = (vector)1.f;
	vector			g_vLightAmbient = (vector)1.f;
	float3			g_vLightDir = (float3)1.f;
    float4			g_vLightPos;
    float			g_fLightRadius;
};

float3 Gamma_LinearSpace(float3 rgb)
{
    return pow(rgb, 2.2f);
};

float3 Gamma_ToMonitor(float3 rgb)
{
    return pow(rgb, 1.f / 2.2f);
};



//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;

BlendState AlphaBlendState
{
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
};

BlendState AddBlendState
{
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

BlendState RevSubtractBlendState
{
	BlendEnable[0] = true;
	BlendOp = Rev_Subtract;
	SrcBlend = one;
	DestBlend = one;
};

BlendState MinBlendState
{
	BlendEnable[0] = true;
	BlendOp = Min;
	SrcBlend = one;
	DestBlend = one;
};

BlendState NonBlendState
{
	BlendEnable[0] = false;
};


//BOOL DepthEnable;
//D3D11_DEPTH_WRITE_MASK DepthWriteMask;
//D3D11_COMPARISON_FUNC DepthFunc;
//BOOL StencilEnable;
//UINT8 StencilReadMask;
//UINT8 StencilWriteMask;
//D3D11_DEPTH_STENCILOP_DESC FrontFace;
//D3D11_DEPTH_STENCILOP_DESC BackFace;


DepthStencilState DefaultDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
    StencilEnable = false;
    FrontFaceStencilFunc = never;
    BackFaceStencilFunc = never;
    StencilReadMask = 0x00;
    StencilWriteMask = 0x00;
};

DepthStencilState NonZTestDepthStencilState
{
	DepthEnable = false;
	DepthWriteMask = all;
    DepthFunc = less_equal;
    StencilEnable = false;
    FrontFaceStencilFunc = never;
    BackFaceStencilFunc = never;
    StencilReadMask = 0x00;
    StencilWriteMask = 0x00;
};

DepthStencilState NonZWriteDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
    DepthFunc = less_equal;
    StencilEnable = false;
    FrontFaceStencilFunc = never;
    BackFaceStencilFunc = never;
    StencilReadMask = 0x00;
    StencilWriteMask = 0x00;
};

DepthStencilState DisableDepthStencilState
{
	DepthEnable = false;
	DepthWriteMask = zero;
    DepthFunc = less_equal;
    StencilEnable = false;
    FrontFaceStencilFunc = never;
    BackFaceStencilFunc = never;
    StencilReadMask = 0x00;
    StencilWriteMask = 0x00;
};

DepthStencilState InsideEdgeDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
    StencilEnable = true;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = never;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;
};

DepthStencilState EdgeDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
    StencilEnable = true;
    FrontFaceStencilPass = keep;
    FrontFaceStencilFunc = not_equal;
    BackFaceStencilFunc = never;
    StencilReadMask = 0xff;
    StencilWriteMask = 0x00;
};

DepthStencilState MapObjectDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
    StencilEnable = true;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = never;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;
};

DepthStencilState PlayerDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
    StencilEnable = true;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = never;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;
};

DepthStencilState PlayerSilhouetteDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = greater;
    StencilEnable = true;
    FrontFaceStencilPass = keep;
    FrontFaceStencilFunc = equal;
    BackFaceStencilFunc = never;
    StencilReadMask = 0xff;
    StencilWriteMask = 0x00;
};


//D3D11_FILL_MODE FillMode;
//D3D11_CULL_MODE CullMode;
//BOOL FrontCounterClockwise;
//INT DepthBias;
//FLOAT DepthBiasClamp;
//FLOAT SlopeScaledDepthBias;
//BOOL DepthClipEnable;
//BOOL ScissorEnable;
//BOOL MultisampleEnable;
//BOOL AntialiasedLineEnable;

RasterizerState DefaultRasterizerState
{
	FillMode = solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState CullCWRasterizerState
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

RasterizerState NoCullRasterizerState
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

RasterizerState WireframeRasterizerState
{
	FillMode = wireframe;
};


SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};