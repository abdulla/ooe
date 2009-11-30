--- query registry -------------------------------------------------------------
local result = ooe.registry.find{ 'hello/FvvE' }
local module = ooe.registry.load( result[ 1 ] )

local hello = module[ 'hello/FvvE' ]
local allocate = module[ 'allocate/FN3ooe13construct_ptrIN12_GLOBAL__N_15printEEERKSsE' ]
local deallocate = module[ 'deallocate/FvN3ooe12destruct_ptrIN12_GLOBAL__N_15printEEEE' ]
local say = module[ 'say/FvPN12_GLOBAL__N_15printEE' ]

--- run functions --------------------------------------------------------------
hello()
deallocate( allocate( 'hello lua delete' ) )

function printer( name )
	return
	{
		value = allocate( name ),
		said = function( self )
			say( self.value )
		end
	}
end

local heap = printer( 'hello lua gc' )
heap:said()

--- print hello ----------------------------------------------------------------
print 'hello lua'
