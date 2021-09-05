// Vertex shader: Wave shading
// ================
#version 450 core

in vec4 vPosition;
in vec4 vColor;

uniform float time;
uniform float xs, zs;
uniform float h;
uniform mat4 ModelViewProjectionMatrix;

void main()
{
    vec4 v = vPosition;
    v.y = vPosition.y + h*sin(time + xs*vPosition.x) + h*sin(time + xs*vPosition.z);
    gl_Position = ModelViewProjectionMatrix * v;

    vec4 diffuse, specular;
    vec4 eyePosition = ModelViewMatrix * vPosition;
    vec4 eyeLightPos = lightPosition;
    vec3 N = normalize(NormalMatrix * Normal);
    vec3 L = normalize(vec3(eyeLightPos – eyePosition));
    vec3 E = -normalize(eyePosition.xyz);
    vec3 H = normalize(L + E);
    float diff = max(dot(L, N), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    diffuse = diff * diffuseLight;
    specular = spec * specularLight;
    color = diffuse + specular;
}