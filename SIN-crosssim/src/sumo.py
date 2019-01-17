#!/usr/bin/env python3

# Project:    SIN - Traffic Crossroad
# Autor:      Martin Kocour (xkocou08)
#             Michal Gabonay (xgabon00)
# Date:       17.11.2018


import random


def generate_routefile():
    random.seed(42)  # make tests reproducible
    N = 3600  # number of time steps
    # demand per second from different directions
    mainRoad = 1. / 15
    sideRoad = 1. / 50
    mainToSideRoad = 1. / 40
    sideToMainRoad = 1. / 60
    bus = 1. / 300
    pedestrian = 1. / 35
    bike = 1. / 35
    with open("data/cross.rou.xml", "w") as routes:
        print("""<routes>
        <vType id="car" accel="0.7" decel="4.5" sigma="0.5" length="4.5" minGap="2.5" maxSpeed="30" \
guiShape="passenger"/>
        <vType id="bike" accel="0.2" decel="3.5" sigma="0.5" length="2.5" minGap="1.5" maxSpeed="5" vClass="bicycle" guiShape="bicycle"/>
        <vType id="bus" accel="0.4" decel="6.5" sigma="0.5" length="12" minGap="1.5" maxSpeed="15" vClass="bus" guiShape="bus"/>

        <route id="w_e_bus" edges="W0_C0 C0_E0" />
        <route id="e_w_bus" edges="E0_C0 C0_W0" />
        <route id="w_e" edges="W0_C0 C0_E0" />
        <route id="w_s" edges="W0_C0 C0_S0" />
        <route id="w_n" edges="W0_C0 C0_N0" />
        <route id="n_e" edges="N0_C0 C0_E0" />
        <route id="n_s" edges="N0_C0 C0_S0" />
        <route id="n_w" edges="N0_C0 C0_W0" />
        <route id="s_e" edges="S0_C0 C0_E0" />
        <route id="s_w" edges="S0_C0 C0_W0" />
        <route id="s_n" edges="S0_C0 C0_N0" />
        <route id="e_w" edges="E0_C0 C0_W0" />
        <route id="e_s" edges="E0_C0 C0_S0" />
        <route id="e_n" edges="E0_C0 C0_N0" />
        <route id="n_s_bike" edges="N0_C0 C0_S0" />
        <route id="s_n_bike" edges="S0_C0 C0_N0" />""", file=routes)
        vehNr = 0
        perNr = 0
        for i in range(N):
            if random.uniform(0, 1) < mainRoad:
                print('    <vehicle id="w_e_%i" type="car" route="w_e" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < mainToSideRoad:
                print('    <vehicle id="w_s_%i" type="car" route="w_s" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < mainToSideRoad:
                print('    <vehicle id="w_n_%i" type="car" route="w_n" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < sideToMainRoad:
                print('    <vehicle id="n_e_%i" type="car" route="n_e" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < sideRoad:
                print('    <vehicle id="n_s_%i" type="car" route="n_s" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < sideToMainRoad:
                print('    <vehicle id="n_w_%i" type="car" route="n_w" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < sideToMainRoad:
                print('    <vehicle id="s_e_%i" type="car" route="s_e" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < sideToMainRoad:
                print('    <vehicle id="s_w_%i" type="car" route="s_w" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < sideRoad:
                print('    <vehicle id="s_n_%i" type="car" route="s_n" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < mainRoad:
                print('    <vehicle id="e_w_%i" type="car" route="e_w" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < mainToSideRoad:
                print('    <vehicle id="e_s_%i" type="car" route="e_s" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < mainToSideRoad:
                print('    <vehicle id="e_n_%i" type="car" route="e_n" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < bike:
                print('    <vehicle id="s_n_bike_%i" type="bike" route="s_n_bike" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < bike:
                print('    <vehicle id="n_s_bike_%i" type="bike" route="n_s_bike" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < bus:
                print('    <vehicle id="w_e_bus_%i" type="bus" route="w_e_bus" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < bus:
                print('    <vehicle id="e_w_bus_%i" type="bus" route="e_w_bus" depart="%i" />' % (
                    vehNr, i), file=routes)
                vehNr += 1
            if random.uniform(0, 1) < pedestrian:
                routes.write('\n\t<person id="person_%i" depart="%i">' % (perNr, i))
                routes.write('<walk speed="0.6" edges = "W0_C0 C0_N0" />')
                routes.write('\n\t</person>')
                perNr += 1
            if random.uniform(0, 1) < pedestrian:
                routes.write('\n\t<person id="person_%i" depart="%i">' % (perNr, i))
                routes.write('<walk speed="0.6" edges = "W0_C0 C0_E0" />')
                routes.write('\n\t</person>')
                perNr += 1
            if random.uniform(0, 1) < pedestrian:
                routes.write('\n\t<person id="person_%i" depart="%i">' % (perNr, i))
                routes.write('<walk speed="0.6" edges = "N0_C0 C0_S0" />')
                routes.write('\n\t</person>')
                perNr += 1
            if random.uniform(0, 1) < pedestrian:
                routes.write('\n\t<person id="person_%i" depart="%i">' % (perNr, i))
                routes.write('<walk speed="0.6" edges = "E0_C0 C0_W0" />')
                routes.write('\n\t</person>')
                perNr += 1
            if random.uniform(0, 1) < pedestrian:
                routes.write('\n\t<person id="person_%i" depart="%i">' % (perNr, i))
                routes.write('<walk speed="0.6" edges = "S0_C0 C0_N0" />')
                routes.write('\n\t</person>')
                perNr += 1

        print("</routes>", file=routes)
