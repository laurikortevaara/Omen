/**
 * Uniforms
 */
uniform sampler2D Texture;
uniform samplerCube uTexEnv;
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

const vec3 Kr = vec3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131);

/**
 * Fragment Shader
 *
 */

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

in vec3 dir;
out vec4 fragColor;
void main()
{
	fragColor = texture(uTexEnv, dir);

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

    float spot = smoothstep(0.0, 15.0, phase(alpha, 0.9995))*spot_brightness;

    float step_count = StepCount;
    float surface_height = SurfaceHeight; // 0-160, 50

    vec3 eye_position = vec3(0.0, surface_height, 0.0);
    float eye_depth = atmospheric_depth(eye_position, eyedir);
    float step_length = eye_depth/float(step_count);

    float eye_extinction = horizon_extinction(eye_position, eyedir, surface_height-EyeExtinctionBias);
    // Nitrogen absorbtion factor for RGB


    vec3 rayleigh_collected = vec3(0.0,0.0,0.0);
    vec3 mie_collected = vec3(0.0,0.0,0.0);

    vec3 intensity = vec3(IntensityRed, IntensityGreen, IntensityBlue);

    for(int i=0; i < step_count; ++i)
    {
        float sample_distance = step_length*float(i);
        vec3 position = eye_position + eyedir*sample_distance;
        float extinction = horizon_extinction(position, lightdir, surface_height - HExtinctionBias);
        float sample_depth =atmospheric_depth(position, lightdir);

        vec3 influx = absorb(sample_depth, vec3(intensity), scatter_strength);

        rayleigh_collected += absorb(sample_distance, Kr*influx, rayleigh_strength);
        mie_collected      += absorb(sample_distance, influx   , mie_strength);
    }

    rayleigh_collected *= eye_extinction * pow(eye_depth, rayleigh_collection_power ) / float(step_count);
    mie_collected      *= eye_extinction * pow(eye_depth, mie_collection_power      ) / float(step_count);

    fragColor = vec4(vec3(mie_factor*mie_collected + rayleigh_factor * rayleigh_collected), 1) + vec4(vec3(spot*length(mie_collected)*vec3(246,223,143)/255.0),1);
}
#endif
// END
