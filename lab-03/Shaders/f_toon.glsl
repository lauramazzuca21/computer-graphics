// Fragment shader: Toon shading
// ================
#version 450 core

in vec3 L;
in vec3 E;
in vec3 N;

in vec3 Color;

out vec4 fragColor;

void main()
{
    vec4 finalColor = vec4(Color, 1.0);

    float intensity = dot(normalize(L),normalize(N));
    if (intensity > 0.95)
        finalColor *=1.5;
    else if (intensity > 0.5)
        finalColor *=1.0;
    else if (intensity > 0.25)
        finalColor *= 0.75;
    else if (intensity > 0.15)
        finalColor *=0.5;
    else if (intensity > 0.05)
        finalColor *=0.25;

    float aa = dot(normalize(E), normalize(N));
    if(aa >= 0.0 && aa < 0.30)
        finalColor = vec4(0.0, 0.0, 0.0, 1.0); //black

    fragColor = finalColor;
}