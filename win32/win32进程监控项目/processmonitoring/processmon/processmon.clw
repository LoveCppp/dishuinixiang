; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CProcessmonDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "processmon.h"

ClassCount=3
Class1=CProcessmonApp
Class2=CProcessmonDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_PROCESSMON_DIALOG

[CLS:CProcessmonApp]
Type=0
HeaderFile=processmon.h
ImplementationFile=processmon.cpp
Filter=N

[CLS:CProcessmonDlg]
Type=0
HeaderFile=processmonDlg.h
ImplementationFile=processmonDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON_MESSAGEBOXOPEN

[CLS:CAboutDlg]
Type=0
HeaderFile=processmonDlg.h
ImplementationFile=processmonDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PROCESSMON_DIALOG]
Type=1
Class=CProcessmonDlg
ControlCount=4
Control1=IDC_BUTTON1,button,1342242816
Control2=IDC_EDIT_PID,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_BUTTON_MESSAGEBOXOPEN,button,1342242816

