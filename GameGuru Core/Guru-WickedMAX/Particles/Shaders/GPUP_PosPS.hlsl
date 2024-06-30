
#define pi 3.1415926
#define pi2 6.2831853

Texture2D texture0 : register( t0 ); // pos
Texture2D texture1 : register( t1 ); // speed
SamplerState samplerLinear : register( s0 );
SamplerState samplerPoint : register( s1 );

struct PixelIn
{
	float2 uvVarying : TEXCOORD0;
};

cbuffer constants : register( b0 )
{
	float4 gravity;
	float4 emittersize;
	float4 emitterrotation;
	float3 emitterline;
	float emittertype;
	float3 emittersway;
	float agk_time;
	float3 localemitter;
	float warp;
	float3 pos1;
	float rnd;
	float3 pos2;
	float3 emitterlinecount;
	float3 moveit;
	float3 particles;
	float3 area;
	float2 spawnpos;
	float2 lifespan;
}

float3x3 rot(float3 ang) 
{
	float3x3 x = float3x3( 1.0, 0.0, 0.0, 0.0, cos(ang.x), -sin(ang.x), 0.0, sin(ang.x), cos(ang.x) );
	float3x3 y = float3x3( cos(ang.y), 0.0, sin(ang.y), 0.0, 1.0, 0.0, -sin(ang.y), 0.0, cos(ang.y) );
	float3x3 z = float3x3( cos(ang.z), -sin(ang.z), 0.0, sin(ang.z), cos(ang.z), 0.0, 0.0, 0.0, 1.0 );
	return x * y * z;
}

static const float4 enco = float4( 1.0, 255.0, 65025.0, 16581375.0 );
static const float4 decco = 1.0 / enco;

float4 fl4pack( float v ) 
{
	float4 enc = enco * v;
	enc = frac(enc);
	enc -= enc.yzww * float2(1.0/255.0, 0.0).xxxy;
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

float random1 (float4 seed) 
{
	float dt = dot(seed, float4(a,b,c,d));
	dt = dt - pi2 * floor(dt / pi2);
	return frac(abs(sin(dt)) * e) - 0.5;
}

float4 main( PixelIn IN ) : SV_TARGET
{
	float2 uv = IN.uvVarying;
	float2 uv2 = frac( uv * 2.0 );
	float fr = floor( uv2.x * particles.x );
	fr += floor( uv2.y * particles.x ) * particles.x;
	float3 vc = 0.0;
	float3 spd = 0.0;
	float2 uv3 = uv2 * 0.5 + float2(0.5,0.5);
	float val2 = fl4unpack( texture0.Sample(samplerPoint, uv3) );
	float area2 = area.x * 2.0;
	float4 outer = 0.0;
	float val = 0.0;
	float ttp = 0.0;
	
	if ( spawnpos.x > -0.3 
	     && ( (fr >= spawnpos.x - 0.2 && fr <= spawnpos.y + 0.2) 
	          || (spawnpos.y > particles.z && fr <= spawnpos.y - particles.y) 
	        ) 
	     ) 
	{
		if ( emittertype == 0  )
		{
			vc.x = random1( float4(uv2.x*0.8+rnd, uv2.y-rnd*1.5, uv2.x+uv2.y+rnd*0.4, uv2.y-rnd*0.3) );
			vc.y = random1( float4(uv2.y*0.6+rnd, uv2.x+rnd*1.7, uv2.y-uv2.x+rnd*0.6, uv2.x+rnd*0.55) );
			vc.z = random1( float4(-uv2.x*0.55+rnd, uv2.x-rnd*0.9, uv2.x-uv2.y+rnd*0.45, uv2.y-rnd*0.4) );
			vc *= emittersize.rgb * 2.0;

			if (emitterrotation.a > 0.5) 
			{
				float4 ttpvec;
				ttpvec.x = uv2.y * 0.3 + rnd;
				ttpvec.y = uv2.x - rnd * 1.4;
				ttpvec.z = uv2.x + uv2.y + rnd * 0.24;
				ttpvec.w = uv2.x - rnd * 0.4;
				ttp = random1( ttpvec ) + 0.5;
				
				float vcx = ( step(0.0, vc.x) * 2.0 - 1.0 ) * emittersize.x;
				float vcy = ( step(0.0, vc.y) * 2.0 - 1.0 ) * emittersize.y;
				float vcz = ( step(0.0, vc.z) * 2.0 - 1.0 ) * emittersize.z;
				vc.x = lerp( vc.x, vcx, 1.0-step(0.66,ttp) );
				vc.y = lerp( vc.y, vcy, step(0.33,ttp) );
				vc.z = lerp( vc.z, vcz, step(0.66,ttp) + 1.0 - step(0.33,ttp) );
			}
			
			if ( gravity.a > 0.5 ) 
			{
				float4 ttpvec;
				ttpvec.x = uv2.y * 0.3 + rnd;
				ttpvec.y = uv2.x - rnd * 1.4;
				ttpvec.z = uv2.x + uv2.y + rnd * 0.24;
				ttpvec.w = uv2.x - rnd * 0.4;
				ttp = random1( ttpvec ) + 0.5;
				
				if ( vc.x < 0.0 ) vc.x = -emittersize.x + vc.x * 0.1;
				else vc.x = emittersize.x + vc.x * 0.1;
				
				if ( vc.y < 0.0 ) vc.y = -emittersize.y + vc.y * 0.1;
				else vc.y = emittersize.y + vc.y * 0.1;
				
				if ( vc.z < 0.0 ) vc.z = -emittersize.z + vc.z * 0.1;
				else vc.z = emittersize.z + vc.z * 0.1;
			}

			// original			
			//float3x3 rota = rot(emitterrotation.rgb);
			//vc = mul(rota, vc) + localemitter;
			//lees fix for rings/cubes/etc
			float3 ang = emitterrotation.rgb;
			float3x3 rotxx = float3x3(1.0, 0.0, 0.0, 0.0, cos(ang.x), -sin(ang.x), 0.0, sin(ang.x), cos(ang.x));
			float3x3 rotyy = float3x3(cos(ang.y), 0.0, sin(ang.y), 0.0, 1.0, 0.0, -sin(ang.y), 0.0, cos(ang.y));
			float3x3 rotzz = float3x3(cos(ang.z), -sin(ang.z), 0.0, sin(ang.z), cos(ang.z), 0.0, 0.0, 0.0, 1.0);
			vc = mul(rotxx, vc);
			vc = mul(rotyy, vc);
			vc = mul(rotzz, vc);
			vc = vc + localemitter;
			val2 = 0.998;
		}

		if ( emittertype == 1  )
		{
			float4 cirvec;
			cirvec.x = uv2.x * 0.8 + rnd;
			cirvec.y = uv2.y - rnd * 1.5;
			cirvec.z = uv2.x + uv2.y + rnd * 0.4;
			cirvec.w = uv2.y - rnd * 0.3;
			
			float4 cirvec2;
			cirvec2.x = uv2.y * uv2.x - rnd;
			cirvec2.y = uv2.x + rnd;
			cirvec2.z = uv2.y;
			cirvec2.w = rnd;
			
			float cir = random1( cirvec ) * pi2;
			cir += random1( cirvec2 ) * 0.06;
			
			float4 rdvec;
			rdvec.x = uv2.y * 0.6 + rnd;
			rdvec.y = uv2.x - rnd * 1.7;
			rdvec.z = uv2.y + uv2.y + rnd * 0.35;
			rdvec.w = uv2.x - rnd * 0.22;
			
			float rd = random1(rdvec) + 0.5;
			rd *= (emittersize.y - emittersize.x);
			rd += emittersize.x;
			
			float4 vcvec;
			vcvec.x = -uv2.y * 0.44 + rnd;
			vcvec.y = uv2.x - rnd * 1.5;
			vcvec.z = uv2.y - uv2.x + rnd * 0.6;
			vcvec.w = uv2.x - rnd * 0.5;
			
			vc = float3( sin(cir) * rd, 0.0, cos(cir) * rd );
			vc.y += random1(vcvec) * emittersize.z * 2.0;
			
			//flawed!
			//float3x3 rota = rot(emitterrotation.rgb);
			//vc = mul(rota, vc) + localemitter;
			float3 ang = emitterrotation.rgb;
			float3x3 rotxx = float3x3(1.0, 0.0, 0.0, 0.0, cos(ang.x), -sin(ang.x), 0.0, sin(ang.x), cos(ang.x));
			float3x3 rotyy = float3x3(cos(ang.y), 0.0, sin(ang.y), 0.0, 1.0, 0.0, -sin(ang.y), 0.0, cos(ang.y));
			float3x3 rotzz = float3x3(cos(ang.z), -sin(ang.z), 0.0, sin(ang.z), cos(ang.z), 0.0, 0.0, 0.0, 1.0);
			vc = mul(rotxx, vc);
			vc = mul(rotyy, vc);
			vc = mul(rotzz, vc);
			vc = vc + localemitter;
			val2 = 0.99;
		}

		if ( emittertype == 2 ) 
		{
			float4 ravec;
			ravec.x = uv2.x * 1.8 - rnd;
			ravec.y = uv2.y + rnd * 1.1;
			ravec.z = uv2.x - uv2.y + rnd * 0.33;
			ravec.w = uv2.y + rnd * 0.4;
			
			float ra = random1( ravec ) + 0.5;
			
			float4 vcrnd;
			vcrnd.x = uv2.x * 0.8 + rnd;
			vcrnd.y = uv2.y - rnd * 1.5;
			vcrnd.z = uv2.x + uv2.y + rnd * 0.4;
			vcrnd.w = uv2.y - rnd * 0.3;
			
			vc.x = random1( vcrnd );
			
			vcrnd.x = uv2.x * 0.7 - rnd;
			vcrnd.y = uv2.y + rnd * 1.8;
			vcrnd.z = uv2.y - uv2.x + rnd * 0.7;
			vcrnd.w = uv2.x + rnd * 0.5;
			
			vc.y = random1( vcrnd );
			
			vcrnd.x = uv2.y * 0.6 + rnd * 1.3;
			vcrnd.y = uv2.x - rnd * 1.2;
			vcrnd.z = uv2.x - uv2.y + rnd * 0.33;
			vcrnd.w = uv2.y + rnd * 0.4;
			
			vc.z = random1( vcrnd );
			vc = normalize( vc );
			vc *= ( emittersize.x + ra * (emittersize.y - emittersize.x) );
			vc += localemitter;
			
			val2 = 0.998;
		}

		if ( emittertype == 3 ) 
		{
			float4 ra2vec;
			ra2vec.x = uv2.x * 3.3 + rnd;
			ra2vec.y = uv2.y - (rnd * 2.5);
			ra2vec.z = uv2.x + (uv2.y * 1.4) + (rnd * 0.6);
			ra2vec.w = uv2.y - (rnd * 6.22);
			
			float4 ra2vec2;
			ra2vec2.x = uv2.y * uv2.x - rnd;
			ra2vec2.y = uv2.x + rnd;
			ra2vec2.z = uv2.y * 5.5;
			ra2vec2.w = rnd * 23.6;
			
			float ra2 = random1(ra2vec);
			ra2 += random1(ra2vec2) * 0.04 + 0.5;
			ra2 = clamp(ra2, 0.0, 1.0);
			
			float ra3 = ra2 * 3.1415926;
			float pir = 3.1415926 * ra2;
			float er = emitterlinecount.x * pir;
			
			float3 ddr = normalize( pos1 - pos2 );
			float3 ddr2 = cross( ddr, float3(0,-1,0) );
			float3 ddr3 = cross( ddr, ddr2 );
			
			float linenr = floor( uv2.x * emitterlinecount.y );
			float rou = agk_time * emittersway.x * 5.0;
			rou += linenr * emitterlinecount.z * 3.1415926;
			rou += er;
			
			float es = (emittersway.z + 0.5) * ra2;
			float rr1 = sin( rou + es + er + linenr*pir );
			float rr2 = cos( rou + es + er + linenr*pir );
			float wa = es * pi2;
			wa += agk_time * emittersway.x * 8.0;
			wa = sin( wa );
			
			vc =  (pos1 - pos2) * ra2 + pos2;
			vc += ddr2 * emittersway.y * sin(ra3) * rr1 * wa; 
			vc += ddr3 * emittersway.y * sin(ra3) * rr2 * wa;
			vc += localemitter;
			val2 = 0.99;
		}
	} 
	else 
	{
		if ( val2 > 0.00001 ) 
		{
			float2 uv2modx = uv2 * 0.5;
			float2 uv2mody = uv2 * 0.5 + float2(0.5,0.0);
			float2 uv2modz = uv2 * 0.5 + float2(0.0,0.5);
			
			float xx = fl4unpack( texture0.Sample(samplerPoint, uv2modx) ) - 0.5;
			float yy = fl4unpack( texture0.Sample(samplerPoint, uv2mody) ) - 0.5;
			float zz = fl4unpack( texture0.Sample(samplerPoint, uv2modz) ) - 0.5;
			
			vc = float3(xx,yy,zz) * area2;
			
			float sx = fl4unpack( texture1.Sample(samplerPoint, uv2modx) ) - 0.5;
			float sy = fl4unpack( texture1.Sample(samplerPoint, uv2mody) ) - 0.5;
			float sz = fl4unpack( texture1.Sample(samplerPoint, uv2modz) ) - 0.5;
			
			spd = float3(sx,sy,sz) * 0.1;
			
			vc += (spd * warp) + moveit;
			
			float4 agerrnd = float4( uv.x, uv.y, -uv.y, -uv.x );
			float interp = random1( agerrnd ) + 0.5;
			interp *= lifespan.g;
			
			float ager = lerp( lifespan.r, lifespan.r * 0.2, interp );
			ager = 0.998 / ager;
			ager = val2 - (ager * warp);
			val2 = max( 0.0, ager );
		}
	}
	
	vc /= area2;
	vc = clamp( vc, -0.49999, 0.49999 );
	
	if (uv.x < 0.5 )
	{
		if ( uv.y < 0.5 ) val = vc.x + 0.5;
		else val = vc.z + 0.5;
	}
	else
	{
		if ( uv.y < 0.5 ) val = vc.y + 0.5;
		else val = val2;
	}
	
	return fl4pack( val );
}
