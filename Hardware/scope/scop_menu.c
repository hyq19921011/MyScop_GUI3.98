#include "scop_menu.h"
#include "scope.h"
u8 Rvalue[2];
u8 MenuDelet=0,FreSelet=0,FFTflag = 0;
u16 fftdep = 128,fftshowbase = 110;
u8 key_value = 0;
u32 EDIT_value = 0;
u32 EDIT_value0 = 0;
u32 EDIT_value1 = 0;
u8 EDIT_Flag = 0;
u8 SAVE_Flag = 0;
u8 BACK_Flag = 0;
//compx s[512];  
//struct rtc_time SysTime,AlterTime;
unsigned MaxNumDataItems=1024,NumDataItems=1024;

static WM_HWIN _hWindow2;
static PROGBAR_Handle ahProgBar;

static const GUI_WIDGET_CREATE_INFO _TimeCreate[] = {
  { WINDOW_CreateIndirect,    "Dialog 3", 0,                   0,   0, 260, 160, FRAMEWIN_CF_MOVEABLE },
  { DROPDOWN_CreateIndirect,   "年"    , GUI_ID_DROPDOWNYEAR,    60,   5, 50  , 95},
  { DROPDOWN_CreateIndirect,   "月"    , GUI_ID_DROPDOWNMON,    120,   5, 40  , 95},
  { DROPDOWN_CreateIndirect,   "日"    , GUI_ID_DROPDOWNDAY,    170,   5, 40  , 95},
  { DROPDOWN_CreateIndirect,   "时"    , GUI_ID_DROPDOWNHOUR,    60,  35, 40  , 65},
  { DROPDOWN_CreateIndirect,   "分"    , GUI_ID_DROPDOWNMIN,    120,  35, 40  , 65},
  { DROPDOWN_CreateIndirect,   "秒"    , GUI_ID_DROPDOWNSEC,    170,  35, 40  , 65},
  { BUTTON_CreateIndirect,   "确定"    ,GUI_ID_BUTTONTIMEOK,	120,  80, 35  , 22 },
  { TEXT_CreateIndirect,  "年月日:"    ,GUI_ID_TEXTDAT,          10,   5,  42,  12, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,  "时分秒:"    ,GUI_ID_TEXTTIM,          10,  35,  42,  12, TEXT_CF_LEFT },
  
};
static const GUI_WIDGET_CREATE_INFO _FreCreate[] = {
  { WINDOW_CreateIndirect,    "Dialog 4", 0,                   0,   0, 260, 160, FRAMEWIN_CF_MOVEABLE },
  { CHECKBOX_CreateIndirect, NULL,          GUI_ID_CHECK2,            5,   5,   50, 0, 0, 0 },//复选框
  { CHECKBOX_CreateIndirect, NULL,          GUI_ID_CHECK1,            5,   80, 100, 0, 0, 0 },//复选框
  { RADIO_CreateIndirect,    NULL,          GUI_ID_RADIO0,           60,    5,  50,  100,  0, 3},//单选框
  

};
static const GUI_WIDGET_CREATE_INFO _DacCreate[] = {
  { WINDOW_CreateIndirect,    "Dialog 5", 0,                   0,   0, 260, 160, FRAMEWIN_CF_MOVEABLE },
  { TEXT_CreateIndirect,  "使能",    GUI_ID_TEXT5,         22,   5,  24,  12, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,  "HZ",      GUI_ID_TEXT6,           105,   40,  24,  12, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,  "mV",       GUI_ID_TEXT7,            105,   60,  24,  12, TEXT_CF_LEFT },
  { BUTTON_CreateIndirect,"确定",    GUI_ID_BUTTONWAVEOK,   60,  85, 35  , 22 },
  { CHECKBOX_CreateIndirect, NULL,   GUI_ID_CHECK0,          5,    5,   0, 0, 0, 0 },//复选框
  { DROPDOWN_CreateIndirect,NULL,    GUI_ID_DROPDOWNWAVE,    50,    5,  65, 55},
  { EDIT_CreateIndirect,    NULL,    GUI_ID_EDIT0,	     30,   40,  70,  15,0,7 },
  { EDIT_CreateIndirect,    NULL,    GUI_ID_EDIT1,	     30,   60,  70,  15,0,7 },
  { BUTTON_CreateIndirect,   "0",    GUI_ID_BUTTON20,	    160,   90,  25,  25 },
  { BUTTON_CreateIndirect,   "1",    GUI_ID_BUTTON21,	    130,   60,  25,  25 },
  { BUTTON_CreateIndirect,   "2",    GUI_ID_BUTTON22,	    160,   60,  25,  25 },
  { BUTTON_CreateIndirect,   "3",    GUI_ID_BUTTON23,	    190,   60,  25,  25 },
  { BUTTON_CreateIndirect,   "4",    GUI_ID_BUTTON24,	    130,   30,  25,  25 },
  { BUTTON_CreateIndirect,   "5",    GUI_ID_BUTTON25,	    160,   30,  25,  25 },
  { BUTTON_CreateIndirect,   "6",    GUI_ID_BUTTON26,	    190,   30,  25,  25 },
  { BUTTON_CreateIndirect,   "7",    GUI_ID_BUTTON27,	    130,    0,  25,  25 },
  { BUTTON_CreateIndirect,   "8",    GUI_ID_BUTTON28,	    160,   0,  25,  25 },
  { BUTTON_CreateIndirect,   "9",    GUI_ID_BUTTON29,	    190,   0,  25,  25 },
  { BUTTON_CreateIndirect,   "R",    GUI_ID_BUTTON30,	    130,   90,  25,  25 },
  { BUTTON_CreateIndirect,   "<",    GUI_ID_BUTTON31,	    190,   90,  25,  25 },  
};

static void Dialog1TimeDownChange(WM_HWIN hDlg, int Id)
{
    WM_HWIN hDown;
    
    switch(Id)
    {
        case GUI_ID_DROPDOWNYEAR:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNYEAR);
           //AlterTime.tm_year = DROPDOWN_GetSel(hDown)+2011;//获取模式编号
           break;
        case GUI_ID_DROPDOWNMON:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNMON);
           //AlterTime.tm_mon = DROPDOWN_GetSel(hDown)+1;//获取模式编号
           break;
        case GUI_ID_DROPDOWNDAY:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNDAY);
           //AlterTime.tm_mday = DROPDOWN_GetSel(hDown)+1;//获取模式编号
           break;
        case GUI_ID_DROPDOWNHOUR:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNHOUR);
           //AlterTime.tm_hour = DROPDOWN_GetSel(hDown);//获取模式编号
           break;
        case GUI_ID_DROPDOWNMIN:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNMIN);
           //AlterTime.tm_min = DROPDOWN_GetSel(hDown);//获取模式编号
           break;
        case GUI_ID_DROPDOWNSEC:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNSEC);
           //AlterTime.tm_sec = DROPDOWN_GetSel(hDown);//获取模式编号
           break;
    }
}
static void Dialog3ButtonChange(WM_HWIN hWin,int Id)
{ 
	
    switch(Id)
    {   
        case GUI_ID_BUTTON20:key_value= 0;
			 
              break;
        case GUI_ID_BUTTON21:key_value = 1;
			
              break;
        case GUI_ID_BUTTON22:key_value = 2;
			
              break;      
        case GUI_ID_BUTTON23:key_value= 3;
              break;
        case GUI_ID_BUTTON24:key_value= 4;
              break;
        case GUI_ID_BUTTON25:key_value = 5;
              break;  
        case GUI_ID_BUTTON26:key_value= 6;
              break;
        case GUI_ID_BUTTON27:key_value = 7;
              break;
        case GUI_ID_BUTTON28:key_value = 8;
              break;      
        case GUI_ID_BUTTON29:key_value = 9;
              break;
        case GUI_ID_BUTTON30:key_value = 0;EDIT_value0 = 0;EDIT_value1 = 0;//清零
              break;
		case GUI_ID_BUTTON31:BACK_Flag = 1;//退格
			{
//				if(!EDIT_Flag)
//				{
//					EDIT_value0 = EDIT_value0/10;
//					EDIT_SetValue (WM_GetDialogItem(hWin, GUI_ID_EDIT0), EDIT_value0);
//				}else
//					{
//						EDIT_value1 = EDIT_value1/10;
//						EDIT_SetValue (WM_GetDialogItem(hWin, GUI_ID_EDIT1), EDIT_value1);
//					}
				
              break;
			}
        case GUI_ID_BUTTONWAVEOK:SAVE_Flag = 1;
			
              break;
		default : key_value = 0xff;break;
    }
		
 		
	if(SAVE_Flag == 1)//保存数据
	{
		SAVE_Flag = 0;
	}
	if(EDIT_Flag == 0 && key_value != 0xff)
	{
		if(BACK_Flag == 0)
		{
			EDIT_value0 = EDIT_value0*10+key_value;
			
		}else
			{
				BACK_Flag = 0;
				EDIT_value0 = EDIT_value0/10;
				
			
			}
		EDIT_SetValue (WM_GetDialogItem(hWin, GUI_ID_EDIT0), EDIT_value0);
	}
	if(EDIT_Flag == 1 && key_value != 0xff)
	{
		if(BACK_Flag == 0)
		{
			EDIT_value1 = EDIT_value1*10+key_value;
			
		}else
			{
				BACK_Flag = 0;
				EDIT_value1 = EDIT_value1/10;
			}
		EDIT_SetValue (WM_GetDialogItem(hWin, GUI_ID_EDIT1), EDIT_value1);
	}

	
}
static void Dialog3WAVEDownChange(WM_HWIN hDlg, int Id)
{
    WM_HWIN hDown;
	u8 i = 0;
    switch(Id)
    {
        case GUI_ID_DROPDOWNWAVE:
           hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNWAVE);		   
           switch( DROPDOWN_GetSel(hDown))
		   {
			   case 0: 
						//	/* 用缓存放一个周期的三角波 */
					for(i =0; i <= 32; i++)
					{
						g_usWaveBuff[i] = i*128;
					}
					
					for(i =0; i < 32; i++)
					{
						g_usWaveBuff[i+32] = 4096-(128*(i+1));
					} 
				   break;
			   case 1:
					/* 用缓存放一个周期的正弦波 */
					for(i =0; i < 64; i++)
					{
						g_usWaveBuff[i] = 2048*sin((double)i*3.14/32)+2048;
					}				   
				   break;
			   case 2: 
					/* 用缓存放一个周期的方波 */
					for(i =0; i < 32; i++)
					{
						g_usWaveBuff[i] = 0;
					}
					
					for(i =0; i < 32; i++)
					{
						g_usWaveBuff[i+32] = 4095;
					}				   
				   break;
			   case 3: 
					//	/* 用缓存放一个周期的锯齿波 */
					for(i =0; i <= 64; i++)
					{
						g_usWaveBuff[i] = i*64;
					}				   
				   break;
		   }
           break;
    }
}
static void _cbDialog1(WM_MESSAGE * pMsg)
{
  char s[5];
  int NCode, Id,i;
  WM_HWIN hDlg, hDown,hText;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_PAINT:
      //_OnPaint();
      return;
    case WM_INIT_DIALOG:
      BUTTON_SetBkColor(WM_GetDialogItem(hDlg,GUI_ID_BUTTONTIMEOK),BUTTON_CI_UNPRESSED,0xa9a9a9);
      BUTTON_SetTextColor(WM_GetDialogItem(hDlg,GUI_ID_BUTTONTIMEOK),BUTTON_CI_UNPRESSED,0x00ffff);
      BUTTON_SetFont(WM_GetDialogItem(hDlg,GUI_ID_BUTTONTIMEOK),&GUI_FontHZ12);
      
	  hText   = WM_GetDialogItem(hDlg, GUI_ID_TEXTDAT);
	  TEXT_SetFont(hText,&GUI_FontHZ12);
      TEXT_SetTextColor(hText,GUI_BLACK);
      hText   = WM_GetDialogItem(hDlg, GUI_ID_TEXTTIM);
	  TEXT_SetFont(hText,&GUI_FontHZ12);
      TEXT_SetTextColor(hText,GUI_BLACK);
      
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNYEAR);
      DROPDOWN_SetAutoScroll(hDown, 1);
     
      
      for(i=2011;i<2031;i++)
      {
          s[0] = i/1000+0x30;
          s[1] = i%1000/100+0x30;
          s[2] = i%100/10+0x30;
          s[3] = i%10+0x30;
          s[4] = NULL;
          DROPDOWN_AddString(hDown,(const char*)s);
      }
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNMON);
      DROPDOWN_SetAutoScroll(hDown, 1);
      for(i=1;i<13;i++)
      {
          s[0] = i/10+0x30;
          s[1] = i%10+0x30;
          s[2] = NULL;
          DROPDOWN_AddString(hDown,(const char*)s);
      }
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNDAY);
      DROPDOWN_SetAutoScroll(hDown, 1);
      for(i=1;i<32;i++)
      {
          s[0] = i/10+0x30;
          s[1] = i%10+0x30;
          s[2] = NULL;
          DROPDOWN_AddString(hDown,(const char*)s);
      }
      
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNHOUR);
      DROPDOWN_SetAutoScroll(hDown, 1);
      for(i=0;i<24;i++)
      {
          s[0] = i/10+0x30;
          s[1] = i%10+0x30;
          s[2] = NULL;
          DROPDOWN_AddString(hDown,(const char*)s);
      }
      
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNMIN);
      DROPDOWN_SetAutoScroll(hDown, 1);
      for(i=0;i<60;i++)
      {
          s[0] = i/10+0x30;
          s[1] = i%10+0x30;
          s[2] = NULL;
          DROPDOWN_AddString(hDown,(const char*)s);
      }
      
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNSEC);
      DROPDOWN_SetAutoScroll(hDown, 1);
      for(i=0;i<60;i++)
      {
          s[0] = i/10+0x30;
          s[1] = i%10+0x30;
          s[2] = NULL;
          DROPDOWN_AddString(hDown,(const char*)s);
      }
      
      hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNYEAR);
     // DROPDOWN_SetSel(hDown, SysTime.tm_year-2011);
      
      hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNMON);
      //DROPDOWN_SetSel(hDown, SysTime.tm_mon-1);
      
      hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNDAY);
     // DROPDOWN_SetSel(hDown, SysTime.tm_mday-1);
      
      hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNHOUR);
      //DROPDOWN_SetSel(hDown, SysTime.tm_hour);
      
      hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNMIN);
      //DROPDOWN_SetSel(hDown, SysTime.tm_min);
      
      hDown = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNSEC);
      //DROPDOWN_SetSel(hDown, SysTime.tm_sec);
      break;
    case WM_KEY:
      switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) {
        case GUI_KEY_ESCAPE:
          ;
          break;
        case GUI_KEY_ENTER:
          GUI_EndDialog(hDlg, 0);
          break;
      }
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
      NCode = pMsg->Data.v;                 /* Notification code */
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:
             if(Id == GUI_ID_BUTTONTIMEOK)
             {
                  //Time_Adjust(&AlterTime);
             }
          break;
				 
        case WM_NOTIFICATION_VALUE_CHANGED: /* Value has changed */
          //_OnValueChanged(hDlg, Id);
          break;
      case WM_NOTIFICATION_SEL_CHANGED://下拉列表值改变
           Dialog1TimeDownChange(hDlg, Id);
           break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
  }
}
/*********************************************************************
*
*       _cbDialog2
*/
static void _cbDialog2(WM_MESSAGE * pMsg) 
{
  int NCode, Id;
  WM_HWIN hDlg,hCheck,hText,hRadio;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_PAINT:
      //_OnPaint();
      return;
    case WM_INIT_DIALOG:
          
          
          hText = WM_GetDialogItem(hDlg, GUI_ID_TEXTFFT);
          TEXT_SetFont(hText,&GUI_Font16_1);
          TEXT_SetTextColor(hText,GUI_BLACK);
          hText = WM_GetDialogItem(hDlg, GUI_ID_TEXTFRE);
          TEXT_SetTextColor(hText,GUI_BLACK);
          
          hRadio  = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
          RADIO_SetText(hRadio, "128P", 0);
          RADIO_SetText(hRadio, "256P", 1);
          RADIO_SetText(hRadio, "512P", 2);
          RADIO_SetValue(hRadio,Rvalue[0]);
          
          hRadio  = WM_GetDialogItem(hDlg, GUI_ID_RADIO1);
          RADIO_SetText(hRadio, "Style1", 0);
          RADIO_SetText(hRadio, "Style2", 1);
          RADIO_SetValue(hRadio,Rvalue[1]);
          
          hCheck = WM_GetDialogItem(hDlg, GUI_ID_CHECK2);
          CHECKBOX_SetText(hCheck, "FFT");
          CHECKBOX_SetState(hCheck,FFTflag);//设置上次状态
          
          hCheck = WM_GetDialogItem(hDlg, GUI_ID_CHECK1);
          CHECKBOX_SetText(hCheck, "Low 40Hz");
          CHECKBOX_SetState(hCheck,FreSelet);//设置上次状态
          break;
    case WM_KEY:
      switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) {
        case GUI_KEY_ESCAPE:
          ;
          break;
        case GUI_KEY_ENTER:
          GUI_EndDialog(hDlg, 0);
          break;
      }
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
      NCode = pMsg->Data.v;                 /* Notification code */
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:
  
          break;     
        case WM_NOTIFICATION_VALUE_CHANGED: /* Value has changed */
               if(Id == GUI_ID_CHECK1)
               {
                    hCheck = WM_GetDialogItem(hDlg, GUI_ID_CHECK1);
                    if(CHECKBOX_IsChecked(hCheck))
                    {
                      FreSelet=1;
//                      
//                      TIM_PrescalerConfig(TIM1, 1439,TIM_PSCReloadMode_Update);
//                      TIMFrequency = 50000.0;
//                      div = 0.05;//低于40
                    }
                    else 
                    {
                      FreSelet=0;
//                      
//                      TIM_PrescalerConfig(TIM1, 17,TIM_PSCReloadMode_Update);
//                      TIMFrequency = 4000000.0;
//                      div = 4.0;//70-2K
                    }
               }
               if(Id == GUI_ID_CHECK2)
               {
                    hCheck = WM_GetDialogItem(hDlg, GUI_ID_CHECK2);
                    if(CHECKBOX_IsChecked(hCheck))
                    {
                          //FFTflag=1;
                        
                          /*WM_SetWindowPos(hGraph, 0, 0, 319,160);//重新设置窗口大小
                          GRAPH_SCALE_SetPos(hScale[1], 115 + 20);
                          WM_HideWindow(_hWindow2);//隐藏RTC窗口
                          fftshowbase = 130;//改变FFT现实起始坐标*/
                    }
                    else 
                    {
                        //FFTflag=0;
                      
                        /*WM_SetWindowPos(hGraph,0,20, 319, 140);//重新设置窗口大小
                        GRAPH_SCALE_SetPos(hScale[1], 115);
                        WM_ShowWindow(_hWindow2);//显示RTC窗口
                        fftshowbase = 110;//改变FFT现实起始坐标*/
                    }
               }
               if(Id == GUI_ID_RADIO0)
              {
                    hRadio = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
                    Rvalue[0]=RADIO_GetValue(hRadio);
                    if(Rvalue[0]==0){fftdep=128;}
                    if(Rvalue[0]==1){fftdep=256;}
                    if(Rvalue[0]==2){fftdep=512;}
              }
        
                break;
      case WM_NOTIFICATION_SEL_CHANGED://下拉列表值改变
          // DownChange(hDlg, Id);
         break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
  }
}
/*********************************************************************
*
*       _cbDialog3
*/
static void _cbDialog3(WM_MESSAGE * pMsg)
{
  int NCode, Id;
  WM_HWIN hDlg, hDown,hText,hEdit0,hEdit1;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_PAINT:
      //_OnPaint();
      return;
    case WM_INIT_DIALOG:
      BUTTON_SetBkColor(WM_GetDialogItem(hDlg,GUI_ID_BUTTONWAVEOK),BUTTON_CI_UNPRESSED,0xa9a9a9);
      BUTTON_SetTextColor(WM_GetDialogItem(hDlg,GUI_ID_BUTTONWAVEOK),BUTTON_CI_UNPRESSED,0x00ffff);
      BUTTON_SetFont(WM_GetDialogItem(hDlg,GUI_ID_BUTTONWAVEOK),&GUI_FontHZ12);
	
      hEdit0   = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
      hEdit1   = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
      hDown   = WM_GetDialogItem(hDlg, GUI_ID_DROPDOWNWAVE);
      DROPDOWN_SetFont(hDown,&GUI_FontHZ12);
      DROPDOWN_AddString(hDown,"三角波"); 
      DROPDOWN_AddString(hDown,"正弦波"); 
      DROPDOWN_AddString(hDown,"方波"); 
      DROPDOWN_AddString(hDown,"锯齿波"); 
      
      hText   = WM_GetDialogItem(hDlg, GUI_ID_TEXT5);
	  TEXT_SetFont(hText,&GUI_FontHZ12);
      TEXT_SetTextColor(hText,GUI_BLACK);
      hText   = WM_GetDialogItem(hDlg, GUI_ID_TEXT6);
      TEXT_SetTextColor(hText,GUI_BLACK);
      hText   = WM_GetDialogItem(hDlg, GUI_ID_TEXT7);
      TEXT_SetTextColor(hText,GUI_BLACK);
      
      EDIT_SetDecMode(hEdit0, EDIT_value0, 0, 100000000,0,0);
      EDIT_SetDecMode(hEdit1, EDIT_value1, 0, 10000,0,0);
      break;
    case WM_KEY:
      switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) {
        case GUI_KEY_ESCAPE:
          ;
          break;
        case GUI_KEY_ENTER:
          GUI_EndDialog(hDlg, 0);
          break;
      }
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
      NCode = pMsg->Data.v;                 /* Notification code */
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:
             Dialog3ButtonChange(hDlg,Id); 
             break; 
		case WM_NOTIFICATION_CLICKED:
             if(Id == GUI_ID_EDIT0)
			 {
				EDIT_Flag = 0;
			 }
			 if(Id == GUI_ID_EDIT1)
			 {
				EDIT_Flag = 1;
			 }
                           
          break;			
        case WM_NOTIFICATION_VALUE_CHANGED: /* Value has changed */
          //_OnValueChanged(hDlg, Id);
          break;
      case WM_NOTIFICATION_SEL_CHANGED://下拉列表值改变
         Dialog3WAVEDownChange(hDlg, Id);
         break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
  }
}

static void _cbFrameWin(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) 
  {
      case  WM_DELETE :
           MenuDelet = 0; 
      default:
        WM_DefaultProc(pMsg);
  }
}
void MEUN_CREAT()//菜单界面建立
{

  WM_HWIN hMultiPage, hFrameWin, hDialog;
    
  hFrameWin = FRAMEWIN_Create("菜单界面", &_cbFrameWin, WM_CF_SHOW, 0, 20, 240, 180);
  //FRAMEWIN_SetClientColor(hFrameWin, GUI_YELLOW);
  FRAMEWIN_SetFont(hFrameWin,&GUI_FontHZ12);
  FRAMEWIN_SetTitleHeight(hFrameWin,15);
	FRAMEWIN_SetTextAlign(hFrameWin,GUI_TA_VCENTER|GUI_TA_CENTER);
  FRAMEWIN_SetActive(hFrameWin, 1);
  FRAMEWIN_SetMoveable(hFrameWin, 1);
  FRAMEWIN_SetBarColor(hFrameWin,1,GUI_DARKYELLOW);
  WM_SetStayOnTop(hFrameWin, 1);//一直置顶  
  
  hMultiPage = MULTIPAGE_CreateEx(0, 0, 0, 0, WM_GetClientWindow(hFrameWin), WM_CF_SHOW, 0, 0);
 
  hDialog = GUI_CreateDialogBox(_TimeCreate, 
                                GUI_COUNTOF(_TimeCreate), 
                                &_cbDialog1, WM_UNATTACHED, 0, 0);
  MULTIPAGE_AddPage(hMultiPage, hDialog, "Time");

  hDialog = GUI_CreateDialogBox(_FreCreate, 
                                GUI_COUNTOF(_FreCreate), 
                                &_cbDialog2, WM_UNATTACHED, 0, 0);
  MULTIPAGE_AddPage(hMultiPage, hDialog, "FFT");
  
  hDialog = GUI_CreateDialogBox(_DacCreate, 
                                GUI_COUNTOF(_DacCreate), 
                                &_cbDialog3, WM_UNATTACHED, 0, 0);
  MULTIPAGE_AddPage(hMultiPage, hDialog, "DAC");
  
  FRAMEWIN_AddCloseButton(hFrameWin,FRAMEWIN_BUTTON_RIGHT,0);
  FRAMEWIN_AddMinButton(hFrameWin,FRAMEWIN_BUTTON_RIGHT,5);
}