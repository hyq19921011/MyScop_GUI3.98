;//********************************************************************************************************
;//                                               uC/OS-II
;//                                         The Real-Time Kernel
;//
;//                                           ARM Cortex-M3 Port
;//
;// File      : OS_CPU_A.ASM
;// Version   : V2.86
;// By        : 
;// For       : ARMv7M Cortex-M3
;// Mode      : Thumb2
;// Toolchain : IAR EWARM 5.20
;//********************************************************************************************************

;//*
;//*********************************************************************************************************
;//*                                           ȫ�ֳ�������
;//*********************************************************************************************************
;//*
;// �����ⲿ����������
    EXTERN  OSRunning
    
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    
    EXTERN  OSIntNesting
    
    EXTERN  OSTaskSwHook

    EXTERN  OSRdyGrp
    EXTERN  OSRdyTbl
    EXTERN  OSPrioHighRdy

;// �ⲿ���Ե��õĺ���
    PUBLIC  OS_CPU_SR_Save
    PUBLIC  OS_CPU_SR_Restore
    PUBLIC  OSStartHighRdy
    PUBLIC  OSCtxSw
    PUBLIC  OSIntCtxSw
    PUBLIC  OS_CPU_PendSVHandler
;// PUBLIC  OS_SchedNew

;//********************************************************************************************************
;//                                           PendV ��ʹ�õļ����Ĵ���
;//********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04                              ;// �жϿ��Ƽ�״̬�Ĵ���
NVIC_SYSPRI14   EQU     0xE000ED22                              ;// ����PendSV���ȼ��ļĴ���
NVIC_PENDSV_PRI EQU           0xFF                              ;// PendSV �쳣���ȼ�(���)
NVIC_PENDSVSET  EQU     0x10000000                              ;// PendSV �쳣����λ����

;//********************************************************************************************************
;//                                           �����ǻ�����ӿ� 
;//********************************************************************************************************

        RSEG CODE:CODE:NOROOT(2)

;//*
;//********************************************************************************************************
;//* �� �� �� : OS_SchedNew()
;//**------------------------------------------------------------------------------------------------------
;//* ��    �� : 
;//*            Ѱ����߼�����������
;//*            �˳���ʹ����CM3�ں˵���ָ��RBIT, CLZ ͨ��������������, �ٶ��൱����
;//             �������������ȼ���������С��63��ϵͳ����û�бȲ����������
;//      
;//             �����ʹ�ô˺���,��������os_core.c�����OS_SchedNew()
;//**------------------------------------------------------------------------------------------------------
;//* ������� : OSRdyGrp, OSRdyTbl
;//**------------------------------------------------------------------------------------------------------
;//* ������� : OSPrioHighRdy
;//**------------------------------------------------------------------------------------------------------
;//* ��   ��  : None.
;//********************************************************************************************************
;//*
;//OS_SchedNew
;//    PUSH   {R0-R4}
;//    LDR    R0, =OSRdyGrp
;//
;//    LDRH   R2, [R0]
;//    RBIT   R2, R2
;//    CLZ    R2, R2                   ; y = CLZ(OSRdyGrp);
;//
;//    LDR    R0, =OSRdyTbl
;//    ADD    R1, R0, R2, LSL #0x01    ; ptbl = &OSRdyTbl[y];
;//
;//    LDRH   R3, [R1]
;//    RBIT   R3, R3
;//    CLZ    R3, R3                   ; x = CLZ(*ptbl);
;//
;//    LDR    R0, =OSPrioHighRdy
;//    ADD    R4, R3, R2, LSL #0x04
;//    STRB   R4, [R0]                 ; OSPrioHighRdy = x + (y << 4);
;//
;//    POP    {R0-R4}
;//    BX      LR

;//*
;//********************************************************************************************************
;//* �� �� �� : 
;//*             OS_CPU_SR  OS_CPU_SR_Save(void);
;//*             void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;//**------------------------------------------------------------------------------------------------------
;//* ��    �� :  ��3�ַ������ٽ���봦��, ���ن���
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ��   ��  : 
;//********************************************************************************************************
;//*
OS_CPU_SR_Save
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0
    BX      LR

;//*
;//********************************************************************************************************
;//* �� �� �� : OSStartHighRdy();
;//**------------------------------------------------------------------------------------------------------
;//* ��    �� : ������ʼ����
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ��   ��  : 
;//********************************************************************************************************
;//*
OSStartHighRdy
    LDR     R0, =NVIC_SYSPRI14                                  ;// ����PendSV���ȼ�
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0                                              ;// ��ʼ���߳�PSP
    MSR     PSP, R0

    LDR     R0, =OSRunning                                      ;// OSRunning = TRUE
    MOVS    R1, #1
    STRB    R1, [R0]

    LDR     R0, =NVIC_INT_CTRL                                  ;// ����PendSV�쳣, ��PendSv�����л���ʼ
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I                                                   ;// �����ж�

OSStartHang
    B       OSStartHang                                         ;// while(1);

;//*
;//********************************************************************************************************
;//* �� �� �� : OSCtxSw();
;//**------------------------------------------------------------------------------------------------------
;//* ��    �� : ���������л�
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ��   ��  : 
;//********************************************************************************************************
;//*
OSCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ;// ����PendSV�쳣
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;//*
;//********************************************************************************************************
;//* �� �� �� : OSIntCtxSw();
;//**------------------------------------------------------------------------------------------------------
;//* ��    �� : �жϼ������л�
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ��   ��  : 
;//********************************************************************************************************
;//*
OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ;// ����PendSV�쳣
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;//*
;//********************************************************************************************************
;//* �� �� �� : OS_CPU_PendSVHandler()
;//**------------------------------------------------------------------------------------------------------
;//* ��    �� : ��cm3�ں���,�����������ı��л��ǿ�������ʵ��
;//                 |     ....        |
;//                 |-----------------|
;//                 |     ....        |
;//                 |-----------------|
;//                 |     ....        |
;//                 |-----------------|       |---- �����л�ʱPSP
;//     Low Memory  |     ....        |       |
;//                 |-----------------|       |    |---------------|      |----------------|
;//        ^        |       R4        |  <----|----|--OSTCBStkPtr  |<-----|   (OS_TCB *)   |
;//        ^        |-----------------|            |---------------|      |----------------|
;//        ^        |       R5        |            |               |         OSTCBHighRdy
;//        |        |-----------------|            |---------------|
;//        |        |       R6        |            |               |
;//        |        |-----------------|            |---------------|
;//        |        |       R7        |            |               |
;//        |        |-----------------|            |---------------|
;//        |        |       R8        |                 Task's
;//        |        |-----------------|                 OS_TCB
;//        |        |       R9        |
;//        |        |-----------------|
;//        |        |      R10        |
;//      Stack      |-----------------|
;//      Growth     |      R11        |
;//       = 1       |-----------------|
;//        |        |    R0 = p_arg   |  <-------- �쳣ʱ��PSP (������������ջ)
;//        |        |-----------------|
;//        |        |       R1        |
;//        |        |-----------------|
;//        |        |       R2        |
;//        |        |-----------------|
;//        |        |       R3        |
;//        |        |-----------------|
;//        |        |       R12       |
;//        |        |-----------------|
;//        |        |       LR        |
;//        |        |-----------------|
;//        |        |    PC = task    |
;//        |        |-----------------|
;//        |        |      xPSR       |
;//    High Memory  |-----------------|
;//                 |      ....       |
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ������� : 
;//**------------------------------------------------------------------------------------------------------
;//* ��   ��  : 
;//********************************************************************************************************
;//*
OS_CPU_PendSVHandler
    CPSID   I                                                   ;// ����context�л��ǹر��ж�
    MRS     R0, PSP                                             ;// ��ȡPSP
    CBZ     R0, OS_CPU_PendSVHandler_nosave                     ;// �ڶ������ʼ��ʱ,PSP����ʼ��Ϊ0
                                                                ;// PSP�����0,��ʾ����û�����й�,��ô����Ҫѹջ
                                                                ;// ֱ�Ӽ�������context
    SUBS    R0, R0, #0x20                                       ;// ����PSPָ��, R4-R11 ��32�ֽ�
    STM     R0, {R4-R11}                                        ;// ѹջR4-R11, ����8���Ĵ��������쳣ʱ�Զ�ѹջ��

    LDR     R1, =OSTCBCur                                       ;// ��ȡOSTCBCur->OSTCBStkPtr
    LDR     R1, [R1]
    STR     R0, [R1]                                            ;// ����ǰ����Ķ�ջ���浽�Լ���������ƿ�
                                                                ;// OSTCBCur->OSTCBStkPtr = PSP
                                                                ;// �������д�λ��,�Ѿ������˵�ǰ�����context��
OS_CPU_PendSVHandler_nosave
    ;// ʹ��OSTaskSwHook��ʱ��ȥ��ע��
    ;//PUSH    {R14}
    ;//LDR     R0, =OSTaskSwHook
    ;//BLX     R0
    ;//POP     {R14}
    
                                                                ;// �ڵ���֮ǰ,ϵͳ�ں��Ѿ������
                                                                ;// OSPrioHighRdy �� OSTCBHighRdy
    LDR     R0, =OSPrioCur                                      ;// ��ǰ�������ȼ� = �����������ȼ�
    LDR     R1, =OSPrioHighRdy                                  ;// OSPrioCur = OSPrioHighRdy;
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCur                                       ;// ��ǰ������ƿ� = ����������ƿ�;
    LDR     R1, =OSTCBHighRdy                                   ;// OSTCBCur  = OSTCBHighRdy;
    LDR     R2, [R1]                                            ;// 
    STR     R2, [R0]                                            ;// ��ʱ [R2] = �������PSP

    LDR     R0, [R2]                                            ;// R0 = �������PSP
    LDM     R0, {R4-R11}                                        ;// ��ջ R4 - R11
    ADDS    R0, R0, #0x20                                       ;// ����PSP
    MSR     PSP, R0                                             ;//
    
                                                                ;// ��ǰ�쳣ʹ�õĶ�ջ��MPS
    ORR     LR, LR, #0x04                                       ;// �޸�LR��BIT2=1, ȷ���쳣�˳�ʱ��ջʹ��PSP
                                                                ;// �����޸�CONTROL[1] = 1

    CPSIE   I
    BX      LR                                                  ;// �쳣����

    END