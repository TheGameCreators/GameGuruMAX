cbuffer FrameCB : register( b0 )
{
	float2		g_xFrame_CanvasSize;
	float2		g_xFrame_CanvasSize_rcp;

	float2		g_xFrame_InternalResolution;
	float2		g_xFrame_InternalResolution_rcp;
};

struct VertexIn
{
    float2 position : POSITION;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	float aspect = g_xFrame_InternalResolution.x / g_xFrame_InternalResolution.y;
	float size = 0.15;
	float2 scale = float2( size, size * aspect );
	float2 offset = scale * 1.5 - 1;
	offset.x += 0.25;
 
	OUT.position = float4( IN.position.xy, 0.5, 1.0 );
	OUT.position.xy = OUT.position.xy * scale + offset;
	OUT.uv = IN.position.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 );
	
    return OUT;
}

