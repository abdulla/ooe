local result = ooe.registry.find{ 'hello/FvvE' }
local table, source = ooe.registry.load( result[ 1 ] )
local hello = table[ 'hello/FvvE' ]
local allocate = table[ 'allocate/FN3ooe13construct_ptrIN12_GLOBAL__N_15printEEERKSsE' ]
local deallocate = table[ 'deallocate/FvN3ooe12destruct_ptrIN12_GLOBAL__N_15printEEEE' ]
local say = table[ 'say/FvPN12_GLOBAL__N_15printEE' ]

hello()
local object = allocate( 'hello lua gc' )
say( object )
deallocate( allocate( 'hello lua delete' ) )
print 'hello lua'
