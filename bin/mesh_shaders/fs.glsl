#version 410 core

uniform int RenderWireframe;

in vec3 fs_normal;
//flat in vec3 fs_normal;
in vec2 fs_texcoord;

out vec4 fColor;

uniform sampler2D tex;

void main() 
{
    if(RenderWireframe==0)
    {
        vec3 n = abs(normalize(fs_normal));
        fColor = vec4(n,1);
        fColor.a = 0.5f;
    }
    else
    {
        fColor = vec4(1);
        fColor.a = 1.0f;
    }

	//fColor = texture(tex, fs_texcoord);
	//fColor = (fColor+vec4(fs_texcoord.x, fs_texcoord.y, 0, 1))/2;
}