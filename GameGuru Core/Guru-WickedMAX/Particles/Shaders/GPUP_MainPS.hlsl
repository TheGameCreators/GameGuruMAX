
Texture2D texture1 : register( t1 ); // imagex
Texture2D texture3 : register( t3 ); // gradient_1
Texture2D texture4 : register( t4 ); // dist2
SamplerState samplerLinear : register( s0 );
SamplerState samplerPoint : register( s1 );
SamplerState samplerLinearWrap : register( s2 );

struct PixelIn
{
	float2 uv0Varying : TEXCOORD0;
	float2 tpVarying : TEXCOORD1;
	float4 distVarying : TEXCOORD2;
	float3 colVarying : TEXCOORD3;
	float alphaVarying : TEXCOORD4;
	float3 posVarying : TEXCOORD5;
};

cbuffer constants : register( b1 )
{
	float4 tile;
	float3 clr;
	float moder;
	float3 image_count;
	float agk_time;
	float opacity;
	float filler; //PE: Must be 16 floats so, reserved for later.
	float filler2;
	float filler4;
}

float4 main( PixelIn IN ) : SV_TARGET
{
	float2 uv = IN.uv0Varying * 0.5;
	float2 uvb = 0.0;
	float pbl = 0.0;
	float an = 0.0;
	float g = 0.0;
	float u0x = IN.uv0Varying.x / tile.x;
	float u0y = IN.uv0Varying.y / tile.y;

	if ( tile.x > 0.5 && IN.tpVarying.x < 1.0 ) 
	{
		float nums = tile.x*tile.y;
		float pbb = IN.tpVarying.y*tile.z*nums;
		pbl = frac(pbb);
		pbb = floor( pbb );
		g = pbb - nums * floor(pbb/nums);
		float floorg = floor(g/tile.x);
		float modg = g - tile.x * floorg;
		uv.x = 1.0/tile.x * modg + u0x;
		uv.y = 1.0/tile.y * floorg + u0y;
		pbb += 1.0;
		g = pbb - nums * floor(pbb/nums);
		floorg = floor(g/tile.x);
		modg = g - tile.x * floorg;
		uvb.x = 1.0/tile.x * modg + u0x;
		uvb.y = 1.0/tile.y * floorg + u0y;
		an = 1.0;
	}

	if ( IN.tpVarying.x > 0.999999 ) 
	{
		if ( IN.tpVarying.x < 2.0 ) 
		{
			uv += float2(0.5, 0.0);
		} 
		else if ( IN.tpVarying.x < 3.0 ) 
		{
			uv += float2(0.0, 0.5);
		} 
		else 
		{
			uv += float2(0.5, 0.5);
		}
	}
	
	float2 uof = 0.0;
	float4 diss = IN.distVarying;
	if ( image_count.y > 0.001 ) 
	{
		float timeoff = 1.0 + (diss.z * 0.1);
		float2 uv_temp = uv * 3.2 * (1.0 - diss.x);
		uv_temp.x += (diss.a * 0.8) + (agk_time * diss.a * 0.1);
		uv_temp.y += agk_time * 0.5 * diss.y * timeoff;
		float3 ddt = texture4.Sample(samplerLinearWrap, uv_temp).rgb;
		uof = (ddt.xy-0.5)*0.3*diss.x;
	}
	
	float4 cl = 0.0;
	if ( an < 0.5 ) 
	{
		cl = texture1.Sample(samplerLinear, uv+uof);
	} 
	else 
	{
		cl = texture3.Sample(samplerLinear, uv);

		if ( tile.a > 0.5 ) 
		{
			cl = lerp( cl, texture3.Sample(samplerLinear, uvb), pbl );
		}
	}
	
	if ( moder == 2.0 && (cl.a * IN.alphaVarying) < 0.12 ) discard;
	if ( moder == 4.0 ) cl.rgb *= IN.alphaVarying * cl.a;
	
	float4 ou = float4( IN.colVarying * cl.rgb, 1.0 );
	if ( moder > 2.0 ) ou = float4( IN.colVarying * cl.rgb * clr, cl.a * IN.alphaVarying );
	ou.w = ou.w * opacity;
	return ou;
}

