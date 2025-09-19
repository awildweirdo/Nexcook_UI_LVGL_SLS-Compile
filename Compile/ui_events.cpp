#include "ui.h"
#include <stdbool.h>
#include <Arduino.h>
#include <string.h>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_ING 11 // maximum number of ingredients supported by hardwre

// globals for timer control in ui_cooking.c
static int CurrentStep = 0;
static int TotalRemaining = 0;
static int StepRemaining = 0;
static lv_timer_t *CookingTimer = NULL;


static void BackHome_cb(lv_timer_t * t)
{
    lv_scr_load(ui_Home);
    lv_timer_del(t);
}

typedef struct
{
	char message[64];
	int dur;
} step;

typedef struct recepie
{
	char name[128];
	int required[NUM_ING];
	int cook_time;
	int stepCount;
	step Steps[12];
	String serialmsg[12];
} recepie;

static recepie curr_recepie;
recepie *current = &curr_recepie;

recepie MasoorDal = {"MasoorDal", {5, 5, 1, 2, 2, 480, 15, 80, 100, 60, 9}, 60, 12, {{"Dispensing and Heating 15ml Oil", 5}, {"Dispensing 5g Cumin", 5}, {"Dispensing 2g Mustard", 5}, {"Dispensing 9g Garlic", 5}, {"Chopping and dispensing 100g Onion", 5}, {"Chopping and dispensing 80g Tomato", 5}, {"Dispensing 1g Turmeric", 5}, {"Steaming and dispensing 50g Toor Dal", 5}, {"Dispensing 5g Salt", 5}, {"Dispensing 480ml Water", 5}, {"Sprinkling Pepper as a finishing Touch", 5},{"Cleaning...",5}},{
		    "MODULE:liquid|2|15",
        "MODULE:spice|1|5",
        "MODULE:spice|2|2",
        "MODULE:hopper||9",
        "MODULE:chop|1|100",
        "MODULE:chop|2|80",
        "MODULE:spice|3|1",
        "MODULE:steam|1|60",
        "MODULE:spice|4|5",
        "MODULE:liquid|1|480",
        "MODULE:spice|3|5"
}};

recepie SpinachSoup = {"SpinachSoup", {5, 5, 1, 2, 2, 480, 15, 80, 100, 60, 9}, 55, 11, {{"Dispensing and Heating 15ml Oil", 5}, {"Dispensing 5g Cumin", 5}, {"Dispensing 2g Mustard", 5}, {"Dispensing 9g Garlic", 5}, {"Chopping and dispensing 100g Onion", 5}, {"Chopping and dispensing 80g Tomato", 5}, {"Dispensing 1g Turmeric", 5}, {"Steaming and dispensing 60g Spinach", 5}, {"Dispensing 5g Salt", 5}, {"Dispensing 480ml Water", 5}, {"Sprinkling Pepper as a finishing Touch", 5}},{
		    "MODULE:liquid|2|15",
        "MODULE:spice|1|5",
        "MODULE:spice|2|2",
        "MODULE:hopper||9",
        "MODULE:chop|1|100",
        "MODULE:chop|2|80",
        "MODULE:spice|3|1",
        "MODULE:steam|1|60",
        "MODULE:spice|4|5",
        "MODULE:liquid|1|480",
        "MODULE:spice|3|5"
}};

recepie TomatoSoup = {"TomatoSoup", {5, 5, 1, 2, 2, 480, 15, 80, 100, 60, 9}, 55, 11, {{"Dispensing and Heating 15ml Oil", 5}, {"Dispensing 5g Cumin", 5}, {"Dispensing 2g Mustard", 5}, {"Dispensing 9g Garlic", 5}, {"Chopping and dispensing 100g Onion", 5}, {"Chopping and dispensing 80g Tomato", 5}, {"Dispensing 1g Turmeric", 5}, {"Steaming and dispensing 60g Spinach", 5}, {"Dispensing 5g Salt", 5}, {"Dispensing 480ml Water", 5}, {"Sprinkling Pepper as a finishing Touch", 5}},{
		    "MODULE:liquid|2|15",
        "MODULE:spice|1|5",
        "MODULE:spice|2|2",
        "MODULE:hopper||9",
        "MODULE:chop|1|100",
        "MODULE:chop|2|80",
        "MODULE:spice|3|1",
        "MODULE:steam|1|60",
        "MODULE:spice|4|5",
        "MODULE:liquid|1|480",
        "MODULE:spice|3|5"
}};

recepie TurDal = {"TurDal", {5, 5, 1, 2, 2, 480, 15, 80, 100, 60, 9}, 57, 11, {{"Dispensing and Heating 15ml Oil", 5}, {"Dispensing 5g Cumin", 5}, {"Dispensing 2g Mustard", 5}, {"Dispensing 9g Garlic", 5}, {"Chopping and dispensing 100g Onion", 5}, {"Chopping and dispensing 80g Tomato", 5}, {"Dispensing 1g Turmeric", 5}, {"Steaming and dispensing 60g Toor Dal", 2}, {"Dispensing 5g Salt", 5}, {"Dispensing 480ml Water", 5}, {"Sprinkling Pepper as a finishing Touch", 5},{"Clearning",5}},{
		    "MODULE:liquid|2|15",
        "MODULE:spice|1|5",
        "MODULE:spice|2|2",
        "MODULE:hopper||9",
        "MODULE:chop|1|100",
        "MODULE:chop|2|80",
        "MODULE:spice|3|1",
        "MODULE:steam|1|60",
        "MODULE:spice|4|5",
        "MODULE:liquid|1|480",
        "MODULE:spice|3|5",
        "MODULE:liquid|3|10"
}};

typedef struct
{
	int inv[NUM_ING];
} inventory;

inventory Inventory = {{10, 10, 2, 4, 4, 960, 30, 160, 200, 120, 20}};

static bool MissingFlags[NUM_ING]; // declared as static hence all values will be false

void SetMasoorDal(lv_event_t *e)
{
	curr_recepie = MasoorDal;
}

void SetSpinachSoup(lv_event_t *e)
{
	curr_recepie = SpinachSoup;
}

void SetTomatoSoup(lv_event_t *e)
{
	curr_recepie = TomatoSoup;
}

void SetTurDal(lv_event_t *e)
{
	curr_recepie = TurDal;
}

void UpdateSpiceLevel(lv_event_t *e)
{
	lv_obj_t *slider = lv_event_get_target(e);
	int spiceLevel = lv_slider_get_value(slider);
	curr_recepie.required[4] = spiceLevel;
}

void UpdateSalt(lv_event_t *e)
{
	lv_obj_t *slider = lv_event_get_target(e);
	int saltLevel = lv_slider_get_value(slider);
	curr_recepie.required[1] = saltLevel;
}

void cooking_timer_cb(lv_timer_t * timer);

bool canCook(char *error)
{
	error[0] = '\0';
	bool enough = true;
	for (int i = 0; i < NUM_ING; i++)
	{
		if (Inventory.inv[i] < curr_recepie.required[i])
		{
			enough = false;
			MissingFlags[i] = true;
			char buf[32];
			sprintf(buf, "ID:%d too low\n", i);
			strncat(error, buf, 128 - strlen(error) - 1);
		}
		else
		{
			MissingFlags[i] = false;
		}
	}
	return enough;
}

static void MissingIng_cb(lv_event_t * e)
{
    lv_obj_t *mbox = lv_event_get_current_target(e);
    lv_obj_del(mbox);                 // close the message box
    lv_scr_load(ui_Home);             // go back to Home screen
}

void updateTimeLabel(void);
void StartCooking(lv_event_t *e)
{
	char error[128];
	if (canCook(error))
	{
		char buffer[128];
		sprintf(buffer, "RECIPE:%s", curr_recepie.name);
		Serial.println(buffer);

		for (int i = 0; i < NUM_ING; i++)
		{
			Inventory.inv[i] = Inventory.inv[i] - curr_recepie.required[i];
		}

		// for ui_cooking.c
		CurrentStep = 0;
		StepRemaining = current->Steps[0].dur;
		TotalRemaining = current->cook_time;

		lv_label_set_text(ui_UiCurrentProcessLabel, current->Steps[0].message);
		updateTimeLabel();
    lv_obj_clear_flag(ui_Arc2, LV_OBJ_FLAG_CLICKABLE);
		lv_arc_set_range(ui_Arc2, 0, current->cook_time);
		lv_arc_set_value(ui_Arc2, 0);
    Serial.println(current->serialmsg[0].c_str());

		if (CookingTimer)
			lv_timer_del(CookingTimer);
		CookingTimer = lv_timer_create(cooking_timer_cb, 1000, NULL);
	}
	else
	{
		static const char *btns[] = {"OK", ""}; // close button
		lv_obj_t *mbox = lv_msgbox_create(NULL, "Missing Ingredients", error, btns, true);
		lv_obj_center(mbox);
    
    lv_obj_t *btnm = lv_msgbox_get_btns(mbox);
    lv_obj_add_event_cb(btnm, MissingIng_cb, LV_EVENT_VALUE_CHANGED, NULL);
	}
}


void cooking_timer_cb(lv_timer_t *timer)
{
	if (TotalRemaining > 0)
	{
		TotalRemaining--;
		StepRemaining--;

		updateTimeLabel();
		lv_arc_set_value(ui_Arc2, current->cook_time - TotalRemaining);

		if (StepRemaining <= 0)
		{
			CurrentStep++;
			if (CurrentStep < current->stepCount)
			{
				StepRemaining = current->Steps[CurrentStep].dur;
				lv_label_set_text(ui_UiCurrentProcessLabel, current->Steps[CurrentStep].message);
				//Serial.println(serialmsg[CurrentStep]);
			}
			if (!current->serialmsg[CurrentStep].isEmpty()) {
            Serial.println(current->serialmsg[CurrentStep].c_str());
        	}
		}
	}else {
    // All steps done
    lv_label_set_text(ui_UiCurrentProcessLabel, "Cooking Complete!");
    lv_label_set_text(ui_Label12, "00:00");
    lv_arc_set_value(ui_Arc2, current->cook_time);

    lv_timer_del(CookingTimer);
    CookingTimer = NULL;

    
    lv_timer_create(BackHome_cb, 2000, NULL);   // 2000ms delay before switching
}


}


// Helper function to refresh the countdown label
void updateTimeLabel(void)
{
	if (!ui_Label12)
		return; // safety in case screen not loaded

	char buf[8];
	snprintf(buf, sizeof(buf), "%02d:%02d", TotalRemaining / 60, TotalRemaining % 60);
	lv_label_set_text(ui_Label12, buf);
}
void IngredientBarSetup(lv_event_t *e)
{
	lv_bar_set_range(ui_CuminBar, 0, 30);
	lv_bar_set_range(ui_SaltBar, 0, 30);
	lv_bar_set_range(ui_TurmericBar, 0, 30);
	lv_bar_set_range(ui_MustardBar, 0, 30);
	lv_bar_set_range(ui_PepperBar, 0, 30);
	lv_bar_set_range(ui_WaterBar, 0, 1000);
	lv_bar_set_range(ui_OilBar, 0, 1000);
	lv_bar_set_range(ui_TomatoBar, 0, 1000);
	lv_bar_set_range(ui_OnionBar, 0, 1000);
	lv_bar_set_range(ui_SpinachBar, 0, 1000);
	lv_bar_set_range(ui_GarlicBar, 0, 1000);

	lv_bar_set_value(ui_CuminBar, Inventory.inv[0], LV_ANIM_ON);
	lv_bar_set_value(ui_SaltBar, Inventory.inv[1], LV_ANIM_ON);
	lv_bar_set_value(ui_TurmericBar, Inventory.inv[2], LV_ANIM_ON);
	lv_bar_set_value(ui_MustardBar, Inventory.inv[3], LV_ANIM_ON);
	lv_bar_set_value(ui_PepperBar, Inventory.inv[4], LV_ANIM_ON);
	lv_bar_set_value(ui_WaterBar, Inventory.inv[5], LV_ANIM_ON);
	lv_bar_set_value(ui_OilBar, Inventory.inv[6], LV_ANIM_ON);
	lv_bar_set_value(ui_TomatoBar, Inventory.inv[7], LV_ANIM_ON);
	lv_bar_set_value(ui_OnionBar, Inventory.inv[8], LV_ANIM_ON);
	lv_bar_set_value(ui_SpinachBar, Inventory.inv[9], LV_ANIM_ON);
	lv_bar_set_value(ui_GarlicBar, Inventory.inv[10], LV_ANIM_ON);

	if (Inventory.inv[0] < curr_recepie.required[0])
	{
		lv_obj_clear_flag(ui_WarningCumin, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningCumin, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[1] < curr_recepie.required[1])
	{
		lv_obj_clear_flag(ui_WarningSalt, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningSalt, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[2] < curr_recepie.required[2])
	{
		lv_obj_clear_flag(ui_WarningTurmeric, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningTurmeric, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[3] < curr_recepie.required[3])
	{
		lv_obj_clear_flag(ui_WarningMustard, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningMustard, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[4] < curr_recepie.required[4])
	{
		lv_obj_clear_flag(ui_WarningPepper, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningPepper, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[5] < curr_recepie.required[5])
	{
		lv_obj_clear_flag(ui_WarningWater, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningWater, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[6] < curr_recepie.required[6])
	{
		lv_obj_clear_flag(ui_WarningOil, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningOil, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[7] < curr_recepie.required[7])
	{
		lv_obj_clear_flag(ui_WarningTomato, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningTomato, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[8] < curr_recepie.required[8])
	{
		lv_obj_clear_flag(ui_WarningOnion, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningOnion, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[9] < curr_recepie.required[9])
	{
		lv_obj_clear_flag(ui_WarningSpinach, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningSpinach, LV_OBJ_FLAG_HIDDEN);
	}

	if (Inventory.inv[10] < curr_recepie.required[10])
	{
		lv_obj_clear_flag(ui_WarningGarlic, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_add_flag(ui_WarningGarlic, LV_OBJ_FLAG_HIDDEN);
	}
}

#ifdef __cplusplus
}
#endif
