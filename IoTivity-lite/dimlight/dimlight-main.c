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

/*
* dimlight-main.c - This is just the "main" section of the application
*/
/*
 tool_version          : 20171123
 input_file            : /home/cstevens1/workspace/emulatornew/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190215
 title of input_file   : Dimming
*/

#include <gtk/gtk.h>

#ifdef __linux__
/* linux specific code */
GtkWidget *g_my_image_0;
GtkWidget *g_my_image_33;
GtkWidget *g_my_image_66;
GtkWidget *g_my_image_100;
#endif

#ifdef __linux__
/* display the light with the specified brightness */
void display_light(int percent)
{
  int image_number;

  gtk_widget_hide(g_my_image_0);
  gtk_widget_hide(g_my_image_33);
  gtk_widget_hide(g_my_image_66);
  gtk_widget_hide(g_my_image_100);

  image_number = percent / 25;
  if (image_number > 3) {
    image_number = 3;
  }

  switch (image_number) {
    case 0:
      gtk_widget_show(g_my_image_0);
      break;
    case 1:
      gtk_widget_show(g_my_image_33);
      break;
    case 2:
      gtk_widget_show(g_my_image_66);
      break;
    case 3:
      gtk_widget_show(g_my_image_100);
      break;
  }
}
#endif

#include "device_builder_server.c"

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

#ifdef OC_SECURITY
void
random_pin_cb(const unsigned char *pin, size_t pin_len, void *data)
{
  (void)data;
  PRINT("\n====================\n");
  PRINT("Random PIN: %.*s\n", (int)pin_len, pin);
  PRINT("====================\n");
}
#endif /* OC_SECURITY */

void
factory_presets_cb(size_t device, void *data)
{
  (void)device;
  (void)data;
#if defined(OC_SECURITY) && defined(OC_PKI)
/* code to include an pki certificate and root trust anchor */
#include "oc_pki.h"
#include "pki_certs.h"
  int credid =
    oc_pki_add_mfg_cert(0, (const unsigned char *)my_cert, strlen(my_cert), (const unsigned char *)my_key, strlen(my_key));
  if (credid < 0) {
    PRINT("ERROR installing manufacturer certificate\n");
  } else {
    PRINT("Successfully installed manufacturer certificate\n");
  }

  if (oc_pki_add_mfg_intermediate_cert(0, credid, (const unsigned char *)int_ca, strlen(int_ca)) < 0) {
    PRINT("ERROR installing intermediate CA certificate\n");
  } else {
    PRINT("Successfully installed intermediate CA certificate\n");
  }

  if (oc_pki_add_mfg_trust_anchor(0, (const unsigned char *)root_ca, strlen(root_ca)) < 0) {
    PRINT("ERROR installing root certificate\n");
  } else {
    PRINT("Successfully installed root certificate\n");
  }

  oc_pki_set_security_profile(0, OC_SP_BLACK, OC_SP_BLACK, credid);
#endif /* OC_SECURITY && OC_PKI */
}


/**
* intializes the global variables
* registers and starts the handler

*/
void
initialize_variables(void)
{
  /* initialize global variables for resource "/binaryswitch" */  g_binaryswitch_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/dimming" */
  g_dimming_dimmingSetting = 30; /* current value of property "dimmingSetting" The current dimming value. */
  /* initialize global variables for resource "/lightstate" */
  g_lightstate_dimmingSetting = 30; /* current value of property "dimmingSetting" The current dimming value. */

  /* set the flag for NO oic/con resource. */
  oc_set_con_res_announced(false);

}

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

  PRINT("Used input file : \"/home/cstevens1/workspace/gentest/device_output/out_codegeneration_merged.swagger.json\"\n");
  PRINT("OCF Server name : \"Emulator\"\n");

  /*intialize the variables */
  initialize_variables();


  /* initializes the handlers structure */
  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources
#ifdef OC_CLIENT
                                       ,
                                       .requests_entry = 0
#endif
                                       };

#ifdef OC_SECURITY
  PRINT("Intialize Secure Resources\n");
  oc_storage_config("./devicebuilderserver_creds");
#endif /* OC_SECURITY */

#ifdef OC_SECURITY
  /* please comment out if the server:
    - have no display capabilities to display the PIN value
    - server does not require to implement RANDOM PIN (oic.sec.doxm.rdp) onboarding mechanism
  */
  oc_set_random_pin_callback(random_pin_cb, NULL);
#endif /* OC_SECURITY */

  oc_set_factory_presets_cb(factory_presets_cb, NULL);


  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return init;
  }

  PRINT("OCF server \"Emulator\" running, waiting on incoming connections.\n");

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
#endif

  /* shut down the stack */
  oc_main_shutdown();
  return 0;
}
