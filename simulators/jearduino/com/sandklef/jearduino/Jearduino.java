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

package com.sandklef.jearduino;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.BoxLayout;
import javax.swing.SwingUtilities;
import java.awt.*;
import java.awt.FlowLayout;
import javax.swing.JScrollPane;
import javax.swing.JLabel;
import javax.swing.JTabbedPane;

import com.sandklef.searduino.Searduino;
import com.sandklef.searduino.SearduinoObserver;

import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;


public class Jearduino extends JFrame implements SearduinoObserver, ExecEvent, PinEvent, BoardEvent, FileEvent, DirEvent, InoDirEvent, ArduinoCodeNameEvent, JearduinoEvent {

    Searduino searduino ;
    
    JearduinoPreferences jpref;
    JearduinoMenu        jmenu;
    GridBagConstraints constraints;

    PinTable pins;
    Logger serial ;
    Logger logger ;
    FileLogger fileLogger ;
    LCD    lcd ;
    InfoPanel infoPanel;
    ProjectPanel projectPanel;

    Container topPane; 
    JPanel    pane;
    JTabbedPane tabbedPane;

    private int codeNamesToStore ;
    private int nrpins = 0;
    public String version;

    JPanel topPanel;
    JPanel controlPanel;
    JPanel pinPanel;
    JPanel logPanel;
    JPanel loggerPanel;
    JPanel fileLoggerPanel;

    ExecControl ec;
    Board       board;
    
    JearduinoState jState;

    static int appSizeHeight = 700;
    static int appSizeWidth  = 1080;

    static int infoSizeHeight = 20;
    static int infoSizeWidth  = 800;

    public Jearduino() {

	jState = new JearduinoState();
    
	jpref = new JearduinoPreferences();

	codeNamesToStore = jpref.getArduinoCodeNameCount();
	constraints = new GridBagConstraints();
	
	
	pane = new JPanel();

	JTabbedPane tabbedPane = new JTabbedPane();
	topPane = getContentPane();
	topPane.add (tabbedPane);

	pane.setLayout(new GridBagLayout());

	/*  info */

	setSize(appSizeHeight, appSizeWidth);


        controlPanel = new JPanel();
	controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.PAGE_AXIS));

	pinPanel = new JPanel();
	logPanel = new JPanel();

	loggerPanel = new JPanel();
	fileLoggerPanel = new JPanel();


	pinPanel.setVisible(true);
	pinPanel.setLayout(new GridLayout());

	logPanel.setLayout(new BoxLayout(logPanel, BoxLayout.PAGE_AXIS));


	ec = new ExecControl(this);
	controlPanel.add(ec);
	controlPanel.setVisible(true);
	

	jmenu = new JearduinoMenu(this, this, this, this, this, this);
	setJMenuBar(jmenu);

	board = new Board(this);
	
	//	controlPanel.add(board);

	searduino = new Searduino();
	infoPanel = new InfoPanel();
	projectPanel = new ProjectPanel();

	logger    = new Logger( "Log");
	fileLogger = new FileLogger( "File Log", this);
	serial = new Logger( "Serial");
	lcd    = new LCD( "LCD");

	serial.setMaximumSize(new Dimension(200, 400)); 
	logger.setMaximumSize(new Dimension(200, 400)); 
	//fileLogger.setMaximumSize(new Dimension(600, 800)); 
	lcd.setMaximumSize(new Dimension(200, 400)); 

	serial.setPreferredSize(new Dimension(200, 100)); 
	logger.setPreferredSize(new Dimension(600, 150)); 
	//fileLogger.setPreferredSize(new Dimension(600, 800)); 
	lcd.setPreferredSize(new Dimension(200, 100)); 

	logPanel.add(serial);
	logPanel.add(lcd);

	loggerPanel.add(logger);
	fileLoggerPanel.add(fileLogger);

	pins = new PinTable(this, searduino);
	pins.setVisible(true);
	//	pinPanel.removeAll();
	pinPanel.add(pins);

        setTitle("Jearduino - Searduino's Java frontend for Arduino simulation");

        setLocationRelativeTo(null);
	setDefaultCloseOperation(EXIT_ON_CLOSE);

	

		
	searduino.setWriteTimelimit(0);
	searduino.disableStreamedOutput();

	searduino.registerPinModeCallback(this);
	searduino.registerPinOutCallback(this);
	searduino.registerPinTypeCallback(this);
	searduino.registerLogCallback(this);
	searduino.registerLCDCallback(this);

	projectPanel.setArduinoCodeName(searduino.getArduinoCodeName());
	infoPanel.setSearduinoVersion(searduino.getSearduinoVersion());

	tabbedPane.addTab("Control", null, pane,
			  "Main tab");

	tabbedPane.addTab("Log", null, fileLoggerPanel,
			  "Log tab");

	setupSizes();
    }

    public void addLog(String data) {
	SimpleDateFormat sdf = new SimpleDateFormat("yyyy-M-dd hh:mm:ss");
	String date = sdf.format(new Date());
	String text =  "[" + date + " jearduino-logger]: " + data + "\n";
	System.out.println("Logger:   " + data);
	System.out.flush();

	logger.addLog(text);
	fileLogger.addLog(text);
    }


    void tailLog() {
	;
    }

    public void setupSizes()
    {
	/*
	logger.setMaximumSize(new Dimension(200, 400));
	logger.setMinimumSize(new Dimension(200, 400));

	serial.setMaximumSize(new Dimension(200, 500));
	serial.setMinimumSize(new Dimension(200, 400));
	*/
	projectPanel.setMinimumSize(new Dimension( 10, 300));
	projectPanel.setMaximumSize(new Dimension( 10, 300));

	infoPanel.setMinimumSize(new Dimension( 400, 50));
	infoPanel.setMaximumSize(new Dimension( 400, 50));

	pinPanel.setMaximumSize(new Dimension(400, 600));
	pinPanel.setMinimumSize(new Dimension(400, 600));

	controlPanel.setMaximumSize(new Dimension(400, 80));
	controlPanel.setMinimumSize(new Dimension(400, 80));

	logger.setMaximumSize(new Dimension(500, 300));
	logger.setMinimumSize(new Dimension(500, 300));

	loggerPanel.setMaximumSize(new Dimension(800, 300));
	loggerPanel.setMinimumSize(new Dimension(800, 300));

	serial.setMaximumSize(new Dimension(300, 300));
	serial.setMinimumSize(new Dimension(300, 300));

	int he = 0;

	/* Row 0 */

	constraints.fill = GridBagConstraints.HORIZONTAL;
	constraints.ipady = he;      //
	constraints.ipadx = 0;      //
	constraints.weightx = 0.0;

	constraints.gridx = 0;
	constraints.gridy = 0;

	constraints.gridwidth = 2;
	System.out.println("projectPanel" + projectPanel + " c " + constraints);
	pane.add(projectPanel, constraints);


	/*  row  1*/

	constraints.fill = GridBagConstraints.HORIZONTAL;
	constraints.ipady = he;      //
	constraints.ipadx = 0;      //
	constraints.weightx = 0.0;

	constraints.gridx = 0;
	constraints.gridy = 1;

	constraints.gridwidth = 2;
	pane.add(controlPanel, constraints);


	/*  row  2*/

	constraints.fill = GridBagConstraints.HORIZONTAL;
	constraints.ipady = he;      //
	constraints.ipadx = 1;      //
	constraints.weightx = 0.0;

	constraints.gridx = 0;
	constraints.gridy = 2;

	constraints.gridwidth = 1;
	pane.add(pinPanel, constraints);

	constraints.fill = GridBagConstraints.HORIZONTAL;
	constraints.ipady = he;      //
	constraints.ipadx = 0;      //
	constraints.weightx = 0.0;

	constraints.gridx = 1;
	constraints.gridy = 2;

	constraints.gridwidth = 1;
	pane.add(logPanel, constraints);



	/* 3 nd row */
	constraints.fill = GridBagConstraints.HORIZONTAL;
	constraints.ipady = 0;      //
	constraints.weightx = 0.0;
	constraints.gridwidth = 2;

	constraints.gridx = 0;
	constraints.gridy = 3;

	pane.add(loggerPanel, constraints);


	constraints.fill = GridBagConstraints.HORIZONTAL;
	constraints.ipady = 0;      //
	constraints.weightx = 0.0;
	constraints.gridwidth = 0;

	constraints.gridx = 0;
	constraints.gridy = 4;
	pane.add(infoPanel, constraints);

    }


    private void setupBoardPins()
    {
	logger.addLog("setupBoardPins() <---------------");

	String bName  = searduino.getBoardName();
	if (bName==null) {
	    return ;
	}
	
	jpref.setBoard(bName);
	infoPanel.setBoardName(bName);
	
	nrpins = searduino.getNrOfPins();

	version = searduino.getSearduinoVersion();

	pins.setupPins();
    }
	
    

    private int setupBoard(String bName)
    {
	int ret;
	ret = searduino.setBoardName(bName);
	if (ret!=0) {
	    ec.unsetAll();
	    return 1;
	}
	jState.setBoard(bName);
	setupBoardPins();
	return 0;
    }


    public void handlePinTypeEvent(int pin, int type)
    {	
	//	System.out.println("EVENT: PIN TYPE-------------> JAVA (really) pin[" + pin + "]: type=" + type );
	pins.setTypeInputPin(pin, type, true);
    }
    
    public void handleLogEvent(int level, String msg)
    {
	// System.out.println("EVENT: LOG MESSAGE-------------> " + level + "  " + msg );

	// HESA HESA HESA
	serial.addLog(msg);
    }
    
    public void handleLCDEvent(String row1, String row2)
    {
	//	System.out.println("EVENT: LCD MESSAGE-------------> " + row1 + "  " + row2 );
	lcd.setText(row1, row2);
    }


    public void handleBoardChoiceEvent(String bName)
    {
	//	System.out.println("EVENT: Board choice -------------> BOARD: " + bName );
	setupBoard(bName);
    }

    public void handlePinModeEvent(int pin, int mode) {
	if (pin==0) return;

	// HESA HESA HESA
	addLog("EVENT: PIN MODE JAVA (really) pin[" + pin + "]: mode=" + mode );

	// System.out.println("JAVA (really) pin[" + pin + "]: mode=" + mode );
	pins.setMode(pin,mode);
    }

    public void handlePinOutEvent(int pin, int value, int type) {
	// System.out.println("OUT: " + pin + "  VALUE: " + value);
	//	addLog("JAVA (really) out[" + pin + "]: value=" + value +"\n");
	pins.setPinValue(pin, value, type);
    }


    public void inputValueEvent(int pin, int val)
    {
	//	System.out.println("PIN: " + pin + "  VALUE: " + val);
	searduino.setInputPinValue(pin, 
				   val, 
				   pins.getInputPinTypeLocal(pin));
    }

    public void ExecEvent(int type)
    {
	if (type==ExecControl.EXEC_CONTROL_PAUSE)
	    {
		addLog("PAUSE");
		searduino.pauseArduinoCode();
	    }
	else if (type==ExecControl.EXEC_CONTROL_RESUME)
	    {
		addLog("RESUME");
		searduino.resumeArduinoCode();
	    }
	else if (type==ExecControl.EXEC_CONTROL_HALT)
	    {
		addLog("HALT");
		stopArduinoCode(false);
	    }
	else if (type==ExecControl.EXEC_CONTROL_START)
	    {
		addLog("START");
		boolean hasDir=false;
		if (!jState.getCurrentSearduinoProject().equals("")) {
		    hasDir=true;
		}
		useCode(new File(jState.getCanonicalCodeName()), hasDir);
		searduino.startArduinoCode();
	    }
    }

    public void getAndUseArduinoCodeName(String code, boolean hasProject)
    {
	int ret;
	String codeName;

	stopArduinoCode(true);
	//	waitAWhile(500);

	/* If no code, use latest */
	if (hasProject) {
	    codeName = code;
	} else {
	    if (code==null) {
		codeName = jpref.getArduinoCodeName(1);
	    } else {
		codeName = code;
	    }
	}
	
	/* Open file */
	File f = new File(codeName);
	if (f==null) {
	    addLog("Couldn't open Arduino file: " + code);
	    return ;
	}

	/* Load code */
	openArduinoFileEvent(f, hasProject);
    }
    
    
    public void showArduinoCodeNameMenu()
    {
	/* update all menu items from jprefs */

	jmenu.removeCodeItems();
	
	for (int i=0;i<codeNamesToStore;i++) {
	    jmenu.updateCodeItem(i, 
				 jpref.getArduinoCodeName(i));
	}
    }


    /*
     * Method to remove faulty/duplicate entries from jpref
     *
     */ 
    public void removeJPrefFawlty()
    {
	for (int i=0; i<codeNamesToStore;i++) {
	    int next = i+1;
	    if (next==codeNamesToStore) {
		break;
	    }
	    
	    if (i!=codeNamesToStore) {
		File f = new File(jpref.getArduinoCodeName(i));
		boolean exists = f.isFile() && f.canRead() ;
		/*
		 *
		 * Check if duplicate entry or non-existing
		 *
		 */
		if ( 
		    (jpref.getArduinoCodeName(next).length() != 0 ) && 
		    jpref.getArduinoCodeName(i).equals(jpref.getArduinoCodeName(next)) ||
		    (!exists)
		     ) {
		    
		    for (int j=next;j<codeNamesToStore;j++) {
			//System.out.println("        MOVE   : " + (j+1) + " to " + j);  
			jpref.setArduinoCodeName(j,jpref.getArduinoCodeName(j+1));
			
			//System.out.println("        REMOVE : " + (j+1) );  
			jpref.unsetArduinoCodeName(j+1);
		    }
		}
	    }
	}
	
    }
    
    public void saveArduinoCodeName(String canonName, String shortName)
    {
	int ret;
	boolean found=false;

	/* remove duplicate/faulty entries if any */
	removeJPrefFawlty();

	/* Check if current code is already stored */
	for (int i=0; i<codeNamesToStore;i++) {
	    int next = i+1;
	    if (canonName.equals(jpref.getArduinoCodeName(i))) {
		found=true;
	    }
	}

	/* Ok, code is already stored, ... leave */
	if(found) { 
	    //	    System.out.println(" Not adding menu item, since it's already there: " 
	    //		       + canonName); 
	    return ; 
	}

	/* Move all items down one step */
	for (int i=(codeNamesToStore-1);i>-1;i--)
	    {
		/*		
				System.out.println("UPDATE MENU["+(i+1)+"]  " + 
				jpref.getArduinoCodeName(i) + 
				" prev: [ " + i + "]" + 
				jpref.getArduinoCodeName(i+1));  
		*/
		jpref.setArduinoCodeName(i+1, 
					 jpref.getArduinoCodeName(i));
		
	    }
	/* Store the new one in the first spot */
	jpref.setArduinoCodeName(0, canonName);
	
	/* DIsplay code name in project panel */
	projectPanel.setArduinoCodeName(shortName);

	/* Update menu items */
	showArduinoCodeNameMenu();
    }

    public void useCode(File f, boolean hasDirectory)  {
	int ret = 1;

	String code=null;
	String canonCode=null;

	addLog("useCode(" + f + ", " + hasDirectory + ")");

	try {
	    code = f.getName();
	    canonCode = f.getCanonicalPath();
	    ret = searduino.setArduinoCodeName(canonCode);
	}
	catch (java.lang.UnsatisfiedLinkError e) {
	    addLog("Failed loading Arduino code: " + f);
	    return ;
	}
	catch (IOException e)  {
	    System.out.println("Uh oh... could not get file name" );
	    addLog("Failed loading Arduino code: " + f.toString());
	}

	if (ret != 0 ) {
	    if (jState.buildTypeStub()) {
		setSearduinoProjectInfo(jState.getCurrentSearduinoProject(),
					"", "");
		addLog("Failed loading Arduino code: " + f);
	    }
	    projectPanel.setArduinoCodeName("");
	    ec.unsetAll();
	} else {
	    addLog("Loaded Arduino code: " + f);
	    
	    /* Set up pins */
	    setupBoardPins();
	    
	    if (f.isDirectory()) {
		setSearduinoProjectInfo(code, "", "");
	    } else {
		String dir;
		if (hasDirectory) {
		    dir = jState.getCurrentSearduinoProject();
		} else {
		    dir = "";
		} 
		setSearduinoProjectInfo(dir, code, canonCode);
		saveArduinoCodeName(code, canonCode);
		ec.setStartable();
	    }
	}
    }

    public void setSearduinoProjectInfo(String project, String shortCode, String longCode) {

	System.out.println("setSearduinoProjectInfo(" +  project +", " +  shortCode + ", " + longCode+")");

	/* Manage project info */
	if (project==null) { 
	    project=""; 
	}
	if (project.equals("")) { 
	    searduino.closeArduinoCode();
	}
	
	/* Set current project name */
	jState.setCurrentSearduinoProject(project);
	projectPanel.setProjectName(project);

	/* Manage code info */
	if (shortCode!=null) {
	    jState.setCodeName(shortCode, longCode);
	    projectPanel.setArduinoCodeName(shortCode);
	}
    }
    
    public void handleArduinoFileEvent(File f) {
	openArduinoFileEvent(f, false);
    }

    public void openArduinoFileEvent(File f, boolean hasProject)
    {
	int ret = 0;
	String canonFile=null;

	addLog("Opening file: " + f);

	setupBoardPins();

	try {
	    canonFile = f.getCanonicalPath();
	} catch (IOException e)  {
	    System.out.println("Failed get the canonical file name for " + f );
	    addLog("Failed loading Arduino code: " + f.toString());
	}

	System.out.println("f: " + f + "  " + f.isDirectory() );


	
	if (hasProject) {
	    setSearduinoProjectInfo(jState.getCurrentSearduinoProject(), 
				    f.getName(),
				    canonFile) ;
	} else {
	    setSearduinoProjectInfo("", f.getName(),canonFile) ;
	}
	useCode(f, hasProject);
    }

    public void stopArduinoCode(boolean unsetAC) {
	addLog("---> stopArduinoCode");
	searduino.haltArduinoCode();
	
	if (unsetAC) {
	    if (jState.buildTypeStub()) {
		// Unload arduino code 
		searduino.closeArduinoCode();
	    }
	}
	setupBoardPins();
	addLog("<--- stopArduinoCode");
    }


    public void waitAWhile(int del) {
	try {
	    Thread.sleep(del);
	} catch (InterruptedException e) {
	    System.out.println("waitAWhile() - exception\n");
	}
    }

    public void handleArduinoCodeNameEvent(int codeIdx)
    {
	stopArduinoCode(true);

	setSearduinoProjectInfo("", 
				jpref.getArduinoCodeName(codeIdx),
				jpref.getArduinoCodeName(codeIdx));

	searduino.setArduinoCodeName(jpref.getArduinoCodeName(codeIdx));
	ec.setStartable();
    }


    public void execCommand(String cmd) throws IOException
    {
	String commandOutput = null ;
	Process process = null;
	BufferedReader inStream = null;
	String line;
	
	process = Runtime.getRuntime().exec(cmd);
	inStream = new BufferedReader(
				      new InputStreamReader( process.getInputStream() ));  
	commandOutput="Command: " + cmd + "\n";
	while((line=inStream.readLine())!=null){
	    commandOutput += line + "\n";
	}
	addLog(commandOutput);
	System.out.println(" Output from: " + cmd);
	System.out.println(" 	 " + commandOutput);
    }


    public void handleJearduinoEvent(int i, Object o) {
	if (i==JearduinoEvent.JEARDUINO_EVENT_BUILD_PROJECT) {

	    jState.setCurrentBuildType(jState.SEARDUINO_STATE_STUB);
	    buildSearduinoProject(jState.getCurrentSearduinoProject());

	} else if (i==JearduinoEvent.JEARDUINO_EVENT_BUILD_ARDUINO) {

	    jState.setCurrentBuildType(jState.SEARDUINO_STATE_ARDUINO);
	    buildSearduinoProject(jState.getCurrentSearduinoProject(), 
				  searduino.getBoardName(), false);

	} else if (i==JearduinoEvent.JEARDUINO_EVENT_UPLOAD) {

	    jState.setCurrentBuildType(jState.SEARDUINO_STATE_ARDUINO);
	    buildSearduinoProject(jState.getCurrentSearduinoProject(), 
				  searduino.getBoardName(), true);

	}
    }

    

    private void getAndUseSearduinoDir(File dir)
    {
	String searduinoDir = System.getProperty("searduino.project.dir") ;
	String shortDir     = dir.getName();
	String libName      = searduinoDir + "/" + shortDir  + "/lib" + shortDir.toLowerCase() + ".so";

	stopArduinoCode(true);

	System.out.println(" --------------- : getAndUseArduinoCodeName(" + libName +");)");
	
	setSearduinoProjectInfo(dir.toString(), "", "");
	getAndUseArduinoCodeName(libName, true);
    }

    public void handleSearduinoDirEvent(File dir)
    {
	stopArduinoCode(true);

	getAndUseSearduinoDir(dir);
    }	


    private void buildSearduinoProject(File  dir, String board, boolean upload) 
    {
	String shortDir     = dir.getName();
	String searduinoDir = System.getProperty("searduino.project.dir") ;
	String buildCommand = System.getProperty("searduino.dir") + 
	    "/bin/searduino-builder " ;
	
	String boardArgs="";
	if (board.equals("")) {
	    board = "stub";
	}

	String uploadArgs="";
	if (upload) {
	    uploadArgs=" --upload ";
	}
	
	boardArgs = " --board " + board + " ";
	String buildArgs    = boardArgs + uploadArgs + searduinoDir + "/" + shortDir ;
	
	buildCommand = buildCommand + " " + buildArgs;
	try { 
	    execCommand(buildCommand);

	    if (jState.buildTypeStub()) {
		getAndUseSearduinoDir(dir);
	    } else {
		ec.unsetAll();
	    }
	} catch (IOException e) {
	    addLog("Failed building file: " + jState.getCurrentSearduinoProject());
	}
    }
    
    private void buildSearduinoProject(File  dir, String board) 
    {
	buildSearduinoProject(dir, board, false) ;
    }
    
    private void buildSearduinoProject(String  dirS, String board) 
    {
	if (dirS==null) return ;

	File dir = new File(dirS);
	buildSearduinoProject(dir, board);
    }

    private void buildSearduinoProject(String  dirS, String board, boolean upload) 
    {
	if (dirS==null) return ;

	File dir = new File(dirS);
	buildSearduinoProject(dir, board, upload);
    }

    private void buildSearduinoProject(String dirS) 
    {
	buildSearduinoProject(dirS, "stub");
    }

    private void buildSearduinoProject(File dir) 
    {
	buildSearduinoProject(dir, "stub");
    }

    private void createSearduinoFromIno(File dir) throws IOException
    {
	String shortDir      = dir.getName();
	String createCommand = System.getProperty("searduino.dir") + "/bin/searduino-builder";
	String createArgs = "--ino-dir " + dir;

	createCommand = createCommand + " " + createArgs;

	execCommand(createCommand);
    }

    public boolean validBoard() {
	return 	(!searduino.getBoardName().equals("<none>"));
    }

    public void handleInoDirEvent(File dir) {
	try {
	    createSearduinoFromIno(dir);
	    buildSearduinoProject(dir);
	    getAndUseSearduinoDir(dir);
	    setSearduinoProjectInfo(dir.toString(), "", "");
	} catch(IOException e) {
	    addLog("ERROR when building from INO file");
	    e.printStackTrace();  
	    return;
	}
    }

    public String getJearduinoState() {
	return jState.toString();
    }

    public String getSearduinoState() {
	return searduino.getSystemInformation();
    }

   public static void main(String[] args) {

	final Jearduino jearduino ;
	jearduino = new Jearduino();
	boolean startDirect = false;
	boolean buildDirect = false;
	int nrArgs     = args.length;
	String board   = null;
	String code    = null;
	String project = null;
	int ret=0;

	for (int i=0; i<nrArgs; i++)  {
	    if (args[i].equals("--board")) {
		if (i+1<nrArgs) 			    {
		    board = args[i+1];
		    i++;
		} else  {
		    System.err.println("Missing args to --board");
		    System.exit(1);
		}
	    }
	    else if (args[i].equals("--arduino-code")) {
		if (i+1<nrArgs) {
		    code = args[i+1];
		    i++;
		} else  {
		    System.err.println("Missing args to --arduino-code");
		    System.exit(1);
		}
	    }
	    else if (args[i].equals("--searduino-project")) {
		if (i+1<nrArgs) 			    {
		    project = args[i+1];
		    i++;
		} else  {
		    System.err.println("Missing args to --seardino-project");
		    System.exit(1);
		}
	    }
	    else if (args[i].equals("--build")) {
		buildDirect=true;
	    }
	    else if (args[i].equals("--start")) {
		startDirect=true;
	    }
	    else {
		System.err.println("Error when pasing command line '" + args[i] + "'");
		System.exit(1);
	    }
	}


	if ( (project!=null) && (code!=null) ){
	    System.out.println("You can't supply both"+
			       " --arduino-code and --searduino-project arguments");
	    System.exit(1);
	}
	
	if (board==null) {
	    ret = jearduino.setupBoard(jearduino.jpref.getBoard());
	} else {
	    ret = jearduino.setupBoard(board);
	}

			   
	jearduino.fileLogger.start();

	/* Update menu items */
	jearduino.showArduinoCodeNameMenu();

	System.out.println("Searduino version: " + jearduino.version);
	System.out.println("Searduino board:   " + jearduino.searduino.getBoardName());

	if (code==null) {
	    startDirect=false;
	}


	if (jearduino.validBoard()) {

	    if (project!=null) {
		jearduino.handleSearduinoDirEvent(new File(project));
	    } else if (code!=null) {
		jearduino.getAndUseArduinoCodeName(code, false);
	    } else {
		/* If no code supplied on cli - use latest */
		jearduino.getAndUseArduinoCodeName(null, false);
	    }
	    
	    if (buildDirect) {
		jearduino.handleJearduinoEvent(JearduinoEvent.JEARDUINO_EVENT_BUILD_PROJECT, null);
	    }
	    
	    if (startDirect) {
		System.out.println("Send start.....\n");
		jearduino.waitAWhile(2000);
		jearduino.ec.sendStart();
	    }
	}

        SwingUtilities.invokeLater(new Runnable() {
		public void run() {
		    jearduino.setVisible(true);
		}
	    });
    }
}
