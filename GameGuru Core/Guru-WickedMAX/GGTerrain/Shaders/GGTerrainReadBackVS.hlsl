cbuffer constants : register( b0 )
{
	float4x4 viewProj;
};

struct VertexIn
{
	float3 position : POSITION;
	uint id : ID;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 uv2 : TEXCOORD1;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
	float4 pos = float4( IN.position.xyz, 1.0 );
	OUT.position = mul( viewProj, pos );
	
	float fX = (float) (IN.id & 0xFF);
	float fZ = (float) (IN.id >> 8);
	OUT.uv.xy = IN.position.xz / float2( 1024, -1024 ) + 0.5;
	OUT.uv2.xy = IN.position.xz / float2( 131072, -131072 ) + 0.5;
			
    return OUT;
}

