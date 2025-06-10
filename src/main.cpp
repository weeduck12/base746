#include "lvgl.h"
#include <Arduino.h>
#define PULSE  8
#define DIR 6 
#define ENABLE 5

volatile int etat_moteur = 0;
int val_slide = 200;

 void event_handler_btn1(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
      etat_moteur = 1;
    }
}

void event_handler_btn2(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
      etat_moteur = 2;
    }
}
 void event_handler_btnstop(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
      etat_moteur = 0;
    }
}

void event_handler_slide(lv_event_t * e)
{
    lv_obj_t * slide = (lv_obj_t *)lv_event_get_target(e);
    val_slide = lv_slider_get_value(slide) + 200;
}

void testLvgl()
{
  // Initialisations générales
  lv_obj_t * label;
  // bouton 1 
  lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn1, event_handler_btn1, LV_EVENT_CLICKED, NULL);
  lv_obj_align(btn1, LV_ALIGN_DEFAULT, 10, 50);
  lv_obj_set_height(btn1, LV_SIZE_CONTENT);

  label = lv_label_create(btn1);
  lv_label_set_text(label, "Sens antihoraire ");
  lv_obj_center(label);
  
  // bouton 2
  lv_obj_t * btn2 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn2, event_handler_btn2, LV_EVENT_CLICKED, NULL);
  lv_obj_align(btn2, LV_ALIGN_DEFAULT, 10, 100);
  lv_obj_set_height(btn2, LV_SIZE_CONTENT);

  label = lv_label_create(btn2);
  lv_label_set_text(label, "Sens horaire");
  lv_obj_center(label);

  // bouton stop
  lv_obj_t * btnstop = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btnstop, event_handler_btnstop, LV_EVENT_CLICKED, NULL);
  lv_obj_align(btnstop, LV_ALIGN_DEFAULT, 10, 150);
  lv_obj_set_height(btnstop, LV_SIZE_CONTENT);

  label = lv_label_create(btnstop);
  lv_label_set_text(label, "Stop");
  lv_obj_center(label);
  //slider
  
  lv_obj_t * slide = lv_slider_create(lv_screen_active());
  lv_obj_add_event_cb(slide, event_handler_slide, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_align(slide, LV_ALIGN_DEFAULT, 10, 200);
  lv_obj_set_width(slide, 200);
  lv_slider_set_range(slide, 0, 1000);
  //cercle
  lv_obj_t * cercle = lv_btn_create(lv_scr_act());
  lv_obj_set_size(cercle, 200, 200);
  lv_obj_set_style_radius(cercle, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(cercle,LV_ALIGN_DEFAULT,270,50);
}

#ifdef ARDUINO

#include "lvglDrivers.h"

// à décommenter pour tester la démo
// #include "demos/lv_demos.h"

void mySetup()
{
  // à décommenter pour tester la démo
  // lv_demo_widgets();
  pinMode(PULSE, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  // Initialisations générales
  testLvgl();

}

void loop()
{
  // Inactif (pour mise en veille du processeur)
}

void myTask(void *pvParameters)
{
  // Init
  int etat = 0;
  TickType_t xLastWakeTime;
  // Lecture du nombre de ticks quand la tâche débute
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    // Loop
    if(etat_moteur == 1)
    {
      digitalWrite(ENABLE, LOW);
      digitalWrite(PULSE, etat);
      digitalWrite(DIR, HIGH);
          etat = !etat;

    }
    else if(etat_moteur == 2)
    {
      digitalWrite(ENABLE, LOW);
      digitalWrite(PULSE, etat);
      digitalWrite(DIR, LOW);
          etat = !etat;

    }
    else if(etat_moteur == 0){
      digitalWrite(ENABLE, HIGH);
      digitalWrite(PULSE, LOW);
    }
    else{
      digitalWrite(ENABLE, HIGH);
      digitalWrite(PULSE, LOW);
    }


    // Endort la tâche pendant le temps restant par rapport au réveil,
    // ici 200ms, donc la tâche s'effectue toutes les 200ms
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(val_slide)); // toutes les 200 ms
  }
}

#else

#include "lvgl.h"
#include "app_hal.h"
#include <cstdio>

int main(void)
{
  printf("LVGL Simulator\n");
  fflush(stdout);

  lv_init();
  hal_setup();

  testLvgl();

  hal_loop();
  return 0;
}

#endif
