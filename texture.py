#!/usr/bin/env python3
import sys
from PIL import Image

Image.MAX_IMAGE_PIXELS = 999999999999999999999999
if len(sys.argv) != 3:
	print('usage: ./texture.py input_image output_image.bmp')
	exit(0);
Image.open(sys.argv[1]).convert('RGBA').save(sys.argv[2])
