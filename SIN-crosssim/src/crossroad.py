#!/usr/bin/env python3

# Project:    SIN - Traffic Crossroad
# Autor:      Martin Kocour (xkocou08)
#             Michal Gabonay (xgabon00)
# Date:       17.11.2018

import sys, os
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("Please declare environment variable 'SUMO_HOME'")

import traci


class Crossroad:
    BUS_WEIGHT = 3

    WEIGHT_ALL = 0.6
    WEIGHT_NOW = 0.4

    MIN_DELTA = 0.12

    @staticmethod
    def create(id: str, manual: bool):
        lanes = 'N0_C0_2 E0_C0_1 E0_C0_2 S0_C0_2 W0_C0_1 W0_C0_2'.split()
        detectors = {
            "d_W0_C0_2": Detector("d_W0_C0_2", "W0_C0_2"),
            "d_W0_C0_1": Detector("d_W0_C0_1", "W0_C0_1"),
            "d_E0_C0_2": Detector("d_E0_C0_2", "E0_C0_2"),
            "d_E0_C0_1": Detector("d_E0_C0_1", "E0_C0_1"),
            "d_N0_C0_2": Detector("d_N0_C0_2", "N0_C0_2"),
            "d_S0_C0_2": Detector("d_S0_C0_2", "S0_C0_2")
        }
        programs = {
            '0': [],  # Default program
            '1': ['N0_C0_2', 'S0_C0_2'],
            '2': ['E0_C0_1', 'W0_C0_1', 'E0_C0_2', 'W0_C0_2'],
            '3': ['E0_C0_2', 'W0_C0_2']
        }
        return Crossroad(id, lanes, detectors, programs, manual)

    def __init__(self, id: str, lanes: list, detectors: dict, programs: dict, manual: bool):
        self.id = id
        self.lanes = lanes
        self.detectors = detectors
        self.programs = programs
        self.loop_cycle = 130
        self.best_program = "0"
        self.vehicles_sum = dict((lane, 0) for lane in lanes)
        self.vehicles = dict((lane, 0) for lane in lanes)
        self.buses = dict((lane, 0) for lane in lanes)
        self.manual_control = manual

    def step(self, step: int):
        self.percept()
        if step > 0 and step % self.loop_cycle == 0:
            if not self.manual_control:
                self.do_action()
            print("Step {:d}: Program {:s}".format(step, self.best_program))
            traci.trafficlight.setProgram(self.id, self.best_program)
            traci.trafficlight.setPhase(self.id, 0)

    def percept(self):
        for (key, detector) in self.detectors.items():
            vehicles = detector.getVehicles()
            buses = detector.getVehiclesOfType('bus')

            self.buses = buses
            self.vehicles_sum[
                detector.getLane()] += vehicles + Crossroad.BUS_WEIGHT * buses  # Aggregate all vehicles per lane
            self.vehicles[detector.getLane()] = vehicles + Crossroad.BUS_WEIGHT * buses

    def do_action(self):

        SUM_A = sum(self.vehicles_sum.values(), 1)  # SUM all vehicles per phase loop
        SUM_W = sum(self.vehicles.values(), 1)

        program_score = []
        for (program, lanes) in self.programs.items():
            score = 0
            for lane in lanes:
                print("Lane {:s}: vehicles = {:d} all = {:d}".format(lane, self.vehicles[lane], self.vehicles_sum[lane]))
                score += Crossroad.WEIGHT_ALL * (float(self.vehicles_sum[lane]) / SUM_A) + Crossroad.WEIGHT_NOW * (
                float(self.vehicles[lane]) / SUM_W)
                pass
            score = 0 if len(lanes) == 0 else float(score) / len(lanes)
            assert score <= 1.0
            program_score.append((program, score))

        program_score = sorted(program_score, key=lambda kv: kv[1], reverse=True)
        delta = program_score[0][1] - program_score[-2][1]

        if delta < Crossroad.MIN_DELTA:
            self.best_program = '0'
        else:
            self.best_program = program_score[0][0]

        self.vehicles_sum = dict((lane, 0) for lane in self.lanes)

    def set_program(self, program):
        self.best_program = program

    def get_program(self):
        return self.best_program

    def set_manual_control(self, manual: bool):
        self.manual_control = manual

    def is_manual_control(self):
        return self.manual_control

    def traffic_light_states(self) -> dict:
        states_str = traci.trafficlight.getRedYellowGreenState(self.id)
        states = ["N_b","N_l", "N_m", "N_r", "E_r", "E_m_1", "E_m_2", "E_r", "S_b","S_r", "S_m", "S_l", "W_r", "W_m_1", "W_m_2", "W_l", "N_p", "E_p", "S_p", "W_p"]
        result = {}
        for i in range(len(states_str)):
            value = states_str[i]
            if value in "rR":
                value = "red"
            elif value in "gG":
                value = "green"
            elif value in "yY":
                value = "orange"
            else:
                raise Exception("Unhandled value: " + value)
            result[states[i]] = value
        return result

class Detector:
    def __init__(self, id, lane):
        self.id = id
        self.lane = lane

    def getId(self):
        return self.id

    def getLane(self):
        return self.lane

    def getVehicles(self) -> int:
        vehicles = traci.lanearea.getJamLengthVehicle(self.id)
        return vehicles

    def getVehiclesOfType(self, vehicle_type) -> bool:
        vehicles = traci.lanearea.getLastStepVehicleIDs(self.id)
        result = 0
        for vehicle_id in vehicles:
            type = traci.vehicle.getTypeID(vehicle_id)
            if vehicle_type == type:
                result += 1
        return result
