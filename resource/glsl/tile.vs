uniform sampler2D height;
varying float texel;
varying vec2 position;

void main( void )
{
	vec4 vertex = vec4( gl_Vertex.xzy * 256., gl_Vertex.w );
	position = gl_Vertex.xy;
	texel = texture2D( height, position ).y;
	vertex.y = texel;
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
