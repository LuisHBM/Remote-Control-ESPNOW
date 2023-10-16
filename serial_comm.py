import os
import json
from serial import Serial

def get_config(config_file=None):
    if config_file:
        config = json.loads(open(config_file, 'r').read())
    else:
        config = json.loads(open('config.json', 'r').read())

    return config

class SerialComm(object):
    def __init__(self):
        super(SerialComm, self).__init__()
        self.config = get_config('config.json')
        self.command_port = os.environ.get('COMM_PORT', self.config['comm']['comm_port'])
        self.baud_rate = int(os.environ.get('BAUD_RATE', self.config['comm']['baud_rate']))

    def start(self):
        print("Starting communication...")
        self.comm = Serial(self.command_port, self.baud_rate)
        print(f"Communication port created on {self.command_port}!")
    
    def send(self, command):
        message = command
        self.comm.write(message.encode())
        print(f'Message [{message}] sent!')


if __name__ == "__main__":
    serial = SerialComm()

    serial.start()

    while True:
        serial.send()