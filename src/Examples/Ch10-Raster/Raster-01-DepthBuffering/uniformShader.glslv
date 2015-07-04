uniform vec4 constColor;
varying vec4 color;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    // Cheapo vertlighting (dot with -x axis in MODEL space)
    color = constColor * clamp(-gl_Normal.x, 0.0, 1.0);
}
