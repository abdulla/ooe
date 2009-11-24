local result = ooe.registry.find{ 'hello/FvvE' }
local module, source = ooe.registry.load( result[ 1 ] )

local hello = module[ 'hello/FvvE' ]
local allocate = module[ 'allocate/FN3ooe13construct_ptrIN12_GLOBAL__N_15printEEERKSsE' ]
local deallocate = module[ 'deallocate/FvN3ooe12destruct_ptrIN12_GLOBAL__N_15printEEEE' ]
local say = module[ 'say/FvPN12_GLOBAL__N_15printEE' ]

hello()
local object = allocate( 'hello lua gc' )
say( object )
deallocate( allocate( 'hello lua delete' ) )

print 'hello lua'
