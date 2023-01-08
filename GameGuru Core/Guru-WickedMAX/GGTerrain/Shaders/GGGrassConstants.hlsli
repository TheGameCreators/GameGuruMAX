#ifndef _H_GRASS_CONSTANTS
#define _H_GRASS_CONSTANTS

#define GGGRASS_REFLECTANCE       0.02
#define GGGRASS_SCALE             40.0
#define GGGRASS_LOD_TRANSITION    2500.0

//#if defined(GGGRASS_CONSTANTS_FULL_DECL) || !defined(__cplusplus)

#ifdef __cplusplus
	#define VAR_UNIT uint32_t
	#define VAR_MAT2X2 float4
#else
	#define VAR_UNIT uint
	#define VAR_MAT2X2 float2x2
#endif

struct GrassType
{
	float scaleFactor;
	float cosTime; // not a grass type parameter, but making use of unused space
	float padding1;
	float padding2;
};

#define GGGRASS_NUM_TYPES 46    // must be larger than 31 to have enough cosTime entries
#define GGGRASS_NUM_SELECTABLE_TYPES 22

#define GGGRASS_FLAGS_SIMPLE_PBR  0x0001 // increase performance by simplifying the PBR shader but with lower quality

#ifdef __cplusplus
struct GrassCB
#else
cbuffer GrassCB : register( b2 )
#endif
{
	float4 grass_rotMat[ 32 ];

	GrassType grass_type[ GGGRASS_NUM_TYPES ];

	float grass_lodDist;
	uint grass_flags;
	float grass_padding1;
	float grass_padding2;
};

// shader only
#ifndef __cplusplus
	uint GetGrassType( uint data ) { return data & 0xFF; }
	uint GetGrassVariation( uint data ) { return (data >> 8) & 31; } // must result in a value less than GGGRASS_NUM_TYPES to have enough cosTime entries
#endif

//#endif // GGTREES_CONSTANTS_FULL_DECL

#endif // _H_GRASS_CONSTANTS