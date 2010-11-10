#!/usr/bin/env python

import math
import optparse
import os
import sys
import PIL.Image

parser = optparse.OptionParser()
parser.add_option( '-c', '--convert', dest = 'convert', default = 'RGBA' )
parser.add_option( '-d', '--directory', dest = 'directory' )
parser.add_option( '-f', '--file', dest = 'file' )
parser.add_option( '-p', '--page-size', dest = 'page_size', type = 'int', default = 256 )
options, arguments = parser.parse_args()

if not options.file or not options.directory:
    parser.error( 'Both directory and file must be specified' )

if __name__ == '__main__':
    stdout = sys.stdout
    sys.stdout = sys.stderr

    os.makedirs( options.directory )
    image = PIL.Image.open( options.file )
    type = image.format.lower()
    image_size = image.size[ 0 ]
    page_size = options.page_size

    if image_size != image.size[ 1 ]:
        image_size = min( image.size )
        print 'Cropping image from size', image.size, 'to', ( image_size, image_size ), '...',
        image = image.crop( ( 0, 0, image_size, image_size ) )
        print 'done.'

    if image_size % page_size:
        bit_round_down = 2 ** int( math.floor( math.log( image_size, 2 ) ) )
        print 'Resizing image from size', image_size, 'to', bit_round_down, '...',
        image = image.resize( ( bit_round_down, bit_round_down ), PIL.Image.ANTIALIAS )
        image_size = bit_round_down
        print 'done.'

    image = image.convert( options.convert )
    level_limit = int( math.log( image_size / page_size, 2 ) )
    metadata = open( os.path.join( options.directory, 'metadata' ), 'w' )
    metadata.write( '{\n    "type": "%s",\n    "level_limit": %u\n}\n' % ( type, level_limit ) )

    print 'Processing "%s":' % options.file
    print '\tImage size:   ', image_size
    print '\tPage size:    ', page_size
    print '\tImage format: ', type
    print '\tMipmap levels:', level_limit + 1

    for level in xrange( level_limit + 1 ):
        sys.stderr.write( '\n\tMipmap level %u: ' % level )
        level_size = image_size >> level

        if level:
            image = image.resize( ( level_size, level_size ), PIL.Image.ANTIALIAS )

        for y in xrange( 0, level_size, page_size ):
            for x in xrange( 0, level_size, page_size ):
                crop = image.crop( ( x, y, x + page_size, y + page_size ) )
                name = '%u_%u_%u.%s' % ( x / page_size, y / page_size,
                    level_limit - level, type )
                crop.save( os.path.join( options.directory, name ) )
                sys.stderr.write( '.' )

        sys.stderr.write( '\n' )
