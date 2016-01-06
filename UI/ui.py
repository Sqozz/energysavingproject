#!/usr/bin/env python3
import json, os.path
from flask import Flask, render_template, abort, request
from elropi import RemoteSwitch
app = Flask(__name__)

@app.route("/")
def index():
    return render_template('index.html', plugs=config.plugs)

@app.route("/settings/<plugId>")
def settings():
    return render_template('settings.html', plug)

@app.route("/new", methods=['POST'])
def createNew():
    print("Going to create new plug")
    return 200

@app.route("/switchPlug", methods=['POST'])
def switchPlug():
    turnOn = request.form['newState']
    deviceno=1
    default_key = [1,1,0,0,1]
    default_pin = 17
    device = RemoteSwitch(device=deviceno, key=default_key, pin=default_pin)
    if turnOn == "true":
        device.switchOn()
        print("switching on")
    else:
        device.switchOff()
        print("switching off")
    return 'ok'

class Config():
    __configJsonString__ = ""
    plugs = ""
    def __init__(self, configPath):
        with open(configPath) as configFileObj:
            self.__configJsonString__ = json.load(configFileObj)
            self.plugs = self.__configJsonString__["plugs"]

    def saveConfig():
        pass

if __name__ == "__main__":
    config = Config('config.json')
    app.run(host='0.0.0.0', debug=True)
