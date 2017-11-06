# embedding_in_qt5.py --- Simple Qt5 application embedding matplotlib canvases
#
# Copyright (C) 2005 Florent Rougon
#               2006 Darren Dale
#               2015 Jens H Nielsen
#
# This file is an example program for matplotlib. It may be used and
# modified with no restriction; raw copies as well as modified versions
# may be distributed without limitation.

from __future__ import unicode_literals
import sys
import os
import random
import matplotlib
# Make sure that we are using QT5
matplotlib.use('Qt5Agg')
from PyQt5 import QtCore, QtWidgets

from numpy import arange, sin, pi
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

progname = os.path.basename(sys.argv[0])
progversion = "0.1"


import serial
import json
import numpy as np



class MyDynamicMplCanvas(FigureCanvas):
    """A canvas that updates itself every second with a new plot."""

    def __init__(self, parent=None, width=5, height=4, dpi=100):
    
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes1 = fig.add_subplot(211)
        self.axes2 = fig.add_subplot(212)

        self.compute_initial_figure()

        FigureCanvas.__init__(self, fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
                                   QtWidgets.QSizePolicy.Expanding,
                                   QtWidgets.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        
        
        self.ser = serial.Serial('/dev/ttyACM0', 19200, timeout=1)
        #////////////////////////////////////
        self.f_force = list()
        
        self.f_rpm = list()
        
        
        self.f_times = list()
        self.f_force_an = list() 

        
        
        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.update_figure)
        timer.start(1000)

    def compute_initial_figure(self):
        pass

    def update_figure(self):
        # Build a list of 4 random integers between 0 and 10 (both inclusive)
        ser = self.ser
        ser.write( json.dumps({'i':0}) )
        i = 0
        line = ""
        while i < 10 and line == "":
            line = ser.readline()   # read a '\n' terminated line        
            print "b", line
            try:
                tmp = json.loads(line)
                
                self.f_force.append(tmp['f'])
                self.f_force_an.append(tmp['fn'])
                self.f_times.append(tmp['t'])
                self.f_rpm.append(tmp['th'])
                i = 100
            except ValueError:
                pass

            except TypeError:
                pass            
            i+=1
    
        
        
        self.axes1.cla()
        rng = -20
        self.axes1.plot(self.f_times[rng:], self.f_force[rng:], 'ro-', lw=1)
        avg = np.average(self.f_force[rng:])
        self.axes1.plot(self.f_times[rng:], np.ones_like(self.f_force[rng:])*avg, 'k--')

        self.axes2.cla()
        rpms = np.unique(self.f_rpm)
        fss = np.array(self.f_force)
        fs = [ np.average(fss[np.where(self.f_rpm == _r)]) for _r in rpms]
        self.axes2.plot(rpms, fs, 'bo-')
        self.draw()


    def tare(self):
        self.ser.write( json.dumps({'r':"tare"}) )

    def set_throttle(self, perc):
        print("Throttle: %d"%perc)
        self.ser.write( json.dumps({'r':"th", 'th': perc}) )


    def set_start(self):
        
        self.ser.write( json.dumps({'r':"start"}) )
        
    def set_stop(self):
        
        self.ser.write( json.dumps({'r':"stop"}) )

    def set_cycle(self):
        perc = self.ct + 1
        if perc < 98:
            self.set_throttle(perc)
            self.ct = perc
        else:
            self.ctimer.stop()
            self.set_throttle(int(59./70. * 100.))

    
    def cycle(self):
        self.ct = int(59./70. * 100.)
        self.set_throttle(self.ct)
  
        
        self.f_force = list()        
        self.f_rpm = list()
        self.f_times = list()
        self.f_force_an = list() 
        
        
        
        self.ctimer = QtCore.QTimer(self)
        self.ctimer.timeout.connect(self.set_cycle)
        self.ctimer.start(10000)        
        
class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("application main window")

        self.file_menu = QtWidgets.QMenu('&File', self)
        self.file_menu.addAction('&Quit', self.fileQuit,
                                 QtCore.Qt.CTRL + QtCore.Qt.Key_Q)
        self.menuBar().addMenu(self.file_menu)

        self.scale_menu = QtWidgets.QMenu('&Scale', self)
        self.motor_menu = QtWidgets.QMenu('&Motor', self)
        
        
        self.menuBar().addSeparator()
        
        self.menuBar().addMenu(self.scale_menu)
        self.menuBar().addMenu(self.motor_menu)

        

        self.main_widget = QtWidgets.QWidget(self)

        l = QtWidgets.QVBoxLayout(self.main_widget)
      #  sc = MyStaticMplCanvas(self.main_widget, width=5, height=4, dpi=100)
        dc = MyDynamicMplCanvas(self.main_widget, width=5, height=4, dpi=100)
        
        self.scale_menu.addAction('&Tare', dc.tare)
        
        self.motor_menu.addAction('&%d'%(0), lambda: dc.set_throttle(0))
        self.motor_menu.addAction('&%d'%(75), lambda: dc.set_throttle(75))        
        self.motor_menu.addAction('&%d'%(80), lambda: dc.set_throttle(80))
        self.motor_menu.addAction('&%d'%(85), lambda: dc.set_throttle(85))
        self.motor_menu.addAction('&%d'%(90), lambda: dc.set_throttle(90))        
        
        self.motor_menu.addAction('&Start', lambda: dc.set_start())        
        self.motor_menu.addAction('&Stop', lambda: dc.set_stop())                
        self.motor_menu.addAction('&Cycle', lambda: dc.cycle())                

#        l.addWidget(sc)
        l.addWidget(dc)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)

        self.statusBar().showMessage("All hail matplotlib!", 2000)

    def fileQuit(self):
        self.close()

    def closeEvent(self, ce):
        self.fileQuit()

    def about(self):
        QtWidgets.QMessageBox.about(self, "About",
                                    """embedding_in_qt5.py example
Copyright 2005 Florent Rougon, 2006 Darren Dale, 2015 Jens H Nielsen

This program is a simple example of a Qt5 application embedding matplotlib
canvases.

It may be used and modified with no restriction; raw copies as well as
modified versions may be distributed without limitation.

This is modified from the embedding in qt4 example to show the difference
between qt4 and qt5"""
                                )


qApp = QtWidgets.QApplication(sys.argv)

aw = ApplicationWindow()
aw.setWindowTitle("%s" % progname)
aw.show()
sys.exit(qApp.exec_())
#qApp.exec_()