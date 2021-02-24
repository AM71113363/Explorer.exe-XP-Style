#ifndef H_RESOURCE_H
#define H_RESOURCE_H

#define TBN_DROPDOWN	    (TBN_FIRST-10)
#define BTNS_SEP	        TBSTYLE_SEP
#define TBSTYLE_FLAT        2048
#define TBSTYLE_LIST        4096
#define TBSTYLE_CUSTOMERASE 8192
#define TB_SETSTYLE	(WM_USER+56)
#define TB_GETSTYLE	(WM_USER+57)
#define TBSTYLE_TRANSPARENT	0x8000
#define I_IMAGENONE (-2)
#define BTNS_AUTOSIZE	16
#define BTNS_SHOWTEXT	0x0040
#define TB_GETRECT	(WM_USER+51)
#define TB_SETEXTENDEDSTYLE	(WM_USER+84)
#define TB_SETIMAGELIST	(WM_USER+48)
#define TB_SETHOTIMAGELIST	(WM_USER+52)
#define BTNS_DROPDOWN	8
#define BTNS_BUTTON	TBSTYLE_BUTTON
#define BTNS_WHOLEDROPDOWN	0x0080
#define TVS_TRACKSELECT 512



//#define MY_DEBUG 1
//define TB_SETPRESSEDIMAGELIST     (WM_USER +104)
#define  YES                        1
#define  NO                         0
#define BOOO                        -121



#define MENU_RIGHT_OPEN             700
#define MENU_RIGHT_EDIT             701
#define MENU_RIGHT_RENAME           702
#define MENU_RIGHT_NEWFOLDER        703
#define MENU_RIGHT_DELETE           704
#define MENU_RIGHT_UPDATE           705


#define ID_MAIN_ICON                        1000
//#define ID_MAIN_ICON_                       1001
#define ID_TREE_VIEW                        1002
#define ID_LIST_VIEW                        1003
#define MAINMENU                            1004
#define MENU_RIGHT                          1005
#define BMP_TOOLBAR_ON                      2001
#define BMP_TOOLBAR_HOT                     2002
#define BMP_WINDOW                          2003
#define BMP_GO_ON                           2004
#define BMP_GO_HOT                          2005

#define ID_STATUSBAR                        4000


#define ID_ADDRESSBAR                       5000
#define ID_ADB_STATIC                       5001
#define ID_ADB_GO                           5002




#define ID_MENUBAR                           6000
#define IDMB_FILE                            6001
#define IDMB_EDIT                            6002
#define IDMB_VIEW                            6003
#define IDMB_FAVORITES                       6004
#define IDMB_TOOLS                           6005
#define IDMB_DEBUG                           6006
#define IDM_DEBUG                            6007
#define IDM_DEBUG_ERROR                      6008
#define IDM_DEBUG_ALL                        6009
#define ID_VIEW_LARGE                        6010
#define ID_VIEW_SMALL                        6011
#define ID_VIEW_LIST                         6012
#define ID_VIEW_DETAILS                      6013

// WM COMMAND ID


// all > 7001 code are for button images of toolbar

#define ID_TOOLBAR                          7000
#define IDTB_BACK                           7001
#define IDTB_FORW                           7002
#define IDTB_UP                             7003
#define IDTB_SEARCH                         7004
#define IDTB_HOME                           7005
#define IDTB_REFRESH                        7006
#define IDTB_DELETE                         7007
#define IDTB_CUTMODE                        7008
#define IDTB_COPYMODE                       7009
#define IDTB_PASTE                          7010
#define IDTB_VIEWMODE                       7011
#define IDTB_NEWFOLDER                      7012




 
#define HEADER_CLICK   -108
#define _lpnm   ((LPNMHDR)lParam)
#define _lpnmTB ((LPNMTOOLBAR)lParam)
#define LISTVIEW_BEGINDRAG           1234

#define  ThisIsDir                   1
#define  ThisIsFile                  2



#endif  //PROTECT
