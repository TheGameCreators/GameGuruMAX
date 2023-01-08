
// _FLAG_ are ones that force a page table update, _FLAG2_ do not affect pages
#define GGTERRAIN_SHADER_FLAG_SHOW_MASK          0x0001
#define GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE     0x0002
#define GGTERRAIN_SHADER_FLAG2_USE_FOG           0x0004
#define GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D  0x0008
#define GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP     0x0010
#define GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE   0x0020
#define GGTERRAIN_SHADER_FLAG_SHOW_MAT_MAP       0x0040

#define GGTERRAIN_USE_SURFACE_TEXTURE

#if defined(GGTERRAIN_CONSTANTS_FULL_DECL) || !defined(__cplusplus)

#ifdef __cplusplus
	#define VAR_UNIT uint32_t
	#define VAR_MAT4X4 XMFLOAT4X4
#else
	#define VAR_UNIT uint
	#define VAR_MAT4X4 float4x4
#endif

struct TerrainLODLevel
{
	float x;
	float z;
	float size;
	float padding;
};

struct TerrainLayer
{
	float start;
	float transition; // 1.0 / (end - start)
	VAR_UNIT material;
	float padding;
};

#ifdef __cplusplus
struct TerrainCB
#else
cbuffer TerrainCB : register( b2 )
#endif
{
	TerrainLODLevel terrain_LOD[ 16 ];

	VAR_UNIT terrain_numLODLevels;
	float    terrain_detailLimit; // 0=full detail
	float    terrain_bumpiness;
	float    terrain_reflectance;
	
	float    terrain_detailScale; // 1.0=full detail
	VAR_UNIT terrain_baseLayerMaterial;
	float    terrain_maskScale;
	float    terrain_mapEditSize;
	
	TerrainLayer terrain_layers[ 5 ];

	TerrainLayer terrain_slopes[ 2 ];

	VAR_UNIT terrain_flags;
	float    terrain_textureGamma;
	float2   terrain_mouseHit;

	float    terrain_brushSize;
	VAR_UNIT terrain_readBackReduction;
	float    terrain_envGamma;
	float    terrain_padding2;

	float4   terrain_maskRotMat[ 64 ];

	VAR_MAT4X4 terrain_rampWorldMat;
};

#endif // GGTERRAIN_CONSTANTS_FULL_DECL