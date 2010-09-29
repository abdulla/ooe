### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

from os import getenv
from os.path import join

output = join( getenv( 'SCONS_OUTPUT', '/tmp' ), 'ooe' )

### variables ######################################################################################
variables = Variables()
variables.Add( PathVariable( 'output', 'output path', output ) )
variables.Add( EnumVariable( 'variant', 'build variant', 'debug', ( 'debug', 'release' ) ) )

output = ARGUMENTS.get( 'output', output )
variant = ARGUMENTS.get( 'variant', 'debug' )
path = join( output, variant )

### actions ########################################################################################
actions = [
    Delete( join( path, 'cache' ) ),
    Mkdir( join( path, 'cache' ) ),
    Delete( join( path, 'resource' ) ),
    Copy( join( path, 'resource' ), 'resource' )
]

for action in actions:
    Execute( action )

Export( 'variables' )
SConsignFile( join( output, 'sconsign' ) )
SConscript( 'SConscript', variant_dir = path, duplicate = 0 )
