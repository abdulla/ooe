//--- query registry -----------------------------------------------------------
var result = ooe.registry.find( [ 'hello/FvvE' ] );
var module = ooe.registry.load( result[ 0 ] );

//--- run functions ------------------------------------------------------------
function printer( name )
{
	for ( var i in module )
		this[ i.substr( 0, i.indexOf( '/' ) ) ] = module[ i ];

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

//--- print hello --------------------------------------------------------------
print( 'hello javascript' );
