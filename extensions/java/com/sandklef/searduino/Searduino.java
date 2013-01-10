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
    public  native String getSearduinoVersion();
    public  native String getSearduinoName();
    public  native String getBoardName();
    public  native int    setBoardName(String board);
    public  native int    getNrOfPins();

    public  native int getDigitalMode(int pin);
    public  native int setArduinoCodeName(String ac);
    public  native int getCurrentPinType(int pin);
    private native int registerPinCallback(SearduinoObserver o, int type);
    public  native int setGenericInput(int pin, int val, int pin_type);

    // private stuff
    private static final int SEARDUINO_PIN_MODE  = 1;
    private static final int SEARDUINO_PIN_VALUE = 2;
    private static final int SEARDUINO_PIN_TYPE  = 3;
    private static final int SEARDUINO_LOG       = 4;


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

    public int getPinMode(int pin)
    {
	return pin-1;
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
    
    public void setPwmPin(int pin, int value)
    {
	//	setGenericInput(pin, value, SEARDUINO_PINTYPE_PWM);
    }


    public int setInputPinValue(int pin, int val, int pin_type)
    {
	System.out.println("SETTING === PIN: " + pin + "  VALUE: " + val + "  TYPE: " + pin_type);	
	return setGenericInput(pin, val, pin_type);
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