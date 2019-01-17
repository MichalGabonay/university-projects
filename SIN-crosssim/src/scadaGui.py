#!/usr/bin/env python3

# Project:    SIN - Traffic Crossroad
# Autor:      Martin Kocour (xkocou08)
#             Michal Gabonay (xgabon00)
# Date:       17.11.2018


import queue
import threading
from tkinter import *
from time import sleep

import crossroadDB


def runloop(thread_queue=None):
    '''
    After result is produced put it in queue
    '''
    while True:
        states = crossroadDB.getTrafficLightStates("C0", dict())
        program = crossroadDB.getTrafficProgram('C0')

        result = (states, program)
        thread_queue.put(result)
        sleep(1)

class Scada():
    def __init__(self, master):
        self.master = master
        self.thread_queue = queue.Queue()
        self.new_thread = threading.Thread(
            target=runloop,
            kwargs={'thread_queue':self.thread_queue})
        master.title("SCADA system for traffick managment")
        master.geometry('500x700')

        lbl1 = Label(master, text="Semaphores", font=("Arial Bold", 15))
        # lbl2 = Label(master, text="State", font=("Arial Bold", 15))
        # lbl3 = Label(master, text="Waiting", font=("Arial Bold", 15))
        lbl4 = Label(master, text="Program", font=("Arial Bold", 15))

        self.semLables = {"N_b": "N Bike", "N_l": "N Left", "N_r": "N Right", "N_m": "N Middle", "N_p": "N Pedestrians",
                           "S_b": "S Bike", "S_l": "S Left", "S_r": "S Right", "S_m": "S Middle", "S_p": "S Pedestrians",
                           "W_l": "W Left", "W_m_2": "W Middle2", "W_m_1": "W Middle1", "W_r": "W Right", "W_p": "W Pedestrians",
                           "E_l": "E Left", "E_m_2": "E Middle2", "E_m_1": "E Middle1", "E_r": "E Right", "E_p": "E Pedestrians"}

        self.semStates = {}

        keys = self.semLables.keys()
        keys = sorted(keys)

        self.row0 = 1
        for key in keys:
            value = self.semLables[key]
            Label(master, text=value).grid(sticky="W", column=0, row=self.row0, padx=(10, 0))
            self.semStates[key] = Label(text="red", bg="red", fg="red")
            self.row0 += 1

        self.selected = IntVar()
        self.chosen = IntVar()
        self.rad1 = Radiobutton(master, text='Default', value=0, variable=self.selected, command=self.switch_programm)
        self.rad2 = Radiobutton(master, text='Reinforce side roads (N and S)', value=1, variable=self.selected, command=self.switch_programm)
        self.rad3 = Radiobutton(master, text='Reinforce main roads (W and E)', value=2, variable=self.selected, command=self.switch_programm)
        self.rad4 = Radiobutton(master, text='Reinforce turnings main/side roads', value=3, variable=self.selected, command=self.switch_programm)
        self.chk = Checkbutton(master, text='Manual program choosing', variable=self.chosen, command=self.switch_manual)

        lbl1.grid(column=0, row=0)
        # lbl2.grid(column=1, row=0)
        # lbl3.grid(column=2, row=0)
        lbl4.grid(column=3, row=0)

        master.grid_columnconfigure(2, minsize=100)

        self.chk.grid(sticky="W", column=3, row=1)
        self.rad1.grid(sticky="W", column=3, row=2)
        self.rad2.grid(sticky="W", column=3, row=3)
        self.rad3.grid(sticky="W", column=3, row=4)
        self.rad4.grid(sticky="W", column=3, row=5)

        self.rad1.configure(state=DISABLED)
        self.rad2.configure(state=DISABLED)
        self.rad3.configure(state=DISABLED)
        self.rad4.configure(state=DISABLED)

        self.row1 = 1
        for key, value in self.semStates.items():
            value.grid(sticky="W", column=1, row=self.row1, pady=(5, 5))
            self.row1 += 1

    def switch_programm(self):
        crossroadDB.setTrafficProgram("C0", str(self.selected.get()))

    def switch_manual(self):
        if self.chosen.get() == 0:
            crossroadDB.setManualControl("C0", False)
            self.rad1.configure(state = DISABLED)
            self.rad2.configure(state = DISABLED)
            self.rad3.configure(state = DISABLED)
            self.rad4.configure(state = DISABLED)
        else:
            crossroadDB.setManualControl("C0", True)
            self.rad1.configure(state=NORMAL)
            self.rad2.configure(state=NORMAL)
            self.rad3.configure(state=NORMAL)
            self.rad4.configure(state=NORMAL)

    def update_all(self):
        try:
            (states, program) = self.thread_queue.get(0)
        except queue.Empty:
            pass
        else:
            for key, value in states.items():
                self.semStates[key].configure(bg=value, fg=value)
        self.master.after(50, self.update_all)


if __name__ == '__main__':
    t = Tk()
    my_gui = Scada(t)
    my_gui.new_thread.start()
    my_gui.master.after(50, my_gui.update_all())

    t.mainloop()
