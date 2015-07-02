#ifndef _scop_menu_h_
#define _scop_menu_h_

#include <stddef.h>
#include "math.h"
#include "stdlib.h"

#include "GUI.h"
#include "DIALOG.h"

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "MULTIPAGE.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"    //≤®–Œœ‘ æ
#include "MESSAGEBOX.h"
#include "main.h"
#define GUI_ID_BUTTON20 GUI_ID_USER+0
#define GUI_ID_BUTTON21 GUI_ID_USER+1
#define GUI_ID_BUTTON22 GUI_ID_USER+2
#define GUI_ID_BUTTON23 GUI_ID_USER+3
#define GUI_ID_BUTTON24 GUI_ID_USER+4
#define GUI_ID_BUTTON25 GUI_ID_USER+5
#define GUI_ID_BUTTON26 GUI_ID_USER+6
#define GUI_ID_BUTTON27 GUI_ID_USER+7
#define GUI_ID_BUTTON28 GUI_ID_USER+8
#define GUI_ID_BUTTON29 GUI_ID_USER+9
#define GUI_ID_BUTTON30 GUI_ID_USER+10
#define GUI_ID_BUTTON31 GUI_ID_USER+11
#define GUI_ID_BUTTONWAVEOK GUI_ID_USER+12
#define GUI_ID_BUTTONTIMEOK GUI_ID_USER+13


#define GUI_ID_DROPDOWNYEAR   GUI_ID_USER+100
#define GUI_ID_DROPDOWNMON    GUI_ID_USER+101
#define GUI_ID_DROPDOWNDAY    GUI_ID_USER+102
#define GUI_ID_DROPDOWNHOUR   GUI_ID_USER+103
#define GUI_ID_DROPDOWNMIN    GUI_ID_USER+104
#define GUI_ID_DROPDOWNSEC    GUI_ID_USER+105
#define GUI_ID_DROPDOWNWAVE   GUI_ID_USER+106


#define GUI_ID_TEXTDAT    GUI_ID_USER+150
#define GUI_ID_TEXTTIM    GUI_ID_USER+151
#define GUI_ID_TEXTFRE    GUI_ID_USER+152
#define GUI_ID_TEXTFFT    GUI_ID_USER+153
extern u8 MenuDelet,FreSelet;
extern void MEUN_CREAT();
#endif