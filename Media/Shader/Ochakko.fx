//--------------------------------------------------------------------------------------
// 
// Copyright (c) ‚¨‚¿‚á‚Á‚±LAB. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

cbuffer Variable
{
	float4 g_diffuse;
	float3 g_ambient;
	float3 g_specular;
	float  g_power;
	float3 g_emissive;

	float4 g_LightDir[8];               // Light's direction in world space
	float4 g_LightDiffuse[8];           // Light's diffuse color

	matrix g_mWorld;                  // World matrix for object
	matrix g_mVP;    // View * Projection matrix
	float3	 g_EyePos;

	float3 g_spriteoffset;
	float2 g_spritescale;
	float3 g_pm3offset;
	float3 g_pm3scale;

	matrix g_m4x4Mat[70];
};

Texture2D <float4> g_MeshTexture;

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//Output.RGBColor = g_MeshTexture.Sample(MeshTextureSampler, In.TextureUV) * In.Diffuse;

//sampler MeshTextureSampler = 
//sampler_state
//{
//    Texture = <g_MeshTexture>;
//    MipFilter = LINEAR;
//    MinFilter = LINEAR;
//    MagFilter = LINEAR;
//};

//AddressU = Clamp;
//AddressV = Clamp;
//AddressU = Wrap;
//AddressV = Wrap;

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : SV_POSITION;   // vertex position 
    float4 Diffuse    : COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
	float3 Specular	  : TEXCOORD0;
    float2 TextureUV  : TEXCOORD1;  // vertex texture coords 
};

struct VS_LINEOUTPUT
{
    float4 Position   : SV_POSITION;   // vertex position 
    float4 Diffuse    : COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
};

struct VS_SPRITEOUTPUT
{
    float4 Position   : SV_POSITION;   // vertex position 
    float4 Diffuse    : COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};

//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneBoneVS( float4 vPos : SV_POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0,
						 float4 bweight : BLENDWEIGHT,
						 int4 bindices : BLENDINDICES,
                         uniform int nNumLights )
{
    VS_OUTPUT Output;
    float4 wPos;

	int bi[4] = { bindices.r, bindices.g, bindices.b, bindices.a };
	float bw[4] = { bweight.x, bweight.y, bweight.z, bweight.w };

	matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for(int i=0; i<4; i++ ){
		matrix addmat = g_m4x4Mat[bi[i]];
		finalmat += bw[i] * addmat;
	}	
	
	wPos = mul( vPos, finalmat );
	Output.Position = mul( wPos, g_mVP );
	wPos /= wPos.w;
    
    float3 wNormal;
    wNormal = normalize(mul(vNormal, (float3x3)finalmat)); // normal (world space)
    
    float3 totaldiffuse = float3(0,0,0);
    float3 totalspecular = float3(0,0,0);
	float calcpower = g_power * 0.1f;

    for(int j=0; j<nNumLights; j++ ){
		float nl;
		float3 h;
		float nh;	
		float4 tmplight;
		
		nl = dot( wNormal, g_LightDir[j] );
		h = normalize( ( g_LightDir[j] + g_EyePos - wPos.xyz ) * 0.5f );
		nh = dot( wNormal, h );

        totaldiffuse += g_LightDiffuse[j].xyz * max(0,dot(wNormal, g_LightDir[j]));
		totalspecular +=  ((nl) < 0) || ((nh) < 0) ? 0 : ((nh) * calcpower);
	}


    Output.Diffuse.rgb = g_diffuse.rgb * totaldiffuse.rgb + g_ambient + g_emissive;   
    Output.Diffuse.a = g_diffuse.a; 
    
	Output.Specular = g_specular * totalspecular;

    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}
VS_OUTPUT RenderSceneBoneNLightVS( float4 vPos : SV_POSITION,
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0,
						 float4 bweight : BLENDWEIGHT,
						 int4 bindices : BLENDINDICES )
{
    VS_OUTPUT Output;
    float4 wPos;

	uint bi[4] = { bindices.r, bindices.g, bindices.b, bindices.a };
	float bw[4] = { bweight.x, bweight.y, bweight.z, bweight.w };

	matrix finalmat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for(int i=0; i<4; i++ ){
		matrix addmat = g_m4x4Mat[bi[i]];
		finalmat += bw[i] * addmat;
	}	

	wPos = mul( vPos, finalmat );
	Output.Position = mul( wPos, g_mVP );
	wPos /= wPos.w;
    
    float3 wNormal;
    wNormal = normalize(mul(vNormal, (float3x3)finalmat)); // normal (world space)
    
    Output.Diffuse.rgb = g_diffuse.rgb + g_ambient + g_emissive;   
    Output.Diffuse.a = g_diffuse.a; 
    
	Output.Specular = float3( 0.0f, 0.0f, 0.0f );

    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}

VS_OUTPUT RenderSceneNoBoneVS( float4 vPos : SV_POSITION,
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0,
                         uniform int nNumLights )
{
    VS_OUTPUT Output;
	float4 scalepos;
    float4 wPos;
	matrix finalmat = g_mWorld;

	scalepos.x = (vPos.x - g_pm3offset.x) * g_pm3scale.x + g_pm3offset.x;
	scalepos.y = (vPos.y - g_pm3offset.y) * g_pm3scale.y + g_pm3offset.y;
	scalepos.z = (vPos.z - g_pm3offset.z) * g_pm3scale.z + g_pm3offset.z;
	scalepos.w = 1.0f;

	wPos = mul(scalepos, finalmat);

	Output.Position = mul( wPos, g_mVP );
	wPos /= wPos.w;
    
    float3 wNormal;
    wNormal = normalize(mul(vNormal, (float3x3)finalmat)); // normal (world space)
    
    float3 totaldiffuse = float3(0,0,0);
    float3 totalspecular = float3(0,0,0);
	float calcpower = g_power * 0.1f;

    for(int i=0; i<nNumLights; i++ ){
		float nl;
		float3 h;
		float nh;	
		float4 tmplight;
		
		nl = dot( wNormal, g_LightDir[i] );
		h = normalize( ( g_LightDir[i] + g_EyePos - wPos.xyz ) * 0.5f );
		nh = dot( wNormal, h );

        totaldiffuse += g_LightDiffuse[i].xyz * max(0,dot(wNormal, g_LightDir[i]));
		totalspecular +=  ((nl) < 0) || ((nh) < 0) ? 0 : ((nh) * calcpower);
	}


    Output.Diffuse.rgb = g_diffuse.rgb * totaldiffuse.rgb + g_ambient + g_emissive;   
    Output.Diffuse.a = g_diffuse.a; 
    
	Output.Specular = g_specular * totalspecular;

    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}

VS_OUTPUT RenderSceneNoBoneNLightVS( float4 vPos : SV_POSITION,
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
	float4 scalepos;
	float4 wPos;

	matrix finalmat = g_mWorld;

	scalepos.x = (vPos.x - g_pm3offset.x) * g_pm3scale.x + g_pm3offset.x;
	scalepos.y = (vPos.y - g_pm3offset.y) * g_pm3scale.y + g_pm3offset.y;
	scalepos.z = (vPos.z - g_pm3offset.z) * g_pm3scale.z + g_pm3offset.z;
	scalepos.w = 1.0f;

	wPos = mul(scalepos, finalmat);

	Output.Position = mul( wPos, g_mVP );
	wPos /= wPos.w;
    
    float3 wNormal;
    wNormal = normalize(mul(vNormal, (float3x3)finalmat)); // normal (world space)
    

    Output.Diffuse.rgb = g_diffuse.rgb + g_ambient + g_emissive;   
    Output.Diffuse.a = g_diffuse.a; 
    
	Output.Specular = float3( 0.0f, 0.0f, 0.0f );

    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}


VS_LINEOUTPUT RenderSceneLineVS( float4 vPos : SV_POSITION )
{
    VS_LINEOUTPUT Output;
    float4 wPos;

	matrix finalmat = g_mWorld;

	wPos = mul( vPos, finalmat );
	Output.Position = mul( wPos, g_mVP );


    Output.Diffuse.rgb = g_diffuse.rgb;
	Output.Diffuse.a = g_diffuse.a; 
        
    return Output;    
}


VS_SPRITEOUTPUT RenderSceneSpriteVS( float4 vPos : SV_POSITION,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_SPRITEOUTPUT Output;

	float4 outv = vPos;
	outv.x *= g_spritescale.x;
	outv.y *= g_spritescale.y;
	outv.x += g_spriteoffset.x;
	outv.y += g_spriteoffset.y;
	outv.z = g_spriteoffset.z;//!!!! =
	Output.Position = outv;

    Output.Diffuse.rgb = g_diffuse.rgb;   
    Output.Diffuse.a = g_diffuse.a; 
    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}



//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : SV_Target;  // Pixel color    
};


//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
//       color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT RenderScenePSTex( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
	Output.RGBColor = g_MeshTexture.Sample(MeshTextureSampler, In.TextureUV) * In.Diffuse + float4(In.Specular, 0);
    //Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV) * In.Diffuse + float4( In.Specular, 0 );
    return Output;
}

PS_OUTPUT RenderScenePSNotex( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
    Output.RGBColor = In.Diffuse + float4( In.Specular, 0 );
    return Output;
}

PS_OUTPUT RenderScenePSLine( VS_LINEOUTPUT In ) 
{ 
    PS_OUTPUT Output;
    Output.RGBColor = In.Diffuse;
    return Output;
}

PS_OUTPUT RenderScenePSSprite( VS_SPRITEOUTPUT In ) 
{ 
    PS_OUTPUT Output;
	Output.RGBColor = g_MeshTexture.Sample(MeshTextureSampler, In.TextureUV) * In.Diffuse;
    //Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV) * In.Diffuse;
    return Output;
}


//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------

technique10 RenderBoneL0
{
    pass P0
    {          
        SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneNLightVS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneNLightVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL1
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 1 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 1 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL2
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 2 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 2 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL3
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 3 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 3 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL4
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 4 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 4 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL5
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 5 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 5 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL6
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 6 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 6 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL7
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 7 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 7 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderBoneL8
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 8 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneBoneVS( 8 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}



technique10 RenderNoBoneL0
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneNLightVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneNLightVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}


technique10 RenderNoBoneL1
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 1 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 1 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL2
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 2 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 2 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL3
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 3 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 3 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL4
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 4 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 4 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL5
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 5 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 5 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL6
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 6 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 6 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL7
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 7 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 7 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderNoBoneL8
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 8 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSTex()));
		//SetDepthStencilState(EnableDepth, 0);
    }
    pass P1
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneNoBoneVS( 8 )));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSNotex()));
		//SetDepthStencilState(EnableDepth, 0);
    }

}

technique10 RenderLine
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneLineVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSLine()));
		//SetDepthStencilState(EnableDepth, 0);
    }
}

technique10 RenderSprite
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, RenderSceneSpriteVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderScenePSSprite()));
		//SetDepthStencilState(EnableDepth, 0);
    }
}
