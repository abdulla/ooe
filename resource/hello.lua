local table, source = registry.load( registry.path .. '../library/libhello' .. registry.suffix )
local hello = table[ 'hello/FvvE' ]

if not hello then
	error( 'hello/FvvE not found' )
end

hello()
print( 'hello lua' )
