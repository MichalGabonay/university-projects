#!/usr/bin/env python3

# Project:    SIN - Traffic Crossroad
# Autor:      Martin Kocour (xkocou08)
#             Michal Gabonay (xgabon00)
# Date:       17.11.2018


import os, sys
import sumo
import crossroadDB
import argparse
from crossroad import Crossroad

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("Please declare environment variable 'SUMO_HOME'")

from sumolib import checkBinary
import traci


def parse_args():
    parser = argparse.ArgumentParser()
    # TODO add arguments
    args = parser.parse_args()
    args.crossroads = ['C0']
    return args


def run(args):
    """execute the TraCI control loop"""
    step = 0
    # we start with phase 2 where EW has green

    crossroad = Crossroad.create('C0', crossroadDB.getManualControl('C0'))

    vehicles = traci.simulation.getMinExpectedNumber()  # Returns the number of vehicles which are in the net
    while vehicles > 0:
        traci.simulationStep()
        update([crossroad])
        # print("Step {:5d}: {:d} vehicles".format(step, vehicles))
        vehicles = traci.simulation.getMinExpectedNumber()
        crossroad.step(step)
        step += 1
    traci.close()
    sys.stdout.flush()


def update(crossroads: list):
    for crossroad in crossroads:
        crossroadDB.setTrafficLightStates(crossroad.id, crossroad.traffic_light_states())
        if crossroadDB.getManualControl(crossroad.id):
            crossroad.set_manual_control(True)
            crossroad.set_program(crossroadDB.getTrafficProgram(crossroad.id))
        else:
            crossroad.set_manual_control(False)
            crossroadDB.setTrafficProgram(crossroad.id, crossroad.best_program)


if __name__ == "__main__":
    args = parse_args()

    sumoBinary = checkBinary('sumo-gui')
    # sumo.generate_routefile()
    traci.start([sumoBinary, "-c", "data/cross.sumocfg"])

    run(args)
