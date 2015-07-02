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
#include "GRAPH.h"    //������ʾ
#include "MESSAGEBOX.h"

#define Peri  0 //����
#define Freq  1 //Ƶ��
#define V_max 2 //��ֵ
#define V_min 3 //��ֵ
#define Duty  4 //ռ�ձ�
#define AD_num 6144
#define AD_max 1024
extern WM_HWIN hGraph;
extern WM_HWIN hDialog;
extern WM_HWIN hBox;
extern GRAPH_DATA_Handle hData,hData1;
extern GRAPH_SCALE_Handle hScale[2];

extern u8 Wave_updat;
extern u32 Sampl_tim;
extern int T_Shousuo;//ʱ�����ű���
extern int V_Shousuo;//��ѹ���ű���
extern u16 New_edge_up[2];//�µı���
extern u16 New_edge_dow[2];//�µı���
extern u16 Wave_para[5];  //���εĲ���
extern const char Item_T[16][12];
extern const u8 Item_V[6][12];
extern u16 AD_New_Buf[AD_num];
extern u16 AD_Cur_Buf[1024];//��ǰ�Դ�
extern uint16_t g_usWaveBuff[64];     /* ���λ��� */

extern short int Vt_Pos;//������ѹ���α�
extern short int Tt_Pos;//����ʱ����α�
extern u16 Touch_x_value;  //����x�����ֵ
extern u16 Touch_y_value;  //����y�����ֵ
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