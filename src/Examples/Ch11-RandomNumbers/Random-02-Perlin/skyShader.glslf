#version 150

in vec3 localPos;
in vec4 color;

out vec4 fragColor;

void main()
{
    vec3 sky = vec3(0.0, 0.4, 0.9);
    vec3 cloud = vec3(0.9, 0.9, 0.9);
    float turb = (noise1(localPos) + noise1(2.0*localPos)*0.5 
                 + noise1(4.0*localPos)*0.25 + noise1(8.0*localPos)*0.125);
    vec3 texColor = mix(sky, cloud, turb);
    fragColor = color*vec4(texColor, 1.0);
}
