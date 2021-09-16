// Fragment shader: : Normal mapping shading
// ================
#version 450 core

in vec2 _TexCoord;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

// Ouput data
out vec4 FragColor;

//uniforms
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

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

#define linear_coef 0.01
#define quadratic_coef 0.01

void main()
{    
}