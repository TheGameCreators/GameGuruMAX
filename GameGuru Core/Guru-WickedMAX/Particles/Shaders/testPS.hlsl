Texture2D tex0 : register( t0 );
SamplerState samplerLinear : register( s0 );

struct PixelIn
{
    float4 color : COLOR;
	float2 uv : TEXCOORD0;
};
 
float4 main( PixelIn IN ) : SV_TARGET
{
    //return IN.color;
	return tex0.Sample( samplerLinear, IN.uv );
}