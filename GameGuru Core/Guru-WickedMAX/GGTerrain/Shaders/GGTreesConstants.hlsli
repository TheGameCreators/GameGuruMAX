
#ifndef WI_SHADERINTEROP_RENDERER_H
#include "PBR/ShaderInterop_Renderer.h"
#endif

#define GGTREES_REFLECTANCE   0.004

#define GGTREES_LOD_TRANSITION    500.0
#define GGTREES_LOD_SHADOW_TRANSITION 500.0

#if defined(GGTREES_CONSTANTS_FULL_DECL) || !defined(__cplusplus)

#ifdef __cplusplus
	#define VAR_UNIT uint32_t
#else
	#define VAR_UNIT uint
#endif

struct TreeType
{
	float scaleX;
	float scaleY;
	float padding1;
	float padding2;
};

static const VAR_UNIT numTreeTypes = 38;

#ifdef __cplusplus
struct TreeCB
#else
cbuffer TreeCB : register( b2 )
#endif
{
	float4   tree_rotMatShadow;

	float4   tree_rotMat[ 8 ];	

	TreeType tree_type[ numTreeTypes ];
	
	float3   tree_playerPos;
	uint     tree_padding0;

	float    tree_lodDist;
	float    tree_lodDistShadow;
	float    tree_padding1;
	float    tree_padding2;
};

uint GetTreeType( uint data ) { return (data >> 11) & 0x3F; }
uint GetTreeVariation( uint data ) { return (data >> 8) & 0x7; }
uint GetTreeHighlighted( uint data ) { return data & 0x4; }
float GetTreeScale( uint data ) { return ((data >> 16) & 0xFE) / 170.0 + 0.5; }

#ifndef __cplusplus
//PE: Animate the trees a bit.
float TreeWaveX(float posy, float posx)
{
    if (g_xFrame_TreeWind <= 0)
        return (0);
    const float swayspeed = g_xFrame_TreeWind * 6.0; // (0.85)
    const float swayamount = g_xFrame_TreeWind * 0.35; //0.075
    const float time = g_xFrame_Time;
    const float sdat = sin((time * (swayspeed * 1.5)) + posx) + cos((time * (swayspeed * 0.8)) + posx) + sin((time * (swayspeed * 1.2)));
    const float wave = sdat * 0.335;
    return (wave * (clamp((posy - 120) * 0.35, 0, posy) * swayamount));
}
float TreeWaveZ(float posy, float posx)
{
    if (g_xFrame_TreeWind <= 0)
        return (0);
    const float swayspeed = g_xFrame_TreeWind * 6.0; // (0.85)
    const float swayamount = g_xFrame_TreeWind * 0.20; //0.055
    const float time = g_xFrame_Time;
    const float sdat = sin((time * swayspeed) + posx) + cos((time * (swayspeed * 1.5)) + posx);
    const float wave = sdat * 0.5;
    return (wave * (clamp((posy - 120) * 0.35, 0, posy) * swayamount));
}
#endif
#endif // GGTREES_CONSTANTS_FULL_DECL