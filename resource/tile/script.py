#!/usr/bin/env python

import math
import optparse
import os
import sys
import Image

parser = optparse.OptionParser()
parser.add_option( '-d', '--directory', dest = 'directory' )
parser.add_option( '-f', '--file', dest = 'file' )
parser.add_option( '-p', '--page-size', dest = 'page_size', type = 'int', default = 256 )
( options, arguments ) = parser.parse_args()

if not options.file or not options.directory:
	parser.error( 'Both directory and file must be specified' )

if __name__ == '__main__':
	image = Image.open( options.file )
	page_size = options.page_size
	image_size = image.size[ 0 ]

	if image_size != image.size[ 1 ]:
		sys.exit( 'Image is not square:', image_size )
	elif image_size % page_size:
		sys.exit( 'Image size', image_size, 'is not divisiable by page size', page_size )

	metadata = open( os.path.join( options.directory, 'metadata' ), 'w' )
	type = image.format.lower()
	metadata.write( '%s %u %u' % ( type, page_size, image_size ) )
	level_limit = int( math.log( image_size / page_size, 2 ) )

	print 'Processing "%s":' % options.file
	print '\tImage size:   ', image_size
	print '\tPage size:    ', page_size
	print '\tImage format: ', type
	print '\tMipmap levels:', level_limit

	for level in xrange( level_limit ):
		level_size = image_size >> level
		resize = image.resize( ( level_size, level_size ) )
		sys.stderr.write( '\n\tMipmap level %u: ' % level )

		for y in xrange( 0, level_size, page_size ):
			for x in xrange( 0, level_size, page_size ):
				crop = image.crop( ( x, y, x + page_size, y + page_size ) )
				name = '%u_%u_%u.%s' % ( x / page_size, y / page_size,
					level_limit - level - 1, type )
				crop.save( os.path.join( options.directory, name ) )
				sys.stderr.write( '.' )

		sys.stderr.write( '\n' )
