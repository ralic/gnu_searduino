#
#       Searduino
#                                                                   
# Basically a couple of Makefile wrapping the Ardunio
# C and C++ files
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

ARDUINO_SRC=arduino-sources
FAKED_ARDUINO_SRC=faked-arduino
DATE=$(shell date '+%y%m%d-%H%M%S')
DIST_FILE=searduino-$(shell date '+%y%m%d-%H%M%S').tar

all: lib stub 

check: check-hw check-sw

clean: ARDUINO=mega
clean: stub-clean lib-clean test-clean

lib:
	ARDUINO=uno  && export BUILD_FROM_VCS=true && cd $(ARDUINO_SRC) && make light-clean uno
	ARDUINO=due  && export BUILD_FROM_VCS=true && cd $(ARDUINO_SRC) && make light-clean due
	ARDUINO=mega && export BUILD_FROM_VCS=true && cd $(ARDUINO_SRC) && make light-clean mega

stub:
	ARDUINO=stub && BOARD=stub && export BUILD_FROM_VCS=true  && cd $(FAKED_ARDUINO_SRC) && make lib shlib

stub-clean:
	cd $(FAKED_ARDUINO_SRC) && make clean

lib-clean:ARDUINO=uno
lib-clean:
	cd $(ARDUINO_SRC) && make clean

mega-clean:ARDUINO=mega
mega-clean:
	cd $(ARDUINO_SRC) && make clean

test-clean:
	cd test/hw          && make clean
	cd test/stub        && make clean
	cd test/shared      && make clean
	cd test/shared-main && make clean


check-sw:
	make stub
	cd test/stub        && make check
	cd test/shared      && make clean && make && make shlib && make check
	cd test/shared-main && make clean all check check2

check-hw: ARDUINO=mega
check-hw:
	cd test/hw          && make clean hex

check2: check-hw2 check-stub2

check-hw2: 
	cd test/hw && make clean upload

check-stub2: stub
	cd test/stub && make clean check check2

all: lib stub

uno-test: ARDUINO=uno
mega-test:CPU=atmega1280
uno-test: CPU=
uno-test: lib
	cd test/hw  && make clean 
	cd test/hw  && make  
	cd test/hw  && make  upload


uno: ARDUINO=uno
uno: CPU=
uno:
#	cd $(ARDUINO_SRC)  &&  make light-clean uno
#	cd test/hw  && make clean 
#	cd test/hw  && export BUILD_FROM_VCS=true && make  
	cd test/hw  && export BUILD_FROM_VCS=true && make  upload

due: ARDUINO=due
due: CPU=
due:
	cd $(ARDUINO_SRC)  &&  make light-clean due
	cd test/hw  && make clean 
	cd test/hw  && export BUILD_FROM_VCS=true && make  
	cd test/hw  && make  upload

mega: ARDUINO=mega
mega: CPU=
mega:
	cd $(ARDUINO_SRC)  &&  make light-clean mega
	cd test/hw  && make clean 
	cd test/hw  && export BUILD_FROM_VCS=true && make  
	cd test/hw  && make  upload


install: INSTALL_DIR=/usr/local
install: ARDUINO=uno
install: 
	mkdir -p $(INSTALL_DIR)/searduino
	echo "Will install in directory: $(INSTALL_DIR)"
	export INSTALL_DIR=$(INSTALL_DIR) && cd arduino-sources && make install
	export INSTALL_DIR=$(INSTALL_DIR) && cd faked-arduino && make install
	cp -r mk/ $(INSTALL_DIR)/searduino
	cp -r example/ $(INSTALL_DIR)/searduino

dist: clean
	-rm searduino-*.tar
	tar cvf $(DIST_FILE) AUTHORS  ChangeLog  faked-arduino  Makefile  mk  NEWS  pardon  qard  README  test Makefile.in misc configure bin example
	@echo "Created: $(DIST_FILE)"

