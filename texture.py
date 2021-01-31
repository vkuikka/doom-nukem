#!/usr/bin/env python3
import sys
from PIL import Image

Image.MAX_IMAGE_PIXELS = 999999999999999999999999
if len(sys.argv) == 1:
	print('no input file')
	exit(0);
Image.open(sys.argv[1]).convert('RGBA').save("out.bmp")
