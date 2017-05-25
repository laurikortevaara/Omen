// Utility functions

#include "Complex.glsl"

#ifndef tex
#define tex

#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_1_SQRTPI 0.564189583547756286948  // 1/sqrt(pi)
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
#define M_SQRT_2PI 2.506628274631000502415  // sqrt(2*pi)

// Definition of a Sphere
struct Sphere {
	vec3 position;
	float radius;
};

// Definition of a Ray
struct Ray {
	vec3 origin;
	vec3 direction;
};

// Definition of a Plane
struct Plane {
	vec3 position;
	vec3 normal;
};

struct Intersection{
	float t;
	float hit;
	vec3  hitPoint;
	vec3  normal;
};

float fmod(float x, float y)
{
  return x - y * floor(x/y);
}

vec2 fmod(vec2 v1, vec2 v2)
{
  return vec2(fmod(v1.x,v2.x), fmod(v1.y,v2.y));
}

/**
   Ray Plane intersection
**/
void intersectPlane(Ray ray, Plane p, inout Intersection i){
    float d = -dot(p.position, p.normal);
	float v = dot(ray.direction, p.normal);
	float t = -(dot(ray.origin, p.normal) + d) / v;
	if(t > 0.0 && t < i.t){
		i.t = t;
		i.hit = 1.0;
		i.hitPoint = vec3(
			ray.origin.x + t * ray.direction.x,
			ray.origin.y + t * ray.direction.y,
			ray.origin.z + t * ray.direction.z
		);
		i.normal = p.normal;
		t = min(i.hitPoint.z, 100.0) * 0.01;
	}
}

/**
	Ray Sphere intersection
**/
void intersectSphere(Ray ray, Sphere s, inout Intersection i){
	vec3  a = ray.origin - s.position;
	float b = dot(a, ray.direction);
	float c = dot(a, a) - (s.radius * s.radius);
	float d = b * b - c;
	if(d > 0.0){
		float t = -b - sqrt(d);
		if(t > 0.0 && t < i.t){
			i.t = t;
			i.hit = 1.0;
			i.hitPoint = vec3(
				ray.origin.x + ray.direction.x * t,
				ray.origin.y + ray.direction.y * t,
				ray.origin.z + ray.direction.z * t
			);
			i.normal = normalize(i.hitPoint - s.position);
		}
	}
}

float aspect() {
	return iResolution.x/iResolution.y;
}

float variableBlur(float blur) {
	return max(0,blur);
}


float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}



float SCurve (float x) {
    // ---- by CeeJayDK
	x = x * 2.0 - 1.0;
	return -x * abs(x) * 0.5 + x + 0.5;

    //return dot(vec3(-x, 2.0, 1.0 ),vec3(abs(x), x, 1.0)) * 0.5; // possibly faster version

    // ---- original for posterity
    // How to do this without if-then-else?
    // +edited the too steep curve value

    // if (value < 0.5)
    // {
    //    return value * value * 2.0;
    // }

    // else
    // {
    // 	value -= 1.0;

    // 	return 1.0 - value * value * 2.0;
    // }
}

float grid(float var, float size) {
    return floor(var*size)/size;
}


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float texture2DCompare(sampler2D depths, vec2 uv, float compare){
float depth = texture2D(depths, uv).r;
return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depths, vec2 size, vec2 uv, float compare)
{
	vec2 texelSize = vec2(1.0)/size;
	vec2 f = fract(uv*size+0.5);
	vec2 centroidUV = floor(uv*size+0.5)/size;

	float lb = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 0.0), compare);
	float lt = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 1.0), compare);
	float rb = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 0.0), compare);
	float rt = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 1.0), compare);
	float a = mix(lb, lt, f.y);
	float b = mix(rb, rt, f.y);
	float c = mix(a, b, f.x);
	return c;
}

float PCF(sampler2D depths, vec2 size, vec2 uv, float compare){
float result = 0.0;
for(int x=-2; x<=2; x++){
    for(int y=-2; y<=2; y++){
        vec2 off = vec2(x,y)/size;
        result += texture2DCompare(depths, uv+off, compare);
    }
}
return result/25.0;
}


float PCF3(sampler2D depths, vec2 size, vec2 uv, float compare, int ShadowBlur)
{
	float result = 0.0;
	int d = ShadowBlur;
	float i=0;
	for(int x=-d; x<=d; x++)
	{
	    for(int y=-d; y<=d; y++)
		{
	        vec2 off = vec2(x,y)/size;
	        result += texture2DShadowLerp(depths, size, uv+off, compare);
	        i=i+1;
	    }
	}
	return result/i;
}

float PCF2(sampler2D depths, vec2 size, vec2 uv, float compare)
{
	return PCF3(depths, size, uv, compare, 1);
}


float linstep(float low, float high, float v){
	return clamp((v-low)/(high-low), 0.0, 1.0);
}

float VSM(sampler2D depths, vec2 uv, float compare){
	vec2 moments = texture2D(depths, uv).xy;
	float p = smoothstep(compare-0.02, compare, moments.x);
	float variance = max(moments.y - moments.x*moments.x, -0.001);
	float d = compare - moments.x;
	float p_max = linstep(0.2, 1.0, variance / (variance + d*d));
	return clamp(max(p, p_max), 0.0, 1.0);
}

const int NUM_BUCKETS = 32;
const int ITER_PER_BUCKET = 1024;
const float HIST_SCALE = 8.0;

const float NUM_BUCKETS_F = float(NUM_BUCKETS);
const float ITER_PER_BUCKET_F = float(ITER_PER_BUCKET);


//note: uniformly distributed, normalized rand, [0;1[
float nrand( vec2 n )
{
	return clamp(0,1,fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453));
}
//note: remaps v to [0;1] in interval [a;b]
float remap( float a, float b, float v )
{
	return clamp( (v-a) / (b-a), 0.0, 1.0 );
}
//note: quantizes in l levels
float truncf( float a, float l )
{
	return floor(a*l)/l;
}

float n1rand( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	return nrnd0;
}
float n2rand( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	float nrnd1 = nrand( n + 0.11*t );
	return (nrnd0+nrnd1) / 2.0;
}
float n3rand( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	float nrnd1 = nrand( n + 0.11*t );
	float nrnd2 = nrand( n + 0.13*t );
	return (nrnd0+nrnd1+nrnd2) / 3.0;
}
float n4rand( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	float nrnd1 = nrand( n + 0.11*t );	
	float nrnd2 = nrand( n + 0.13*t );
	float nrnd3 = nrand( n + 0.17*t );
	return (nrnd0+nrnd1+nrnd2+nrnd3) / 4.0;
}

float n8rand( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	float nrnd1 = nrand( n + 0.11*t );	
	float nrnd2 = nrand( n + 0.13*t );
	float nrnd3 = nrand( n + 0.17*t );
    
    float nrnd4 = nrand( n + 0.19*t );
    float nrnd5 = nrand( n + 0.23*t );
    float nrnd6 = nrand( n + 0.29*t );
    float nrnd7 = nrand( n + 0.31*t );
    
	return (nrnd0+nrnd1+nrnd2+nrnd3 +nrnd4+nrnd5+nrnd6+nrnd7) / 8.0;
}

float n4rand_inv( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	float nrnd1 = nrand( n + 0.11*t );	
	float nrnd2 = nrand( n + 0.13*t );
	float nrnd3 = nrand( n + 0.17*t );
    float nrnd4 = nrand( n + 0.19*t );
	float v1 = (nrnd0+nrnd1+nrnd2+nrnd3) / 4.0;
    float v2 = 0.5 * remap( 0.0, 0.5, v1 ) + 0.5;
    float v3 = 0.5 * remap( 0.5, 1.0, v1 );
    return (nrnd4<0.5) ? v2 : v3;
}

//alternative Gaussian,
//thanks to @self_shadow
//see http://www.dspguide.com/ch2/6.htm
float n4rand_ss( vec2 n )
{
	float nrnd0 = nrand( n + 0.07*fract( iGlobalTime ) );
	float nrnd1 = nrand( n + 0.11*fract( iGlobalTime + 0.573953 ) );	
	return 0.23*sqrt(-log(nrnd0+0.00001))*cos(2.0*3.141592*nrnd1)+0.5;
}

/*
//Mouse Y give you a curve distribution of ^1 to ^8
//thanks to Trisomie21
float n4rand( vec2 n )
{
	float t = fract( iGlobalTime );
	float nrnd0 = nrand( n + 0.07*t );
	
	float p = 1. / (1. + iMouse.y * 8. / iResolution.y);
	nrnd0 -= .5;
	nrnd0 *= 2.;
	if(nrnd0<0.)
		nrnd0 = pow(1.+nrnd0, p)*.5;
	else
		nrnd0 = 1.-pow(nrnd0, p)*.5;
	return nrnd0; 
}
*/

float histogram( int iter, vec2 uv, vec2 interval, float height, float scale )
{
	float t = remap( interval.x, interval.y, uv.x );
	vec2 bucket = vec2( truncf(t,NUM_BUCKETS_F), truncf(t,NUM_BUCKETS_F)+1.0/NUM_BUCKETS_F);
	float bucketval = 0.0;
	for ( int i=0;i<ITER_PER_BUCKET;++i)
	{
		float seed = float(i)/ITER_PER_BUCKET_F;
		
		float r;
		if ( iter < 2 )
			r = n1rand( vec2(uv.x,0.5) + seed );
		else if ( iter<3 )
			r = n2rand( vec2(uv.x,0.5) + seed );
		else if ( iter<4 )
			r = n4rand( vec2(uv.x,0.5) + seed );
		else
			r = n8rand( vec2(uv.x,0.5) + seed );
		
		bucketval += step(bucket.x,r) * step(r,bucket.y);
	}
	bucketval /= ITER_PER_BUCKET_F;
	bucketval *= scale;
    
    float v0 = step( uv.y / height, bucketval );
    float v1 = step( (uv.y-1.0/iResolution.y) / height, bucketval );
    float v2 = step( (uv.y+1.0/iResolution.y) / height, bucketval );
	return 0.5 * v0 + v1-v2;
}

#endif
