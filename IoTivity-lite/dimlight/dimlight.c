/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright 2017-2019 Open Connectivity Foundation
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/* Application Design
*
* support functions:
* app_init
*  initializes the oic/p and oic/d values.
* register_resources
*  function that registers all endpoints, e.g. sets the RETRIEVE/UPDATE handlers for each end point
*
* main
*  starts the stack, with the registered resources.
*
* Each resource has:
*  global property variables (per resource path) for:
*    the property name
*       naming convention: g_<path>_RESOURCE_PROPERTY_NAME_<propertyname>
*    the actual value of the property, which is typed from the json data type
*      naming convention: g_<path>_<propertyname>
*  global resource variables (per path) for:
*    the path in a variable:
*      naming convention: g_<path>_RESOURCE_ENDPOINT
*    array of interfaces, where by the first will be set as default interface
*      naming convention g_<path>_RESOURCE_INTERFACE
*
*  handlers for the implemented methods (get/post)
*   get_<path>
*     function that is being called when a RETRIEVE is called on <path>
*     set the global variables in the output
*   post_<path>
*     function that is being called when a UPDATE is called on <path>
*     checks the input data
*     if input data is correct
*       updates the global variables
*
*/
/*
 tool_version          : 20171123
 input_file            : /home/cstevens1/workspace/emulatornew/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190215
 title of input_file   : Dimming
*/

#include "oc_api.h"
#include "port/oc_clock.h"
#include <signal.h>

#ifdef WIN32
/* windows specific code */
#include <windows.h>
static CONDITION_VARIABLE cv;   /* event loop variable */
static CRITICAL_SECTION cs;     /* event loop variable */
#endif

#define MAX_STRING 30           /* max size of the strings. */
#define MAX_PAYLOAD_STRING 65   /* max size strings in the payload */
#define MAX_ARRAY 10            /* max size of the array */
/* Note: Magic numbers are derived from the resource definition, either from the example or the definition.*/

volatile int quit = 0;          /* stop variable, used by handle_signal */


/* global property variables for path: "/binaryswitch" */
static char g_binaryswitch_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_binaryswitch_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/dimming" */
static char g_dimming_RESOURCE_PROPERTY_NAME_dimmingSetting[] = "dimmingSetting"; /* the name for the attribute */
int g_dimming_dimmingSetting = 30; /* current value of property "dimmingSetting" The current dimming value. */
/* global property variables for path: "/lightstate" */
static char g_lightstate_RESOURCE_PROPERTY_NAME_dimmingSetting[] = "dimmingSetting"; /* the name for the attribute */
int g_lightstate_dimmingSetting = 30; /* current value of property "dimmingSetting" The current dimming value. */
static char g_lightstate_RESOURCE_PROPERTY_NAME_range[] = "range"; /* the name for the attribute */
/* array range  The valid range for the value Property in integer */
int g_lightstate_range[2];
size_t g_lightstate_range_array_size;/* registration data variables for the resources */

/* global resource variables for path: /binaryswitch */
static char g_binaryswitch_RESOURCE_ENDPOINT[] = "/binaryswitch"; /* used path for this resource */
static char g_binaryswitch_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_binaryswitch_nr_resource_types = 1;
static char g_binaryswitch_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.a","oic.if.baseline"}; /* interface if (as an array) */
int g_binaryswitch_nr_resource_interfaces = 2;

/* global resource variables for path: /dimming */
static char g_dimming_RESOURCE_ENDPOINT[] = "/dimming"; /* used path for this resource */
static char g_dimming_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.light.dimming"}; /* rt value (as an array) */
int g_dimming_nr_resource_types = 1;
static char g_dimming_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.a","oic.if.baseline"}; /* interface if (as an array) */
int g_dimming_nr_resource_interfaces = 2;

/* global resource variables for path: /lightstate */
static char g_lightstate_RESOURCE_ENDPOINT[] = "/lightstate"; /* used path for this resource */
static char g_lightstate_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.light.dimming"}; /* rt value (as an array) */
int g_lightstate_nr_resource_types = 1;
static char g_lightstate_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.a","oic.if.baseline"}; /* interface if (as an array) */
int g_lightstate_nr_resource_interfaces = 2;

/**
* function to set up the device.
*
*/
static int
app_init(void)
{
  int ret = oc_init_platform("ocf", NULL, NULL);
  /* the settings determine the appearance of the device on the network
     can be OCF1.3.1 or OCF2.0.0 (or even higher)
     supplied values are for OCF1.3.1 */
  ret |= oc_add_device("/oic/d", "oic.d.emulatorsample", "Dimming",
                       "ocf.1.0.0", /* icv value */
                       "ocf.res.1.3.0, ocf.sh.1.3.0",  /* dmv value */
                       NULL, NULL);
  return ret;
}

/**
* helper function to convert the interface string definition to the constant defintion used by the stack.
* @param interface the interface string e.g. "oic.if.a"
* @return the stack constant for the interface
*/
static int
convert_if_string(char *interface_name)
{
  if (strcmp(interface_name, "oic.if.baseline") == 0) return OC_IF_BASELINE;  /* baseline interface */
  if (strcmp(interface_name, "oic.if.rw") == 0) return OC_IF_RW;              /* read write interface */
  if (strcmp(interface_name, "oic.if.r" )== 0) return OC_IF_R;                /* read interface */
  if (strcmp(interface_name, "oic.if.s") == 0) return OC_IF_S;                /* sensor interface */
  if (strcmp(interface_name, "oic.if.a") == 0) return OC_IF_A;                /* actuator interface */
  if (strcmp(interface_name, "oic.if.b") == 0) return OC_IF_B;                /* batch interface */
  if (strcmp(interface_name, "oic.if.ll") == 0) return OC_IF_LL;              /* linked list interface */
  return OC_IF_A;
}


/**
* get method for "/binaryswitch" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_binaryswitch(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */

  PRINT("get_binaryswitch: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_binaryswitch_value);
    PRINT("   %s : %d\n", g_binaryswitch_RESOURCE_PROPERTY_NAME_value,  g_binaryswitch_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/dimming" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a dimming function.
* The Property "dimmingSetting" is an integer showing the current dimming level.
* If Property "step" is present then it represents the increment between dimmer values.
* When the Property "range" is omitted, then the range is [0,100].
* A value of 0 means total dimming; a value of 100 means no dimming.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_dimming(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */

  PRINT("get_dimming: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "dimmingSetting" */
    oc_rep_set_int(root, dimmingSetting, g_dimming_dimmingSetting );
    PRINT("   %s : %d\n", g_dimming_RESOURCE_PROPERTY_NAME_dimmingSetting, g_dimming_dimmingSetting );  /* not handled dimmingSetting */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/lightstate" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a dimming function.
* The Property "dimmingSetting" is an integer showing the current dimming level.
* If Property "step" is present then it represents the increment between dimmer values.
* When the Property "range" is omitted, then the range is [0,100].
* A value of 0 means total dimming; a value of 100 means no dimming.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_lightstate(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */

  PRINT("get_lightstate: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "dimmingSetting" */
    oc_rep_set_int(root, dimmingSetting, g_lightstate_dimmingSetting );
    PRINT("   %s : %d\n", g_lightstate_RESOURCE_PROPERTY_NAME_dimmingSetting, g_lightstate_dimmingSetting );  /* not handled dimmingSetting */
    /* property "range" */
    oc_rep_set_array(root, range);
    PRINT("   %s int = [ ", g_lightstate_RESOURCE_PROPERTY_NAME_range );
    for (int i=0; i< (int)g_lightstate_range_array_size; i++) {
      oc_rep_add_int(range, g_lightstate_range[i]);
      PRINT("   %d ", g_lightstate_range[i]);
    }
    oc_rep_close_array(root, range);
      /* not handled range */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* post method for "/binaryswitch" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_binaryswitch(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_binaryswitch:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_binaryswitch_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }

    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_binaryswitch_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_binaryswitch_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_binaryswitch_value);
    oc_rep_end_root_object();

    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    display_light(g_binaryswitch_value * 100);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/dimming" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:
* Sets the desired dimming level.
*
* @param requestRep the request representation.
*/
static void
post_dimming(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_dimming:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    if (strcmp ( oc_string(rep->name), g_dimming_RESOURCE_PROPERTY_NAME_dimmingSetting) == 0) {
      /* property "dimmingSetting" of type integer exist in payload */
      if (rep->type != OC_REP_INT) {
        error_state = true;
        PRINT ("   property 'dimmingSetting' is not of type int %d \n", rep->type);
      }


    }
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_dimming_RESOURCE_PROPERTY_NAME_dimmingSetting) == 0) {
        /* assign "dimmingSetting" */
        g_dimming_dimmingSetting = rep->value.integer;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_int(root, dimmingSetting, g_dimming_dimmingSetting );
    oc_rep_end_root_object();

    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    display_light(g_dimming_dimmingSetting);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
/**
* register all the resources to the stack
* this function registers all application level resources:
* - each resource path is bind to a specific function for the supported methods (GET, POST, PUT)
* - each resource is
*   - secure
*   - observable
*   - discoverable
*   - used interfaces (from the global variables).
*/
static void
register_resources(void)
{

  PRINT("Register Resource with local path \"/binaryswitch\"\n");
  oc_resource_t *res_binaryswitch = oc_new_resource(NULL, g_binaryswitch_RESOURCE_ENDPOINT, g_binaryswitch_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_binaryswitch_nr_resource_types);
  for( int a = 0; a < g_binaryswitch_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_binaryswitch_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_binaryswitch,g_binaryswitch_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_binaryswitch_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_binaryswitch, convert_if_string(g_binaryswitch_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_binaryswitch, convert_if_string(g_binaryswitch_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_binaryswitch_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_binaryswitch, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_binaryswitch, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_binaryswitch, true); */

  oc_resource_set_request_handler(res_binaryswitch, OC_GET, get_binaryswitch, NULL);

  oc_resource_set_request_handler(res_binaryswitch, OC_POST, post_binaryswitch, NULL);
  oc_add_resource(res_binaryswitch);

  PRINT("Register Resource with local path \"/dimming\"\n");
  oc_resource_t *res_dimming = oc_new_resource(NULL, g_dimming_RESOURCE_ENDPOINT, g_dimming_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_dimming_nr_resource_types);
  for( int a = 0; a < g_dimming_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_dimming_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_dimming,g_dimming_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_dimming_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_dimming, convert_if_string(g_dimming_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_dimming, convert_if_string(g_dimming_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_dimming_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_dimming, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_dimming, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_dimming, true); */

  oc_resource_set_request_handler(res_dimming, OC_GET, get_dimming, NULL);

  oc_resource_set_request_handler(res_dimming, OC_POST, post_dimming, NULL);
  oc_add_resource(res_dimming);

  PRINT("Register Resource with local path \"/lightstate\"\n");
  oc_resource_t *res_lightstate = oc_new_resource(NULL, g_lightstate_RESOURCE_ENDPOINT, g_lightstate_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_lightstate_nr_resource_types);
  for( int a = 0; a < g_lightstate_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_lightstate_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_lightstate,g_lightstate_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_lightstate_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_lightstate, convert_if_string(g_lightstate_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_lightstate, convert_if_string(g_lightstate_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_lightstate_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_lightstate, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_lightstate, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_lightstate, true); */

  oc_resource_set_request_handler(res_lightstate, OC_GET, get_lightstate, NULL);
  oc_add_resource(res_lightstate);
}

#ifndef NO_MAIN
#ifdef WIN32
/**
* signal the event loop (windows version)
* wakes up the main function to handle the next callback
*/
static void
signal_event_loop(void)
{
  WakeConditionVariable(&cv);
}
#endif
#ifdef __linux__
int oc_event_loop(void *arg);

/**
* signal the event loop (Linux)
* wakes up the main function to handle the next callback
*/
static void
signal_event_loop(void)
{
  g_timeout_add(0, oc_event_loop, 0);
}
#endif

/**
* handle Ctrl-C
* @param signal the captured signal
*/
void
handle_signal(int signal)
{
  (void)signal;

  #ifdef __linux__
  gtk_main_quit();
  #endif
}

#ifdef __linux__
int
oc_event_loop(void *arg)
{
  (void)arg;

  oc_clock_time_t next_event;

  next_event = oc_main_poll();
  if (next_event != 0) {
    g_timeout_add((next_event - oc_clock_time()) / 1.e03, oc_event_loop, 0);
  }

  return FALSE;
}

/* called when window is closed */
void on_window_main_destroy()
{
  handle_signal(0);
}
#endif

/**
* main application.
* intializes the global variables
* registers and starts the handler
* handles (in a loop) the next event.
* shuts down the stack
*/
int
main(int argc, char *argv[])
{
int init;

#ifdef WIN32
  /* windows specific */
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  /* install Ctrl-C */
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  /* linux specific */
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  /* install Ctrl-C */
  sigaction(SIGINT, &sa, NULL);

  GtkBuilder *builder;
  GtkWidget *window;

  g_timeout_add(0, oc_event_loop, 0);

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gtk_builder_connect_signals(builder, NULL);

  /* get pointers to the 4 images */
  g_my_image_0 = GTK_WIDGET(gtk_builder_get_object(builder, "my_image_0"));
  g_my_image_33 = GTK_WIDGET(gtk_builder_get_object(builder, "my_image_33"));
  g_my_image_66 = GTK_WIDGET(gtk_builder_get_object(builder, "my_image_66"));
  g_my_image_100 = GTK_WIDGET(gtk_builder_get_object(builder, "my_image_100"));

  g_object_unref(builder);

  gtk_widget_show(g_my_image_0);

  gtk_widget_show(window);
#endif
  /* initialize global variables for resource "/binaryswitch" */
  g_binaryswitch_value = false; /* current value of property "value" The status of the switch. */

  /* initialize global variables for resource "/dimming" */
  g_dimming_dimmingSetting = 30; /* current value of property "dimmingSetting" The current dimming value. */

  /* initialize global variables for resource "/lightstate" */
  g_lightstate_dimmingSetting = 30; /* current value of property "dimmingSetting" The current dimming value. */
  /* initialize array "range" : The valid range for the value Property in integer */
  g_lightstate_range[0] = 0;
  g_lightstate_range[1] = 100;
  g_lightstate_range_array_size = 2;
  /* set the flag for NO oic/con resource. */
  oc_set_con_res_announced(false);

  /* initializes the handlers structure */
  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources
#ifdef OC_CLIENT
                                       ,
                                       .requests_entry = 0
#endif
                                       };
  PRINT("Used input file : \"/home/cstevens1/workspace/emulatornew/device_output/out_codegeneration_merged.swagger.json\"\n");
  PRINT("OCF Server name : \"Dimming\"\n");

#ifdef OC_SECURITY
  PRINT("Intialize Secure Resources\n");
  oc_storage_config("./device_builder_server_creds/");
#endif /* OC_SECURITY */


  /* start the stack */
  init = oc_main_init(&handler);
  if (init < 0)
    return init;

  PRINT("OCF server \"Dimming\" running, waiting on incomming connections.\n");

#ifdef WIN32
  /* windows specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      oc_clock_time_t now = oc_clock_time();
      if (now < next_event) {
        SleepConditionVariableCS(&cv, &cs,
                                 (DWORD)((next_event-now) * 1000 / OC_CLOCK_SECOND));
      }
    }
  }
#endif

#ifdef __linux__
  /* linux specific loop */
  gtk_main();

/*
  while (quit != 1) {
    next_event = oc_main_poll();
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    pthread_mutex_unlock(&mutex);
  }
*/
#endif

  /* shut down the stack */
  oc_main_shutdown();
  return 0;
}
#endif /*NO_MAIN*/
