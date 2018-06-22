#pragma once
namespace omen {
	typedef float floatprec;
    typedef float clampf;
    
    /*typedef struct vec2 { 
        float x, y; 
        vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {};
    } vec2;*/
    
    /*typedef struct vec3 { 
        float x, y, z; 
        vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {};
        vec3(const vec2& v1, const float& f) : x(v1.x), y(v1.y), z(f) {};
        operator const vec2() { return vec2(x, y); }
    } vec3;

    typedef struct vec4 { 
        float x, y, z, w; 
        vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) : x(x), y(y), z(z), w(w) {};
        vec4(const vec2& v1, const vec2& v2) : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {};

        operator const vec2() { return vec2(x, y); }
        operator const vec3() { return vec3(x, y, z); }
    } vec4;


    typedef struct mat4x4 {
        vec4 rows[4];
    };*/

    template<typename genType> constexpr genType pi() {return genType(3.14159265358979323846264338327950288);}

    enum texture_type
    {
        TEXTURE_2D = 0x001,
        TEXTURE_3D = 0x002,
        TEXTURE_CUBE_MAP = 0x003
    };
}
