#include "cbase.h"
#include "hud_menu_static.h"
#include "mom_player_shared.h"

#include "tier0/memdbgon.h"

static void OnRulerMenuClose()
{
    engine->ExecuteClientCmd("mom_ruler_close");
}

static void OnSavelocMenuClose()
{
    engine->ExecuteClientCmd("mom_saveloc_close");
}

static void ShowMenu(const char *filename, void (*ClosFunc)())
{
    CHudMenuStatic *pMenu = GET_HUDELEMENT(CHudMenuStatic);
    if (pMenu && engine->IsInGame())
    {
        if (pMenu->IsMenuDisplayed())
        {
            pMenu->HideMenu(); // NOTE: if another menu is open this will close it!
        }
        else
        {
            pMenu->ShowMenu(filename, ClosFunc);
        }
    }
}

CON_COMMAND(showRuler, "Opens the ruler tool.\n")
{
    ShowMenu("rulermenu", OnRulerMenuClose);
}

CON_COMMAND(mom_saveloc_show, "Opens the saved locations menu.\n")
{
    ShowMenu("savelocmenu", OnSavelocMenuClose);
}

CON_COMMAND(mom_mapcmds_menu_show, "Opens the map commands menu.\n")
{
    ShowMenu("mapcmdsmenu", nullptr);
}

CON_COMMAND(mom_custom_menu_show, "Opens the user-customized commands menu. See cfg/custommenu for configuration!\n")
{
    ShowMenu("custommenu", nullptr);
}
