// Utility functions

#ifndef tex
#define tex

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

#endif
