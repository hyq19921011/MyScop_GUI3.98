
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

const char Item_T[16][12] =  //����λˮƽɨ��ʱ����ʾ�ַ�
//    0��4M��    1(2M)       2(800K)     3(400K)     4(200K)       5(80K)       6(40k)    7 (20K)      8(8K)     9(4K)      10(2K)
{" 10uS/��", " 20uS/��", " 50uS/��" ,  "100uS/��",  "200uS/��",  "500uS/��","  1mS/��","  2mS/��", "  5mS/��"," 10mS/��"," 20mS/��",
// 11(800)    12(400)     13(200)    14(80)      15(40)          16          17         18          19          20          21
 " 50mS/��"," 0.1S/��"," 0.2S/��"," 0.5S/��","   1S/��"};
const float T_Scale[16]=   //ˮƽ�����ȵ�λ����ϵ��
// 0   1   2    3    4    5     6    7  8   9  10  11  12  13  14  15   
{2.1, 3, 3.75 , 3.75 , 3 ,  3.75 , 3.75, 3, 3.75 , 3.75 , 3 , 3.75 , 3.75 , 3 , 3.75 , 3.75 };

const u8 Item_V[6][12] = //����λ��ֱ��������ʾ�ַ�
//    0��        1��        2��        3��        4��      5       
{" 0.1V/��"," 0.2V/��"," 0.5V/��","   1V/��","   2V/��","   5V/��"};
const u16 V_Scale[6] =    //��ֱ�����ȵ�λ����ϵ��
// 0��  1��   2��   3��   4��    5         
{ 1056,528,264,132,66,26};
u8 Wave_updat = 1;

/*****************ʾ��������**********************/
int T_Shousuo=1; //ʱ�����ű���
int V_Shousuo=3; //��ѹ���ű���
u16 First_edge=0;//��һ������
u16 Last_edge=0;//���һ������
u16 New_edge_up[2]={0};//�µı���
u16 New_edge_dow[2]={0};//�µı���
u16 AD_New_Buf[AD_num];//��������
u16 AD_Cur_Buf[1024];//��ǰ�Դ�
/* �洢ƽ��ֵ�����ֵ��Ƶ�ʣ���Сֵ�����ֵ�ı��� */
float   g_WaveMean = 0;
float   g_WavePkPk = 0;
float   g_WaveFreq = 0;
float   g_WaveMax = 0;
float   g_WaveMin = 1023;
u16 CurTRIPos = 0;   // ��¼���������ش�����λ�� 
u8 Stop_Flag = 0;
u8 Auto_Flag = 0;
u8 UP_Flag = 1;
u8 DOWN_Flag = 0;
/*****************���η�����**********************/
uint16_t g_usWaveBuff[64];     /* ���λ��� */
/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "����ʾ����",             0,0,  0,  480,320,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "�Զ�",              GUI_ID_BUTTON0,          424,0, 50, 30, 0,0},
    { BUTTON_CreateIndirect,    "����",              GUI_ID_BUTTON1,          424,35, 50, 30, 0,0},
    { BUTTON_CreateIndirect,    "����",              GUI_ID_BUTTON2,          424,70,50, 30, 0,0},
    { BUTTON_CreateIndirect,    "����",              GUI_ID_BUTTON3,          424,105,50, 30, 0,0},
    { BUTTON_CreateIndirect,    "�˵�",              GUI_ID_BUTTON4,          424,140,50, 30, 0,0},
	{ BUTTON_CreateIndirect,    "��ʼ",              GUI_ID_BUTTON5,          424,175,50, 30, 0,0},
    { BUTTON_CreateIndirect,    "��λ",              GUI_ID_BUTTON6,          424,210,50, 30, 0,0},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO0,           430,250,50, 40, 0,2},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER0,          5,260,200, 20, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER1,          225,260,200,20, 0,0},
	{ GRAPH_CreateIndirect,      NULL,               GUI_ID_GRAPH0,           0 ,0,420,250, 0,0},
	{ TEXT_CreateIndirect,      "10us/��",           GUI_ID_TEXT0,            90, 280,78, 12, 0,0},
	{ TEXT_CreateIndirect,      "0.1��/��",          GUI_ID_TEXT1,            300, 280,78, 12, 0,0},
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
				  GUI_DispStringAt("Ƶ��", 280, 10);
//				  sprintf(buf, "%5.3fV", g_WaveMean);
//				  GUI_DispStringAt(buf, 307+28, 10);
				  GUI_DispDecAt(1000,295+28,10,4);
				  GUI_DispStringAt("����", 40, 200);
//				  sprintf(buf, "%5.3fV", g_WaveMean);
//				  GUI_DispStringAt(buf, 557+28, 200);
				  GUI_DispDecAt(50,55+28,200,4);
				  GUI_DispStringAt("ռ�ձ�", 40, 220);
//				  sprintf(buf, "%5.3fV", g_WaveMean);
//				  GUI_DispStringAt(buf, 67+28, 220);
				  GUI_DispDecAt(50,67+28,220,3);
				  
				  GUI_DispStringAt("���ֵ", 160, 200);
				  sprintf(buf, "%5.3fV", g_WaveMax);
				  GUI_DispStringAt(buf, 187+28, 200);
				  
				  GUI_DispStringAt("��Сֵ", 160, 220);
				  sprintf(buf, "%5.3fV", g_WaveMin);
				  GUI_DispStringAt(buf, 187+28, 220);
				  //GUI_DispDecAt(10,187+28,220,4);
				  
				  GUI_DispStringAt("���ֵ", 280, 200);
				  sprintf(buf, "%5.3fV", g_WavePkPk);
				  GUI_DispStringAt(buf, 307+28, 200);
				  //GUI_DispDecAt(3300,307+28,200,4);
				  GUI_DispStringAt("ƽ��ֵ", 280, 220);
				  sprintf(buf, "%5.3fV", g_WaveMean);
				  GUI_DispStringAt(buf, 307+28, 220);
				  
				  
              }
             
//              if(FFTflag)
//              {
//                  u16 i,k;
//                  GUI_SetColor(GUI_LIGHTMAGENTA);
//                  
//                  //�����������FFTshift
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
	SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),0,15);//��������ֵ������
    SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),3);//��������ʼֵ����
	//
    //GUI_ID_SLIDER1
    //
    SLIDER_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),0xff00ff);
	SLIDER_SetNumTicks(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),6);
	SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),0,5);//��������ֵ������
    SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),3);//��������ʼֵ����
	//	
	//GUI_ID_GRAPH0

	hGraph = WM_GetDialogItem(hWin,GUI_ID_GRAPH0);//��ȡͼ��ľ��
    GRAPH_SetUserDraw(hGraph, _UserDraw);//�û��ص�����
    
    GRAPH_SetBorder(hGraph,0,0,0,0);//���ñ߿� �� �� �� �� 
    GRAPH_SetGridVis(hGraph,1);//	���������߻��� 
	//GRAPH_SetLineStyleH();//����ˮƽ��������
    GRAPH_SetGridDistX(hGraph,40);//���������ߵļ��
    GRAPH_SetGridDistY(hGraph,40);
   // GRAPH_SetGridFixedX(hGraph, 1);//�̶�ˮƽ��������1�̶���0���̶�

    //����GRAPH_DATA_YT����Color�������ݵ���ɫ MaxNumItems�������������
	//PData����ָI16��NumItems��������
    hData = GRAPH_DATA_YT_Create(GUI_YELLOW,1000,0, 0);
    GRAPH_AttachData(hGraph, hData);//�����ݶ�����ӵ�ͼ��
    GRAPH_DATA_YT_SetAlign(hData, GRAPH_ALIGN_RIGHT);
	GRAPH_DATA_YT_SetOffY(hData, 400);
	 //GRAPH_DATA_YT_SetAlign(hData, GRAPH_ALIGN_LEFT);
	
//	hData1 = GRAPH_DATA_YT_Create(GUI_BLUE,1024, Cos_Data, 1024);
//    GRAPH_AttachData(hGraph, hData1);//�����ݶ�����ӵ�ͼ��
//    GRAPH_DATA_YT_SetAlign(hData1, GRAPH_ALIGN_RIGHT);
     //GRAPH_DATA_YT_SetAlign(hData1, GRAPH_ALIGN_LEFT);
	
    GRAPH_SetVSizeY(hGraph, 800);//����ͼ��������
    GRAPH_SetVSizeX(hGraph, 1000);
    
    hScale[0] = GRAPH_SCALE_Create(18, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 40);//�������ݿ̶�
    hScale[1] = GRAPH_SCALE_Create(190, GUI_TA_CENTER, GRAPH_SCALE_CF_HORIZONTAL, 40);
 
    GRAPH_AttachScale(hGraph, hScale[0]);//���̶ȶ�����ӵ�ͼ��
    GRAPH_AttachScale(hGraph, hScale[1]);
    GRAPH_SCALE_SetOff(hScale[0],399);//��һ���ǹ����ű�ߵ�0��λ��
	GRAPH_SCALE_SetOff(hScale[1],0);
    GRAPH_SCALE_SetTextColor(hScale[0], GUI_RED);
	GRAPH_SCALE_SetTextColor(hScale[1], GUI_YELLOW);

    GRAPH_SetColor(hGraph, GUI_MAGENTA,GRAPH_CI_BORDER);
    GRAPH_SCALE_SetFactor(hScale[0], 0.025f);
    GRAPH_SCALE_SetFactor(hScale[1], 0.025f);//���ÿ̶�����
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

		case GUI_ID_BUTTON0://�Զ�
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
//			  WM_MakeModal(hBox);//�˾�һ��ָ���豸�����뵽ģ̬���ڡ�
//			  WM_SetStayOnTop(hBox, 1);
              break;
        case GUI_ID_BUTTON1://����
              
              break;
        case GUI_ID_BUTTON2: //����
			 UP_Flag = 1;
			DOWN_Flag = 0;
				hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON2);
              WM_DisableWindow(hButton);
              WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON3));
               break;
        case GUI_ID_BUTTON3://����
				UP_Flag = 0;
				DOWN_Flag = 1;
                hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON3);
                WM_DisableWindow(hButton);
                WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)); 
                break;
        case GUI_ID_BUTTON4://�˵�
                if(!MenuDelet)
                 {
                     MenuDelet = 1; 
                     MEUN_CREAT();
                 }
                break;
        case GUI_ID_BUTTON5://ֹͣ
               hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON5);
               flag ^= 1;
                if(flag)
                {
					Wave_updat = 0;
                    BUTTON_SetText(hButton,"ֹͣ"); 
					BUTTON_SetBkColor(hButton,0,GUI_BLUE);
                }
                else
                {
					Wave_updat = 1;
                    BUTTON_SetText(hButton,"��ʼ");
					BUTTON_SetBkColor(hButton, 0, 0xa9a9a9);
                               
                }
               break;
        case GUI_ID_BUTTON6://��λ 
                WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
                WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
               
                break;   
        case GUI_ID_BUTTON7://�˵�
                 
                 break;
        case GUI_ID_BUTTON8://��Ϸ�ʽ
                hButton = WM_GetDialogItem(hWin, GUI_ID_BUTTON8);
               
                break;  
         case GUI_ID_BUTTON9://�洢
                 
                 break;
    }
}

static void OnRadioValueChanged(WM_HWIN hDlg, int Id)//��ѡ��ť��ֵ�仯
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

static void OnSliderValueChanged(WM_HWIN hDlg, int Id)//������0��ֵ�����仯�ǵĲ���
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
        case WM_PAINT://�ػ洰��
            //WM_Paint(pMsg);
            break;
        case WM_INIT_DIALOG://��ʼ���Ի���
			
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
							
                OnSliderValueChanged(hWin, Id);//��������ֵ�����仯
				OnRadioValueChanged(hWin, Id);//��ѡ����ֵ�����仯
                break;
				case WM_NOTIFICATION_SEL_CHANGED://�����б�ֵ�ı�
                //DownChange(hDlg, Id);
				break;
             }             
        default:
            WM_DefaultProc(pMsg);
    }

	 
}


/******************************************************************************/
//                  						�ɼ�����
/******************************************************************************/
void Get_Signal()//�����Ը�Ϊ˫ͨ����
{
    u16 i,j,k;
	u16 usCurPos;
	g_WaveMin = 4095;
	g_WaveMax = 0.0f;
	g_WaveMean = 0.0f;
	/* ��ȡ����ADC3��λ�� */
	usCurPos = 10240 - DMA2_Stream1->NDTR;
	if(TimeBaseId < 10)
	{
	    if(usCurPos < 2048)
		{
			j = 8192 + usCurPos;	
			/* ��ȡ2k���ݵ�ǰ����  */
			for(i = j; i < 10240; i++)
			{
				k = (i-j)*3;
				AD_New_Buf[k] = ADC1ConvertedValue[i];
				AD_New_Buf[k+1] = ADC2ConvertedValue[i];
				AD_New_Buf[k+2] = ADC3ConvertedValue[i];
			}
			
			j = (2048 - usCurPos)*3;
			/* ��ȡ2K���ݵĺ󲿷� */
			for(i = 0; i < usCurPos; i++)
			{
				k = i*3;
				AD_New_Buf[j+k] = ADC1ConvertedValue[i];
				AD_New_Buf[j+k+1] = ADC2ConvertedValue[i];
				AD_New_Buf[j+k+2] = ADC3ConvertedValue[i];
			}		
		}
		/* �ڶ����������ǰDMA����λ���ڴ��ڵ���2k�ĵط� */
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
		//���δ���
		if(Auto_Flag)//�Զ�
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
		CurTRIPos = i;/* ��¼���������ش�����λ�� */
		if(CurTRIPos < 0) CurTRIPos = 0;
		if(CurTRIPos > 5120) CurTRIPos = 5120;
		/* ��λ��g_usCurTriPos+g_usCurTriStep��ʼ��1024������ȫ����ֵ��g_usWaveBuf */
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
	/* ��ƽ��ֵ */
		g_WaveMean = (g_WaveMean / 1000.0f) *3.3f / 1023.0f;
		
		/* �����ֵ */
		g_WaveMax =  g_WaveMax * 3.3f / 1023;
		
		/* ����Сֵ */
		g_WaveMin = g_WaveMin *3.3f / 1023;
		
		/* ����ֵ */
		g_WavePkPk = g_WaveMax - g_WaveMin;
}

void Updata_signal()
{
	u16 i = 0;
	for(i = 0;i<1000;i++)//���θ���
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