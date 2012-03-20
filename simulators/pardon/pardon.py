#!/usr/bin/python

#
#       Searduino
#
#  Copyright (C) 2011, 2012 Henrik Sandklef      
#                                                                   
# This program is free software; you can redistribute it and/or     
# modify it under the terms of the GNU General Public License       
# as published by the Free Software Foundation; either version 3    
# of the License, or any later version.                             
#                                                                   
#                                                                   
# This program is distributed in the hope that it will be useful,   
# but WITHOUT ANY WARRANTY; without even the implied warranty of    
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     
# GNU General Public License for more details.                      
#                                                                   
# You should have received a copy of the GNU General Public License 
# along with this program; if not, write to the Free Software       
# Foundation, Inc., 51 Franklin Street, Boston,            
# MA  02110-1301, USA.                                              
#
#

from pearduino import *
from threading import Thread
from threading import BoundedSemaphore
import argparse
import time
import sys
from gi.repository import Gtk
from gi.repository import Gdk
#from gi.repository import Glib
from gi.repository import GObject

semaphore = BoundedSemaphore(1)

#
# searduino
#

paused = 0 

size = 10

redColor = Gdk.RGBA()
redColor.red=1.0
redColor.green=0.0
redColor.blue=0.5
redColor.alpha=0.7
#gdk_color_parse ("red");
#redColor = Gdk.Color("red")
#redColor = Gdk.color_parse("red")

def getSem():
#    print "Get semaphore"
    semaphore.acquire()        

def relSem():
    semaphore.release()        
#    print "Rel semaphore"

def pardonPause():
    seasim_pause();
    global pause 
    pause = 1

def pardonResume():
    seasim_resume();
    global pause 
    pause = 0


class pauseButton(Gtk.HBox):
    def __init__(self, parent):
        Gtk.HBox.__init__(self)
        self.par = parent
    
        self.label = Gtk.Label("Pause")
        self.label.set_width_chars(10);
        self.pin = Gtk.ToggleButton()
        self.pin.set_active(True)
        
        self.pack_start(self.label, False, True, 0)
        self.pack_start(self.pin, False, True, 0)

        self.pin.connect("clicked", self.on_pin_toggled, "1")

    def on_pin_toggled(self, widget, name):
        if self.pin.get_active():
            pardonResume()
            print "pause"
            self.label.set_text("Pause  ")
        else:
            pardonPause()
            self.label.set_text("Resume ")
        

class SpinButtonWindow(Gtk.Box):
    
    def __init__(self, parent):
        Gtk.Box.__init__(self)
        self.set_border_width(10)
        
        hbox = Gtk.Box(spacing=10)
        self.add(hbox)
        
        adjustment = Gtk.Adjustment(0, 0, 10000, 1, 100, 0)
        self.spinbutton = Gtk.SpinButton()
        self.spinbutton.set_adjustment(adjustment)
        self.spinbutton.set_numeric(True)
        self.spinbutton.set_value(seasim_get_digitalWrite_timelimit())
        self.setIncrement()
        self.spinbutton.connect("value-changed", self.on_update)
        
        lab = Gtk.Label("Update limit (msec)")

        hbox.pack_start(lab, False, False, 0)
        hbox.pack_start(self.spinbutton, False, False, 0)
        
    def setIncrement(self):
        cur_value = int(self.spinbutton.get_value_as_int())
        if (cur_value>5000):
            self.increment = 500
        if (cur_value>1000):
            self.increment = 200
        elif (cur_value>500):
            self.increment = 100
        elif (cur_value>200):
            self.increment = 50
        elif (cur_value>120):
            self.increment = 20
        self.spinbutton.set_increments(self.increment, 100)
        

    def on_update(self, disc):
        self.setIncrement()

#        print "===========================================  " +         str(self.spinbutton.get_value_as_int())
        seasim_set_digitalWrite_timelimit(int(self.spinbutton.get_value_as_int()))
#        print "======= set to: " + str(searduino_get_digitalWrite_timelimit())


class digitalPin(Gtk.HBox):

    def __init__(self, parent, nr):
        Gtk.HBox.__init__(self)
        self.par = parent
        self.myNr = nr
        

        # Label 
        label = Gtk.Label("Pin ")
        pinLabel = Gtk.Label(str(nr))
        pinLabel.set_width_chars(2);

        # Mode 
        self.mode_box=Gtk.HBox()
        self.mode_label = Gtk.Label(" Mode ")
        self.mode = Gtk.Label("Undef")
        self.mode.set_width_chars(7);

        self.mode_box.pack_start(self.mode_label, False, True,0)
        self.mode_box.pack_start(self.mode, False, True,0)

        # Input 
        self.input_box=Gtk.HBox()
        self.input = Gtk.ToggleButton()
        self.input.set_active(False)
        self.input_box.pack_start(self.input, False, True,0)
        
        # Output
        self.output_box=Gtk.HBox()
        self.output_label = Gtk.Label("NaN")
        self.output_box.pack_start(self.output_label, False, True, 0)
        
        self.pack_start(label, False, True, 0)
        self.pack_start(pinLabel, False, True, 0)
        self.pack_start(self.mode_box, False, True, 0)
        self.pack_start(self.input_box, False, True, 0)
        self.pack_start(self.output_box, False, True, 0)

        self.input.connect("clicked", self.on_dig_toggled, "1")

    def setMode(self, mode):
        print "===> setMode"
        if (mode==1):
            print "=== setMode 1"
            self.mode.set_text("OUTPUT")
            print "=== setMode 1"
#            self.input.set_property('visible', False)
        else:
            print "=== setMode 0"
#            self.input.set_property('visible', True)
            print "=== setMode 0"
            self.mode.set_text("INPUT")
            print "=== setMode 0"
            self.output_label.set_text("")
        print "<=== setMode"

    def setVal(self, val):
        self.output_label.set_text(str(val))

    def getVal(self):
        if self.input.get_active():
            return 1
        else:
            return 0

    def setValCond(self):
        if seasim_get_pin_mode(self.myNr)==1:
            self.output_label.set_text(str(seasim_digitalRead(self.myNr)))

    def on_dig_toggled(self, widget, name):
#        print "GUI 1 toggle"
        if self.input.get_active():
#            py_ext_set_input(1,1)
            state = "on"
        else:
            state = "off"
#            py_ext_set_input(1,0)
#        print "state " + state
        self.par.pinUpdate(self.myNr,state)



class analogPin(Gtk.HBox):

    def __init__(self, parent, nr):
        Gtk.HBox.__init__(self)
        self.par = parent
        self.myNr = nr
        

        # Label 
        label = Gtk.Label("Pin ")
        pinLabel = Gtk.Label(str(nr))
        pinLabel.set_width_chars(2);

        # Mode 
        self.mode_box=Gtk.HBox()
        self.mode_label = Gtk.Label(" Mode ")
        self.mode = Gtk.Label("Undef")
        self.mode.set_width_chars(7);

        self.mode_box.pack_start(self.mode_label, False, True,0)
        self.mode_box.pack_start(self.mode, False, True,0)

        # Input 
        self.input_box=Gtk.HBox()
        adjustment = Gtk.Adjustment(0, 0, 1023, 1, 100, 0)
        self.spinbutton = Gtk.SpinButton()
        self.spinbutton.set_adjustment(adjustment)

        self.spinbutton.set_numeric(True)
        self.spinbutton.connect("value-changed", self.on_update)
#        self.input = Gtk.ToggleButton()
#        self.input.set_active(False)
        self.input_box.pack_start(self.spinbutton, False, True,0)
        
        # Output
        self.output_box=Gtk.HBox()
        self.output_label = Gtk.Label("NaN")
        self.output_box.pack_start(self.output_label, False, True, 0)
        
        self.pack_start(label, False, True, 0)
        self.pack_start(pinLabel, False, True, 0)
        self.pack_start(self.mode_box, False, True, 0)
        self.pack_start(self.input_box, False, True, 0)
        self.pack_start(self.output_box, False, True, 0)

#        self.input.connect("clicked", self.on_dig_toggled, "1")

    def on_update(self,disc):
        seasim_set_ana_input(self.myNr,
                                    self.spinbutton.get_value_as_int())

    def setMode(self, mode):
        if (mode==1):
            self.mode.set_text("OUTPUT")
        else:
            self.mode.set_text("INPUT ")
            self.output_label.set_text("")

    def setVal(self, val):
        self.output_label.set_text(str(val))
#        self.spinbutton.set_value(val)

    def getVal(self):
        if self.input.get_active():
            return 1
        else:
            return 0

    def setValCond(self):
        if seasim_get_pin_mode(self.myNr)==1:
            self.output_label.set_text(str(seasim_digitalRead(self.myNr)))

    def on_dig_toggled(self, widget, name):
#        print "GUI 1 toggle"
        if self.input.get_active():
#            py_ext_set_input(1,1)
            state = "on"
        else:
            state = "off"
#            py_ext_set_input(1,0)
#        print "state " + state
        self.par.pinUpdate(self.myNr,state)




class MyWindow(Gtk.Window):

    
    digs = [None]*size
    anas = [None]*size

    def updateGUI(self):
#        print "updateGUI()  <-- " + str(paused)
        if (paused):
            print "no GUI update"
        else:
            print "Updating GUI manually"
            self.updateAllOut()
                
        return True


    def updateModes(self):
#        print "  --------------------------------------------- UPDATEMODES ============================================================"
        if (paused):
            print "no GUI update"
        else:
            for i in range(1,size-1):
                mode = seasim_get_pin_mode(i)
                print "updateModes (" + str(i)+ ", " + str(mode) + ")"
                self.pinUpdateMode(i,mode);
                
        self.sendInputPins()        
        return True
        
    def sendInputPins(self):
        if (paused):
            print "no GUI update"
        else:
 #           print "  --------------------------------------------- SIN"
            for i in range(1,size-1):
  #              print "  --------------------------------------------- SIN " + str(i)
                if (seasim_get_pin_mode(i)==0):
                    print "updateModes:"
                    getSem()        
                    value = self.digs[i].getVal()
                    relSem()
#                    print "                                                                         WILL SEND: " + str(value) + "  from " + str(i)
                    seasim_set_dig_input(i,
                                         value)
                
        return True
        
    def __init__(self):
        Gtk.Window.__init__(self, title="Pardon - a simulator frontend for the Searduino project")

        topTable = Gtk.Table(2, 2, False)
        topTable.attach(Gtk.Label("Digital"),0,1,0,1)
        topTable.attach(Gtk.Label("Analogue"),1,2,0,1)
        
        ioTable = Gtk.Table(1, 2, True)
        topTable.attach(ioTable, 0,1,1,2)
        aioTable = Gtk.Table(1, 2, True)
        topTable.attach(aioTable, 1,2,1,2)

        outTable = Gtk.Table(10, 2, True)
        digTable = Gtk.Table(10, 1, True)

        aoutTable = Gtk.Table(10, 2, True)
        adigTable = Gtk.Table(10, 1, True)

        ioTable.attach(outTable, 0, 1, 0, 1)
        ioTable.attach(digTable, 1, 2, 0, 1)
        topTable.attach(outTable, 0, 1, 0, 1)
        topTable.attach(digTable, 1, 2, 0, 1)

        aioTable.attach(aoutTable, 0, 1, 0, 1)
        aioTable.attach(adigTable, 1, 2, 0, 1)
        topTable.attach(outTable, 1, 2, 0, 1)
        topTable.attach(digTable, 3, 3, 0, 1)


        pause = pauseButton(self)
        pause2 = pauseButton(self)
        spin = SpinButtonWindow(self)

        self.box = Gtk.VBox(spacing=6)
        self.add(self.box)

        self.innerbox = Gtk.HBox(spacing=6)

        self.extrasbox=Gtk.VBox(spacing=6)
        self.extraslabel = Gtk.Label("General")
        self.extrasbox.pack_start(self.extraslabel,    False, True, 0)
        self.extrasbox.pack_start(pause,    False, True, 0)
        self.extrasbox.pack_start(spin,    False, True, 0)

        self.innerbox.pack_start(topTable, False, True, 0)
        self.innerbox.pack_start(self.extrasbox, False, True, 0)
        
        self.bottomlabel = Gtk.Label("Pardon - a simulator frontend. Pardon is part of the Searduino project")
        self.box.pack_start(self.innerbox,    False, True, 0)
        self.box.pack_start(self.bottomlabel,    False, True, 0)

        for i in range(1,(size-1)):
            self.digs[i] = digitalPin(self,i)
            digTable.attach(self.digs[i], 0, 1, i, i+1)
            self.pinUpdate(i,"on")

            self.anas[i] = analogPin(self,i)
            adigTable.attach(self.anas[i], 0, 1, i, i+1)
#            self.pinUpdate(i,"on")

            
        print "init...1"
        self.updateAllOut()
        print "init..."
#        self._positiontimeoutid = GObject.timeout_add(2000, self.updateModes)
        print "init..."
        self._positiontimeoutid = GObject.timeout_add(5000, self.updateGUI)
        print "init..."
        self.updateGUI()
        print "init..."

    def updateDigOutPin(self,pin, val):
#        print "---> get sem #####################################################################     " + str(pin) + " " + str(val)
        print "updateDigOutPin "+ str(pin) + " " + str(val)
        getSem()
#        print "self.digs[" + str(pin) + "].set_text(" + str(val)+ ")"
        self.digs[pin].setVal(val)
        relSem()
#        print "<--- rel sem #####################################################################"

    def updateDigMode(self, pin, mode):
        print "updateDigOutMode( " + str(pin) + " , " + str(mode) + ")"
        getSem()
        self.digs[pin].setMode(mode)
        relSem()
#        print "<--- rel sem #####################################################################"

    def updateAnaOutPin(self,pin, val):
        #        print "---> get sem #####################################################################     " + str(pin) + " " + str(val)
        print "updateAnaOutPin"
        getSem()
#        print "self.digs[" + str(pin) + "].set_text(" + str(val)+ ")"
        self.anas[pin].setVal(val)
        relSem()
#        print "<--- rel sem #####################################################################"

    def updateAllOut(self):
#        print "will update late"
        for i in range(1,(size-1)):
            self.digs[i].setValCond()


    def pinUpdateMode(self, pin, mode):
#        print "digs at  " + str(pin) + " : " + str(self.digs[pin])
        print "pinUpdateMode"
        getSem()
        self.digs[pin].setMode(mode)
        relSem()

    def pinUpdate(self, nr, val_str):
        val=0
#        print "CHECK pin set in GUI: "+str(nr)+ ": " + str(val)
        if (val_str=="on"):
            val=1
        seasim_set_dig_input(nr,val)            
#        self.updateAllOut()



class FileChooserWindow(Gtk.Window):

    def __init__(self):
        dialog = Gtk.FileChooserDialog("Please choose a file", self,
            Gtk.FileChooserAction.OPEN,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
             Gtk.STOCK_OPEN, Gtk.ResponseType.OK))

        self.add_filters(dialog)

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            print "Open clicked"
            print "File selected: " + dialog.get_filename()
        elif response == Gtk.ResponseType.CANCEL:
            print "Cancel clicked"

        dialog.destroy()

    def add_filters(self, dialog):
        filter_text = Gtk.FileFilter()
        filter_text.set_name("Text files")
        filter_text.add_mime_type("text/plain")
        dialog.add_filter(filter_text)

        filter_py = Gtk.FileFilter()
        filter_py.set_name("Python files")
        filter_py.add_mime_type("text/x-python")
        dialog.add_filter(filter_py)

        filter_any = Gtk.FileFilter()
        filter_any.set_name("Any files")
        filter_any.add_pattern("*")
        dialog.add_filter(filter_any)

    def on_folder_clicked(self, widget):
        dialog = Gtk.FileChooserDialog("Please choose a folder", self,
            Gtk.FileChooserAction.SELECT_FOLDER,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
             "Select", Gtk.ResponseType.OK))
        dialog.set_default_size(800, 400)

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            print "Select clicked"
            print "Folder selected: " + dialog.get_filename()
        elif response == Gtk.ResponseType.CANCEL:
            print "Cancel clicked"

        dialog.destroy()


def newDigOutCallback(pin, val):
#    print ""
#    print "==================== in Py:  new dig out: " + str(pin) + " = " + str(val)
#    print ""
    if (pin>size):
        print "Pin " + str(pin) + " is bigger than highest pin in simulator (" + str(size) + ". Ignoring update"
    else:
        global win
        win.updateDigOutPin(pin,val)

def newAnaOutCallback(pin, val):
#    print ""
#    print "==================== in Py:  new ANALOG out: " + str(pin) + " = " + str(val)
#    print ""
    if (pin>size):
        print "Pin " + str(pin) + " is bigger than highest pin in simulator (" + str(size) + ". Ignoring update"
    else:
        global win
        win.updateAnaOutPin(pin,val)

def newDigModeCallback(pin, mode):
#    print ""
    print "==================== in Py:  new Dig Mode: " + str(pin) + " = " + str(mode)
    if (pin>size):
        print "Pin " + str(pin) + " is bigger than highest pin in simulator (" + str(size) + ". Ignoring update"
    else:
        global win
        win.updateDigMode(pin,mode)
    print " done with new mode"



#file_win = FileChooserWindow()
#file_win.show_all()
def getArduinocodeLibrary():

    dialog = Gtk.FileChooserDialog("Please choose a file", None,
                                   Gtk.FileChooserAction.OPEN,
                                   (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                                    Gtk.STOCK_OPEN, Gtk.ResponseType.OK))
    
#self.add_filters(dialog)
    
    response = dialog.run()
    if response == Gtk.ResponseType.OK:
        print "Open clicked"
        print "File selected: " + dialog.get_filename()
    elif response == Gtk.ResponseType.CANCEL:
        print "Cancel clicked"
    
    file = dialog.get_filename()
    dialog.destroy()
    return file
    



parser = argparse.ArgumentParser(prog='Pardon (Arduino Simulator)')
parser.add_argument('--arduino-code', nargs=1, action="store", dest="ac", help='Arduino code to test')
args = parser.parse_args()

ard_code=""
if args.ac != None:
    ard_code=args.ac[0]
else:
    print "Reading via fc"
    ard_code=getArduinocodeLibrary()

print "ard_code: " + ard_code

time.sleep(1)
seasim_set_arduino_code(ard_code)
seasim_initialise();
seasim_start();

#parser.print_help()

       
seasim_set_dig_callback(newDigOutCallback)
seasim_set_ana_callback(newAnaOutCallback)
seasim_set_dig_mode_callback(newDigModeCallback)

#sys.exit(1)

time.sleep(1)

win = MyWindow()
win.connect("delete-event", Gtk.main_quit)
win.show_all()

GObject.idle_add(newDigOutCallback)
GObject.idle_add(newAnaOutCallback)
GObject.idle_add(newDigModeCallback)

GObject.threads_init()


  #GObject.threads_init()
  #mainloop = GObject.MainLoop()
  #mainloop.run()

Gtk.main()
