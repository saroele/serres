# -*- coding: utf-8 -*-
"""
Created on Sat Jan  4 22:52:07 2014

@author: roel
"""

import SimpleHTTPServer
import SocketServer

PORT = 8001

Handler = SimpleHTTPServer.SimpleHTTPRequestHandler

httpd = SocketServer.TCPServer(("", PORT), Handler)

print "serving at port", PORT
httpd.serve_forever()