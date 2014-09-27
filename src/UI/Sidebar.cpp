#include "Sidebar.h"

#include "AllWindows.h"
#include "Advisors.h"
#include "MessageDialog.h"
#include "Minimap.h"
#include "Warning.h"
#include "Window.h"

#include "../CityView.h"
#include "../Graphics.h"
#include "../SidebarMenu.h"
#include "../Sound.h"
#include "../Terrain.h"
#include "../Time.h"
#include "../Undo.h"
#include "../Widget.h"

#include "../Data/CityInfo.h"
#include "../Data/Constants.h"
#include "../Data/Graphics.h"
#include "../Data/Message.h"
#include "../Data/Mouse.h"
#include "../Data/Screen.h"
#include "../Data/State.h"
#include "../Data/Settings.h"

#define SIDEBAR_BORDER ((Data_Screen.width + 20) % 60)
#define BOTTOM_BORDER ((Data_Screen.height - 24) % 15)
#define XOFFSET_EXPANDED (Data_Screen.width - (Data_Screen.width + 20) % 60 - 162)

static void drawFillerBorders();
static void drawSidebar();
static void drawButtons();
static void drawOverlayText(int xOffset);
static void drawMinimap(int force);

static void buttonOverlay(int param1, int param2);
static void buttonCollapseExpand(int param1, int param2);
static void buttonBuild(int param1, int param2);
static void buttonUndo(int param1, int param2);
static void buttonMessages(int param1, int param2);
static void buttonHelp(int param1, int param2);
static void buttonGoToProblem(int param1, int param2);
static void buttonAdvisors(int param1, int param2);
static void buttonEmpire(int param1, int param2);
static void buttonMissionBriefing(int param1, int param2);
static void buttonRotateNorth(int param1, int param2);
static void buttonRotate(int param1, int param2);

static ImageButton buttonOverlaysCollapseSidebar[] = {
	{127, 5, 31, 20, 4, 90, 0, buttonCollapseExpand, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{4, 3, 117, 31, 4, 93, 0, buttonOverlay, buttonHelp, 1, 0, 0, 0, 0, 18}
};

static ImageButton buttonExpandSidebar[] = {
	{6, 4, 31, 20, 4, 90, 4, buttonCollapseExpand, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0}
};

static ImageButton buttonBuildCollapsed[] = {
	{2, 32, 39, 26, 2, 92, 0, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{2, 67, 39, 26, 2, 92, 8, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 1, 0},
	{2, 102, 39, 26, 2, 92, 12, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 2, 0},
	{2, 137, 39, 26, 2, 92, 4, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 3, 0},
	{2, 172, 39, 26, 2, 92, 40, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 4, 0},
	{2, 207, 39, 26, 2, 92, 28, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 5, 0},
	{2, 242, 39, 26, 2, 92, 24, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 6, 0},
	{2, 277, 39, 26, 2, 92, 20, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 7, 0},
	{2, 312, 39, 26, 2, 92, 16, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 8, 0},
	{2, 347, 39, 26, 2, 92, 44, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 9, 0},
	{2, 382, 39, 26, 2, 92, 36, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 10, 0},
	{2, 417, 39, 26, 2, 92, 32, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 11, 0},
};

static ImageButton buttonBuildExpanded[] = {
	{13, 277, 39, 26, 2, 92, 0, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{63, 277, 39, 26, 2, 92, 8, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 1, 0},
	{113, 277, 39, 26, 2, 92, 12, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 2, 0},
	{13, 313, 39, 26, 2, 92, 4, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 3, 0},
	{63, 313, 39, 26, 2, 92, 40, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 4, 0},
	{113, 313, 39, 26, 2, 92, 28, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 5, 0},
	{13, 349, 39, 26, 2, 92, 24, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 6, 0},
	{63, 349, 39, 26, 2, 92, 20, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 7, 0},
	{113, 349, 39, 26, 2, 92, 16, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 8, 0},
	{13, 385, 39, 26, 2, 92, 44, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 9, 0},
	{63, 385, 39, 26, 2, 92, 36, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 10, 0},
	{113, 385, 39, 26, 2, 92, 32, buttonBuild, Widget_Button_doNothing, 1, 0, 0, 0, 11, 0},
	{13, 421, 39, 26, 4, 92, 48, buttonUndo, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{63, 421, 39, 26, 4, 90, 18, buttonMessages, buttonHelp, 1, 0, 0, 0, 0, 34},
	{113, 421, 39, 26, 2, 90, 22, buttonGoToProblem, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
};

static ImageButton buttonTopExpanded[] = {
	{7, 155, 71, 23, 4, 13, 0, buttonAdvisors, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{84, 155, 71, 23, 4, 13, 3, buttonEmpire, buttonHelp, 1, 0, 0, 0, 0, 32},
	{7, 184, 33, 22, 4, 89, 0, buttonMissionBriefing, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{46, 184, 33, 22, 4, 89, 3, buttonRotateNorth, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{84, 184, 33, 22, 4, 89, 6, buttonRotate, Widget_Button_doNothing, 1, 0, 0, 0, 0, 0},
	{123, 184, 33, 22, 4, 89, 9, buttonRotate, Widget_Button_doNothing, 1, 0, 0, 0, 1, 0},
};

static int minimapRedrawRequested = 0;
static int lastAdvisor = 0;

// sliding sidebar stuff
static const int progressToOffset[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 18, 21, 24, 27,
	30, 33, 37, 41, 45, 49, 54, 59, 64, 70, 76, 83, 91, 99, 106, 113,
	119, 125, 130, 135, 139, 143, 146, 149, 152, 154, 156, 158, 160, 162, 165
};

static struct {
	TimeMillis slideStart;
	int progress;
} data;

void UI_Sidebar_setLastAdvisor(int advisor)
{
	lastAdvisor = advisor;
}

void UI_Sidebar_requestMinimapRefresh()
{
	minimapRedrawRequested = 1;
}

void UI_Sidebar_enableBuildingButtons()
{
	for (int i = 0; i < 12; i++) {
		buttonBuildExpanded[i].enabled = 1;
		if (SidebarMenu_countBuildingMenuItems(buttonBuildExpanded[i].parameter1) <= 0) {
			buttonBuildExpanded[i].enabled = 0;
		}

		buttonBuildCollapsed[i].enabled = 1;
		if (SidebarMenu_countBuildingMenuItems(buttonBuildCollapsed[i].parameter1) <= 0) {
			buttonBuildCollapsed[i].enabled = 0;
		}
	}
}

void UI_Sidebar_drawBackground()
{
	drawSidebar();
	drawFillerBorders();
}

static void drawNumberOfMessages()
{
	if (UI_Window_getId() == Window_City && !Data_State.sidebarCollapsed) {
		buttonBuildExpanded[12].enabled = Data_State.undoReady && Data_State.undoAvailable;
		buttonBuildExpanded[13].enabled = Data_Message.totalMessages > 0;
		buttonBuildExpanded[14].enabled = Data_Message.hotspotCount > 0;
		if (Data_Message.totalMessages) {
			Widget_Text_drawNumberCenteredColored(
				Data_Message.totalMessages, '@', " ",
				XOFFSET_EXPANDED + 74, 452, 32, Font_SmallPlain, Color_Black);
			Widget_Text_drawNumberCenteredColored(
				Data_Message.totalMessages, '@', " ",
				XOFFSET_EXPANDED + 73, 453, 32, Font_SmallPlain, Color_White);
		}
	}
}

static void drawSidebar()
{
	int graphicBase = GraphicId(ID_Graphic_SidePanel);
	int xOffsetPanel = Data_Screen.width - SIDEBAR_BORDER;
	if (Data_State.sidebarCollapsed) {
		xOffsetPanel -= 42;
		Graphics_drawImage(graphicBase, xOffsetPanel, 24);
	} else {
		xOffsetPanel -= 162;
		Graphics_drawImage(graphicBase + 1, xOffsetPanel, 24);
	}
	drawButtons();
	drawOverlayText(xOffsetPanel + 4);
	UI_BuildingMenu_drawSidebarImage(xOffsetPanel + 6, 0);
	drawMinimap(1);
	drawNumberOfMessages();

	// relief images below panel
	int yOffset = 474;
	while (Data_Screen.width - yOffset > 0) {
		if (Data_Screen.width - yOffset <= 120) {
			Graphics_drawImage(graphicBase + 2 + Data_State.sidebarCollapsed, xOffsetPanel, yOffset);
			yOffset += 120;
		} else {
			Graphics_drawImage(graphicBase + 4 + Data_State.sidebarCollapsed, xOffsetPanel, yOffset);
			yOffset += 285;
		}
	}
}

static void drawFillerBorders()
{
	int borderRightWidth = SIDEBAR_BORDER;
	if (borderRightWidth) {
		int graphicId = GraphicId(ID_Graphic_TopMenuSidebar) + 13;
		if (borderRightWidth > 24) {
			// larger border
			graphicId -= 1;
		}
		int xOffset = Data_Screen.width - borderRightWidth;
		for (int yOffset = 24; yOffset < Data_Screen.height; yOffset += 24) {
			Graphics_drawImage(graphicId, xOffset, yOffset);
		}
	}

	int borderBottomHeight = BOTTOM_BORDER;
	Graphics_fillRect(0, Data_Screen.height - borderBottomHeight, Data_Screen.width, borderBottomHeight, Color_Black);
}

static void drawButtons()
{
	buttonBuildExpanded[12].enabled = Data_State.undoReady && Data_State.undoAvailable;
	if (Data_State.sidebarCollapsed) {
		int xOffset = Data_Screen.width - SIDEBAR_BORDER - 42;
		Widget_Button_drawImageButtons(xOffset, 24, buttonExpandSidebar, 1);
		Widget_Button_drawImageButtons(xOffset, 24, buttonBuildCollapsed, 12);
	} else {
		int xOffset = XOFFSET_EXPANDED;
		Widget_Button_drawImageButtons(xOffset, 24, buttonOverlaysCollapseSidebar, 2);
		Widget_Button_drawImageButtons(xOffset, 24, buttonBuildExpanded, 15);
		Widget_Button_drawImageButtons(xOffset, 24, buttonTopExpanded, 6);
	}
}

static void drawOverlayText(int xOffset)
{
	if (!Data_State.sidebarCollapsed) {
		if (Data_State.currentOverlay) {
			Widget_GameText_drawCentered(14, Data_State.currentOverlay, xOffset, 32, 117, Font_NormalGreen);
		} else {
			Widget_GameText_drawCentered(6, 4, xOffset, 32, 117, Font_NormalGreen);
		}
	}
}

static void drawMinimap(int force)
{
	if (!Data_State.sidebarCollapsed) {
		if (minimapRedrawRequested || Data_State.isScrollingMap || force) {
			int xOffset = XOFFSET_EXPANDED;
			UI_Minimap_draw(xOffset + 8, 59, 73, 111);
			Graphics_drawLine(xOffset + 7, 58, xOffset + 153, 58, Color_Minimap_Dark);
			Graphics_drawLine(xOffset + 7, 59, xOffset + 7, 170, Color_Minimap_Dark);
			Graphics_drawLine(xOffset + 153, 59, xOffset + 153, 170, Color_Minimap_Light);
		}
	}
}

void UI_Sidebar_handleMouse()
{
	if (Data_State.sidebarCollapsed) {
		int xOffset = Data_Screen.width - SIDEBAR_BORDER - 42;
		if (!Widget_Button_handleImageButtons(xOffset, 24, buttonExpandSidebar, 1)) {
			Widget_Button_handleImageButtons(xOffset, 24, buttonBuildCollapsed, 12);
		}
	} else {
		int xOffset = XOFFSET_EXPANDED;
		if (!Widget_Button_handleImageButtons(xOffset, 24, buttonOverlaysCollapseSidebar, 2)) {
			if (!Widget_Button_handleImageButtons(xOffset, 24, buttonBuildExpanded, 15)) {
				Widget_Button_handleImageButtons(xOffset, 24, buttonTopExpanded, 6);
			}
		}
	}
}

void UI_Sidebar_handleMouseBuildButtons()
{
	if (Data_State.sidebarCollapsed) {
		int xOffset = Data_Screen.width - SIDEBAR_BORDER - 42;
		Widget_Button_handleImageButtonsClickOnly(xOffset, 24, buttonBuildCollapsed, 12);
	} else {
		int xOffset = XOFFSET_EXPANDED;
		Widget_Button_handleImageButtonsClickOnly(xOffset, 24, buttonBuildExpanded, 15);
	}
}

static void buttonOverlay(int param1, int param2)
{
	UI_Window_goTo(Window_OverlayMenu);
}

static void buttonCollapseExpand(int param1, int param2)
{
	data.progress = 0;
	data.slideStart = Time_getMillis();
	UI_Window_goTo(Window_SlidingSidebar);
	CityView_setViewportWithoutSidebar();
	CityView_checkCameraBoundaries();
	Sound_Effects_playChannel(SoundChannel_Sidebar);
}

static void buttonBuild(int submenu, int param2)
{
	UI_BuildingMenu_init(submenu);
}

static void buttonUndo(int param1, int param2)
{
	Undo_perform();
	UI_Window_requestRefresh();
}

static void buttonMessages(int param1, int param2)
{
	UI_Window_goTo(Window_PlayerMessageList);
}

static void buttonHelp(int param1, int param2)
{
	UI_MessageDialog_show(param2, 0);
}

static void buttonGoToProblem(int param1, int param2)
{
	// TODO
}
static void buttonAdvisors(int param1, int param2)
{
	UI_Advisors_goToFromSidepanel(lastAdvisor);
}

static void buttonEmpire(int param1, int param2)
{
	// TODO
	UI_Window_goTo(Window_Empire);
}
static void buttonMissionBriefing(int param1, int param2)
{
	if (!Data_Settings.isCustomScenario) {
		UI_Intermezzo_show(0, Window_MissionBriefingReview, 1000);
	}
}

static void buttonRotateNorth(int param1, int param2)
{
	switch (Data_Settings_Map.orientation) {
		case 0: // already north
			return;
		case 2:
			CityView_rotateRight();
			Terrain_rotateMap(1);
			break;
		case 4:
			CityView_rotateLeft();
			Terrain_rotateMap(0);
			// fallthrough
		case 6:
			CityView_rotateLeft();
			Terrain_rotateMap(0);
			break;
	}
	CityView_checkCameraBoundaries();
	UI_Warning_show(Warning_Orientation);
	UI_Window_requestRefresh();
}

static void buttonRotate(int clockWise, int param2)
{
	if (clockWise) {
		CityView_rotateRight();
	} else {
		CityView_rotateLeft();
	}
	Terrain_rotateMap(clockWise);
	CityView_checkCameraBoundaries();
	UI_Warning_show(Warning_Orientation);
	UI_Window_requestRefresh();
}


static void updateProgress()
{
	TimeMillis now = Time_getMillis();
	TimeMillis diff = now - data.slideStart;
	data.progress = diff / 10;
}

void UI_SlidingSidebar_drawBackground()
{
	UI_City_drawCity();
}

void UI_SlidingSidebar_drawForeground()
{
	updateProgress();
	if (data.progress >= 47) {
		if (Data_State.sidebarCollapsed) {
			Data_State.sidebarCollapsed = 0;
			CityView_setViewportWithSidebar();
		} else {
			Data_State.sidebarCollapsed = 1;
			CityView_setViewportWithoutSidebar();
		}
		CityView_checkCameraBoundaries();
		UI_Window_goTo(Window_City);
		UI_Window_refresh(1);
		return;
	}

	Graphics_setClipRectangle(
		Data_Screen.width - SIDEBAR_BORDER - 162, 24,
		162, Data_Screen.height - 24 - BOTTOM_BORDER);

	int graphicBase = GraphicId(ID_Graphic_SidePanel);
	// draw collapsed sidebar
	int xOffsetCollapsed = Data_Screen.width - SIDEBAR_BORDER - 42;
	Graphics_drawImage(graphicBase, xOffsetCollapsed, 24);
	Widget_Button_drawImageButtons(xOffsetCollapsed, 24, buttonExpandSidebar, 1);
	Widget_Button_drawImageButtons(xOffsetCollapsed, 24, buttonBuildCollapsed, 12);

	// draw expanded sidebar on top of it
	int xOffsetExpanded = XOFFSET_EXPANDED;
	if (Data_State.sidebarCollapsed) {
		xOffsetExpanded += progressToOffset[47 - data.progress];
	} else {
		xOffsetExpanded += progressToOffset[data.progress];
	}
	Graphics_drawImage(graphicBase + 1, xOffsetExpanded, 24);
	Widget_Button_drawImageButtons(xOffsetExpanded, 24, buttonOverlaysCollapseSidebar, 2);
	Widget_Button_drawImageButtons(xOffsetExpanded, 24, buttonBuildExpanded, 15);
	Widget_Button_drawImageButtons(xOffsetExpanded, 24, buttonTopExpanded, 6);

	// black out minimap
	Graphics_fillRect(xOffsetExpanded + 8, 59, 145, 111, Color_Black);

	drawOverlayText(xOffsetExpanded + 4);
	UI_BuildingMenu_drawSidebarImage(xOffsetExpanded + 6, 1);

	// relief images below buttons
	int yOffset = 474;
	while (Data_Screen.width - yOffset > 0) {
		if (Data_Screen.width - yOffset <= 120) {
			Graphics_drawImage(graphicBase + 3, xOffsetCollapsed, yOffset);
			Graphics_drawImage(graphicBase + 2, xOffsetExpanded, yOffset);
			yOffset += 120;
		} else {
			Graphics_drawImage(graphicBase + 5, xOffsetCollapsed, yOffset);
			Graphics_drawImage(graphicBase + 4, xOffsetExpanded, yOffset);
			yOffset += 285;
		}
	}
	Graphics_resetClipRectangle();
}

