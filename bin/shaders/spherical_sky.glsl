/**
 * Uniforms
 */
uniform sampler2D DiffuseColorMap;
//uniform sampler2D Texture;
//uniform samplerCube uTexEnv;
//uniform sampler2D TransmittanceSampler;

uniform mat4 invPV;
uniform mat4 inv_proj;
uniform mat4 inv_view_rot;
uniform float SpotBrightness;
uniform float RayleighBrightness;
uniform float MieBrightness;
uniform float MieDistribution;
uniform int StepCount;
uniform float SurfaceHeight;
uniform float RayleighStrength;
uniform float MieStrength;
uniform float ScatterStrength;
uniform float RayleighCollectionPower;
uniform float MieCollectionPower;
uniform float IntensityRed;
uniform float IntensityGreen;
uniform float IntensityBlue;
uniform vec3 LightPos;
uniform float EyeExtinctionBias;
uniform float HExtinctionBias;
uniform float RayleighFactor;
uniform vec3 Kr;
/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER

const vec2 madd=vec2(0.5,0.5);

layout(location=0) in vec2 vertexIn;

out vec3 dir;

void main() {
    vec2 pos  = vec2( (gl_VertexID & 2)>>1, 1 - (gl_VertexID & 1)) * 2.0 - 1.0;
    vec4 front= invPV * vec4(pos, -1.0, 1.0);
    vec4 back = invPV * vec4(pos,  1.0, 1.0);

    dir=back.xyz / back.w - front.xyz / front.w;
    gl_Position = vec4(pos,.0,1.0);
}
#endif

#ifdef FRAGMENT_SHADER

/**
 * Fragment Shader
 *
 */

 #include "ats_functions.include"

vec3 getWorldNormal()
{
    vec2 fragCoord = gl_FragCoord.xy/iResolution;
    fragCoord = (fragCoord-0.5)*2.0;

    vec4 device_normal = vec4(fragCoord, 0.0, 1.0);
    vec3 eye_normal = normalize((inv_proj * device_normal).xyz);
    vec3 world_normal = normalize(mat3(inv_view_rot)*eye_normal);
    return world_normal;
}

float phase(float alpha, float g)
{
    float a = 3*(1-g*g);
    float b = 2*(2+g*g);
    float c = 1+alpha*alpha;
    float d = pow(1+g*g-2*g*alpha, 1.5);
    return (a/b)*(c/d);
}

float atmospheric_depth(vec3 position, vec3 dir)
{
    float a = dot(dir,dir);
    float b = 2*dot(dir,position);
    float c = dot(position, position)-1;
    float det = b*b-4*a*c;
    float detSqrt = sqrt(det);
    float q = (-b - detSqrt)/2;
    float t1 = c/q;
    return t1;
}

float horizon_extinction(vec3 position, vec3 dir, float radius)
{
    float u = dot(dir, -position);
    if(u<0.0)
    {
        return 1.0;
    }
    vec3 near = position + u*dir;
    if(length(near) < radius)
    {
        return 0.0;
    }
    else
    {
        vec3 v2 = normalize(near)*radius - position;
        float diff = acos(dot(normalize(v2), dir));
        return smoothstep(0.0, 1.0, pow(diff*2.0, 3.0));
    }
}

vec3 absorb(float dist, vec3 color, float factor)
{
    return color-color*pow(Kr, vec3(factor/dist));
}


float opticalDepth(float H, float r, float mu)
{
    float result = 0.0;
    float dx = limit(r, mu) / float(TRANSMITTANCE_INTEGRAL_SAMPLES);
    float xi = 0.0;
    float yi = exp(-(r - Rg) / H);
    for (int i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i) {
        float xj = float(i) * dx;
        float yj = exp(-(sqrt(r * r + xj * xj + 2.0 * xj * r * mu) - Rg) / H);
        result += (yi + yj) / 2.0 * dx;
        xi = xj;
        yi = yj;
    }
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? 1e9 : result;
}

in vec3 dir;
out vec4 fragColor;
void main()
{
    vec3 lightdir = normalize(-LightPos);
    vec3 eyedir = getWorldNormal();
    float alpha = dot(eyedir, lightdir);

    // Factors: rayleigh, mie and spot
    float rayleigh_brightness = RayleighBrightness; // 0-10.0, 33
    float mie_brightness = MieBrightness; // 0-0.168, 10.0
    float spot_brightness = SpotBrightness; // 0-1.0

    float mie_distribution = MieDistribution; // 0-100, 63.0

    float rayleigh_strength = RayleighStrength;
    float mie_strength = MieStrength;
    float scatter_strength = ScatterStrength; // 0-160, 28.0

    float rayleigh_collection_power = RayleighCollectionPower;
    float mie_collection_power = MieCollectionPower;

    // Strength, rayleigh, and mie
    float rayleigh_factor = phase(alpha, RayleighFactor)*rayleigh_brightness; // 0-400, 139
    float mie_factor      = phase(alpha, mie_distribution)*mie_brightness; // 0-400, 264

    float spot = smoothstep(0.0, 150.0, phase(alpha, 0.9995))*spot_brightness;

	vec3 n = eyedir;
	float u = (MATH_PI+atan(n.x, n.z))/(MATH_PI*2);
	float v = atan(n.y,1.0)/(MATH_PI/4);

    //vec3 eye_position = vec3(0.0, surface_height, 0.0);
    //float eye_depth = atmospheric_depth(eye_position, eyedir);

	//(vec4(1)-(vec4(170,98,50,255)/255)*vec4(dot(eyedir, vec3(0,1,0)))

	float f = clamp(dot(eyedir, vec3(0,1,0)),0,1);
	float f2 = clamp(1.0-dot(eyedir, vec3(0,1,0)),0,1);
	vec4 orange = vec4(200,98,50,255)/255;
	vec4 midblue = vec4(61,79,97,255)/255;
	vec4 blue = vec4(0,3,12,255)/255;
	


	vec4 color = vec4(1);
	color = mix(vec4(4,20,10,255)/255, orange, smoothstep(0.0, 0.001, f));
	color = mix(color, midblue, smoothstep(0.001, 0.1, f));
	color = mix(color, blue, smoothstep(0.35, 1.0, f));

	/*
	color = mix(vec4(0,0,0,1), orange, smoothstep(0.0, 0.001, f2));
	color = mix(color, midblue, smoothstep(0.001, 0.1, f2));
	color = mix(color, blue, smoothstep(0.35, 1.0, f2));
	*/

	fragColor = vec4(spot);
	//fragColor = mix(fragColor,orange,smoothstep(1,0,spot));
	fragColor += color;
	fragColor = 2*texture(DiffuseColorMap, vec2(0,1)-vec2(u-0.0005,v))+
				3*texture(DiffuseColorMap, vec2(0,1)-vec2(u+0.0000,v))+
				2*texture(DiffuseColorMap, vec2(0,1)-vec2(u+0.0005,v));
	fragColor /= 7;
	if(v<0)
	fragColor = vec4(0.3,0.3,0.5,1);
}
#endif
// END
