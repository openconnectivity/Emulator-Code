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

#include "oc_api.h"
#include "port/oc_clock.h"
#include <signal.h>

#include <gtk/gtk.h>

#ifdef __linux__
/* linux specific code */
GtkWidget *g_my_image_0;
GtkWidget *g_my_image_33;
GtkWidget *g_my_image_66;
GtkWidget *g_my_image_100;
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
