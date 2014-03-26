varying vec3 localPos;
varying vec4 color;

void main()
{
    vec3 sky = vec3(0.0, 0.3, 0.8);
    vec3 cloud = vec3(0.8, 0.8, 0.8);
    float turb = (noise1(localPos) + noise1(2.0*localPos)*0.5 
                 + noise1(4.0*localPos)*0.25 + noise1(8.0*localPos)*0.125);
    vec3 texColor = mix(sky, cloud, turb);
    gl_FragColor = color*vec4(texColor, 1.0);
}
