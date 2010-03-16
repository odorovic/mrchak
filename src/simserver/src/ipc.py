'''Kod za interprocesnu komunikaciju. U sustini socket-based distribucija.
Created on May 1, 2009

@author: odor
'''

import socket

cmds = { 'GET' : gethandler,
         'WAIT': waithandler,
         'SIGNAL' : signalhandler}

def gethandler(request, sock):
    type, id = request['params']
    try:
        data = datasource.get(type, id)
        sock.write(data)
    except:
        sock.write('404 Not found.\n')

def waithandler(request, sock):
    evt_type = request['params'][0]
    try:
        waitlist[evt_type].append(sock)
    except:
        sock.write('404 Not found.\n')
        
def signalhandler(request, sock):
    def perform_signal(sock):
        sock.write('200 Ok.\n')
    evt_type = request['params'][0]
    waiters = waitlist[evt_type]
    waiters[evt_type] = []
    for waiter in waiters:
        perform_signal(sock)
    

def recv_command(sock):
    '''Chita celokupnu komandu sa soketa'''
    f = sock.makefile()
    return f.readline()

def clienthandler(sock, addr):
    request = decode_request(recv_request(sock))
    cmds[request['cmd']](request, sock)
    
class Server(object):
    def __init__(self, port = 4242, handler):
        self.port = port
        self.handler = handler
        self.serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.running = True
        
        
    def main_loop(self, serversock):
        self.serversock.bind('localhost', port)
        while self.running:
            clientsock, addr = self.serversock.listen(5)
            handler(clientsock, addr)
            