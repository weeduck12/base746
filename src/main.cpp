#include "lvgl.h"
#include <Arduino.h>
#define PULSE  8
#define DIR 6 
#define ENABLE 5

volatile int etat_moteur = 0;
int val_slide = 0;
volatile long position = 0;
volatile bool is_homed = false;
volatile bool is_homing = false;
volatile long target_position = 0;
volatile bool is_going_to_target = false;

lv_obj_t * cercle_arc;

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
    val_slide = lv_slider_get_value(slide);
}

void event_handler_go_home(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
      if (!is_homing && is_homed) {
        is_going_to_target = true;
        target_position = 0;
        etat_moteur = 0;
      }
    }
}

// New event handler for arc value change
void event_handler_arc_changed(lv_event_t * e)
{
    lv_obj_t * arc = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        if (!is_homing && is_homed) {
            target_position = lv_arc_get_value(arc);
            is_going_to_target = true;
            etat_moteur = 0; // Stop any current manual movement
        }
    }
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
  // slider
  
  lv_obj_t * slide = lv_slider_create(lv_screen_active());
  lv_obj_add_event_cb(slide, event_handler_slide, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_align(slide, LV_ALIGN_DEFAULT, 30, 230);
  lv_obj_set_width(slide, 200);
  lv_slider_set_range(slide, 0, 1000);

  // cercle (now lv_arc for visual feedback and potential control)
  cercle_arc = lv_arc_create(lv_screen_active());
  lv_obj_set_size(cercle_arc, 200, 200);
  lv_obj_align(cercle_arc,LV_ALIGN_DEFAULT,270,50);
  lv_arc_set_range(cercle_arc, 0, 399);
  lv_arc_set_rotation(cercle_arc, 270);
  lv_arc_set_bg_angles(cercle_arc, 0, 360);
  lv_obj_add_event_cb(cercle_arc, event_handler_arc_changed, LV_EVENT_VALUE_CHANGED, NULL);

  // Style the arc and indicator
  lv_obj_set_style_arc_width(cercle_arc, 10, LV_PART_MAIN);
  lv_obj_set_style_arc_width(cercle_arc, 10, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(cercle_arc, lv_color_make(0x00, 0x00, 0xFF), LV_PART_INDICATOR);
  
  // Add a "Go Home" button on the arc
  lv_obj_t * go_home_btn = lv_button_create(cercle_arc);
  lv_obj_set_size(go_home_btn, 80, 40);
  lv_obj_center(go_home_btn);
  lv_obj_add_event_cb(go_home_btn, event_handler_go_home, LV_EVENT_CLICKED, NULL);

  lv_obj_t * btn_label = lv_label_create(go_home_btn);
  lv_label_set_text(btn_label, "Go Home");
  lv_obj_center(btn_label);
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
  
  // Homing sequence
  if (!is_homed) {
    is_homing = true;
    // Rotate for 4 seconds at full speed
    for (int i = 0; i < 400; i++) { // 400 iterations * 10ms = 4 seconds (assuming 10ms delay = 1 pulse)
      digitalWrite(ENABLE, LOW);
      digitalWrite(PULSE, etat);
      digitalWrite(DIR, HIGH); // Assuming HIGH for initial homing direction (Sens antihoraire, decreases position)
      etat = !etat;
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
    is_homing = false;
    is_homed = true;
    position = 0; // Reset position counter after homing
  }

  while (1)
  {
    if (!is_homing) { // Only allow control if homing is complete
      if (is_going_to_target) {
        digitalWrite(ENABLE, LOW); // Enable motor for movement
        if (position != target_position) {
          if (position < target_position) {
            digitalWrite(DIR, LOW); // Move clockwise to increase position
            position++;
          } else { // position > target_position
            digitalWrite(DIR, HIGH); // Move counter-clockwise to decrease position
            position--;
          }
          digitalWrite(PULSE, etat);
          etat = !etat;
        } else {
          is_going_to_target = false; // Reached target
          digitalWrite(ENABLE, HIGH); // Disable motor
        }
      } else { // Manual control via buttons
        if(etat_moteur == 1) // Sens antihoraire
        {
          digitalWrite(ENABLE, LOW);
          digitalWrite(PULSE, etat);
          digitalWrite(DIR, HIGH);
          etat = !etat;
          position--; // Decrement position for "Sens antihoraire" (counter-clockwise)
        }
        else if(etat_moteur == 2) // Sens horaire
        {
          digitalWrite(ENABLE, LOW);
          digitalWrite(PULSE, etat);
          digitalWrite(DIR, LOW);
          etat = !etat;
          position++; // Increment position for "Sens horaire" (clockwise)
        }
        else{
          digitalWrite(ENABLE, HIGH);
          digitalWrite(PULSE, LOW);
        }
      }
    }
    
    // Update the arc display based on current motor position
    if (cercle_arc != NULL) { // Ensure arc object exists
        // Normalize position to be within 0-399 for display on arc, handling negative values
        lv_arc_set_value(cercle_arc, (position % 400 + 400) % 400); 
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10 + val_slide)); 
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
