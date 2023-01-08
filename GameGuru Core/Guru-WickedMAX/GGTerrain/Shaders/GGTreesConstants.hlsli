
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

#endif // GGTREES_CONSTANTS_FULL_DECL