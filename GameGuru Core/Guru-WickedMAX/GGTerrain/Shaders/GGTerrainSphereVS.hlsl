

cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
};

struct VertexIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	uint instanceID : SV_InstanceID;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD1;
	float3 normal : TEXCOORD3;
	uint instanceID : TEXCOORD2;
	float2 uv : TEXCOORD4;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	uint indexX = IN.instanceID / 11;
	uint indexZ = IN.instanceID % 11;
 
	float4 pos = float4( IN.position.xyz * 30, 1.0 );
	if ( IN.instanceID == 22 ) 
	{
		pos.xyz *= 2;
		pos.x += 250;
	}
	pos.xyz += float3( -7500, 700, -700 );
	pos.xyz += float3( indexX * 250, 0, indexZ * 150 );
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.worldPos = pos.xyz;
	OUT.normal = IN.normal;
	OUT.uv = IN.uv*2;
	OUT.instanceID = IN.instanceID;
		
    return OUT;
}

