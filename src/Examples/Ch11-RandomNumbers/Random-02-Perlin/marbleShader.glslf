varying vec3 localPos;
varying vec4 color;

void main()
{
    vec3 light = vec3(0.7, 0.7, 0.7);
    vec3 dark = vec3(0.0, 0.0, 0.0);
    float turb = (noise1(localPos) + noise1(2.0*localPos)*0.5 
                 + noise1(4.0*localPos)*0.25 + noise1(8.0*localPos)*0.125);
    float interp = sin(6.0*localPos.y + 8.0*turb)*0.65;
    vec3 texColor = mix(light, dark, interp);
    gl_FragColor = color*vec4(texColor, 1.0);
}
