### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

from os.path import join

### variables ##################################################################
variables = Variables()
variables.Add( EnumVariable( 'variant', 'build variant', 'debug', ( 'debug', 'release' ) ) )
variant = ARGUMENTS.get( 'variant', 'debug' )

Export( 'variables variant' )
SConscript( 'SConscript', variant_dir = join( 'build', variant ), duplicate = 0 )
