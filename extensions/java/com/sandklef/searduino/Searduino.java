/*****
 *                                                                   
 *                   Searduino
 *                      
 *   Copyright (C) 2013, 2014 Henrik Sandklef 
 *                                                                   
 * This program is free software; you can redistribute it and/or     
 * modify it under the terms of the GNU General Public License       
 * as published by the Free Software Foundation; either version 3    
 * of the License, or any later version.                             
 *                                                                   
 *                                                                   
 * This program is distributed in the hope that it will be useful,   
 * but WITHOUT ANY WARRANTY; without even the implied warranty of    
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     
 * GNU General Public License for more details.                      
 *                                                                   
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software       
 * Foundation, Inc., 51 Franklin Street, Boston,            
 * MA  02110-1301, USA.                                              
 ****/

package com.sandklef.searduino;

public class Searduino
{
    public  native int  getWriteTimelimit();
    public  native int  setWriteTimelimit(int limit);

    public  native void pauseArduinoCode();
    public  native void resumeArduinoCode();
    public  native void haltArduinoCode();
    public  native int  isPausedArduinoCode();
    public  native int  isHaltedArduinoCode();
    public  native int  isRunningArduinoCode();
    public  native int  isPausableArduinoCode();
    public  native void disableStreamedOutput();
    public  native void enableStreamedOutput();
    public  native void startArduinoCode();

    public  native String getArduinoCodeName();
    public  native void   closeArduinoCode();
    public  native String getSearduinoVersion();
    public  native String getSearduinoName();
    public  native String getBoardName();
    public  native int    setBoardName(String board);
    public  native int    setUpBoard();
    public  native String getBoardSetup();
    public  native int    getNrOfPins();
    public  native int    getBoardPins(String board);
    public  native String getSupportedBoards();

    public  native int getDigitalMode(int pin);
    public  native int setArduinoCodeName(String ac);
    public  native int getCurrentPinType(int pin);
    public  native int hasGenericPinType(int pin, int type);
    public  native String getSystemInformation();
    private native int registerPinCallback(SearduinoObserver o, int type);

    public native  int fakeAnalogInput(int pin, int val);
    public native  int fakeDigitalInput(int pin, int val);

    // private stuff
    private static final int SEARDUINO_PIN_MODE  = 1;
    private static final int SEARDUINO_PIN_VALUE = 2;
    private static final int SEARDUINO_PIN_TYPE  = 3;
    private static final int SEARDUINO_LOG       = 4;
    private static final int SEARDUINO_LCD       = 5;


    /* Must be the same as in faked-arduino/include/arduino/searduino-pin.h */
    public static final int SEARDUINO_PINTYPE_DIGITAL = 1 ;
    public static final int SEARDUINO_PINTYPE_ANALOG  = 2 ;
    public static final int SEARDUINO_PINTYPE_PWM     = 3 ;

    public static final String SEARDUINO_PINTYPE_DIGITAL_STRING = "Digital" ;
    public static final String SEARDUINO_PINTYPE_ANALOG_STRING  = "Analog" ;
    public static final String SEARDUINO_PINTYPE_PWM_STRING     = "PWM" ;

    SearduinoObserver observer;

    public Searduino()
    {
	System.loadLibrary("searduinojni");
    }

    public boolean hasGenericPinTypeBoolean(int pin, int type)
    {
	return (hasGenericPinType(pin, type)==1);
    }

    public int getPinMode(int pin)
    {
	return pin-1;
    }

    public static int getPinType(String pintype) 
    {
	if (pintype==null) {
	    return -1;
	}

	if (pintype.equals(SEARDUINO_PINTYPE_DIGITAL_STRING)) 
	    return SEARDUINO_PINTYPE_DIGITAL;
	if (pintype.equals(SEARDUINO_PINTYPE_ANALOG_STRING)) 
	    return SEARDUINO_PINTYPE_ANALOG;
	if (pintype.equals(SEARDUINO_PINTYPE_PWM_STRING)) 
	    return SEARDUINO_PINTYPE_PWM;
	return -1;
    }

    public static String getPinType(int pin) 
    {
	if (pin==SEARDUINO_PINTYPE_DIGITAL) 
	    return SEARDUINO_PINTYPE_DIGITAL_STRING;
	if (pin==SEARDUINO_PINTYPE_PWM) 
	    return SEARDUINO_PINTYPE_PWM_STRING;
	if (pin==SEARDUINO_PINTYPE_ANALOG) 
	    return SEARDUINO_PINTYPE_ANALOG_STRING;
	return "";
    }



    private void registerObserver(SearduinoObserver obs)
    {
	if (observer == null)
	    {
		observer = obs;
	    }
    }
    
    public void registerPinModeCallback(SearduinoObserver obs)
    {
	registerObserver(obs);
	registerPinCallback(obs, SEARDUINO_PIN_MODE);
    }
    
    public void registerPinOutCallback(SearduinoObserver obs)
    {
	registerObserver(obs);
	registerPinCallback(obs, SEARDUINO_PIN_VALUE);
    }
    
    public void registerPinTypeCallback(SearduinoObserver obs)
    {
	registerObserver(obs);
	registerPinCallback(obs, SEARDUINO_PIN_TYPE);
    }
    
    public void registerLogCallback(SearduinoObserver obs)
    {
	registerObserver(obs);
	try {
	    registerPinCallback(obs, SEARDUINO_LOG);
	} catch (Exception ie) {
	    System.out.println(ie.getMessage());
	}
    }
    
    public void registerLCDCallback(SearduinoObserver obs)
    {
	registerObserver(obs);
	try {
	    registerPinCallback(obs, SEARDUINO_LCD);
	} catch (Exception ie) {
	    System.out.println(ie.getMessage());
	}
    }
    
    public void setPwmPin(int pin, int value)
    {
	//	setGenericInput(pin, value, SEARDUINO_PINTYPE_PWM);
    }


    public int setInputPinValue(int pin, int val, int pin_type)
    {
	if (pin_type==SEARDUINO_PINTYPE_DIGITAL)
	    {
		return fakeDigitalInput(pin, val);
	    }
	else if (pin_type==SEARDUINO_PINTYPE_ANALOG)
	    {
		return fakeAnalogInput(pin, val);
	    }
	return -1;
    }
    
    public String getCurrentPinTypeString(int pin)
    {
	if (getCurrentPinType(pin)==SEARDUINO_PINTYPE_DIGITAL)
	    {
		return SEARDUINO_PINTYPE_DIGITAL_STRING;
	    }
	else if (getCurrentPinType(pin)==SEARDUINO_PINTYPE_ANALOG)
	    {
		return SEARDUINO_PINTYPE_ANALOG_STRING;
	    }
	else if (getCurrentPinType(pin)==SEARDUINO_PINTYPE_PWM)
	    {
		return SEARDUINO_PINTYPE_PWM_STRING;
	    }
	return "<undefined>";
    }
    
    public void setupPins()
    {
	for (int i=0;i<100;i++)
	    {
		System.out.println("setupPins() pin "+
				   i + " " +
				   getCurrentPinType(i));
	    }
    }
    
}