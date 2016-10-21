//========= Copyright &copy; 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: Crossroads devtest
//
// $NoKeywords: $
//=============================================================================

#include "BaseVSShader.h"
#include "ssao_ps30.inc"
#include "ssao_vs30.inc"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
// SHADER PARAMS DEFINED IN SHADER FXC CODE
static ConVar ssao_samples("ssao_samples", "8", FCVAR_ARCHIVE);
static ConVar ssao_contrast("ssao_contrast", "2.0", FCVAR_ARCHIVE);
static ConVar ssao_radius("ssao_radius", "16", FCVAR_ARCHIVE);
static ConVar ssao_bias("ssao_bias", "0.02");
static ConVar ssao_bias_offset("ssao_bias_offset", "0.05", FCVAR_ARCHIVE);
static ConVar ssao_illuminfluence("ssao_illuminfluence", "5.0", FCVAR_ARCHIVE);
static ConVar ssao_zfar("ssao_zfar", "8.0", FCVAR_ARCHIVE);
static ConVar ssao_znear("ssao_znear", "1.0", FCVAR_ARCHIVE);

BEGIN_VS_SHADER_FLAGS(SSAO, "Help for SSAO", SHADER_NOT_EDITABLE)
BEGIN_SHADER_PARAMS
SHADER_PARAM(BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_FullFrameFB", "Framebuffer")
END_SHADER_PARAMS

SHADER_INIT_PARAMS() { SET_FLAGS2(MATERIAL_VAR2_NEEDS_FULL_FRAME_BUFFER_TEXTURE); }

SHADER_FALLBACK { return 0; }

SHADER_INIT
{
    if (params[BASETEXTURE]->IsDefined())
    {
        LoadTexture(BASETEXTURE);
    }
}

SHADER_DRAW
{
    SHADOW_STATE
    {
        pShaderShadow->VertexShaderVertexFormat(VERTEX_POSITION, 1, 0, 0);

        pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);

        // Render targets are pegged as sRGB on POSIX, so just force these reads and writes
        bool bForceSRGBReadAndWrite = IsOSX() && g_pHardwareConfig->CanDoSRGBReadFromRTs();
        pShaderShadow->EnableSRGBRead(SHADER_SAMPLER0, bForceSRGBReadAndWrite);
        pShaderShadow->EnableSRGBWrite(bForceSRGBReadAndWrite);

        DECLARE_STATIC_VERTEX_SHADER(ssao_vs30);
        SET_STATIC_VERTEX_SHADER(ssao_vs30);

        DECLARE_STATIC_PIXEL_SHADER(ssao_ps30);
        SET_STATIC_PIXEL_SHADER(ssao_ps30);
    }

    DYNAMIC_STATE
    {
        BindTexture(SHADER_SAMPLER0, BASETEXTURE, -1);

        ITexture *src_texture = params[BASETEXTURE]->GetTextureValue();

        int width = src_texture->GetActualWidth();
        int height = src_texture->GetActualHeight();

        float g_TexelSize[2] = {1.0f / float(width), 1.0f / float(height)};

        pShaderAPI->SetPixelShaderConstant(0, g_TexelSize);

        DECLARE_DYNAMIC_VERTEX_SHADER(ssao_vs30);
        SET_DYNAMIC_VERTEX_SHADER(ssao_vs30);

        DECLARE_DYNAMIC_PIXEL_SHADER(ssao_ps30);
        SET_DYNAMIC_PIXEL_SHADER(ssao_ps30);

        float samples = ssao_samples.GetInt();
        float contrast = ssao_contrast.GetFloat();
        float radius = ssao_radius.GetFloat();
        float bias = ssao_bias.GetFloat();
        float biasoffset = ssao_bias_offset.GetFloat();
        float illuminf = ssao_illuminfluence.GetFloat();
        float zfar = ssao_zfar.GetFloat();
        float znear = ssao_znear.GetFloat();

        pShaderAPI->SetPixelShaderConstant(1, &samples);
        pShaderAPI->SetPixelShaderConstant(2, &radius);
        pShaderAPI->SetPixelShaderConstant(3, &bias);
        pShaderAPI->SetPixelShaderConstant(4, &illuminf);
        pShaderAPI->SetPixelShaderConstant(5, &contrast);
        pShaderAPI->SetPixelShaderConstant(6, &znear);
        pShaderAPI->SetPixelShaderConstant(7, &zfar);
        pShaderAPI->SetPixelShaderConstant(8, &biasoffset);
    }
    Draw();
}
END_SHADER