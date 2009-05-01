uniform vec3 min_1;
uniform vec3 max_1;
uniform vec3 min_2;
uniform vec3 max_2;

uniform float scale;
uniform float morph;
attribute float delta;

vec2 texcoord( vec3 min_limit, vec3 max_limit )
{
	vec2 limit = min_limit.xz;
	vec2 range = max_limit.xz - limit;

	return ( gl_Vertex.xz - limit ) / range;
}

void main( void )
{
	vec4 vertex = gl_Vertex;
	vertex.y = ( vertex.y + delta * morph ) * scale;

	gl_TexCoord[ 0 ].st = texcoord( min_1, max_1 );
	gl_TexCoord[ 1 ].st = texcoord( min_2, max_2 );
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
