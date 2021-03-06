/*****
 *                                                                   
 *                   Searduino
 *                      
 *   Copyright (C) 2011, 2012 Henrik Sandklef 
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

#include <Python.h>

#include <Arduino.h>
#include <pthread.h>

#include "seasim/seasim.h"

pthread_t   searduino_thread_impl;
pthread_t  *searduino_thread = &searduino_thread_impl;


//#define PEARDUINO_DEBUG_FUNCATION_CALLS

#ifdef PEARDUINO_DEBUG_FUNCATION_CALLS
#define PEARDUINO_PRINT_IN()            printf ("--->  %s() \n", __func__); fflush(stdout); 
#define PEARDUINO_PRINT_INSIDE()        printf ("--- INSIDE  %s()\n", __func__); fflush(stdout);
#define PEARDUINO_PRINT_INSIDE_STR(str) printf ("--- INSIDE  %s():  %s\n", __func__, str); fflush(stdout);
#define PEARDUINO_PRINT_OUT()           printf ("<---  %s()\n", __func__);fflush(stdout); 
#else
#define PEARDUINO_PRINT_IN() 
#define PEARDUINO_PRINT_INSIDE() 
#define PEARDUINO_PRINT_INSIDE_STR(str) 
#define PEARDUINO_PRINT_OUT() 
#endif


/*
static PyObject *my_dig_callback = NULL;
static PyObject *my_ana_callback = NULL;
*/
static PyObject *my_callback = NULL;
static PyObject *my_dig_mode_callback = NULL;
static PyObject *my_log_callback = NULL;
/*
static PyObject *c_my_set_dig_callback(PyObject *dummy, PyObject *args);
static PyObject *c_my_set_ana_callback(PyObject *dummy, PyObject *args);
*/
static PyObject *c_my_set_callback(PyObject *dummy, PyObject *args);
static PyObject *c_my_set_dig_mode_callback(PyObject *dummy, PyObject *args);

void* c_arduino_code(void *in);


void 
new_out(uint8_t pin, unsigned int val, uint8_t pin_type)
{
  PyObject *arglist;
  PyObject *result; 

  PyGILState_STATE gstate;   
  gstate = PyGILState_Ensure();   

  PEARDUINO_PRINT_IN();

  PEARDUINO_PRINT_INSIDE();

  if (my_callback!=NULL)
    {
      arglist = Py_BuildValue("(iii)", pin, val, pin_type);
      
      if (arglist==NULL)
	{
	  printf ("wooops, arglist is no no\n");
	  exit(1);
	}
      
      //printf(" Arguments to callback: (%d,%d) ", pin, val);      fflush(stdout); 
      //PyObject_Print(arglist, stdout, Py_PEARDUINO_PRINT_RAW);

      result = PyEval_CallObject(my_callback, arglist);

      Py_DECREF(arglist);
      
      if (result)
	{
	  Py_DECREF(result);
	}      
    }
  else
    {
      fprintf (stderr, "*** ERRROR ***\n");
      fprintf (stderr, "*** Could not call out callback since no callback ***\n");
    }


  PyGILState_Release(gstate);
  PEARDUINO_PRINT_OUT();
}






void 
log_callback(uint8_t level, const char *text)
{
  PyObject *arglist;
  PyObject *result; 

  PyGILState_STATE gstate;   
  gstate = PyGILState_Ensure();   

  PEARDUINO_PRINT_IN();

  PEARDUINO_PRINT_INSIDE();

  /*
  printf ("PEAR got %d %s\n", level, text);
  printf ("PEAR will call %p\n", my_log_callback); fflush(stdout);
  */
  if (my_log_callback!=NULL)
    {

      arglist = Py_BuildValue("(is)", level, text);

      if (arglist==NULL)
	{
	  printf ("wooops, arglist is no no\n");
	  exit(1);
	}

      /* printf(" Arguments to callback:  ");      fflush(stdout); */
      //PyObject_Print(arglist, stdout, Py_PEARDUINO_PRINT_RAW);

      /* printf ("PEAR will call\n"); fflush(stdout); */

      result = PyEval_CallObject(my_log_callback, arglist);

      /* printf ("PEAR just called\n"); fflush(stdout); */

      Py_DECREF(arglist);
      
      if (result)
	{
	  Py_DECREF(result);
	}      
    }
  else
    {
      fprintf (stderr, "*** ERRROR ***\n");
      fprintf (stderr, "*** Could not call log callback since no callback ***\n");
    }

  PyGILState_Release(gstate);
  PEARDUINO_PRINT_OUT();
}

void 
new_dig_mode(uint8_t pin, uint8_t mode)
{
  PyObject *arglist;
  PyObject *result; 

  PyGILState_STATE gstate;   
  gstate = PyGILState_Ensure();   

  PEARDUINO_PRINT_IN();

  PEARDUINO_PRINT_INSIDE();

  if (my_dig_mode_callback!=NULL)
    {

       arglist = Py_BuildValue("(ii)", pin, mode);

      if (arglist==NULL)
	{
	  printf ("wooops, arglist is no no\n");
	  exit(1);
	}

      /* printf(" Arguments to callback:  ");      fflush(stdout);  */
      result = PyEval_CallObject(my_dig_mode_callback, arglist);

      Py_DECREF(arglist);
      
      if (result)
	{
	  Py_DECREF(result);
	}      
    }
  else
    {
      fprintf (stderr, "*** ERRROR ***\n");
      fprintf (stderr, "*** Could not call digital mode callback since no callback ***\n");
    }

  PyGILState_Release(gstate);
  PEARDUINO_PRINT_OUT();
}


static PyObject* c_get_pin_mode(PyObject* self, PyObject* args)
{
  uint8_t pin;
  uint8_t mode ;
  PEARDUINO_PRINT_IN();
  
  PyArg_ParseTuple(args, "i", &pin);

  mode = seasim_get_dig_mode(pin);

  PyObject* o = Py_BuildValue("i", mode);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_add_i2c_device(PyObject* self, PyObject* args)
{
  int ret;
  unsigned int dev_nr ; 
  char *i2c_lib;
  PEARDUINO_PRINT_IN();
  
  PyArg_ParseTuple(args, "is", &dev_nr, &i2c_lib);
  
  ret = seasim_i2c_add_device(dev_nr, i2c_lib);
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}




static PyObject* c_set_arduino_code(PyObject* self, PyObject* args)
{
  uint8_t ret;
  char *ard_lib;
  PEARDUINO_PRINT_IN();
  
  PyArg_ParseTuple(args, "s", &ard_lib);

  ret = seasim_set_arduino_code_name(ard_lib);
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_set_write_timelimit(PyObject* self, PyObject* args)
{
  unsigned int ret;
  unsigned int val;
  PEARDUINO_PRINT_IN();
  
  PyArg_ParseTuple(args, "i", &val);

  /* printf ("pear: before %d\n", get_digitalWrite_timelimit()); */
  seasim_set_write_timelimit(val);
  /* printf ("pear: after  %d\n", get_digitalWrite_timelimit()); */
  PyObject* o = Py_BuildValue("i", val);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_disable_streamed_output(PyObject* self, PyObject* args)
{
  unsigned int ret;
  PEARDUINO_PRINT_IN();
  
  seasim_disable_streamed_output();
  PyObject* o = Py_BuildValue("i", 0);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_enable_streamed_output(PyObject* self, PyObject* args)
{
  unsigned int ret;
  PEARDUINO_PRINT_IN();
  
  seasim_enable_streamed_output();
  PyObject* o = Py_BuildValue("i", 0);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_hid_enable_feedback(PyObject* self, PyObject* args)
{
  unsigned int ret;
  PEARDUINO_PRINT_IN();
  
  seasim_hid_enable_feedback();
  PyObject* o = Py_BuildValue("i", 0);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_hid_disable_feedback(PyObject* self, PyObject* args)
{
  unsigned int ret;
  PEARDUINO_PRINT_IN();

  seasim_hid_disable_feedback();
  PyObject* o = Py_BuildValue("i", 0);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_has_generic_pin_type(PyObject* self, PyObject* args)
{
  unsigned int ret;
  unsigned int pin;
  unsigned int type;
  
  PyArg_ParseTuple(args, "i", &pin);
  PyArg_ParseTuple(args, "i", &type);

  PEARDUINO_PRINT_IN();
  
  ret = seasim_has_generic_pin_type(pin, type);
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}


static PyObject* c_get_current_pin_type(PyObject* self, PyObject* args)
{
  unsigned int ret;
  unsigned int pin;
  
  PyArg_ParseTuple(args, "i", &pin);

  PEARDUINO_PRINT_IN();
  
  ret = seasim_get_current_pin_type(pin);
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}


static PyObject* c_get_write_timelimit(PyObject* self, PyObject* args)
{
  unsigned int ret;
  PEARDUINO_PRINT_IN();
  
  ret = seasim_get_write_timelimit();
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_searduino_initialise(PyObject* self, PyObject* args)
{
  uint8_t ret;
  PEARDUINO_PRINT_IN();
  
  PEARDUINO_PRINT_INSIDE_STR("Setting up searduino\n");

  /*  searduino_set_arduino_code_name("libarduino-code.so"); */
  ret = searduino_setup();
  if (ret!=0)
    {
      PEARDUINO_PRINT_INSIDE_STR("Setting up searduino seem to have failed\n");
      return;
    }
  PEARDUINO_PRINT_INSIDE_STR("Register callback for dig out"
			"(in communication module)\n");

  /*
  seasim_register_digout_sim_cb(new_dig_out);
  seasim_register_anaout_sim_cb(new_ana_out);
  */

  my_log_callback = NULL;

  seasim_register_out_sim_cb(new_out);
  seasim_register_dig_mode_sim_cb(new_dig_mode);
  seasim_register_log_cb(log_callback);
  
  PEARDUINO_PRINT_OUT();
  PyObject* o = Py_BuildValue("i", 0);
  return o;
}


static PyObject* c_start(PyObject* self, PyObject* args)
{
  uint8_t ret;
  int retval;
  PEARDUINO_PRINT_IN();
  
  printf ("joining thread....\n");
  pthread_join(searduino_thread_impl, (void**)&retval);
  printf ("joining thread....%p\n", retval);

  PEARDUINO_PRINT_INSIDE_STR("Starting thread for arduino code\n");
  pthread_create(searduino_thread, NULL, c_arduino_code, NULL);

  PEARDUINO_PRINT_OUT();
  PyObject* o = Py_BuildValue("i", 0);
  return o;
}


static PyObject* c_stop(PyObject* self, PyObject* args)
{
  uint8_t ret;
  PEARDUINO_PRINT_IN();
  
  PEARDUINO_PRINT_INSIDE_STR("Stoping thread for arduino code\n");
  ret = pthread_cancel(searduino_thread_impl);

  PEARDUINO_PRINT_OUT();
  PyObject* o = Py_BuildValue("i", ret);
  return o;
}




static PyObject* c_Read(PyObject* self, PyObject* args)
{
  int pin;
  unsigned int val ;
  unsigned int pin_type ;
  PEARDUINO_PRINT_IN();
  
  PyArg_ParseTuple(args, "(ii)", &pin, &pin_type);

  val = seasim_get_output(pin, pin_type);

  PyObject* o = Py_BuildValue("i", val);

  PEARDUINO_PRINT_OUT();
  return o;
}


/*
 * Function to be called from Python
 */
static PyObject* c_ext_fake_input(PyObject* self, PyObject* args)
{
  unsigned int pin;
  unsigned int val;
  unsigned int pin_type;
  PEARDUINO_PRINT_IN();

  if (!PyArg_ParseTuple(args, "iii", &pin, &val, &pin_type))
    {
      return NULL;
    }

  PEARDUINO_PRINT_INSIDE_STR("wrapper code sets input pin\n");

  val= seasim_fake_input(pin, val, pin_type);

  PyObject* o = Py_BuildValue("i", val);

  PEARDUINO_PRINT_OUT();
  return o;
}


static PyObject* c_is_pausable(PyObject* self, PyObject* args)
{
  unsigned int ret;

  PEARDUINO_PRINT_IN();
  
  ret = seasim_is_pausable();
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}



PyObject * c_pause(void)
{
  PEARDUINO_PRINT_INSIDE_STR("in C wrapper: want to pause\n");

  PyObject* res = Py_BuildValue("i", 0);

  seasim_set_paused();

  PEARDUINO_PRINT_INSIDE_STR("in C: have paused\n");
  return res;
}

PyObject * c_resume(void)
{
  PyObject* res = Py_BuildValue("i", 0);
  Py_INCREF(Py_None);
  PEARDUINO_PRINT_INSIDE_STR("in C wrapper: want to resume\n");

  seasim_set_running();

  PEARDUINO_PRINT_INSIDE_STR("in C: is resumed\n");
  return res;
}


PyObject * c_quit(void)
{
  PyObject* res = Py_BuildValue("i", 0);
  Py_INCREF(Py_None);
  PEARDUINO_PRINT_INSIDE_STR("in C wrapper: want to quit\n");

  searduino_set_halted();

  PEARDUINO_PRINT_INSIDE_STR("in C: is set to quit\n");
  return res;
}

static PyObject *
c_my_set_log_callback(PyObject *dummy, PyObject *args)
{
  PyObject *result = NULL;
  PyObject *temp;
  PEARDUINO_PRINT_IN();

  usleep(1000*1000*3);

  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      PEARDUINO_PRINT_OUT();

      return NULL;
    }

    Py_XINCREF(temp);         /* Add a ref to the new callback */
    Py_XDECREF(my_log_callback);  /* Dispose possible previous callback */
    my_log_callback = temp;       /* Remember new callback */
    
    /* Boilerplate to return "None" */
    Py_INCREF(Py_None);
    result = Py_None;

    PEARDUINO_PRINT_INSIDE_STR("Python log callback is registered");
    usleep (100);
  }

  PEARDUINO_PRINT_OUT();
  return result;
}

static PyObject *
c_get_searduino_version(PyObject *dummy, PyObject *args)
{
  const char * val;
  PEARDUINO_PRINT_IN();

  val= seasim_get_searduino_version();
  PyObject* o = Py_BuildValue("s", val);

  PEARDUINO_PRINT_OUT();
  return o;
}


static PyObject *
c_get_searduino_name(PyObject *dummy, PyObject *args)
{
  const char * val;
  PEARDUINO_PRINT_IN();

  val= seasim_get_searduino_name();
  PyObject* o = Py_BuildValue("s", val);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject *
c_get_supported_boards(PyObject *dummy, PyObject *args)
{
  const char * val;
  PEARDUINO_PRINT_IN();

  val= seasim_get_supported_boards();
  PyObject* o = Py_BuildValue("s", val);

  PEARDUINO_PRINT_OUT();
  return o;
}



static PyObject *
c_get_board_name(PyObject *dummy, PyObject *args)
{
  const char * val;
  PEARDUINO_PRINT_IN();

  val= seasim_get_board_name();

  PyObject* o = Py_BuildValue("s", val);

  PEARDUINO_PRINT_OUT();
  return o;
}

static PyObject* c_set_board_name(PyObject* self, PyObject* args)
{
  int ret;
  char *name;
  PEARDUINO_PRINT_IN();
  
  PyArg_ParseTuple(args, "s", &name);
  
  ret = seasim_set_board_name(name);
  PyObject* o = Py_BuildValue("i", ret);

  PEARDUINO_PRINT_OUT();
  return o;
}

/*
 * Another function to be called from Python
 */
/*
 * Bind Python function names to our C functions
 */
static PyMethodDef myModule_methods[] = {
  {"seasim_Read",                    (PyCFunction)c_Read, METH_VARARGS, NULL},
  {"seasim_fake_input",              (PyCFunction)c_ext_fake_input, METH_VARARGS, NULL},
  {"seasim_set_dig_mode_callback",   (PyCFunction)c_my_set_dig_mode_callback, METH_VARARGS, NULL},
  {"seasim_set_callback",            (PyCFunction)c_my_set_callback, METH_VARARGS, NULL},
  {"seasim_set_log_callback",        (PyCFunction)c_my_set_log_callback, METH_VARARGS, NULL},
  {"my_arduino_code",                (PyCFunction)c_arduino_code, METH_VARARGS, NULL},
  {"seasim_pause",                   (PyCFunction)c_pause, METH_VARARGS, NULL},
  {"seasim_resume",                  (PyCFunction)c_resume, METH_VARARGS, NULL},
  {"seasim_quit",                    (PyCFunction)c_quit, METH_VARARGS, NULL},
  {"seasim_get_pin_mode",            (PyCFunction)c_get_pin_mode, METH_VARARGS, NULL},
  {"seasim_set_arduino_code",        (PyCFunction)c_set_arduino_code, METH_VARARGS, NULL},
  {"seasim_add_i2c_device",          (PyCFunction)c_add_i2c_device, METH_VARARGS, NULL},
  {"seasim_initialise",              (PyCFunction)c_searduino_initialise, METH_VARARGS, NULL},
  {"seasim_start",                   (PyCFunction)c_start, METH_VARARGS, NULL},
  {"seasim_stop",                    (PyCFunction)c_stop, METH_VARARGS, NULL},
  {"seasim_set_write_timelimit",     (PyCFunction)c_set_write_timelimit, METH_VARARGS, NULL},
  {"seasim_get_write_timelimit",     (PyCFunction)c_get_write_timelimit, METH_VARARGS, NULL},
  {"seasim_disable_streamed_output", (PyCFunction)c_disable_streamed_output, METH_VARARGS, NULL},
  {"seasim_enable_streamed_output",  (PyCFunction)c_enable_streamed_output, METH_VARARGS, NULL},
  {"seasim_get_searduino_version",   (PyCFunction)c_get_searduino_version, METH_VARARGS, NULL},
  {"seasim_get_searduino_name",      (PyCFunction)c_get_searduino_name, METH_VARARGS, NULL},
  {"seasim_set_board_name",          (PyCFunction)c_set_board_name, METH_VARARGS, NULL},
  {"seasim_get_board_name",          (PyCFunction)c_get_board_name, METH_VARARGS, NULL},
  {"seasim_has_generic_pin_type",    (PyCFunction)c_has_generic_pin_type, METH_VARARGS, NULL},
  {"seasim_get_current_pin_type",    (PyCFunction)c_get_current_pin_type, METH_VARARGS, NULL},
  {"seasim_hid_enable_feedback",     (PyCFunction)c_hid_enable_feedback, METH_VARARGS, NULL},
  {"seasim_hid_disable_feedback",    (PyCFunction)c_hid_disable_feedback, METH_VARARGS, NULL},
  {"seasim_get_supported_boards",    (PyCFunction)c_get_supported_boards, METH_VARARGS, NULL},
  {"seasim_is_pausable",             (PyCFunction)c_is_pausable, METH_VARARGS, NULL},

  {NULL, NULL, 0, NULL}
};
  /*  {"seasim_set_dig_callback", (PyCFunction)c_my_set_dig_callback, METH_VARARGS, NULL},
  {"seasim_set_ana_input", (PyCFunction)c_ext_set_ana_input, METH_VARARGS, NULL},
  {"seasim_set_dig_input", (PyCFunction)c_ext_set_dig_input, METH_VARARGS, NU
  {"seasim_analogRead", (PyCFunction)c_analogRead, METH_VARARGS, NULL},
  {"seasim_digitalRead", (PyCFunction)c_digitalRead, METH_VARARGS, NULL},
*/





void* c_arduino_code(void *in)
{

  usleep(1000*1000);
  fprintf (stderr, "Starting Arduino code\n");
  usleep(1000000);

  searduino_main_entry(NULL);

  //  while(1) { printf ("ping \n"); usleep (500*1000); }

  return NULL;
}


static PyObject *
c_my_set_dig_mode_callback(PyObject *dummy, PyObject *args)
{
  PyObject *result = NULL;
  PyObject *temp;
  PEARDUINO_PRINT_IN();

  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      PEARDUINO_PRINT_OUT();

      return NULL;
    }

    Py_XINCREF(temp);         /* Add a ref to the new callback */
    Py_XDECREF(my_dig_mode_callback);  /* Dispose possible previous callback */
    my_dig_mode_callback = temp;       /* Remember new callback */
    
    /* Boilerplate to return "None" */
    Py_INCREF(Py_None);
    result = Py_None;

    PEARDUINO_PRINT_INSIDE_STR("Python callback is registered");
    usleep (1000);
  }

  PEARDUINO_PRINT_OUT();
  return result;
}


static PyObject *
c_my_set_callback(PyObject *dummy, PyObject *args)
{
  PyObject *result = NULL;
  PyObject *temp;
  PEARDUINO_PRINT_IN();

  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      PEARDUINO_PRINT_OUT();

      return NULL;
    }

    Py_XINCREF(temp);         /* Add a ref to the new callback */
    Py_XDECREF(my_callback);  /* Dispose possible previous callback */
    my_callback = temp;       /* Remember new callback */

    /* Boilerplate to return "None" */
    Py_INCREF(Py_None);
    result = Py_None;

    PEARDUINO_PRINT_INSIDE_STR("Python callback is registered");
    usleep (1000);
  }

  PEARDUINO_PRINT_OUT();
  return result;
}

/*
 * Python calls this to let us initialize our module
 */
void initpearduino()
{
  int ret;

  PyEval_InitThreads();
  //PyEval_ReleaseLock();


  PEARDUINO_PRINT_INSIDE_STR("Init pearduino module\n");
  (void) Py_InitModule("pearduino", myModule_methods);

  
  PEARDUINO_PRINT_INSIDE_STR("*** All set up ***  in wrapper\n");
}
