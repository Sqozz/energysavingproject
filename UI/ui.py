#!/usr/bin/env python3
import json, os.path
from flask import Flask, render_template, abort
app = Flask(__name__)

@app.route("/")
def index():
    print(config.plugs)
    return render_template('index.html', plugs=config.plugs)

@app.route("/settings/<plugId>")
def settings():
    return render_template('settings.html', plug)

@app.route("/new", methods=['POST'])
def createNew():
    print("Going to create new plug")
    return 200

@app.route("/switchOn", methods=['POST'])
def switchOn():
    print("power on switch")

@app.route("/switchOff", methods=['POST'])
def switchOff():
    print("power off switch")

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
