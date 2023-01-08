
#define pi2 6.2831853

static const float4 enco = float4( 1.0, 255.0, 65025.0, 16581375.0 );
static const float4 decco = 1.0 / enco;

Texture2D texture0 : register( t0 ); // pos
Texture2D texture1 : register( t1 ); // speed
Texture2D texture2 : register( t2 ); // emitter_noise
Texture2D texture3 : register( t3 ); // t_field
SamplerState samplerLinear : register( s0 );
SamplerState samplerPoint : register( s1 );
SamplerState samplerLinearWrap : register( s2 );

struct PixelIn
{
	float2 uvVarying : TEXCOORD0;
};

cbuffer constants : register( b0 )
{
	float4 area;
	float4 gravity;
	float4 speedvar;
	float4 field;
	float4 autorot;
	float4 reffloor;
	float4 refsphere;
	float3 particles;
	float rnd;
	float2 spawnpos;
	float warp;
	float rnd2;
}

float4 fl4pack( float v ) 
{
	float4 enc = enco * v;
	enc = frac(enc);
	enc -= enc.yzww * float2(1./255., 0.).xxxy;
	return enc;
}

float fl4unpack( float4 v ) 
{
	v = floor(v * 255.0 + 0.5) / 255.0;
	return dot(v, decco);
}

static const float a = 14.3883;
static const float b = 91.428;
static const float c = 57.836;
static const float d = 121.403;
static const float e = 75633.244;

float random1( float4 seed ) 
{
	float dt = dot( seed, float4(a,b,c,d) );
	dt = dt - pi2 * floor(dt / pi2);
	dt = abs( sin(dt) );
	return frac( dt * e ) - 0.5;
}

float3x3 rot( float3 ang ) 
{
	float3x3 x = float3x3( 1.0, 0.0, 0.0, 0.0, cos(ang.x), -sin(ang.x), 0.0, sin(ang.x), cos(ang.x) );
	float3x3 y = float3x3( cos(ang.y), 0.0, sin(ang.y), 0.0, 1.0, 0.0, -sin(ang.y), 0.0, cos(ang.y) );
	float3x3 z = float3x3( cos(ang.z), -sin(ang.z), 0.0, sin(ang.z), cos(ang.z), 0.0, 0.0, 0.0, 1.0 );
	return x * y * z;
}

float4 main( PixelIn IN ) : SV_TARGET
{
	float2 uv = IN.uvVarying;
	float2 uv2 = frac( uv * 2.0 );
	float2 uv2h = uv2 * 0.5;
	
	float fr = floor( uv2.x * particles.x );
	fr += floor( uv2.y * particles.x ) * particles.x;
		
	float3 spd = 0;
	float4 noiser = 0;
	
	float2 uv2hmod = uv2h + float2(0.5,0.5);
	float val2 = fl4unpack( texture0.Sample(samplerPoint, uv2hmod) );
	
	if ( spawnpos.x > -0.3 
	   && ( (fr >= spawnpos.x - 0.2 && fr <= spawnpos.y + 0.2) 
	      || (spawnpos.y > particles.z && fr <= spawnpos.y - particles.y)
	      ) 
	   ) 
	{
		float4 spdrnd;
		spdrnd.x = uv2.x * 0.8 + rnd;
		spdrnd.y = uv2.y - (rnd * 1.5);
		spdrnd.z = uv2.x + (uv2.y * 0.4) + (rnd2 * 0.4);
		spdrnd.w = uv2.y - (rnd * 0.3);
		
		spd.x = random1( spdrnd );
		
		spdrnd.x = uv2.y * 1.8 + rnd2;
		spdrnd.y = uv2.x - (rnd * 1.2);
		spdrnd.z = uv2.x + uv2.y + (rnd * 0.6);
		spdrnd.w = uv2.y + (rnd2 * 0.5);

		spd.y = random1( spdrnd );
		
		spdrnd.x = uv2.x * 0.37 + (rnd2 * 0.7);
		spdrnd.y = uv2.y - (rnd * 1.1);
		spdrnd.z = uv2.x - uv2.y + (rnd * 0.7);
		spdrnd.w = uv2.x + (rnd * 0.8);

		spd.z = random1( spdrnd );
		
		spd = normalize( spd );
		
		spdrnd.x = uv2.x * 1.4 + rnd2;
		spdrnd.y = uv2.y - (rnd2 * 1.7);
		spdrnd.z = uv2.x - uv2.y + (rnd2 * 0.9);
		spdrnd.w = uv2.x + (rnd2 * 0.7);
		
		spd = spd * (random1(spdrnd) + 0.5);
		
		float speedvarlen = length( speedvar.rgb );
		speedvarlen = max(speedvarlen, 0.1);
		float3 spdtmp = speedvar.rgb + (spd * speedvarlen);
		
		spd = lerp( speedvar.rgb, spdtmp, speedvar.a );
	} 
	else 
	{
		if ( val2 > 0.00001 ) 
		{
			float2 uv2hyy = uv2h + float2(0.5, 0.0);
			float2 uv2hzz = uv2h + float2(0.0, 0.5);
			
			float xx = fl4unpack(texture0.Sample(samplerPoint, uv2h)) - 0.5;
			float yy = fl4unpack(texture0.Sample(samplerPoint, uv2hyy)) - 0.5;
			float zz = fl4unpack(texture0.Sample(samplerPoint, uv2hzz)) - 0.5;
			
			float3 rotvec = autorot.xyz * 0.1 * rnd;
			float3x3 rota = rot( rotvec );
			
			float3 vc = mul( rota, float3(xx,yy,zz) );
			vc = vc * area.x * 2.0;
			vc = vc / (area.a * 2.0);
			vc = vc + float3( 0.5, 0.5, 0.5 );
			
			float3 vc2 = float3(xx,yy,zz) * area.x * 2.0;
			vc2 = clamp( vc2, float3(-area.y,-area.y,-area.y), float3(area.y,area.y,area.y) );
			vc2 = vc2 / (area.y * 2.0);
			vc2 = vc2 + float3( 0.5, 0.5, 0.5 );
						
			float dv = 1.0 / area.z;
			
			float zoff2 = floor( vc2.z * (area.z - 0.00001) );
			float nfr2 = frac( vc2.z * (area.z-0.00001) );
			
			float2 nuvf;
			nuvf.x = (vc2.x * dv) + (zoff2 * dv);
			nuvf.y = vc2.y;
			
			float2 nuvf2;
			nuvf2.x = nuvf.x + dv;
			nuvf2.y = nuvf.y;
			
			float3 vfield = texture3.Sample(samplerLinear, nuvf ).rgb;
			float3 vfield2 = texture3.Sample(samplerLinear, nuvf2 ).rgb;
			
			vfield = lerp( vfield, vfield2, nfr2 );
			vfield = vfield * 2.0 - 0.99609375;
			
			float zoff = floor(vc.z * (area.z - 0.00001) );
			float nfr = frac(vc.z * (area.z - 0.00001) );
			
			float2 nuv;
			nuv.x = (vc.x * dv) + (zoff * dv);
			nuv.y = vc.y;
			
			float2 nuv2;
			nuv2.x = nuv.x + dv;
			nuv2.y = nuv.y;
			
			noiser = texture2.Sample(samplerLinearWrap, nuv );
			float4 noiser2 = texture2.Sample(samplerLinearWrap, nuv2 );
			noiser = lerp( noiser, noiser2, nfr );
			
			float interp = sin(autorot.a * rnd);
			interp = abs( interp );
			noiser.rgb = lerp( noiser.rgb, noiser.abr, interp );
			noiser.rgb = noiser.rgb * 2.0 - 1.0;

			if ( gravity.a > 0.0 ) 
			{
				vc -= 0.5;
				vc *= 2.244;
				vc += 0.5;
				
				zoff = floor( vc.z * (area.z - 0.00001) );
				nfr = frac( vc.z * (area.z - 0.00001) );
				
				nuv.x = (vc.x * dv) + (zoff * dv);
				nuv.y = vc.y;
				
				nuv2.x = nuv.x + dv;
				nuv2.y = nuv.y;
				
				noiser2 = texture2.Sample(samplerLinearWrap, nuv );
				float4 noiser3 = texture2.Sample(samplerLinearWrap, nuv2 );
				noiser2 = lerp( noiser2, noiser3, nfr );
				
				interp = sin(autorot.a * rnd);
				interp = abs( interp );
				noiser2.rgb = lerp( noiser2.rgb, noiser2.abr, interp );
				noiser2.rgb = noiser2.rgb * 2.0 - 1.0;
				noiser += noiser2 * gravity.a * 16.0;
			}

			float sx = fl4unpack( texture1.Sample(samplerPoint, uv2h) ) - 0.5;
			float sy = fl4unpack( texture1.Sample(samplerPoint, uv2hyy) ) - 0.5;
			float sz = fl4unpack( texture1.Sample(samplerPoint, uv2hzz) ) - 0.5;
			
			spd = float3(sx,sy,sz) * 0.1;
			
			float fld = 1.0;
			float flo = (reffloor.y - 0.5) / area.x * area.y;
			if ( reffloor.x > 0.5 && yy < flo && abs(flo - yy) < 0.2 ) 
			{
				if ( spd.y <= 0.0 ) 
				{
					spd.y = -spd.y;
					spd *= reffloor.a;
				}
				fld = 0.0;
			}
			
			float3 sph = (refsphere.yzw - 0.5) / area.x * area.y;
			float spr = reffloor.z / (area.x * area.y);
			float3 vpo = normalize( float3(xx,yy,zz) - sph );
			float dii = 999.0;
			if ( refsphere.x > 0.5 ) 
			{
				dii = distance( float3(xx,yy,zz), sph );
				if ( dot(spd, vpo) <= 0.0 ) 
				{
					if ( dii < spr && dii > spr * 0.9 ) spd = reflect(spd, vpo) * reffloor.a;
				}
			}
			spd = lerp( spd, noiser.rgb * field.rgb * 0.005, field.a );
			spd += vfield * 0.002;
			if ( dii > spr * 1.001 ) spd += gravity.rgb * 0.05 * fld * warp;
		}
	}
	
	float val = 0.0;
	spd = clamp(spd * 10.0, -0.49999, 0.49999);
	
	if ( uv.x < 0.5 )
	{
		if ( uv.y < 0.5 ) val = spd.x + 0.5;
		else val = spd.z + 0.5;
	}
	else
	{
		if ( uv.y < 0.5 ) val = spd.y + 0.5;
		else val = noiser.a;
	}
	
	return fl4pack( val );
}
