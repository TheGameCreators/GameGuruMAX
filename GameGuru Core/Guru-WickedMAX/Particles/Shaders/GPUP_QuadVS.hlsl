struct VertexIn
{
    float2 position : POSITION;
};

struct VertexOut
{
	float2 uv : TEXCOORD0;
	float4 position : SV_POSITION;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
	OUT.position = float4( IN.position.xy, 0.5, 1.0 );
	OUT.uv = IN.position.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 );
	
    return OUT;
}

