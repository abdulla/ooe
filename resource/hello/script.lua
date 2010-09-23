--[[ Copyright (C) 2010 Abdulla Kamar. All rights reserved. ]]

--- query registry ---------------------------------------------------------------------------------
local result = ooe.component.find{ 'hello/FvvE' }
local module = ooe.component.load( result[ 1 ] )

--- run functions ----------------------------------------------------------------------------------
function printer( name )
	local self = {}

	for k, v in pairs( module ) do
		if type( v ) == 'function' then
			self[ k:sub( 1, k:find( '/', 1, true ) - 1 ) ] = v
		end
	end

	self.value = self.construct( name )
	self.said = function( self )
		self.say( self.value )
	end

	return self
end

local heap = printer( 'hello lua gc' )
heap.hello()
heap:said()
heap.destruct( heap.construct( 'hello lua delete' ) )

local doc = ooe.component.doc( module, 'hello/FvvE' )
print( "lua-doc( hello ): " .. doc )

local alphabet = { a = 9, b = 8, c = 7 }

for k, v in pairs( heap.stdmap( alphabet ) ) do
	print( k, v )
end

for k, v in pairs( heap.stdset( alphabet ) ) do
	print( k, v )
end

--- print hello ------------------------------------------------------------------------------------
print 'hello lua'
