#!/usr/bin/env python3

# Project:    SIN - Traffic Crossroad
# Autor:      Martin Kocour (xkocou08)
#             Michal Gabonay (xgabon00)
# Date:       17.11.2018

import redis
import json

redisDB = redis.Redis(host='localhost', port=6379, db=0)


def _gen_key(primary: str, secondary: str):
    return "{:s}.{:s}".format(primary, secondary)


def setTrafficLightStates(crossroad_id: str, states: dict):
    states_json = json.dumps(states).encode('utf-8')
    redisDB.set(_gen_key(crossroad_id, 'tls'), states_json)
    pass


def getTrafficLightStates(crossroad_id: str, default=None) -> dict:
    states_json = redisDB.get(_gen_key(crossroad_id, 'tls'))
    if states_json is None:
        return default
    return json.loads(states_json.decode('utf-8'))


def setManualControl(crossroad_id: str, value: bool):
    value = 1 if value else 0
    redisDB.set(_gen_key(crossroad_id, 'manual'), value)


def getManualControl(crossroad_id: str, default: bool = False) -> bool:
    value = redisDB.get(_gen_key(crossroad_id, 'manual'))
    if value is None:
        return default
    value = int(value)
    return value == 1


def setTrafficProgram(crossroad_id: str, program: str):
    redisDB.set(_gen_key(crossroad_id, 'program'), program.encode('utf-8'))


def getTrafficProgram(crossroad_id: str, default: str = '0') -> str:
    value = redisDB.get(_gen_key(crossroad_id, 'program'))
    if value is None:
        return default
    return value.decode('utf-8')
