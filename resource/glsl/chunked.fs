uniform sampler2D tex_1;
uniform sampler2D tex_2;

uniform float morph;

void main( void )
{
	vec4 colour_1 = texture2D( tex_1, gl_TexCoord[ 0 ].st );
	vec4 colour_2 = texture2D( tex_2, gl_TexCoord[ 1 ].st );
	gl_FragColor = mix( colour_1, colour_2, morph );
}
