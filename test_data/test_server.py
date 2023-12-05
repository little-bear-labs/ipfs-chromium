#!/usr/bin/env python3

import http.server
import sys
from os.path import dirname, isfile, join
from sys import argv

here = dirname(__file__)

class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/ping':
            print('ping->pong')
            self.send_response(200)
            self.wfile.write(bytes('pong', 'utf-8'))
            return
        components = self.path.split('/')
        if len(components) != 3:
            print(self.path, 'aka', components, '(size=', len(components), ") doesn't follow the exact pattern expected of /ip?s/arg ... TODO at least return 404 for CAR")
            exit(8)
        match components[1]:
            case 'ipfs':
                path = join(here, 'blocks')
            case 'ipns':
                path = join(here, 'names')
            case _ :
                print(f"{self.path} ({components}) not handled request type ({components[0]})", file=sys.stderr)
                exit(9)
        path = join(path, components[2])
        try:
            with open(path, 'rb') as f:
                self.send_response(200)
                mime = self.headers.get('Accept')
                self.send_header("Content-type", mime)
                self.end_headers()
                self.wfile.write(f.read())
                # print('test server responded to', self.path, ' Accept:', mime)
                return
        except OSError as e:
            print('test server does not have access to ', path, e)
        self.send_response(404)

try:
    server = http.server.HTTPServer(('localhost', int(argv[1])), Handler)
    print('Started test server on ', argv[1])
    server.serve_forever()
except KeyboardInterrupt:
    print('^C received, shutting down test server')
    server.socket.close()
