#!/usr/bin/env python3

import http.server
import sys
from os.path import dirname, isfile, join
from sys import argv

here = dirname(__file__)

with open(join(here, 'gotit.json'), 'w') as gotit:
    gotit.write('{"Providers":[{"Addrs": [')
    gotit.write(f'"/ip4/127.0.0.1/tcp/{argv[1]}/http"')
    gotit.write('],"ID": "12D3KooWHEzPJNmo4shWendFFrxDNttYf8DW4eLC7M2JzuXHC1hE","Protocol": "transport-ipfs-gateway-http"}]}')

class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/ping':
            print('ping->pong')
            self.send_response(200)
            self.wfile.write(bytes('pong', 'utf-8'))
            return
        components = self.path.split('/')
        match components[1]:
            case 'ipfs':
                if len(components) == 3:
                    path = join(here, 'blocks', components[2])
                else:
                    path = join(here, 'cars', components[2])
            case 'ipns':
                path = join(here, 'names')
                for comp in components[2:]:
                    if len(path) > 0:
                        path = join(path, comp.split('?')[0])
            case 'routing':
                self.respond(join(here, 'gotit.json'))
                return
            case _ :
                print(f"{self.path} ({components}) not handled request type ({components[0]})", file=sys.stderr)
                exit(9)
        self.respond(path)

    def respond(self, path):
        try:
            with open(path, 'rb') as f:
                self.send_response(200)
                mime = self.headers.get('Accept')
                self.send_header("Content-type", mime)
                content = f.read()
                if '/names/' in path and len(content) < 99:
                    try:
                        content = content.decode("utf-8")
                        content = content.strip()
                        self.send_header("X-Ipfs-Roots", content)
                        content = str.encode(content)
                    except UnicodeError:
                        pass
                self.end_headers()
                self.wfile.write(content)
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
