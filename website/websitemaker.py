#!/usr/bin/env python
#
# WebsiteMaker - Constructs as websdite from Markdown files.
#
# Written 2012 Hal Canary
#
# Copyright 2012, The University of North Carolina at Chapel Hill.
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * The name of the copyright holder may not be used to endorse or
#   promote products derived from this soft ware without specific prior
#   written permission.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Notes:
# * Requires the Markdown program to convert .md to .html.
#   Markdown is copyright 2004 John Gruber.
# * To add files to your website, add to the main function.

import subprocess, sys, shutil, os

#markdown = '/usr/bin/markdown'
markdown = 'markdown'

def make_web_page(src,dst,title="",header="",tagline=""):
	def copy(i,o):
		x = i.read(8192)
		while x != "":
			o.write(x)
			x = i.read(8192)
	head = """
<!doctype html>
<html>
<head>
<meta charset="UTF-8">
<link rel="stylesheet" href="stylesheets/stylesheet.css">
<title>%s</title>
</head>
<body>
<div id="header_wrap" class="outer">
<header class="inner">
<h1 id="project_title">%s</h1>
<h2 id="project_tagline">%s</h2>
</header>
</div>
<div id="main_content_wrap" class="outer">
<section id="main_content" class="inner">
"""
	foot = """
</section>
</div>
<div id="footer_wrap" class="outer">
<footer class="inner">
<p >&nbsp;</p>
</footer>
</div>
</body>
</html>
"""
	with open(dst,'w') as o:
		o.write(head % (title,header,tagline))
		proc = subprocess.Popen(
			[markdown, '--html4tags', src],
			stdout=subprocess.PIPE)
		copy(proc.stdout,o)
		o.write(foot)

def copy_dir(src,dst):
	dst = os.path.join(dst,os.path.basename(src))
	if not os.path.isdir(dst):
		shutil.copytree(src,dst)

def main():
	tagline = 'Create, View, and Share 3D Scenes Using a Depth Camera and a WebGL/iOS/Android application'

	thisdir = os.path.dirname(os.path.realpath(__file__))
	sitedir = os.path.join(thisdir, 'site')
	if not os.path.isdir(sitedir):
		os.mkdir(sitedir)

	make_web_page(
		os.path.join(thisdir, '..', 'README.md'),
		os.path.join(thisdir, 'site','index.html'),
		'Hephaestus', 'Hephaestus', tagline)

	make_web_page(
		os.path.join(thisdir, '..', 'team.md'),
		os.path.join(thisdir, 'site','team.html'),
		'The Hephaestus Team', 'Hephaestus', tagline)

	copy_dir(
		os.path.join(thisdir, 'stylesheets'),
		os.path.join(thisdir, 'site'))

if __name__ == "__main__":
	main()
