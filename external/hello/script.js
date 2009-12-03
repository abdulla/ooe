//--- query registry -----------------------------------------------------------
var result = ooe.registry.find( [ 'hello/FvvE' ] );
var module = ooe.registry.load( result[ 0 ] );

//--- run functions ------------------------------------------------------------
function printer( name )
{
	for ( var i in module )
	{
		var value = module[ i ];

		if ( typeof( value ) == 'function' )
			this[ i.substr( 0, i.indexOf( '/' ) ) ] = value;
	}

	this.value = this.allocate( name );
	this.said = function()
	{
		this.say( this.value );
	}
}

var heap = new printer( 'hello javascript gc' );
heap.hello();
heap.said();
heap.deallocate( heap.allocate( 'hello javascript delete' ) );

var alphabet = { a: 1, b: 2, c: 3 };
alphabet = heap.stdmap( alphabet );

for ( var i in alphabet )
	print( i, alphabet[ i ] );

//--- print hello --------------------------------------------------------------
print( 'hello javascript' );
