local result = ooe.registry.find{ 'hello/FvvE' }
local table, source = ooe.registry.load( result[ 1 ] )
local hello = table[ 'hello/FvvE' ]
local allocate = table[ 'allocate/FN3ooe13construct_ptrIN12_GLOBAL__N_15printEEERKSsE' ]

for k, v in pairs( table ) do
	print( k, v )
end

hello()
allocate( 'hello lua gc' )
print 'hello lua'
