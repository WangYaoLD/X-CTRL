#include "FileGroup.h"
#include "FileSystem.h"
#include "GUI_Private.h"
#include "LuaCallCpp.h"
#include "LuaGroup.h"

/*Lua�ļ�*/
static File LuaFile;

/*Lua�ļ�·��*/
String LuaFilePath;

static void ScreenPrint(const char* s)
{
    TextSetDefault();
    if(screen.getCursorY() > screen.height() - TEXT_HEIGHT_1 || screen.getCursorY() < TEXT_HEIGHT_1)
    {
        ClearDisplay();
        screen.setCursor(0, TEXT_HEIGHT_1);
    }
    screen.print(s);
}

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    State_StatusBarEnable = false;
    ClearDisplay();
    TextSetDefault();
    screen.setCursor(0, TEXT_HEIGHT_1);
    screen.print("Loading ");
    screen.print(LuaFilePath);
    screen.println("...");
    
    LuaFile = SD.open(LuaFilePath);
    if(!LuaFile)
    {
        page.PageChangeTo(PAGE_FileExplorer);
        return;
    }
    LuaFile.setTimeout(20);

    Is_LuaError = false;
    Is_LuaRunning = false;

    L = luaL_newstate();
    luaL_openlibs(L);
    LuaReg_Output(ScreenPrint);

    LuaReg_Com();
    LuaReg_GPIO();
    LuaReg_GUI();
    LuaReg_ModuleCtrl();
    LuaReg_MouseKeyboard();
    LuaReg_Time();

//    luaL_dostring(L, "print(\"> Version:\",_VERSION)");
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    if(LuaFile.available())
    {
        String UserInput = LuaFile.readString();
        const char *LuaCode_buffer = UserInput.c_str();
        
        screen.println("\r\n" + UserInput);
        PageDelay(500);
        
        Is_LuaRunning = true;
        Is_LuaError = luaL_dostring(L, LuaCode_buffer);
        Is_LuaRunning = false;
        page.PageChangeTo(PAGE_FileExplorer);
    }
    if (Is_LuaError)
    {
        screen.println(lua_tostring(L, -1));
        Is_LuaError = false;
        Is_LuaRunning = false;
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    LuaFile.close();
    lua_close(L);
    ClearDisplay();
    Init_StatusBar();
}

/************ �¼����� **********/
/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{ 
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonLongPressEvent()
{
    if(btBACK)
    {
        if(Is_LuaRunning)
        {
            Lua_ForceToBreak();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_LuaScript(uint8_t ThisPage)
{
    /*����*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);

    /*�¼�*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}