//--- query registry -----------------------------------------------------------
var result = ooe.registry.find( [ 'hello/FvvE' ] );
var module = ooe.registry.load( result[ 0 ] );

var hello = module[ 'hello/FvvE' ];
var allocate = module[ 'allocate/FN3ooe13construct_ptrIN12_GLOBAL__N_15printEEERKSsE' ];
var deallocate = module[ 'deallocate/FvN3ooe12destruct_ptrIN12_GLOBAL__N_15printEEEE' ];
var say = module[ 'say/FvPN12_GLOBAL__N_15printEE' ];

//--- run functions ------------------------------------------------------------
hello();
deallocate( allocate( 'hello javascript delete' ) );

function printer( name )
{
	this.value = allocate( name );
	this.said = function()
	{
		say( this.value );
	}
}

var heap = new printer( 'hello javascript gc' );
heap.said();

//--- print hello --------------------------------------------------------------
print( 'hello javascript' );
