local result = ooe.registry.find{ 'hello/FvvE' }
local table, source = ooe.registry.load( result[ 1 ] )
local hello = table[ 'hello/FvvE' ]

hello()
print 'hello lua'
