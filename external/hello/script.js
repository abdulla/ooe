var result = ooe.registry.find( [ 'hello/FvvE' ] );
var module = ooe.registry.load( result[ 0 ] );

var hello = module[ 'hello/FvvE' ];
var allocate = module[ 'allocate/FN3ooe13construct_ptrIN12_GLOBAL__N_15printEEERKSsE' ];
var deallocate = module[ 'deallocate/FvN3ooe12destruct_ptrIN12_GLOBAL__N_15printEEEE' ];
var say = module[ 'say/FvPN12_GLOBAL__N_15printEE' ];

hello();
var object = allocate( 'hello javascript gc' );
say( object );
deallocate( allocate( 'hello javascript delete' ) );

print( 'hello javascript' );
