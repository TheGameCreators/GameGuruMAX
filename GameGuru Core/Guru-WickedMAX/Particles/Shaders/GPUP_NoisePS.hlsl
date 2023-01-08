Texture2D tex0 : register( t0 );
SamplerState samplerLinearWrap : register( s2 );

struct PixelIn
{
	float2 uv : TEXCOORD0;
};

cbuffer constants : register( b0 )
{
	float3 off;
}
 
float4 main( PixelIn IN ) : SV_TARGET
{
	float2 uv1 = IN.uv; uv1.x += off.x;
	float2 uv2 = IN.uv; uv2.x += off.y;
	float4 cl1 = tex0.Sample( samplerLinearWrap, uv1 );
	float4 cl2 = tex0.Sample( samplerLinearWrap, uv2 );
	return lerp( cl1, cl2, off.z );
}
