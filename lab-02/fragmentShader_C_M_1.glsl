// fragment solo per nemico
#version 330 core
in vec4 ourColor;
out vec4 FragColor;  

uniform float t;


void main()
{
    FragColor = vec4(ourColor*t);
}