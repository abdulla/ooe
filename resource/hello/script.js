/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

//--- query registry -------------------------------------------------------------------------------
var result = ooe.component.find( [ 'hello/FvvE' ] );
var module = ooe.component.load( result[ 0 ] );

//--- run functions --------------------------------------------------------------------------------
function printer( name )
{
	for ( var i in module )
	{
		var value = module[ i ];

		if ( typeof( value ) == 'function' )
			this[ i.substr( 0, i.indexOf( '/' ) ) ] = value;
	}

	this.value = this.construct( name );
	this.said = function()
	{
		this.say( this.value );
	}
}

var heap = new printer( 'hello javascript gc' );
heap.hello();
heap.said();
heap.destruct( heap.construct( 'hello javascript delete' ) );

var doc = ooe.component.doc( module, 'hello/FvvE' );
print( "javascript-doc( hello ): " + doc );

var alphabet = { a: 9, b: 8, c: 7 };
alphabet = heap.stdmap( alphabet );

for ( var i in alphabet )
	print( i, alphabet[ i ] );

alphabet = heap.stdset( alphabet );

for ( var i in alphabet )
	print( i, alphabet[ i ] );

//--- print hello ----------------------------------------------------------------------------------
print( 'hello javascript' );
