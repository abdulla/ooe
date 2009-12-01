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

--- print hello ----------------------------------------------------------------
print 'hello lua'
