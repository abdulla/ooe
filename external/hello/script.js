var result = ooe.registry.find( [ 'hello/FvvE' ] );
var module = ooe.registry.load( result[ 0 ] );

/*for ( var i in module )
	print( i );*/

var hello = module[ 'hello/FvvE' ];

hello();

print( 'hello javascript' );
