cbuffer constants : register( b0 )
{
    matrix projectionMatrix;
    matrix viewMatrix;
    matrix worldMatrix;
}

struct VertexIn
{
    float3 position : POSITION;
    float3 color : COLOR;
	float2 uv : UV;
};

struct VertexOut
{
	float4 color : COLOR;
    float2 uv : TEXCOORD0;
	float4 position : SV_POSITION;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
   // matrix mvp = mul( projectionMatrix, mul( viewMatrix, worldMatrix ) );
    //OUT.position = mul( mvp, float4( IN.position, 1.0f ) );
	float4 pos = float4( IN.position, 1.0 );
	pos = mul( worldMatrix, pos );
	pos = mul( viewMatrix, pos );
	OUT.position = mul( projectionMatrix, pos );

	//OUT.position = float4( pos, 1.0f );
    OUT.color = float4( IN.color, 1.0f );
	OUT.uv = IN.uv;
	
    return OUT;
}

