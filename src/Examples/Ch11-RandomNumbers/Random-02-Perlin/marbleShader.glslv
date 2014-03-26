uniform mat4 IvNormalMatrix;
uniform vec4 IvLightDirection;
uniform vec4 IvLightAmbient;
uniform vec4 IvLightDiffuse;
varying vec4 color;
varying vec3 localPos;

void main()
{
    localPos      = vec3(gl_Vertex); 
    gl_Position     = ftransform();
    vec4 transNormal = normalize(IvNormalMatrix * vec4(gl_Normal,0.0));
    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);
    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;
    color = gl_Color*lightValue;
}
