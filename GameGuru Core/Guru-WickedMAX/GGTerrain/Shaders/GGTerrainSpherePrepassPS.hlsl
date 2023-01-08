
struct PixelIn
{
    float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD1;
	float3 normal : TEXCOORD3;
	uint instanceID : TEXCOORD2;
	float2 uv : TEXCOORD4;
};

struct Output
{
	float4 velocity : SV_TARGET0;
	uint   readback : SV_TARGET1;  // virtual texture read back
};

Output main( PixelIn IN )
{
	Output output;
	output.velocity = float4( 0, 0, 0, 0 );
	output.readback = 0;	
	return output;
}