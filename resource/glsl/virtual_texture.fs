uniform sampler2D page_table;
uniform sampler2D page_cache;

vec4 vtexture2D( in vec2 coord )
{
	gl_FragData[ 1 ] = texture2D( page_table, coord );
	texture2D( page_cache, coord );
	return vec4( 1, 1, 1, 1 );
}
