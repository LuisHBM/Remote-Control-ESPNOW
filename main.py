from pynput.keyboard import Key, Listener, KeyCode
from serial_comm import SerialComm
import os

COMMANDS = {
    'w': "FOWARD",
    'a': "LEFT",
    's': "BACKWARD",
    'd': "RIGHT",
    'n': "STOP",
    '1': "INCREASE PWM (+10)",
    '2': "DECREASE PWM (-10)"
}

accept_keys = list()
for command in COMMANDS:
    accept_keys.append(command)

serial = SerialComm()
serial.start()

def menu(key):
    os.system('clear||cls')
    print(f'|', end= " ")
    for accept_key in accept_keys:
        print(f'{accept_key}: {COMMANDS[accept_key]} |', end= " ")
    print()
    print(f'\nCommand: {COMMANDS[key]}')
menu('n')

class KeyListener:
    def __init__(self):
        self.key = 'n'

    def on_key_press(self, key):
        isAccepted = False
        
        if isinstance(key, Key):
            exit(1)
        if isinstance(key, KeyCode):
            self.key = key.char
            
            for acceptkey in accept_keys:
                if self.key == acceptkey:
                    isAccepted = True
                    break
        
            if not isAccepted:
                self.key = 'n'
        
        menu(self.key)
        serial.send(self.key)

    def on_key_release(self, key):
        self.key = 'n'
        menu(self.key)
        serial.send(self.key)

    def start(self):
        # Cria um Listener para capturar as teclas pressionadas
        with Listener(on_press=self.on_key_press, on_release=self.on_key_release) as listener:
            listener.join()

if __name__ == "__main__":
    keylistener = KeyListener()
    keylistener.start()
