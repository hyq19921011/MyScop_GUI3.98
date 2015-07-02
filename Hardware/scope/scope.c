
#include "scope.h"
#include "scop_menu.h"
#include "main.h"
#include "adc.h"
#include "led.h"

WM_HWIN hGraph;
WM_HWIN hDialog;
WM_HWIN hBox;
GRAPH_DATA_Handle hData,hData1;
GRAPH_SCALE_Handle hScale[2];

const char Item_T[16][12] =  //各档位水平扫描时基显示字符
//    0（4M）    1(2M)       2(800K)     3(400K)     4(200K)       5(80K)       6(40k)    7 (20K)      8(8K)     9(4K)      10(2K)
{" 10uS/格", " 20uS/格", " 50uS/格" ,  "100uS/格",  "200uS/格",  "500uS/格","  1mS/格","  2mS/格", "  5mS/格"," 10mS/格"," 20mS/格",
// 11(800)    12(400)     13(200)    14(80)      15(40)          16          17         18          19          20          21
 " 50mS/格"," 0.1S/格"," 0.2S/格"," 0.5S/格","   1S/格"};
const float T_Scale[16]=   //水平灵敏度档位倍乘系数
// 0   1   2    3    4    5     6    7  8   9  10  11  12  13  14  15   
{2.1, 3, 3.75 , 3.75 , 3 ,  3.75 , 3.75, 3, 3.75 , 3.75 , 3 , 3.75 , 3.75 , 3 , 3.75 , 3.75 };

const u8 Item_V[6][12] = //各档位垂直灵敏度显示字符
//    0，        1，        2，        3，        4，      5       
{" 0.1V/格"," 0.2V/格"," 0.5V/格","   1V/格","   2V/格","   5V/格"};
const u16 V_Scale[6] =    //垂直灵敏度档位倍乘系数
// 0，  1，   2，   3，   4，    5         
{ 1056,528,264,132,66,26};
u8 Wave_updat = 1;

/*****************示波器参数**********************/
int T_Shousuo=1; //时间缩放比例
int V_Shousuo=3; //电压缩放比例
u16 First_edge=0;//第一个边沿
u16 Last_edge=0;//最后一个边沿
u16 New_edge_up[2]={0};//新的边沿
u16 New_edge_dow[2]={0};//新的边沿
u16 AD_New_Buf[AD_num];//采样缓存
u16 AD_Cur_Buf[1024];//当前显存
/* 存储平均值，峰峰值，频率，最小值和最大值的变量 */
float   g_WaveMean = 0;
float   g_WavePkPk = 0;
float   g_WaveFreq = 0;
float   g_WaveMax = 0;
float   g_WaveMin = 1023;
u16 CurTRIPos = 0;   // 记录满足上升沿触发的位置 
u8 Stop_Flag = 0;
u8 Auto_Flag = 0;
u8 UP_Flag = 1;
u8 DOWN_Flag = 0;
/*****************波形发生器**********************/
uint16_t g_usWaveBuff[64];     /* 波形缓存 */
/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "数字示波器",             0,0,  0,  480,320,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "自动",              GUI_ID_BUTTON0,          424,0, 50, 30, 0,0},
    { BUTTON_CreateIndirect,    "单次",              GUI_ID_BUTTON1,          424,35, 50, 30, 0,0},
    { BUTTON_CreateIndirect,    "升沿",              GUI_ID_BUTTON2,          424,70,50, 30, 0,0},
    { BUTTON_CreateIndirect,    "降沿",              GUI_ID_BUTTON3,          424,105,50, 30, 0,0},
    { BUTTON_CreateIndirect,    "菜单",              GUI_ID_BUTTON4,          424,140,50, 30, 0,0},
	{ BUTTON_CreateIndirect,    "开始",              GUI_ID_BUTTON5,          424,175,50, 30, 0,0},
    { BUTTON_CreateIndirect,    "复位",              GUI_ID_BUTTON6,          424,210,50, 30, 0,0},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO0,           430,250,50, 40, 0,2},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER0,          5,260,200, 20, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER1,          225,260,200,20, 0,0},
	{ GRAPH_CreateIndirect,      NULL,               GUI_ID_GRAPH0,           0 ,0,420,250, 0,0},
	{ TEXT_CreateIndirect,      "10us/格",           GUI_ID_TEXT0,            90, 280,78, 12, 0,0},
	{ TEXT_CreateIndirect,      "0.1伏/格",          GUI_ID_TEXT1,            300, 280,78, 12, 0,0},
};

static void _UserDraw(WM_HWIN hWin, int Stage)
{
	  char buf[10] = {0}; 
      if (Stage==GRAPH_DRAW_LAST)
      {
              if(1)//WaveCtrl.Burr==0xfe
              {
				  GUI_SetColor(GUI_LIGHTBLUE);
				  GUI_SetFont(&GUI_FontHZ12);
				  GUI_DispStringAt("频率", 280, 10);
//				  sprintf(buf, "%5.3fV", g_WaveMean);
//				  GUI_DispStringAt(buf, 307+28, 10);
				  GUI_DispDecAt(1000,295+28,10,4);
				  GUI_DispStringAt("周期", 40, 200);
//				  sprintf(buf, "%5.3fV", g_WaveMean);
//				  GUI_DispStringAt(buf, 557+28, 200);
				  GUI_DispDecAt(50,55+28,200,4);
				  GUI_DispStringAt("占空比", 40, 220);
//				  sprintf(buf, "%5.3fV", g_WaveMean);
//				  GUI_DispStringAt(buf, 67+28, 220);
				  GUI_DispDecAt(50,67+28,220,3);
				  
				  GUI_DispStringAt("最大值", 160, 200);
				  sprintf(buf, "%5.3fV", g_WaveMax);
				  GUI_DispStringAt(buf, 187+28, 200);
				  
				  GUI_DispStringAt("最小值", 160, 220);
				  sprintf(buf, "%5.3fV", g_WaveMin);
				  GUI_DispStringAt(buf, 187+28, 220);
				  //GUI_DispDecAt(10,187+28,220,4);
				  
				  GUI_DispStringAt("峰峰值", 280, 200);
				  sprintf(buf, "%5.3fV", g_WavePkPk);
				  GUI_DispStringAt(buf, 307+28, 200);
				  //GUI_DispDecAt(3300,307+28,200,4);
				  GUI_DispStringAt("平均值", 280, 220);
				  sprintf(buf, "%5.3fV", g_WaveMean);
				  GUI_DispStringAt(buf, 307+28, 220);
				  
				  
              }
             
//              if(FFTflag)
//              {
//                  u16 i,k;
//                  GUI_SetColor(GUI_LIGHTMAGENTA);
//                  
//                  //在里面进行了FFTshift
//                  k=0;
////                  for(i=0;i<fftdep/2;i++)
////                  {
////                      GUI_DrawVLine(160+i, fftshowbase-(u8)s[k].real, fftshowbase);
////                      k ++;
////                      if(160+i>310)break;
////                  }
////                  k = fftdep-1;
////                  for(i=0;i<fftdep/2;i++)
////                  {
////                      GUI_DrawVLine(159-i, fftshowbase-(u8)s[k].real, fftshowbase);
////                      k -- ;
////                      if(160-i<22)break;
////                  }
//                    
//              }               
               
      }
      if (Stage==GRAPH_DRAW_FIRST)
      {
               
      }
}
/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/

void InitDialog(WM_MESSAGE * pMsg)
{

    WM_HWIN hWin = pMsg->hWin;
	
    //
    //FRAMEWIN
    //
    FRAMEWIN_SetClientColor(hWin,0xff00ff);
    FRAMEWIN_SetTextColor(hWin,0x0000ff);
    FRAMEWIN_SetFont(hWin,&GUI_FontHZ12);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    //FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);//
    FRAMEWIN_AddMaxButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMinButton(hWin, FRAMEWIN_BUTTON_RIGHT, 1);
    FRAMEWIN_SetTitleHeight(hWin,20);
	    //
    //GUI_ID_SCROLLBAR1
    //
    //SCROLLBAR_SetNumItems(WM_GetDialogItem(hWin,GUI_ID_SCROLLBAR1),1024);
    //
    //GUI_ID_BUTTON0
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),&GUI_FontHZ12);
    //
    //GUI_ID_BUTTON1
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON1),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON1),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON1),&GUI_FontHZ12);
    //
    //GUI_ID_BUTTON2
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON2),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON2),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON2),&GUI_FontHZ12);
    //
    //GUI_ID_BUTTON3
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON3),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON3),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON3),&GUI_FontHZ12);
    //
    //GUI_ID_BUTTON4
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON4),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON4),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON4),&GUI_FontHZ12);
	//
    //GUI_ID_BUTTON5
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON5),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON5),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON5),&GUI_FontHZ12);
	//
    //GUI_ID_BUTTON6
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON6),BUTTON_CI_UNPRESSED,0xa9a9a9);
    BUTTON_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON6),BUTTON_CI_UNPRESSED,0x00ffff);
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON6),&GUI_FontHZ12);
    //
    //GUI_ID_RADIO0
    //
    RADIO_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_RADIO0),0xff00ff);
	RADIO_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_RADIO0),GUI_YELLOW);
    RADIO_SetGroupId(WM_GetDialogItem(hWin,GUI_ID_RADIO0),0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"DC",0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"AC",1);
    //
    //GUI_ID_SLIDER0
    //
    SLIDER_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),0xff00ff);
    SLIDER_SetNumTicks(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),16);
	SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),0,15);//滑动条数值上下限
    SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),3);//滑动条初始值设置
	//
    //GUI_ID_SLIDER1
    //
    SLIDER_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),0xff00ff);
	SLIDER_SetNumTicks(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),6);
	SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),0,5);//滑动条数值上下限
    SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),3);//滑动条初始值设置
	//	
	//GUI_ID_GRAPH0

	hGraph = WM_GetDialogItem(hWin,GUI_ID_GRAPH0);//获取图表的句柄
    GRAPH_SetUserDraw(hGraph, _UserDraw);//用户回调函数
    
    GRAPH_SetBorder(hGraph,0,0,0,0);//设置边框 左 顶 右 低 
    GRAPH_SetGridVis(hGraph,1);//	启用网格线绘制 
	//GRAPH_SetLineStyleH();//设置水平网格线性
    GRAPH_SetGridDistX(hGraph,40);//设置网格线的间距
    GRAPH_SetGridDistY(hGraph,40);
   // GRAPH_SetGridFixedX(hGraph, 1);//固定水平滑动条，1固定，0不固定

    //创建GRAPH_DATA_YT对象：Color绘制数据的颜色 MaxNumItems数据最大项数；
	//PData数据指I16；NumItems数据项数
    hData = GRAPH_DATA_YT_Create(GUI_YELLOW,1000,0, 0);
    GRAPH_AttachData(hGraph, hData);//将数据对象添加到图标
    GRAPH_DATA_YT_SetAlign(hData, GRAPH_ALIGN_RIGHT);
	GRAPH_DATA_YT_SetOffY(hData, 400);
	 //GRAPH_DATA_YT_SetAlign(hData, GRAPH_ALIGN_LEFT);
	
//	hData1 = GRAPH_DATA_YT_Create(GUI_BLUE,1024, Cos_Data, 1024);
//    GRAPH_AttachData(hGraph, hData1);//将数据对象添加到图标
//    GRAPH_DATA_YT_SetAlign(hData1, GRAPH_ALIGN_RIGHT);
     //GRAPH_DATA_YT_SetAlign(hData1, GRAPH_ALIGN_LEFT);
	
    GRAPH_SetVSizeY(hGraph, 800);//设置图表缓存区域
    GRAPH_SetVSizeX(hGraph, 1000);
    
    hScale[0] = GRAPH_SCALE_Create(18, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 40);//创建横纵刻度
    hScale[1] = GRAPH_SCALE_Create(190, GUI_TA_CENTER, GRAPH_SCALE_CF_HORIZONTAL, 40);
 
    GRAPH_AttachScale(hGraph, hScale[0]);//将刻度对象添加到图表
    GRAPH_AttachScale(hGraph, hScale[1]);
    GRAPH_SCALE_SetOff(hScale[0],399);//这一句是管竖着标尺的0点位置
	GRAPH_SCALE_SetOff(hScale[1],0);
    GRAPH_SCALE_SetTextColor(hScale[0], GUI_RED);
	GRAPH_SCALE_SetTextColor(hScale[1], GUI_YELLOW);

    GRAPH_SetColor(hGraph, GUI_MAGENTA,GRAPH_CI_BORDER);
    GRAPH_SCALE_SetFactor(hScale[0], 0.025f);
    GRAPH_SCALE_SetFactor(hScale[1], 0.025f);//设置刻度因子
	//
    //GUI_ID_TEXT0
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),0xff00ff);
    TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),GUI_YELLOW);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_FontHZ12);
    //
    //GUI_ID_TEXT1
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT1),0xff00ff);
    TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT1),GUI_YELLOW);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_FontHZ12);

}



static void ButtonChange(WM_HWIN hWin,int Id)
{
    static u8 flag = 0;
    WM_HWIN hButton, hSlider1;
    switch(Id)
    { 

		case GUI_ID_BUTTON0://自动
			 hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON0);
			  Auto_Flag ^=1; 
			  if(Auto_Flag)
              {	
					BUTTON_SetBkColor(hButton,0,GUI_BLUE);
                }
                else
                {
					BUTTON_SetBkColor(hButton, 0, 0xa9a9a9);
                               
                }
//			  hBox = MESSAGEBOX_Create("Button is pressed",
//                            "Warning", GUI_MESSAGEBOX_CF_MOVEABLE);
//			  WM_MakeModal(hBox);//此举一加指针设备仅输入到模态窗口。
//			  WM_SetStayOnTop(hBox, 1);
              break;
        case GUI_ID_BUTTON1://单次
              
              break;
        case GUI_ID_BUTTON2: //升沿
			 UP_Flag = 1;
			DOWN_Flag = 0;
				hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON2);
              WM_DisableWindow(hButton);
              WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON3));
               break;
        case GUI_ID_BUTTON3://降沿
				UP_Flag = 0;
				DOWN_Flag = 1;
                hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON3);
                WM_DisableWindow(hButton);
                WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)); 
                break;
        case GUI_ID_BUTTON4://菜单
                if(!MenuDelet)
                 {
                     MenuDelet = 1; 
                     MEUN_CREAT();
                 }
                break;
        case GUI_ID_BUTTON5://停止
               hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON5);
               flag ^= 1;
                if(flag)
                {
					Wave_updat = 0;
                    BUTTON_SetText(hButton,"停止"); 
					BUTTON_SetBkColor(hButton,0,GUI_BLUE);
                }
                else
                {
					Wave_updat = 1;
                    BUTTON_SetText(hButton,"开始");
					BUTTON_SetBkColor(hButton, 0, 0xa9a9a9);
                               
                }
               break;
        case GUI_ID_BUTTON6://复位 
                WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
               
                break;   
        case GUI_ID_BUTTON7://菜单
                 
                 break;
        case GUI_ID_BUTTON8://耦合方式
                hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON8);
               
                break;  
         case GUI_ID_BUTTON9://存储
                 
                 break;
    }
}

static void OnRadioValueChanged(WM_HWIN hDlg, int Id)//单选按钮数值变化
{
	WM_HWIN hRadio;
	if(Id == GUI_ID_RADIO0)
	{
		switch(RADIO_GetValue(WM_GetDialogItem(hDlg,GUI_ID_RADIO1)))
							{
								case 0:
									
									GUI_DispStringAt("radio.0",0,20);
									break;
								case 1:
									
									GUI_DispStringAt("radio.1",0,20);
									break;
							}
	}
}

static void OnSliderValueChanged(WM_HWIN hDlg, int Id)//滑动条0数值发生变化是的操作
{
	  char Timstr[8],Volstr[8];
  WM_HWIN hSlider0,hSlider1,hText0,hText1;
  
  if (Id == GUI_ID_SLIDER0)
  {
        hSlider0 = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0);
        hText0   = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
        T_Shousuo = SLIDER_GetValue(hSlider0);
	    if(T_Shousuo > 15) T_Shousuo = 15; 
        GUI_DispDecAt(T_Shousuo,0,20,3);
	    TimeBaseId = T_Shousuo;
		ADC_DMA_Close();
		ADC_DMA_Open();
        TEXT_SetText(hText0, Item_T[T_Shousuo]);
  } 
  if (Id == GUI_ID_SLIDER1)
  {
      hSlider1 = WM_GetDialogItem(hDlg, GUI_ID_SLIDER1);
      hText1   = WM_GetDialogItem(hDlg, GUI_ID_TEXT1);
      V_Shousuo = SLIDER_GetValue(hSlider1);
	  GUI_DispDecAt(V_Shousuo,0,0,3);
      TEXT_SetText(hText1,Item_V[V_Shousuo]);
  }
  WM_InvalidateWindow(WM_GetClientWindow(hDlg));
	
}
/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	char radio_value = 0;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT://重绘窗口
            //WM_Paint(pMsg);
            break;
        case WM_INIT_DIALOG://初始化对话框
			
            InitDialog(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); /* Id of widget */
            NCode = pMsg->Data.v;        /* Notification code */
            switch(NCode)
            {
				case WM_NOTIFICATION_RELEASED:
				  ButtonChange(hWin,Id); 
				  break;
				case WM_NOTIFICATION_CLICKED:
 
                break;
				case WM_NOTIFICATION_VALUE_CHANGED:/* Value has changed */
							
                OnSliderValueChanged(hWin, Id);//滑动条数值发生变化
				OnRadioValueChanged(hWin, Id);//单选框数值发生变化
                break;
				case WM_NOTIFICATION_SEL_CHANGED://下拉列表值改变
                //DownChange(hDlg, Id);
				break;
             }             
        default:
            WM_DefaultProc(pMsg);
    }

	 
}


/******************************************************************************/
//                  						采集波形
/******************************************************************************/
void Get_Signal()//最后可以改为双通道的
{
    u16 i,j,k;
	u16 usCurPos;
	g_WaveMin = 4095;
	g_WaveMax = 0.0f;
	g_WaveMean = 0.0f;
	/* 读取的是ADC3的位置 */
	usCurPos = 10240 - DMA2_Stream1->NDTR;
	if(TimeBaseId < 10)
	{
	    if(usCurPos < 2048)
		{
			j = 8192 + usCurPos;	
			/* 获取2k数据的前部分  */
			for(i = j; i < 10240; i++)
			{
				k = (i-j)*3;
				AD_New_Buf[k] = ADC1ConvertedValue[i];
				AD_New_Buf[k+1] = ADC2ConvertedValue[i];
				AD_New_Buf[k+2] = ADC3ConvertedValue[i];
			}
			
			j = (2048 - usCurPos)*3;
			/* 获取2K数据的后部分 */
			for(i = 0; i < usCurPos; i++)
			{
				k = i*3;
				AD_New_Buf[j+k] = ADC1ConvertedValue[i];
				AD_New_Buf[j+k+1] = ADC2ConvertedValue[i];
				AD_New_Buf[j+k+2] = ADC3ConvertedValue[i];
			}		
		}
		/* 第二种情况：当前DMA传输位置在大于等于2k的地方 */
		else
		{
			usCurPos = usCurPos - 2048;
			for(i = 0; i < 2048; i++)
			{
				k = i*3;
				AD_New_Buf[k] = ADC1ConvertedValue[i+usCurPos];
				AD_New_Buf[k+1] = ADC2ConvertedValue[i+usCurPos];
				AD_New_Buf[k+2] = ADC3ConvertedValue[i+usCurPos];
			}	
		}
		//波形处理
		if(Auto_Flag)//自动
		{
			for(i = 1; i < 5120; i++)
			{
			
				if(UP_Flag)
				{
					if((AD_New_Buf[i-1]<g_usTrigValue)&&(AD_New_Buf[i]<=g_usTrigValue)&&(AD_New_Buf[i+1]>g_usTrigValue)&&(AD_New_Buf[i+2]>g_usTrigValue))
					{	
						break;
					}
				}
				if(DOWN_Flag)
				{
					if((AD_New_Buf[i]>=g_usTrigValue)&&(AD_New_Buf[i+1]<g_usTrigValue))
					{	
						break;
					}
				}
				
			}
			
		}
		CurTRIPos = i;/* 记录满足上升沿触发的位置 */
		if(CurTRIPos < 0) CurTRIPos = 0;
		if(CurTRIPos > 5120) CurTRIPos = 5120;
		/* 从位置g_usCurTriPos+g_usCurTriStep开始的1024个数据全部赋值给g_usWaveBuf */
		for(i = 0; i < 1000; i++)
		{
			if(CurTRIPos != 5120)
			{
				AD_Cur_Buf[i] =AD_New_Buf[(u16)(i*T_Scale[T_Shousuo])+CurTRIPos-1];
			
			}
			else
			{
				AD_Cur_Buf[i] =AD_New_Buf[i+CurTRIPos];
			}
			if(AD_Cur_Buf[i] < g_WaveMin)
			{
				g_WaveMin = AD_Cur_Buf[i];
			}
			
			if(AD_Cur_Buf[i] > g_WaveMax)
			{
				g_WaveMax = AD_Cur_Buf[i];
			}
			g_WaveMean += AD_Cur_Buf[i];
		}
	}
	else
	{
	
	}
		g_usTrigValue = (g_WaveMax + g_WaveMin)/2;
	/* 求平均值 */
		g_WaveMean = (g_WaveMean / 1000.0f) *3.3f / 1023.0f;
		
		/* 求最大值 */
		g_WaveMax =  g_WaveMax * 3.3f / 1023;
		
		/* 求最小值 */
		g_WaveMin = g_WaveMin *3.3f / 1023;
		
		/* 求峰峰值 */
		g_WavePkPk = g_WaveMax - g_WaveMin;
}

void Updata_signal()
{
	u16 i = 0;
	for(i = 0;i<1000;i++)//波形更新
	{
		
		GRAPH_DATA_YT_AddValue(hData, AD_Cur_Buf[i]*V_Scale[V_Shousuo]>>10);	
//		delay_nms(20);
	}
}
/*********************************************************************
*
*       MainTask
*
**********************************************************************
WaveTaskCreat*/
void WaveTaskCreat(void) 
{ 
		int i = 0;
	static int j = 0;
    WM_SetDesktopColor(GUI_LIGHTBLUE);      /* Automacally update desktop window */
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */

	GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
	//MEUN_CREAT();
	
}