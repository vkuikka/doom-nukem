#!/usr/bin/python3

#	remove os dependent code from sources.
#	move macros to doom_nukem.h

import os
import math

file_header_top = '\
/* ************************************************************************** */\n\
/*                                                                            */\n\
/*                                                        :::      ::::::::   */\n'
file_header_name_start = "/*   "
file_header_name_end = "        :+:      :+:    :+:   */\n"
file_header_bot = '\
/*                                                    +:+ +:+         +:+     */\n\
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */\n\
/*                                                +#+#+#+#+#+   +#+           */\n\
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */\n\
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */\n\
/*                                                                            */\n\
/* ************************************************************************** */\n'

def	skip_lines(sep, i):
	if sep[i].startswith("/*") and "*/" not in sep[i]:
		while "*/" not in sep[i]:
			i += 1
		i += 1
	elif sep[i].startswith("/*") and sep[i].endswith("*/"):
		i += 1
	elif sep[i].startswith("#include"):
		i += 1
	elif sep[i].startswith("//"):
		i += 1
	elif sep[i] == "":
		i += 1
	return i

def	clean_function(block):
	block = block.strip()
	if not block.endswith("\n}"):	# is not last in file
		block += "\n}"
	sep = block.split("\n")
	cleaned = []
	i = 0
	while i < len(sep):
		tmp = -1
		while i != tmp and i < len(sep):
			tmp = i
			i = skip_lines(sep, i)
		if i < len(sep):
			if sep[i] == "{":
				curls = 1
				while curls != 0 and i < len(sep):
					if "{" in sep[i]:
						curls += 1
					if "}" in sep[i]:
						curls -= 1
					cleaned.append(sep[i])
					i += 1
			else:	# function declaration
				sep[i] = sep[i].replace('static ','')
				cleaned.append(sep[i])
		i += 1
	return (cleaned)

def	make_file_header(filename):
	header_width = len(file_header_top.split("\n")[0])
	name_width = header_width - len(file_header_name_start) - len(file_header_name_end)
	filename = filename[:name_width]
	name_str = file_header_name_start + filename
	str_wid = len(name_str + file_header_name_end)
	header = file_header_top + name_str
	for _ in range(header_width - str_wid):
		header += " "
	header += " " + file_header_name_end
	header += file_header_bot
	return (header)

def	move_functions(block, filename):
	file_text = make_file_header(filename)
	if "init" in filename:
		file_text += '\n#include "embed.h"\n'
	file_text += '\n#include \"doom_nukem.h\"\n\n'
	f = open("src_new/" + filename, "w")
	i = 0
	for line in block:
		i += 1
		file_text += line + "\n"
		if i != len(block):
			file_text += "\n"
	f.write(file_text)

def	divide_files():
	if not os.path.isdir('src_new'):
		os.mkdir("src_new")
	walk_dir = './src'
	declarations = []
	for root, subdirs, files in os.walk(walk_dir):
		for filename in files:
			file_path = os.path.join(root, filename)
			if not filename.endswith(".c"):
				continue
			file = open(file_path, 'r').read()
			combined = []
			file_index = 0
			in_combined = 1
			for block in file.split("}\n\n"):	# divide file into functions
				func = clean_function(block)	# remove comments take out static from declaration
				f_index = 0
				while "#" in func[f_index]:		# defines in the middle of function
					f_index += 1
				a = func[f_index]
				if not func[f_index].endswith(")"):	# two line declaration
					a += " " + func[f_index + 1].strip()
				declarations.append(a)
				func = "\n".join(func)
				combined.append(func)
				if in_combined == 5:	# 5 functions in file
					new_file = filename[:-2] + "_" + str(file_index) + ".c"
					move_functions(combined, new_file)
					file_index += 1
					combined = []
					in_combined = 0
				in_combined += 1
			if len(combined) > 0:
				new_file = filename[:-2] + "_" + str(file_index) + ".c"
				move_functions(combined, new_file)
	return declarations

def	remove_declarations(header):
	index = 0
	for i in range(len(header)):
		if "}" in header[len(header) - i - 1]:
			index = i
			break
	return header[:-index]

def	declaration_width(dec):
	# find max return value length
	maxlen = 0
	for line in dec:
		line = line.split("\t")
		if int(len(line[0]) > maxlen):
			maxlen = int(len(line[0]))

	# make all return values max length +1tab
	maxlen += 4
	cleaned = []
	for line in dec:
		line = line.split("\t")
		tabs = math.ceil((maxlen - len(line[0])) / 4)
		s = line[0]
		for _ in range(tabs):
			s += '\t'
		cleaned.append(s + line[1] + ";")
	
	# check if line is longer than 80
	for i, line in enumerate(cleaned):
		length = len(line)
		for c in line:
			if c == '\t':
				length += 3
		if length > 80:
			j = length - 80
			while j < length - 2:
				if line[len(line) - j] == ' ' and line[len(line) - j - 1] == ',':
					cleaned[i] = line[:len(line) - j]
					insert = line[len(line) - j:]
					insert = insert[1:]
					tabs = math.ceil(maxlen / 4) + 1
					for _ in range(tabs):
						insert = '\t' + insert
					cleaned.insert(i + 1, insert)
					break
				if line[len(line) - j] == '\t':
					j += 4
				else:
					j += 1
	return cleaned

def	add_to_header(decl):
	header_path = './src/doom_nukem.h'
	header = open(header_path, 'r').read()
	header = header.split("\n")
	header = remove_declarations(header)
	decl = declaration_width(decl)
	header.append("")
	header += decl
	header = "\n".join(header) + "\n#endif\n"
	f = open("src_new/" + "doom_nukem.h", "w")
	f.write(header)

def main():
	declarations = divide_files()
	add_to_header(declarations)

	for line in declarations:	# print duplicate file names
		found = 0
		compare = line.split("\t")[1]
		compare = compare.split("(")[0]
		for line2 in declarations:
			line2 = line2.split("\t")[1]
			line2 = line2.split("(")[0]
			if line2 == compare:
				found += 1
		if found > 1:
			print(line)

if __name__ == "__main__":
    main()
