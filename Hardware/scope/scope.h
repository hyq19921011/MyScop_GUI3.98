#ifndef _SCOPE_h_
#define _scope_h_
#include "stm32f4xx_conf.h"
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
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"    //波形显示
#include "MESSAGEBOX.h"

#define Peri  0 //周期
#define Freq  1 //频率
#define V_max 2 //峰值
#define V_min 3 //谷值
#define Duty  4 //占空比
#define AD_num 6144
#define AD_max 1024
extern WM_HWIN hGraph;
extern WM_HWIN hDialog;
extern WM_HWIN hBox;
extern GRAPH_DATA_Handle hData,hData1;
extern GRAPH_SCALE_Handle hScale[2];

extern u8 Wave_updat;
extern u32 Sampl_tim;
extern int T_Shousuo;//时间缩放比例
extern int V_Shousuo;//电压缩放比例
extern u16 New_edge_up[2];//新的边沿
extern u16 New_edge_dow[2];//新的边沿
extern u16 Wave_para[5];  //波形的参数
extern const char Item_T[16][12];
extern const u8 Item_V[6][12];
extern u16 AD_New_Buf[AD_num];
extern u16 AD_Cur_Buf[1024];//当前显存
extern uint16_t g_usWaveBuff[64];     /* 波形缓存 */

extern short int Vt_Pos;//测量电压的游标
extern short int Tt_Pos;//测量时间的游标
extern u16 Touch_x_value;  //触控x坐标差值
extern u16 Touch_y_value;  //触控y坐标差值
extern u8 Stop_Flag;
extern u8 Auto_Flag;

extern void WaveTaskCreat(void);
extern void OnButtonClicked(WM_MESSAGE * pMsg);
static void OnSliderValueChanged(WM_HWIN hDlg, int Id);
static void ButtonChange(WM_HWIN hWin,int Id);
extern void Get_Signal();
extern void Clculate_Signal();
extern void Updata_signal();
#endif