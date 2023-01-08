
#define pi2 6.2831853

Texture2D texture0 : register( t0 ); // pos
Texture2D texture2 : register( t2 ); // image1
Texture2D texture5 : register( t5 ); // speed
SamplerState samplerPoint : register( s1 );

struct VertexIn
{
    float3 position : POSITION;
	float2 uv : UV;
};

struct VertexOut
{
	float2 uv0Varying : TEXCOORD0;
	float2 tpVarying : TEXCOORD1;
	float4 distVarying : TEXCOORD2;
	float3 colVarying : TEXCOORD3;
	float alphaVarying : TEXCOORD4;
	float3 posVarying : TEXCOORD5;
	float4 finalPos : SV_POSITION;
};

cbuffer constants : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
	float4x4 ViewProj;
	float4 globalpos[8];
	float4 tilex;
	float4 particles;
	float3 pgrow;
	float padding1; //PE: Added
	float3 ppos;
	float padding2; //PE: Added
	float3 area;
	float padding3; //PE: Added
	float3 rotat;
	float padding4; //PE: Added
	float3 rota;
	float padding5; //PE: Added
	float3 image_count;
	float padding6; //PE: Added
	float3 globalsize;
	float padding7; //PE: Added
	float3 globalrot;
	float padding8; //PE: Added
	float3 CameraPos;
	float padding9; //PE: Added
	float2 pcolor;
	float padding10; //PE: Added
	float padding11; //PE: Added
	//84 float4 = 336 floats (1344 bytes) , need 1,536 for aligning , missing 192/4 = 48 floats.
	float padding48[48];
}

static const float4 dec = float4( 1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0 );

float fl4unpack( float4 v ) 
{
	v = floor(v * 255.0 + 0.5) / 255.0;
	return dot( v, dec );
}

float fl2unpack( float4 rgba ) 
{
	return dot( rgba, dec );
}

static const float a = 14.3883;
static const float b = 91.428;
static const float c = 57.836;
static const float d = 121.403;
static const float e = 75633.244;

float random1( float4 seed ) 
{
	float dt = dot( seed, float4(a,b,c,d) );
	dt = dt - pi2 * floor(dt/pi2);
	return frac(abs(sin(dt)) * e) - 0.5;
}

float3x3 rot( float3 ang ) 
{
	float3x3 x = float3x3( 1.0, 0.0, 0.0, 0.0, cos(ang.x), -sin(ang.x), 0.0, sin(ang.x), cos(ang.x) );
	float3x3 y = float3x3( cos(ang.y), 0.0, sin(ang.y), 0.0, 1.0, 0.0, -sin(ang.y), 0.0, cos(ang.y) );
	float3x3 z = float3x3( cos(ang.z), -sin(ang.z), 0.0, sin(ang.z), cos(ang.z), 0.0, 0.0, 0.0, 1.0 );
	return x * y * z;
}

float2 rot2d( float2 v, float a ) 
{
	float s = sin( a );
	float c = cos( a );
	float2x2 m = float2x2( c, -s, s, c );
	return mul(m, v);
}

VertexOut main( VertexIn IN )
{	
	VertexOut OUT;

	OUT.uv0Varying = IN.uv;
	float2 uvs = IN.uv - 0.5;

	/*
	float4 newPos = float4( IN.position, 1 );
	newPos.x += uvs.x*10.0;
	newPos.z += uvs.y*10.0;
	newPos = mul( World, newPos );
	newPos.xyz += globalpos[0].xyz;
	newPos = mul( View, newPos );
	newPos = mul( Proj, newPos );
	OUT.finalPos = newPos;
	return OUT;
	*/

	//I am now using World (which did nothing, just scale to 1.0) for a real world global rotation below!
	//float4x4 WorldForTranslation = World;
	//WorldForTranslation[0].xyzw = float4(1, 0, 0, 0);
	//WorldForTranslation[1].xyzw = float4(0, 1, 0, 0);
	//WorldForTranslation[2].xyzw = float4(0, 0, 1, 0);
	//float4 posit = mul(WorldForTranslation, float4( IN.position, 1 ) );
	//float4x4 WorldForRotation = World;
	//WorldForRotation[3].xyzw = float4(0, 0, 0, 1);
	float4 posit = mul(World, float4( IN.position, 1 ) );

	float2 uv2;
	uv2.x = posit.x / particles.x * 0.5;
	uv2.y = posit.z / particles.x * 0.5;
	float subemittercount = globalpos[0].a;
	float modx = uv2.x*particles.x;
	int le = int( modx - subemittercount * floor(modx/subemittercount) );
	
	float3 globalpos2 = globalpos[le].rgb;
	
	float age = fl4unpack( texture0.SampleLevel( samplerPoint, uv2+float2(0.5,0.5), 0 ) );
	float xx = fl4unpack( texture0.SampleLevel( samplerPoint, uv2, 0 ) ) - 0.5;
	float yy = fl4unpack( texture0.SampleLevel( samplerPoint, uv2+float2(0.5,0.0), 0 ) ) - 0.5;
	float zz = fl4unpack( texture0.SampleLevel( samplerPoint, uv2+float2(0.0,0.5), 0 ) ) - 0.5;
	float uv3 = clamp( uv2.x, 0.0001, 0.49999 );

	OUT.tpVarying.x = random1( float4(uv3*8.0, uv3*5.0, image_count.x*2.0, 3.0*uv3) ) + 0.5;
	OUT.tpVarying.x = OUT.tpVarying.x * image_count.x * 0.999;
	OUT.tpVarying.y = 0.995 - (age * 0.99);

	OUT.distVarying.xy = image_count.yz;
	OUT.distVarying.z = random1( float4(uv2.x*0.7, uv2.y*0.44, uv2.x*0.3-uv2.y, uv2.x*0.1) );
	OUT.distVarying.w = random1( float4(uv2.y*0.35, uv2.x*0.76, uv2.x*0.22+uv2.y*0.6, uv2.y*0.4) );

	OUT.colVarying = texture2.SampleLevel( samplerPoint, float2(0.995-age*0.99, 0.1), 0 ).rgb;
	OUT.alphaVarying = texture2.SampleLevel( samplerPoint, float2(0.995-age*0.99, 0.5), 0 ).r;

	float sizegrad = max( texture2.SampleLevel( samplerPoint, float2(0.995-age*0.99, 0.9), 0 ).r, 0.0039);
	float sizer = 0.1;
	float3 posi = 0.0;
	if ( age>0.00001 ) 
	{
		posi = float3( xx*2.0*area.x+ppos.x, yy*2.0*area.x+ppos.y, zz*2.0*area.x+ppos.z );
		sizer = lerp(pgrow.y, pgrow.x,age) * sizegrad * pgrow.z;
	} 
	else 
	{
		posi = posit.rgb-float3(0.0, -15000.0, 0.0);
		sizer = 0.0;
	}
	
	float4 pos = float4( posi, 1 );
	float4 pos2 = mul( View, pos );
	OUT.posVarying = pos2.xyz;
	float ang = 0.0;
	float ang2 = 0.0;
	float ang3 = 0.0;
	float3 cpy = float3( CameraPos.x, pos.y, CameraPos.z );
	float3x3 glr = rot( globalrot );

	if ( particles.a == 0 )
	{
		float vr = random1( float4( uv2.x, uv2.y*0.3, uv2.x+uv2.y, uv2.y*1.5-uv2.x*0.7 ) );
		ang = rota.b * (rota.r + vr*rota.g);
		float3 temp = mul(glr, posi) * globalsize + globalpos2;
		// LB: transform to world global rotation first!
		//temp = mul(WorldForRotation, float4(temp, 1 ) ).xyz;

		pos2 = mul( View, float4( temp, 1 ) );
		pos2 += float4( rot2d(float2(uvs.x*sizer, -uvs.y*sizer), ang)*globalsize.x, 0.0, 0.0);
		pos2 = mul( Proj, pos2 );
	}

	if ( particles.a == 1 ) 
	{
		float3 of = cross( normalize(pos.xyz-cpy), float3(0.0, 1.0, 0.0) );
		
		float3 temp_pos;
		temp_pos.x = of.x * sizer * uvs.x;
		temp_pos.y = -sizer * uvs.y;
		temp_pos.z = of.z * sizer * uvs.x;

		temp_pos *= globalsize;
		temp_pos += mul(glr, pos.xyz) * globalsize + globalpos2;
		// LB: transform to world global rotation first!
		//temp_pos = mul(WorldForRotation, float4(temp_pos, 1)).xyz;

		pos2 = mul( ViewProj, float4( temp_pos, 1 ) );
	}

	if ( particles.a == 2 ) 
	{
		float sx = fl2unpack( texture5.SampleLevel( samplerPoint, uv2, 0 ) ) - 0.5;
		float sy = fl2unpack( texture5.SampleLevel( samplerPoint, uv2+float2(0.5,0.0), 0 ) ) - 0.5;
		float sz = fl2unpack( texture5.SampleLevel( samplerPoint, uv2+float2(0.0,0.5), 0 ) ) - 0.5;
		float3 dir = -normalize( float3(sx,sy,sz) );

		float vel = length( float3(sx,sy,sz) );
		vel *= 7.0;
		vel = max( vel, 0.5 );

		float3 side = normalize( pos.xyz - CameraPos );
		side = cross( dir, side );

		if ( tilex.x < 0.5 ) 
		{
			posi += dir * sizer * uvs.y * vel;
			posi += side * sizer * uvs.x;
		} 
		else 
		{
			float bop = 1.0;
			if ( dot(side, float3(0,-1,0)) < 0.0 ) bop = -1.0;
			posi += dir * sizer * -uvs.x * vel;
			posi += side * sizer * uvs.y * bop;
		}

		float3 temp_pos = mul(glr, posi) * globalsize;
		temp_pos += globalpos2;
		// LB: transform to world global rotation first!
		//temp_pos = mul(WorldForRotation, float4(temp_pos, 1)).xyz;

		pos2 = mul( ViewProj, float4( temp_pos, 1 ) );
	}

	if ( particles.a == 3 ) 
	{
		ang = rota.b * (rota.r + rota.g);
		ang *= random1( float4( uv2.x*0.6, uv2.y*0.15, uv2.x-uv2.y, uv2.y*1.5-uv2.x*0.7 ) );

		ang2 = rota.b * (rota.r + rota.g);
		ang2 *= random1( float4( uv2.y, uv2.x*0.4, uv2.x+uv2.y*0.5, uv2.y*1.1-uv2.x*0.4 ) );

		ang3 = rota.b * (rota.r + rota.g);
		ang3 *= random1( float4( uv2.x, uv2.y*0.3, uv2.x+uv2.y, uv2.y*1.5-uv2.x*0.7 ) );

		float3 uvoff = float3( uvs.x*sizer, 0.0, uvs.y*sizer );
		pos.xyz += mul( rot( float3(ang,ang2,ang3) ), uvoff );

		float3 temp_pos = mul(glr, pos.xyz) * globalsize;
		temp_pos += globalpos2;
		// LB: transform to world global rotation first!
		//temp_pos = mul(WorldForRotation, float4(temp_pos, 1)).xyz;

		pos2 = mul( ViewProj, float4( temp_pos, 1 ) );
	}

	if ( particles.a == 4 ) 
	{
		float2 uof;
		uof.x = random1( float4( uv2.x, uv2.y*0.3, uv2.x+uv2.y, uv2.y*1.5-uv2.x*0.7 ) );
		uof.y = random1( float4( uv2.y*0.15, uv2.x*0.4, uv2.x-uv2.y, uv2.y*1.2+uv2.x*0.3 ) );
		uof.y *= (3.0 / particles.x);

		float2 uv4 = clamp( uv2 + uof, 0.0, 0.499 );

		float xx2 = fl4unpack( texture0.SampleLevel( samplerPoint, uv4, 0 ) ) - 0.5;
		float yy2 = fl4unpack( texture0.SampleLevel( samplerPoint, uv4+float2(0.5,0.0), 0 ) ) - 0.5;
		float zz2 = fl4unpack( texture0.SampleLevel( samplerPoint, uv4+float2(0.0,0.5), 0 ) ) - 0.5;
		float age2 = fl4unpack( texture0.SampleLevel( samplerPoint, uv4+float2(0.5,0.5), 0 ) );

		float3 posi2;
		posi2.x = (xx2 * 2.0 * area.x) + ppos.x;
		posi2.y = (yy2 * 2.0 * area.x) + ppos.y;
		posi2.z = (zz2 * 2.0 * area.x) + ppos.z;

		float dist = distance( pos.xyz, posi2 );
		float dist2 = area.x * pgrow.y * 0.02;
		if ( dist < dist2 && age2 > 0.001 ) 
		{
			pos.xyz = lerp( pos.xyz, posi2, uvs.y + 0.5 );
			float3 temp_pos = cross( normalize(pos.xyz - cpy), normalize(pos.xyz - posi2) );
			temp_pos *= uvs.x * pgrow.x * 0.1;
			pos.xyz += temp_pos;

			temp_pos = mul(glr, pos.xyz) * globalsize;
			temp_pos += globalpos2;
			// LB: transform to world global rotation first!
			//temp_pos = mul(WorldForRotation, float4(temp_pos, 1)).xyz;

			pos2 = mul( ViewProj, float4( temp_pos, 1 ) );
		}
	}
	
	OUT.finalPos = pos2;
	return OUT;
}
