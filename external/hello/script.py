### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###
from types import BuiltinFunctionType

#--- query registry --------------------------------------------------------------------------------
result = ooe.registry.find( [ 'hello/FvvE' ] )
module = ooe.registry.load( result[ 0 ] )

#--- run functions ---------------------------------------------------------------------------------
class printer:
	def __init__( self, name ):
		for k, v in module.items():
			if type( v ) is BuiltinFunctionType:
				setattr( self, k[ : k.find( '/' ) ], v )

		self.value = self.allocate( name )

	def said( self ):
		self.say( self.value )

heap = printer( 'hello python gc' )
heap.said()
heap.deallocate( heap.allocate( 'hello python delete' ) )

doc = ooe.registry.doc( module, 'hello/FvvE' )
print( "python-doc( hello ):", doc )
help( heap.hello )

alphabet = { 'a': 9, 'b': 8, 'c': 7 }

for k, v in heap.stdmap( alphabet ).items():
	print( k, v, sep = '\t' )

for i in heap.stdset( set( alphabet ) ):
	print( i )

#--- print hello -----------------------------------------------------------------------------------
print( 'hello python' )
