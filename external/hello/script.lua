--[[ Copyright (C) 2010 Abdulla Kamar. All rights reserved. ]]

--- query registry -------------------------------------------------------------
local result = ooe.registry.find{ 'hello/FvvE' }
local module = ooe.registry.load( result[ 1 ] )

--- run functions --------------------------------------------------------------
function printer( name )
	local self = {}

	for k, v in pairs( module ) do
		if type( k ) ~= 'number' then
			self[ k:sub( 1, k:find( '/', 1, true ) - 1 ) ] = v
		end
	end

	self.value = self.allocate( name )
	self.said = function( self )
		self.say( self.value )
	end

	return self
end

local heap = printer( 'hello lua gc' )
heap.hello()
heap:said()
heap.deallocate( heap.allocate( 'hello lua delete' ) )

local alphabet = { a = 9, b = 8, c = 7 }

for k, v in pairs( heap.stdmap( alphabet ) ) do
	print( k, v )
end

for k, v in pairs( heap.stdset( alphabet ) ) do
	print( k, v )
end

--- print hello ----------------------------------------------------------------
print 'hello lua'
