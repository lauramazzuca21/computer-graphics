// Fragment shader: : Phong shading
// ================
#version 450 core

in vec3 N;
in vec3 L;
in vec3 E;
in vec2 _TexCoord;

// Ouput data
out vec4 FragColor;
uniform sampler2D textureBuffer;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct PointLight{
	vec3 position;
	vec3 color;
	float power;
 };

uniform Material material;
uniform PointLight light;

void main()
{    
}