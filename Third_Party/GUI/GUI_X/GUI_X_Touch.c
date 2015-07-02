/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUI_X.h"
#include "touch.h"

void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}

int  GUI_TOUCH_X_MeasureX(void) 
{
	u8  i;
	u16 ADX = 0;
    //Touch_GetPhyXY();
	for(i=0;i<32;i++)
	ADX += ADS_Read_AD(CMD_RDX);
	return ADX>>5;
}

int  GUI_TOUCH_X_MeasureY(void) 	
{
	
	 u8 i = 0;
	u16 ADY = 0;
    //Touch_GetPhyXY();
	for(i=0;i<32;i++)
	ADY += ADS_Read_AD(CMD_RDY);
	return ADY>>5;
}
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
