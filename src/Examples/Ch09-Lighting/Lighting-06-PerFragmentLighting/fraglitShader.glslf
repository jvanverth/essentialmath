varying vec3 normal;

uniform vec4 dirLightPosition;

void main()
{
	vec3 unitNormal = normalize(normal);
	float lighting = clamp(dot(unitNormal, dirLightPosition.xyz), 0.0, 1.0);
    gl_FragColor = vec4(lighting, lighting, lighting, 1);
}
