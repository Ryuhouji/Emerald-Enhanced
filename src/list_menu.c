#include "global.h"
#include "menu.h"
#include "list_menu.h"
#include "window.h"
#include "text_window.h"
#include "main.h"
#include "task.h"
#include "trig.h"
#include "decompress.h"
#include "palette.h"
#include "malloc.h"
#include "strings.h"
#include "sound.h"
#include "constants/songs.h"
#include "event_data.h"
#include "string_util.h"
#include "item.h"
#include "lifeskill.h"
#include "constants/items.h"
#include "ryu_challenge_modifiers.h"

struct UnkIndicatorsStruct
{
    u8 field_0;
    u16 *field_4;
    u16 field_8;
    u16 field_A;
    u16 field_C;
    u16 field_E;
    u8 field_10;
    u8 field_11;
    u8 field_12;
    u8 field_13;
    u8 field_14_0:4;
    u8 field_14_1:4;
    u8 field_15_0:4;
    u8 field_15_1:4;
    u8 field_16_0:3;
    u8 field_16_1:3;
    u8 field_16_2:2;
    u8 field_17_0:6;
    u8 field_17_1:2;
};

struct ScrollIndicatorPair
{
    u8 field_0;
    u16 *scrollOffset;
    u16 fullyUpThreshold;
    u16 fullyDownThreshold;
    u8 topSpriteId;
    u8 bottomSpriteId;
    u16 tileTag;
    u16 palTag;
};

struct RedOutlineCursor
{
    struct SubspriteTable subspriteTable;
    struct Subsprite *subspritesPtr; // not a const pointer
    u8 spriteId;
    u16 tileTag;
    u16 palTag;
};

struct RedArrowCursor
{
    u8 spriteId;
    u16 tileTag;
    u16 palTag;
};

// this file's functions
static u8 ListMenuInitInternal(struct ListMenuTemplate *listMenuTemplate, u16 scrollOffset, u16 selectedRow);
static bool8 ListMenuChangeSelection(struct ListMenu *list, bool8 updateCursorAndCallCallback, u8 count, bool8 movingDown);
static void ListMenuPrintEntries(struct ListMenu *list, u16 startIndex, u16 yOffset, u16 count);
static void ListMenuDrawCursor(struct ListMenu *list);
static void ListMenuCallSelectionChangedCallback(struct ListMenu *list, u8 onInit);
static u8 ListMenuAddCursorObject(struct ListMenu *list, u32 cursorKind);
static void Task_ScrollIndicatorArrowPair(u8 taskId);
static u8 ListMenuAddRedOutlineCursorObject(struct CursorStruct *cursor);
static u8 ListMenuAddRedArrowCursorObject(struct CursorStruct *cursor);
static void ListMenuUpdateRedOutlineCursorObject(u8 taskId, u16 x, u16 y);
static void ListMenuUpdateRedArrowCursorObject(u8 taskId, u16 x, u16 y);
static void ListMenuRemoveRedOutlineCursorObject(u8 taskId);
static void ListMenuRemoveRedArrowCursorObject(u8 taskId);
static u8 ListMenuAddCursorObjectInternal(struct CursorStruct *cursor, u32 cursorKind);
static void ListMenuUpdateCursorObject(u8 taskId, u16 x, u16 y, u32 cursorKind);
static void ListMenuRemoveCursorObject(u8 taskId, u32 cursorKind);
static void SpriteCallback_ScrollIndicatorArrow(struct Sprite *sprite);
static void SpriteCallback_RedArrowCursor(struct Sprite *sprite);

// EWRAM vars
static EWRAM_DATA struct {
    s32 currItemId;
    u8 state;
    u8 windowId;
    u8 listTaskId;
} sMysteryGiftLinkMenu = {0};

EWRAM_DATA struct ScrollArrowsTemplate gTempScrollArrowTemplate = {0};

// IWRAM common
struct {
    u8 cursorPal:4;
    u8 fillValue:4;
    u8 cursorShadowPal:4;
    u8 lettersSpacing:6;
    u8 field_2_2:6; // unused
    u8 fontId:7;
    bool8 enabled:1;
} gListMenuOverride;

struct ListMenuTemplate gMultiuseListMenuTemplate;

// const rom data
static const struct
{
    u8 animNum:4;
    u8 bounceDir:4;
    u8 multiplier;
    u16 frequency;
} sScrollIndicatorTemplates[] =
{
    {0, 0, 2, 8},
    {1, 0, 2, -8},
    {2, 1, 2, 8},
    {3, 1, 2, -8},
};

static const struct OamData sOamData_ScrollArrowIndicator =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_ScrollArrowIndicator0[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_ScrollArrowIndicator1[] =
{
    ANIMCMD_FRAME(0, 30, 1, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_ScrollArrowIndicator2[] =
{
    ANIMCMD_FRAME(4, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_ScrollArrowIndicator3[] =
{
    ANIMCMD_FRAME(4, 30, 0, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_ScrollArrowIndicator[] =
{
    sSpriteAnim_ScrollArrowIndicator0,
    sSpriteAnim_ScrollArrowIndicator1,
    sSpriteAnim_ScrollArrowIndicator2,
    sSpriteAnim_ScrollArrowIndicator3
};

static const struct SpriteTemplate sSpriteTemplate_ScrollArrowIndicator =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOamData_ScrollArrowIndicator,
    .anims = sSpriteAnimTable_ScrollArrowIndicator,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallback_ScrollIndicatorArrow,
};

static const struct Subsprite sSubsprite_RedOutline1 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 0,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline2 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 1,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline3 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 2,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline4 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 3,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline5 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 4,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline6 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 5,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline7 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 6,
    .priority = 0,
};

static const struct Subsprite sSubsprite_RedOutline8 =
{
    .x = 0,
    .y = 0,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .tileOffset = 7,
    .priority = 0,
};

static const struct OamData sOamData_RedArrowCursor =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_RedArrowCursor[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_RedArrowCursor[] =
{
    sSpriteAnim_RedArrowCursor
};

static const struct SpriteTemplate sSpriteTemplate_RedArrowCursor =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOamData_RedArrowCursor,
    .anims = sSpriteAnimTable_RedArrowCursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallback_RedArrowCursor,
};

static const u16 sRedArrowPal[] = INCBIN_U16("graphics/interface/red_arrow.gbapal");
static const u32 sRedArrowOtherGfx[] = INCBIN_U32("graphics/interface/red_arrow_other.4bpp.lz");
static const u32 sSelectorOutlineGfx[] = INCBIN_U32("graphics/interface/selector_outline.4bpp.lz");
static const u32 sRedArrowGfx[] = INCBIN_U32("graphics/interface/red_arrow.4bpp.lz");


EWRAM_DATA static u8 sPrintRecipeWindowId = 0;
EWRAM_DATA static u8 sPrintAlchemyMetalWindowId = 0;
EWRAM_DATA static u8 sBetaMenuInfoWindow = 0;
static void RyuShowRecipeInfoWindow(u16);

// code
static void ListMenuDummyTask(u8 taskId)
{

}

s32 DoMysteryGiftListMenu(const struct WindowTemplate *windowTemplate, const struct ListMenuTemplate *listMenuTemplate, u8 arg2, u16 tileNum, u16 palNum)
{
    switch (sMysteryGiftLinkMenu.state)
    {
    case 0:
    default:
        sMysteryGiftLinkMenu.windowId = AddWindow(windowTemplate);
        switch (arg2)
        {
        case 2:
            LoadUserWindowBorderGfx(sMysteryGiftLinkMenu.windowId, tileNum, palNum);
        case 1:
            DrawTextBorderOuter(sMysteryGiftLinkMenu.windowId, tileNum, palNum / 16);
            break;
        }
        gMultiuseListMenuTemplate = *listMenuTemplate;
        gMultiuseListMenuTemplate.windowId = sMysteryGiftLinkMenu.windowId;
        sMysteryGiftLinkMenu.listTaskId = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
        CopyWindowToVram(sMysteryGiftLinkMenu.windowId, 1);
        sMysteryGiftLinkMenu.state = 1;
        break;
    case 1:
        sMysteryGiftLinkMenu.currItemId = ListMenu_ProcessInput(sMysteryGiftLinkMenu.listTaskId);
        if (JOY_NEW(A_BUTTON))
        {
            sMysteryGiftLinkMenu.state = 2;
        }
        if (JOY_NEW(B_BUTTON))
        {
            sMysteryGiftLinkMenu.currItemId = LIST_CANCEL;
            sMysteryGiftLinkMenu.state = 2;
        }
        if (sMysteryGiftLinkMenu.state == 2)
        {
            if (arg2 == 0)
            {
                ClearWindowTilemap(sMysteryGiftLinkMenu.windowId);
            }
            else
            {
                switch (arg2)
                {
                case 0: // can never be reached, because of the if statement above
                    ClearStdWindowAndFrame(sMysteryGiftLinkMenu.windowId, FALSE);
                    break;
                case 2:
                case 1:
                    ClearStdWindowAndFrame(sMysteryGiftLinkMenu.windowId, FALSE);
                    break;
                }
            }
            ClearStdWindowAndFrame(sPrintRecipeWindowId, TRUE);
            CopyWindowToVram(sMysteryGiftLinkMenu.windowId, 1);
        }
        break;
    case 2:
        DestroyListMenuTask(sMysteryGiftLinkMenu.listTaskId, NULL, NULL);
        RemoveWindow(sMysteryGiftLinkMenu.windowId);
        RemoveWindow(sPrintRecipeWindowId);
        sMysteryGiftLinkMenu.state = 0;
        return sMysteryGiftLinkMenu.currItemId;
    }

    return LIST_NOTHING_CHOSEN;
}

u8 ListMenuInit(struct ListMenuTemplate *listMenuTemplate, u16 scrollOffset, u16 selectedRow)
{
    u8 taskId = ListMenuInitInternal(listMenuTemplate, scrollOffset, selectedRow);
    PutWindowTilemap(listMenuTemplate->windowId);
    CopyWindowToVram(listMenuTemplate->windowId, 2);

    return taskId;
}

// unused
u8 ListMenuInitInRect(struct ListMenuTemplate *listMenuTemplate, struct ListMenuWindowRect *rect, u16 scrollOffset, u16 selectedRow)
{
    s32 i;

    u8 taskId = ListMenuInitInternal(listMenuTemplate, scrollOffset, selectedRow);
    for (i = 0; rect[i].palNum != 0xFF; i++)
    {
        PutWindowRectTilemapOverridePalette(listMenuTemplate->windowId,
                                            rect[i].x,
                                            rect[i].y,
                                            rect[i].width,
                                            rect[i].height,
                                            rect[i].palNum);
    }
    CopyWindowToVram(listMenuTemplate->windowId, 2);

    return taskId;
}

s32 ListMenu_ProcessInput(u8 listTaskId)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    if (JOY_NEW(A_BUTTON))
    {
        if(sPrintRecipeWindowId != 0xFF)
        {
            ClearStdWindowAndFrame(sPrintRecipeWindowId, TRUE);
            RemoveWindow(sPrintRecipeWindowId);
            sPrintRecipeWindowId = 0xFF;
        }
        if(sPrintAlchemyMetalWindowId != 0xFF)
        {
            ClearStdWindowAndFrame(sPrintAlchemyMetalWindowId, TRUE);
            RemoveWindow(sPrintAlchemyMetalWindowId);
            sPrintAlchemyMetalWindowId = 0xFF;
        }
        return list->template.items[list->scrollOffset + list->selectedRow].id;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        if(sPrintRecipeWindowId != 0xFF)
        {
            ClearStdWindowAndFrame(sPrintRecipeWindowId, TRUE);
            RemoveWindow(sPrintRecipeWindowId);
            sPrintRecipeWindowId = 0xFF;
        }
        if(sPrintAlchemyMetalWindowId != 0xFF)
        {
            ClearStdWindowAndFrame(sPrintAlchemyMetalWindowId, TRUE);
            RemoveWindow(sPrintAlchemyMetalWindowId);
            sPrintAlchemyMetalWindowId = 0xFF;
        }
        return LIST_CANCEL;
    }
    else if (JOY_REPEAT(DPAD_UP))
    {
        ListMenuChangeSelection(list, TRUE, 1, FALSE);
        return LIST_NOTHING_CHOSEN;
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        ListMenuChangeSelection(list, TRUE, 1, TRUE);
        return LIST_NOTHING_CHOSEN;
    }
    else // try to move by one window scroll
    {
        bool16 rightButton, leftButton;
        switch (list->template.scrollMultiple)
        {
        case LIST_NO_MULTIPLE_SCROLL:
        default:
            leftButton = FALSE;
            rightButton = FALSE;
            break;
        case LIST_MULTIPLE_SCROLL_DPAD:
            // note: JOY_REPEAT won't match here
            leftButton = gMain.newAndRepeatedKeys & DPAD_LEFT;
            rightButton = gMain.newAndRepeatedKeys & DPAD_RIGHT;
            break;
        case LIST_MULTIPLE_SCROLL_L_R:
            // same as above
            leftButton = gMain.newAndRepeatedKeys & L_BUTTON;
            rightButton = gMain.newAndRepeatedKeys & R_BUTTON;
            break;
        }

        if (leftButton)
        {
            ListMenuChangeSelection(list, TRUE, list->template.maxShowed, FALSE);
            return LIST_NOTHING_CHOSEN;
        }
        else if (rightButton)
        {
            ListMenuChangeSelection(list, TRUE, list->template.maxShowed, TRUE);
            return LIST_NOTHING_CHOSEN;
        }
        else
        {
            return LIST_NOTHING_CHOSEN;
        }
    }
}

#define TASK_NONE 0xFF

void DestroyListMenuTask(u8 listTaskId, u16 *scrollOffset, u16 *selectedRow)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    if (scrollOffset != NULL)
        *scrollOffset = list->scrollOffset;
    if (selectedRow != NULL)
        *selectedRow = list->selectedRow;

    if (list->taskId != TASK_NONE)
        ListMenuRemoveCursorObject(list->taskId, list->template.cursorKind - 2);

    DestroyTask(listTaskId);
}

void RedrawListMenu(u8 listTaskId)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    FillWindowPixelBuffer(list->template.windowId, PIXEL_FILL(list->template.fillValue));
    ListMenuPrintEntries(list, list->scrollOffset, 0, list->template.maxShowed);
    ListMenuDrawCursor(list);
    CopyWindowToVram(list->template.windowId, 2);
}

// unused
void ChangeListMenuPals(u8 listTaskId, u8 cursorPal, u8 fillValue, u8 cursorShadowPal)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    list->template.cursorPal = cursorPal;
    list->template.fillValue = fillValue;
    list->template.cursorShadowPal = cursorShadowPal;
}

// unused
void ChangeListMenuCoords(u8 listTaskId, u8 x, u8 y)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    SetWindowAttribute(list->template.windowId, WINDOW_TILEMAP_LEFT, x);
    SetWindowAttribute(list->template.windowId, WINDOW_TILEMAP_TOP, y);
}

// unused
s32 ListMenuTestInput(struct ListMenuTemplate *template, u32 scrollOffset, u32 selectedRow, u16 keys, u16 *newScrollOffset, u16 *newSelectedRow)
{
    struct ListMenu list;

    list.template = *template;
    list.scrollOffset = scrollOffset;
    list.selectedRow = selectedRow;
    list.unk_1C = 0;
    list.unk_1D = 0;

    if (keys == DPAD_UP)
        ListMenuChangeSelection(&list, FALSE, 1, FALSE);
    if (keys == DPAD_DOWN)
        ListMenuChangeSelection(&list, FALSE, 1, TRUE);

    if (newScrollOffset != NULL)
        *newScrollOffset = list.scrollOffset;
    if (newSelectedRow != NULL)
        *newSelectedRow = list.selectedRow;

    return LIST_NOTHING_CHOSEN;
}

void ListMenuGetCurrentItemArrayId(u8 listTaskId, u16 *arrayId)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    if (arrayId != NULL)
        *arrayId = list->scrollOffset + list->selectedRow;
}

void ListMenuGetScrollAndRow(u8 listTaskId, u16 *scrollOffset, u16 *selectedRow)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    if (scrollOffset != NULL)
        *scrollOffset = list->scrollOffset;
    if (selectedRow != NULL)
        *selectedRow = list->selectedRow;
}

u16 ListMenuGetYCoordForPrintingArrowCursor(u8 listTaskId)
{
    struct ListMenu *list = (void*) gTasks[listTaskId].data;
    u8 yMultiplier = GetFontAttribute(list->template.fontId, FONTATTR_MAX_LETTER_HEIGHT) + list->template.itemVerticalPadding;

    return list->selectedRow * yMultiplier + list->template.upText_Y;
}

static u8 ListMenuInitInternal(struct ListMenuTemplate *listMenuTemplate, u16 scrollOffset, u16 selectedRow)
{
    u8 listTaskId = CreateTask(ListMenuDummyTask, 0);
    struct ListMenu *list = (void*) gTasks[listTaskId].data;

    list->template = *listMenuTemplate;
    list->scrollOffset = scrollOffset;
    list->selectedRow = selectedRow;
    list->unk_1C = 0;
    list->unk_1D = 0;
    list->taskId = TASK_NONE;
    list->unk_1F = 0;

    gListMenuOverride.cursorPal = list->template.cursorPal;
    gListMenuOverride.fillValue = list->template.fillValue;
    gListMenuOverride.cursorShadowPal = list->template.cursorShadowPal;
    gListMenuOverride.lettersSpacing = list->template.lettersSpacing;
    gListMenuOverride.fontId = list->template.fontId;
    gListMenuOverride.enabled = FALSE;

    if (list->template.totalItems < list->template.maxShowed)
        list->template.maxShowed = list->template.totalItems;

    FillWindowPixelBuffer(list->template.windowId, PIXEL_FILL(list->template.fillValue));
    ListMenuPrintEntries(list, list->scrollOffset, 0, list->template.maxShowed);
    ListMenuDrawCursor(list);
    ListMenuCallSelectionChangedCallback(list, TRUE);

    sPrintRecipeWindowId = 0xFF;
    sPrintAlchemyMetalWindowId = 0xFF;
    return listTaskId;
}

static void ListMenuPrint(struct ListMenu *list, const u8 *str, u8 x, u8 y)
{
    u8 colors[3];
    if (gListMenuOverride.enabled)
    {
        colors[0] = gListMenuOverride.fillValue;
        colors[1] = gListMenuOverride.cursorPal;
        colors[2] = gListMenuOverride.cursorShadowPal;
        AddTextPrinterParameterized4(list->template.windowId,
                                     gListMenuOverride.fontId,
                                     x, y,
                                     gListMenuOverride.lettersSpacing,
                                     0, colors, TEXT_SPEED_FF, str);

        gListMenuOverride.enabled = FALSE;
    }
    else
    {
        colors[0] = list->template.fillValue;
        colors[1] = list->template.cursorPal;
        colors[2] = list->template.cursorShadowPal;
        AddTextPrinterParameterized4(list->template.windowId,
                                     list->template.fontId,
                                     x, y,
                                     list->template.lettersSpacing,
                                     0, colors, TEXT_SPEED_FF, str);
    }
}

static void ListMenuPrintEntries(struct ListMenu *list, u16 startIndex, u16 yOffset, u16 count)
{
    s32 i;
    u8 x, y;
    u8 yMultiplier = GetFontAttribute(list->template.fontId, FONTATTR_MAX_LETTER_HEIGHT) + list->template.itemVerticalPadding;

    for (i = 0; i < count; i++)
    {
        if (list->template.items[startIndex].id != LIST_HEADER)
            x = list->template.item_X;
        else
            x = list->template.header_X;

        y = (yOffset + i) * yMultiplier + list->template.upText_Y;
        if (list->template.itemPrintFunc != NULL)
            list->template.itemPrintFunc(list->template.windowId, list->template.items[startIndex].id, y);

        if (FlagGet(FLAG_RYU_SHOW_DIFFICULTY_MOD_MENU) == TRUE){
            if (GetModFlag(startIndex) == TRUE){
                StringCopy(gRyuStringVar4, ((const u8[])_("{COLOR LIGHT_GREEN}{SHADOW GREEN}")));
                StringAppend(gRyuStringVar4, list->template.items[startIndex].name);
                StringExpandPlaceholders(gStringVar4, gRyuStringVar4);
                ListMenuPrint(list, gStringVar4, x, y);
            }
            else{
                StringCopy(gRyuStringVar4, ((const u8[])_("{COLOR LIGHT_RED}{SHADOW RED}")));
                StringAppend(gRyuStringVar4, list->template.items[startIndex].name);
                StringExpandPlaceholders(gStringVar4, gRyuStringVar4);
                ListMenuPrint(list, gStringVar4, x, y);
            }
        }
        else{
            ListMenuPrint(list, list->template.items[startIndex].name, x, y);
        }
        startIndex++;
    }
}

static void ListMenuDrawCursor(struct ListMenu *list)
{
    u8 yMultiplier = GetFontAttribute(list->template.fontId, FONTATTR_MAX_LETTER_HEIGHT) + list->template.itemVerticalPadding;
    u8 x = list->template.cursor_X;
    u8 y = list->selectedRow * yMultiplier + list->template.upText_Y;
    switch (list->template.cursorKind)
    {
    case 0:
        ListMenuPrint(list, gText_SelectorArrow2, x, y);
        break;
    case 1:
        break;
    case 2:
        if (list->taskId == TASK_NONE)
            list->taskId = ListMenuAddCursorObject(list, 0);
        ListMenuUpdateCursorObject(list->taskId,
                                   GetWindowAttribute(list->template.windowId, WINDOW_TILEMAP_LEFT) * 8 - 1,
                                   GetWindowAttribute(list->template.windowId, WINDOW_TILEMAP_TOP) * 8 + y - 1, 0);
        break;
    case 3:
        if (list->taskId == TASK_NONE)
            list->taskId = ListMenuAddCursorObject(list, 1);
        ListMenuUpdateCursorObject(list->taskId,
                                   GetWindowAttribute(list->template.windowId, WINDOW_TILEMAP_LEFT) * 8 + x,
                                   GetWindowAttribute(list->template.windowId, WINDOW_TILEMAP_TOP) * 8 + y, 1);
        break;
    }
}

#undef TASK_NONE

static u8 ListMenuAddCursorObject(struct ListMenu *list, u32 cursorKind)
{
    struct CursorStruct cursor;

    cursor.left = 0;
    cursor.top = 160;
    cursor.rowWidth = GetWindowAttribute(list->template.windowId, WINDOW_WIDTH) * 8 + 2;
    cursor.rowHeight = GetFontAttribute(list->template.fontId, FONTATTR_MAX_LETTER_HEIGHT) + 2;
    cursor.tileTag = 0x4000;
    cursor.palTag = SPRITE_INVALID_TAG;
    cursor.palNum = 15;

    return ListMenuAddCursorObjectInternal(&cursor, cursorKind);
}

static void ListMenuErasePrintedCursor(struct ListMenu *list, u16 selectedRow)
{
    u8 cursorKind = list->template.cursorKind;
    if (cursorKind == 0)
    {
        u8 yMultiplier = GetFontAttribute(list->template.fontId, FONTATTR_MAX_LETTER_HEIGHT) + list->template.itemVerticalPadding;
        u8 width  = GetMenuCursorDimensionByFont(list->template.fontId, 0);
        u8 height = GetMenuCursorDimensionByFont(list->template.fontId, 1);
        FillWindowPixelRect(list->template.windowId,
                            PIXEL_FILL(list->template.fillValue),
                            list->template.cursor_X,
                            selectedRow * yMultiplier + list->template.upText_Y,
                            width,
                            height);
    }
}

static u8 ListMenuUpdateSelectedRowIndexAndScrollOffset(struct ListMenu *list, bool8 movingDown)
{
    u16 selectedRow = list->selectedRow;
    u16 scrollOffset = list->scrollOffset;
    u16 newRow;
    u32 newScroll;

    if (!movingDown)
    {
        if (list->template.maxShowed == 1)
            newRow = 0;
        else
            newRow = list->template.maxShowed - ((list->template.maxShowed / 2) + (list->template.maxShowed % 2)) - 1;

        if (scrollOffset == 0)
        {
            while (selectedRow != 0)
            {
                selectedRow--;
                if (list->template.items[scrollOffset + selectedRow].id != LIST_HEADER)
                {
                    list->selectedRow = selectedRow;
                    return 1;
                }
            }

            return 0;
        }
        else
        {
            while (selectedRow > newRow)
            {
                selectedRow--;
                if (list->template.items[scrollOffset + selectedRow].id != LIST_HEADER)
                {
                    list->selectedRow = selectedRow;
                    return 1;
                }
            }

            newScroll = scrollOffset - 1;
        }
    }
    else
    {
        if (list->template.maxShowed == 1)
            newRow = 0;
        else
            newRow = ((list->template.maxShowed / 2) + (list->template.maxShowed % 2));

        if (scrollOffset == list->template.totalItems - list->template.maxShowed)
        {
            while (selectedRow < list->template.maxShowed - 1)
            {
                selectedRow++;
                if (list->template.items[scrollOffset + selectedRow].id != LIST_HEADER)
                {
                    list->selectedRow = selectedRow;
                    return 1;
                }
            }

            return 0;
        }
        else
        {
            while (selectedRow < newRow)
            {
                selectedRow++;
                if (list->template.items[scrollOffset + selectedRow].id != LIST_HEADER)
                {
                    list->selectedRow = selectedRow;
                    return 1;
                }
            }

            newScroll = scrollOffset + 1;
        }
    }

    list->selectedRow = newRow;
    list->scrollOffset = newScroll;
    return 2;
}

static void ListMenuScroll(struct ListMenu *list, u8 count, bool8 movingDown)
{
    if (count >= list->template.maxShowed)
    {
        FillWindowPixelBuffer(list->template.windowId, PIXEL_FILL(list->template.fillValue));
        ListMenuPrintEntries(list, list->scrollOffset, 0, list->template.maxShowed);
    }
    else
    {
        u8 yMultiplier = GetFontAttribute(list->template.fontId, FONTATTR_MAX_LETTER_HEIGHT) + list->template.itemVerticalPadding;

        if (!movingDown)
        {
            u16 y, width, height;

            ScrollWindow(list->template.windowId, 1, count * yMultiplier, PIXEL_FILL(list->template.fillValue));
            ListMenuPrintEntries(list, list->scrollOffset, 0, count);

            y = (list->template.maxShowed * yMultiplier) + list->template.upText_Y;
            width = GetWindowAttribute(list->template.windowId, WINDOW_WIDTH) * 8;
            height = (GetWindowAttribute(list->template.windowId, WINDOW_HEIGHT) * 8) - y;
            FillWindowPixelRect(list->template.windowId,
                                PIXEL_FILL(list->template.fillValue),
                                0, y, width, height);
        }
        else
        {
            u16 width;

            ScrollWindow(list->template.windowId, 0, count * yMultiplier, PIXEL_FILL(list->template.fillValue));
            ListMenuPrintEntries(list, list->scrollOffset + (list->template.maxShowed - count), list->template.maxShowed - count, count);

            width = GetWindowAttribute(list->template.windowId, WINDOW_WIDTH) * 8;
            FillWindowPixelRect(list->template.windowId,
                                PIXEL_FILL(list->template.fillValue),
                                0, 0, width, list->template.upText_Y);
        }
    }
}

void RyuShowRecipeInfoWindow(u16 selection)
{
    struct WindowTemplate template;
    u32 i = 0;
    u16 group = VarGet(VAR_TEMP_D);

    if (group == 1)
        {
            selection = (selection + NUM_CONSUMABLE_RECIPES);
        }
    else if (group == 2)
        {
            selection = (selection + NUM_CONSUMABLE_RECIPES + NUM_MEDICINE_RECIPES);
        }
    if(sPrintRecipeWindowId == 0xFF)
    {
        SetWindowTemplateFields(&template, 0, 16, 5, 12, 8, 15, 1);
        sPrintRecipeWindowId = AddWindow(&template);
        DrawStdFrameWithCustomTileAndPalette(sPrintRecipeWindowId, TRUE, 0x214, 14);
    }
    FillWindowPixelBuffer(sPrintRecipeWindowId, 0x11);
    for(i = 0; i < NUM_INGREDIENTS_PER_RECIPE; i++)
    {
        static const u8 sIngredColors[][3] = {
            {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED},
            {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_GREEN, TEXT_COLOR_GREEN},
        };

        if(sBotanyRecipes[selection][i][0] != ITEM_NONE)
        {
            u8 * str;
            u32 itemCount = CountTotalItemQuantityInBag(sBotanyRecipes[selection][i][0]);
            u8 const * color = itemCount >= sBotanyRecipes[selection][i][1] ? sIngredColors[1] : sIngredColors[0];
            str = StringCopy(gStringVar1, ItemId_GetName(sBotanyRecipes[selection][i][0]));
            *str++ = CHAR_LEFT_PAREN;
            str = ConvertIntToDecimalStringN(str, sBotanyRecipes[selection][i][1], STR_CONV_MODE_LEADING_ZEROS, 1);
            *str++ = CHAR_RIGHT_PAREN;
            *str = EOS;
            AddTextPrinterParameterized3(sPrintRecipeWindowId, 0, 0, i * 12, color, 0xFF, gStringVar1);
        }
    }
    CopyWindowToVram(sPrintRecipeWindowId, 3);
}

static const u8 sMetalNames[][7] = {
    _("Copper"),
    _("Silver"),
    _("Gold"),
};

static const u16 sMetalVars[] = {
    VAR_RYU_ALCHEMY_COPPER,
    VAR_RYU_ALCHEMY_SILVER,
    VAR_RYU_ALCHEMY_GOLD,
};
void RyuShowAlchemyInfo(u16 selection)
{
    struct WindowTemplate template;
    u32 i = 0;

    if(sPrintAlchemyMetalWindowId == 0xFF)
    {
        SetWindowTemplateFields(&template, 0, 18, 1, 11, 5, 15, 1);
        sPrintAlchemyMetalWindowId = AddWindow(&template);
        DrawStdFrameWithCustomTileAndPalette(sPrintAlchemyMetalWindowId, FALSE, 0x214, 14);
    }
    if(sPrintRecipeWindowId == 0xFF)
    {
        SetWindowTemplateFields(&template, 0, 21, 8, 8, 5, 15, 56);
        sPrintRecipeWindowId = AddWindow(&template);
        DrawStdFrameWithCustomTileAndPalette(sPrintRecipeWindowId, FALSE, 0x214, 14);
    }
    FillWindowPixelBuffer(sPrintAlchemyMetalWindowId, 0x11);
    FillWindowPixelBuffer(sPrintRecipeWindowId, 0x11);
    selection++; // selection starts off with ALCHEMY_EFFECT_DAMAGE_BOOST_T1 as 0 but 0 is ALCHEMY_EFFECT_NONE
    for(i = 0; i < 3; i++)
    {
        static const u8 sIngredColors[][3] = {
            {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED},
            {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_GREEN, TEXT_COLOR_GREEN},
        };

        if(gAlchemyRecipes[selection].ingredients[i].itemId != ITEM_NONE)
        {
            u8 * str;
            u32 itemCount = CountTotalItemQuantityInBag(gAlchemyRecipes[selection].ingredients[i].itemId);
            u8 const * color = itemCount >= gAlchemyRecipes[selection].ingredients[i].quantity ? sIngredColors[1] : sIngredColors[0];
            str = StringCopy(gStringVar1, ItemId_GetName(gAlchemyRecipes[selection].ingredients[i].itemId));
            *str++ = CHAR_LEFT_PAREN;
            str = ConvertIntToDecimalStringN(str, gAlchemyRecipes[selection].ingredients[i].quantity, STR_CONV_MODE_LEADING_ZEROS, 2);
            *str++ = CHAR_RIGHT_PAREN;
            *str = EOS;
            AddTextPrinterParameterized3(sPrintAlchemyMetalWindowId, 0, 0, i * 12, color, 0xFF, gStringVar1);
        }  
    }
    for(i = 0; i < 3; i++)
    {
        static const u8 sIngredColors[][3] = {
            {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED},
            {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_GREEN, TEXT_COLOR_GREEN},
        };

        if(gAlchemyRecipes[selection].metal == i)
        {
            u8 * str;
            str = StringCopy(gStringVar1, sMetalNames[i]);
            *str++ = CHAR_LEFT_PAREN;
            str = ConvertIntToDecimalStringN(str, gAlchemyRecipes[selection].metalDustAmt, STR_CONV_MODE_LEADING_ZEROS, 3);
            *str++ = CHAR_RIGHT_PAREN;
            *str = EOS;
            AddTextPrinterParameterized3(sPrintRecipeWindowId, 0, 0, i * 12, VarGet(sMetalVars[i]) < gAlchemyRecipes[selection].metalDustAmt ? sIngredColors[0] : sIngredColors[1], 0xFF, gStringVar1);
        } 
        else
        {
            u8 * str;
            str = StringCopy(gStringVar1, sMetalNames[i]);
            *str++ = CHAR_LEFT_PAREN;
            *str++ = CHAR_0;
            *str++ = CHAR_0;
            *str++ = CHAR_0;
            //str = ConvertIntToDecimalStringN(str, gAlchemyRecipes[selection].metalDustAmt, STR_CONV_MODE_LEADING_ZEROS, 3);
            *str++ = CHAR_RIGHT_PAREN;
            *str = EOS;
            AddTextPrinterParameterized3(sPrintRecipeWindowId, 0, 0, i * 12, sIngredColors[1], 0xFF, gStringVar1);
        }
        
    }
    CopyWindowToVram(sPrintAlchemyMetalWindowId, 3);
    CopyWindowToVram(sPrintRecipeWindowId, 3);
}

const u8 RyuBetaMenuHelpStrings[13][75] = 
{
    _("Show bug report info"),
    _("Try fixing the waystone by\nchecking quest data."),
    _("Reset temporary battle effects like\nboss mechanics or autobattle."),
    _("Reset temporary cutscene data\nto fix potential soft locks."),
    _("Remove all badges so that they\ncan be obtained again"),
    _("Reset the data in GCMS to try\nagain."),
    _("Go to littleroot."),
    _("Reset the clock / time based events.\nWill take 24hours to finish."),
    _("Attempt to fill in missing dex\npages from the pokemon storage."),
    _("Try to reset the followers\nto their home location."),
    _("View unlocked tutorials."),
    _("View currently active effects\nand mods."),
    _("Exit the menu.")
};

const u8 RyuDifficultyModDescs[18][75] = 
{
    _("Capture one mon per route,\nDefeated mons die."),
    _("Select and use only Pokémon of the\nselected type."),
    _("Monotype challenge, but you can also\nonly use moves of selected type."),
    _("Random Status effects plague the\nteam while exploring."),
    _("Random Party member's moves PP\nreduced to 4 occasionally."),
    _("Random party members drop to 1 HP\noccasionally while exploring."),
    _("Random party members lose half\ntheir current hp occasionally."),
    _("Revelation Mode\nAll four horsemen effects above."),
    _("Pokemon don't have abilities,\nnatures, or held items."),
    _("Pokemon evolve 10 levels late."),
    _("Team Aether's Magnetosphere always\nactive."),
    _("Player's pokemon always move last."),
    _("Wild Pokemon caught with 0 IV.\nIncoming status chance is 100%"),
    _("Player's Pokemon don't evolve."),
    _("Trainers steal your items and most\nof your money when you pass out."),
    _("You can't use or learn moves greater\nthan 60BP"),
    _("Everything costs money.\nYou won't get free medical treatment."),
    _("Close Mod Selection."),
};

void RyuDrawBetaMenuHelpText(u16 selection)
{
    struct WindowTemplate template;
    void (*callback)(struct TextPrinterTemplate *, u16) = NULL;
    StringCopy(gStringVar1, RyuBetaMenuHelpStrings[selection]);
    if(sBetaMenuInfoWindow == 0xFF)
    {
        SetWindowTemplateFields(&template, 0, 0, 16, 12, 8, 15, 1);
        sBetaMenuInfoWindow = AddWindow(&template);
        DrawStdFrameWithCustomTileAndPalette(sBetaMenuInfoWindow, TRUE, 0x50C, 14);
    }
    FillWindowPixelBuffer(sBetaMenuInfoWindow, 0x11);
    AddTextPrinterParameterized(sBetaMenuInfoWindow, 1, gStringVar1, 0, 0, 0xFF, callback);
    CopyWindowToVram(sBetaMenuInfoWindow, 3);
}

void RyuDrawDifficultyModStrings(u16 selection)
{
    struct WindowTemplate template;
    void (*callback)(struct TextPrinterTemplate *, u16) = NULL;
    if (GetModFlag((u32)selection) == TRUE){
        StringCopy(gRyuStringVar1, ((const u8[])_("{COLOR LIGHT_GREEN}{SHADOW GREEN}")));
        StringExpandPlaceholders(gStringVar1, gRyuStringVar1);
    }
    else{
        StringCopy(gRyuStringVar1, ((const u8[])_("{COLOR LIGHT_RED}{SHADOW RED}")));
        StringExpandPlaceholders(gStringVar1, gRyuStringVar1);
    }
    StringAppend(gStringVar1, RyuDifficultyModDescs[selection]);
    if(sBetaMenuInfoWindow == 0xFF)
    {
        SetWindowTemplateFields(&template, 0, 0, 16, 12, 8, 15, 1);
        sBetaMenuInfoWindow = AddWindow(&template);
        DrawStdFrameWithCustomTileAndPalette(sBetaMenuInfoWindow, TRUE, 0x50C, 14);
    }
    FillWindowPixelBuffer(sBetaMenuInfoWindow, 0x11);
    AddTextPrinterParameterized(sBetaMenuInfoWindow, 1, gStringVar1, 0, 0, 0xFF, callback);
    CopyWindowToVram(sBetaMenuInfoWindow, 3);
}

static bool8 ListMenuChangeSelection(struct ListMenu *list, bool8 updateCursorAndCallCallback, u8 count, bool8 movingDown)
{
    u16 oldSelectedRow;
    u8 selectionChange, i, cursorCount;
    u16 currentSelection = 0;

    oldSelectedRow = list->selectedRow;
    cursorCount = 0;
    selectionChange = 0;
    for (i = 0; i < count; i++)
    {
        do
        {
            u8 ret = ListMenuUpdateSelectedRowIndexAndScrollOffset(list, movingDown);
            selectionChange |= ret;
            if (ret != 2)
                break;
            cursorCount++;
        } while (list->template.items[list->scrollOffset + list->selectedRow].id == LIST_HEADER);
    }

    currentSelection = (list->selectedRow + list->scrollOffset);
    //starting to get YanDev levels of complexity
    if (FlagGet(FLAG_RYU_DISPLAY_BOTANY_INGREDIENTS) == 1)
        RyuShowRecipeInfoWindow(currentSelection);
    else if (FlagGet(FLAG_RYU_DISPLAY_ALCHEMY_INGREDIENTS) == 1)
        RyuShowAlchemyInfo(currentSelection);
    else if (FlagGet(FLAG_RYU_BETA_MENU_OPEN) == 1)
        RyuDrawBetaMenuHelpText(currentSelection);
    else if (FlagGet(FLAG_RYU_SHOW_DIFFICULTY_MOD_MENU) == 1)
        RyuDrawDifficultyModStrings(currentSelection);
    
    if (updateCursorAndCallCallback)
    {
        switch (selectionChange)
        {
        case 0:
        default:
            return TRUE;
        case 1:
            ListMenuErasePrintedCursor(list, oldSelectedRow);
            ListMenuDrawCursor(list);
            ListMenuCallSelectionChangedCallback(list, FALSE);
            CopyWindowToVram(list->template.windowId, 2);
            break;
        case 2:
        case 3:
            ListMenuErasePrintedCursor(list, oldSelectedRow);
            ListMenuScroll(list, cursorCount, movingDown);
            ListMenuDrawCursor(list);
            ListMenuCallSelectionChangedCallback(list, FALSE);
            CopyWindowToVram(list->template.windowId, 2);
            break;
        }
    }

    return FALSE;
}

static void ListMenuCallSelectionChangedCallback(struct ListMenu *list, u8 onInit)
{
    if (list->template.moveCursorFunc != NULL)
        list->template.moveCursorFunc(list->template.items[list->scrollOffset + list->selectedRow].id, onInit, list);
}

// unused
void ListMenuOverrideSetColors(u8 cursorPal, u8 fillValue, u8 cursorShadowPal)
{
    gListMenuOverride.cursorPal = cursorPal;
    gListMenuOverride.fillValue = fillValue;
    gListMenuOverride.cursorShadowPal = cursorShadowPal;
    gListMenuOverride.enabled = TRUE;
}

void ListMenuDefaultCursorMoveFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (!onInit)
        PlaySE(SE_SELECT);
}

// unused
s32 ListMenuGetUnkIndicatorsStructFields(u8 taskId, u8 field)
{
    struct UnkIndicatorsStruct *data = (void*) gTasks[taskId].data;

    switch (field)
    {
    case 0:
    case 1:
        return (s32)(data->field_4);
    case 2:
        return data->field_C;
    case 3:
        return data->field_E;
    case 4:
        return data->field_10;
    case 5:
        return data->field_11;
    case 6:
        return data->field_12;
    case 7:
        return data->field_13;
    case 8:
        return data->field_14_0;
    case 9:
        return data->field_14_1;
    case 10:
        return data->field_15_0;
    case 11:
        return data->field_15_1;
    case 12:
        return data->field_16_0;
    case 13:
        return data->field_16_1;
    case 14:
        return data->field_16_2;
    case 15:
        return data->field_17_0;
    case 16:
        return data->field_17_1;
    default:
        return -1;
    }
}

void ListMenuSetUnkIndicatorsStructField(u8 taskId, u8 field, s32 value)
{
    struct UnkIndicatorsStruct *data = (void*) &gTasks[taskId].data;

    switch (field)
    {
    case 0:
    case 1:
        data->field_4 = (void*)(value);
        break;
    case 2:
        data->field_C = value;
        break;
    case 3:
        data->field_E = value;
        break;
    case 4:
        data->field_10 = value;
        break;
    case 5:
        data->field_11 = value;
        break;
    case 6:
        data->field_12 = value;
        break;
    case 7:
        data->field_13 = value;
        break;
    case 8:
        data->field_14_0 = value;
        break;
    case 9:
        data->field_14_1 = value;
        break;
    case 10:
        data->field_15_0 = value;
        break;
    case 11:
        data->field_15_1 = value;
        break;
    case 12:
        data->field_16_0 = value;
        break;
    case 13:
        data->field_16_1 = value;
        break;
    case 14:
        data->field_16_2 = value;
        break;
    case 15:
        data->field_17_0 = value;
        break;
    case 16:
        data->field_17_1 = value;
        break;
    }
}

#define tState data[0]
#define tAnimNum data[1]
#define tBounceDir data[2]
#define tMultiplier data[3]
#define tFrequency data[4]
#define tSinePos data[5]

static void SpriteCallback_ScrollIndicatorArrow(struct Sprite *sprite)
{
    s32 multiplier;

    switch (sprite->tState)
    {
    case 0:
        StartSpriteAnim(sprite, sprite->tAnimNum);
        sprite->tState++;
        break;
    case 1:
        switch (sprite->tBounceDir)
        {
        case 0:
            multiplier = sprite->tMultiplier;
            sprite->pos2.x = (gSineTable[(u8)(sprite->tSinePos)] * multiplier) / 256;
            break;
        case 1:
            multiplier = sprite->tMultiplier;
            sprite->pos2.y = (gSineTable[(u8)(sprite->tSinePos)] * multiplier) / 256;
            break;
        }
        sprite->tSinePos += sprite->tFrequency;
        break;
    }
}

static u8 AddScrollIndicatorArrowObject(u8 arrowDir, u8 x, u8 y, u16 tileTag, u16 palTag)
{
    u8 spriteId;
    struct SpriteTemplate spriteTemplate;

    spriteTemplate = sSpriteTemplate_ScrollArrowIndicator;
    spriteTemplate.tileTag = tileTag;
    spriteTemplate.paletteTag = palTag;

    spriteId = CreateSprite(&spriteTemplate, x, y, 0);
    gSprites[spriteId].invisible = TRUE;
    gSprites[spriteId].tState = 0;
    gSprites[spriteId].tAnimNum = sScrollIndicatorTemplates[arrowDir].animNum;
    gSprites[spriteId].tBounceDir = sScrollIndicatorTemplates[arrowDir].bounceDir;
    gSprites[spriteId].tMultiplier = sScrollIndicatorTemplates[arrowDir].multiplier;
    gSprites[spriteId].tFrequency = sScrollIndicatorTemplates[arrowDir].frequency;
    gSprites[spriteId].tSinePos = 0;

    return spriteId;
}

#undef tState
#undef tAnimNum
#undef tBounceDir
#undef tMultiplier
#undef tFrequency
#undef tSinePos

u8 AddScrollIndicatorArrowPair(const struct ScrollArrowsTemplate *arrowInfo, u16 *scrollOffset)
{
    struct CompressedSpriteSheet spriteSheet;
    struct SpritePalette spritePal;
    struct ScrollIndicatorPair *data;
    u8 taskId;

    spriteSheet.data = sRedArrowOtherGfx;
    spriteSheet.size = 0x100;
    spriteSheet.tag = arrowInfo->tileTag;
    LoadCompressedSpriteSheet(&spriteSheet);

    if (arrowInfo->palTag == SPRITE_INVALID_TAG)
    {
        LoadPalette(sRedArrowPal, (16 * arrowInfo->palNum) + 0x100, 0x20);
    }
    else
    {
        spritePal.data = sRedArrowPal;
        spritePal.tag = arrowInfo->palTag;
        LoadSpritePalette(&spritePal);
    }

    taskId = CreateTask(Task_ScrollIndicatorArrowPair, 0);
    data = (void*) gTasks[taskId].data;

    data->field_0 = 0;
    data->scrollOffset = scrollOffset;
    data->fullyUpThreshold = arrowInfo->fullyUpThreshold;
    data->fullyDownThreshold = arrowInfo->fullyDownThreshold;
    data->tileTag = arrowInfo->tileTag;
    data->palTag = arrowInfo->palTag;
    data->topSpriteId = AddScrollIndicatorArrowObject(arrowInfo->firstArrowType, arrowInfo->firstX, arrowInfo->firstY, arrowInfo->tileTag, arrowInfo->palTag);
    data->bottomSpriteId = AddScrollIndicatorArrowObject(arrowInfo->secondArrowType, arrowInfo->secondX, arrowInfo->secondY, arrowInfo->tileTag, arrowInfo->palTag);

    if (arrowInfo->palTag == SPRITE_INVALID_TAG)
    {
        gSprites[data->topSpriteId].oam.paletteNum = arrowInfo->palNum;
        gSprites[data->bottomSpriteId].oam.paletteNum = arrowInfo->palNum;
    }

    return taskId;
}

u8 AddScrollIndicatorArrowPairParameterized(u32 arrowType, s32 commonPos, s32 firstPos, s32 secondPos, s32 fullyDownThreshold, s32 tileTag, s32 palTag, u16 *scrollOffset)
{
    if (arrowType == SCROLL_ARROW_UP || arrowType == SCROLL_ARROW_DOWN)
    {
        gTempScrollArrowTemplate.firstArrowType = SCROLL_ARROW_UP;
        gTempScrollArrowTemplate.firstX = commonPos;
        gTempScrollArrowTemplate.firstY = firstPos;
        gTempScrollArrowTemplate.secondArrowType = SCROLL_ARROW_DOWN;
        gTempScrollArrowTemplate.secondX = commonPos;
        gTempScrollArrowTemplate.secondY = secondPos;
    }
    else
    {
        gTempScrollArrowTemplate.firstArrowType = SCROLL_ARROW_LEFT;
        gTempScrollArrowTemplate.firstX = firstPos;
        gTempScrollArrowTemplate.firstY = commonPos;
        gTempScrollArrowTemplate.secondArrowType = SCROLL_ARROW_RIGHT;
        gTempScrollArrowTemplate.secondX = secondPos;
        gTempScrollArrowTemplate.secondY = commonPos;
    }

    gTempScrollArrowTemplate.fullyUpThreshold = 0;
    gTempScrollArrowTemplate.fullyDownThreshold = fullyDownThreshold;
    gTempScrollArrowTemplate.tileTag = tileTag;
    gTempScrollArrowTemplate.palTag = palTag;
    gTempScrollArrowTemplate.palNum = 0;

    return AddScrollIndicatorArrowPair(&gTempScrollArrowTemplate, scrollOffset);
}

static void Task_ScrollIndicatorArrowPair(u8 taskId)
{
    struct ScrollIndicatorPair *data = (void*) gTasks[taskId].data;
    u16 currItem = (*data->scrollOffset);

    if (currItem == data->fullyUpThreshold && currItem != 0xFFFF)
        gSprites[data->topSpriteId].invisible = TRUE;
    else
        gSprites[data->topSpriteId].invisible = FALSE;

    if (currItem == data->fullyDownThreshold)
        gSprites[data->bottomSpriteId].invisible = TRUE;
    else
        gSprites[data->bottomSpriteId].invisible = FALSE;
}

#define tIsScrolled data[15]

void Task_ScrollIndicatorArrowPairOnMainMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    struct ScrollIndicatorPair *scrollData = (void*) data;

    if (tIsScrolled)
    {
        gSprites[scrollData->topSpriteId].invisible = FALSE;
        gSprites[scrollData->bottomSpriteId].invisible = TRUE;
    }
    else
    {
        gSprites[scrollData->topSpriteId].invisible = TRUE;
        gSprites[scrollData->bottomSpriteId].invisible = FALSE;
    }
}

#undef tIsScrolled

void RemoveScrollIndicatorArrowPair(u8 taskId)
{
    struct ScrollIndicatorPair *data = (void*) gTasks[taskId].data;

    if (data->tileTag != SPRITE_INVALID_TAG)
        FreeSpriteTilesByTag(data->tileTag);
    if (data->palTag != SPRITE_INVALID_TAG)
        FreeSpritePaletteByTag(data->palTag);

    DestroySprite(&gSprites[data->topSpriteId]);
    DestroySprite(&gSprites[data->bottomSpriteId]);

    DestroyTask(taskId);
}

static u8 ListMenuAddCursorObjectInternal(struct CursorStruct *cursor, u32 cursorKind)
{
    switch (cursorKind)
    {
    case 0:
    default:
        return ListMenuAddRedOutlineCursorObject(cursor);
    case 1:
        return ListMenuAddRedArrowCursorObject(cursor);
    }
}

static void ListMenuUpdateCursorObject(u8 taskId, u16 x, u16 y, u32 cursorKind)
{
    switch (cursorKind)
    {
    case 0:
        ListMenuUpdateRedOutlineCursorObject(taskId, x, y);
        break;
    case 1:
        ListMenuUpdateRedArrowCursorObject(taskId, x, y);
        break;
    }
}

static void ListMenuRemoveCursorObject(u8 taskId, u32 cursorKind)
{
    switch (cursorKind)
    {
    case 0:
        ListMenuRemoveRedOutlineCursorObject(taskId);
        break;
    case 1:
        ListMenuRemoveRedArrowCursorObject(taskId);
        break;
    }
}

static void Task_RedOutlineCursor(u8 taskId)
{

}

u8 ListMenuGetRedOutlineCursorSpriteCount(u16 rowWidth, u16 rowHeight)
{
    s32 i;
    s32 count = 4;

    if (rowWidth > 16)
    {
        for (i = 8; i < (rowWidth - 8); i += 8)
            count += 2;
    }
    if (rowHeight > 16)
    {
        for (i = 8; i < (rowHeight - 8); i += 8)
            count += 2;
    }

    return count;
}

void ListMenuSetUpRedOutlineCursorSpriteOamTable(u16 rowWidth, u16 rowHeight, struct Subsprite *subsprites)
{
    s32 i, j, id = 0;

    subsprites[id] = sSubsprite_RedOutline1;
    subsprites[id].x = 136;
    subsprites[id].y = 136;
    id++;

    subsprites[id] = sSubsprite_RedOutline2;
    subsprites[id].x = rowWidth + 128;
    subsprites[id].y = 136;
    id++;

    subsprites[id] = sSubsprite_RedOutline7;
    subsprites[id].x = 136;
    subsprites[id].y = rowHeight + 128;
    id++;

    subsprites[id] = sSubsprite_RedOutline8;
    subsprites[id].x = rowWidth + 128;
    subsprites[id].y = rowHeight + 128;
    id++;

    if (rowWidth > 16)
    {
        for (i = 8; i < rowWidth - 8; i += 8)
        {
            subsprites[id] = sSubsprite_RedOutline3;
            subsprites[id].x = i - 120;
            subsprites[id].y = -120;
            id++;

            subsprites[id] = sSubsprite_RedOutline6;
            subsprites[id].x = i - 120;
            subsprites[id].y = rowHeight + 128;
            id++;
        }
    }

    if (rowHeight > 16)
    {
        for (j = 8; j < rowHeight - 8; j += 8)
        {
            subsprites[id] = sSubsprite_RedOutline4;
            subsprites[id].x = 136;
            subsprites[id].y = j - 120;
            id++;

            subsprites[id] = sSubsprite_RedOutline5;
            subsprites[id].x = rowWidth + 128;
            subsprites[id].y = j - 120;
            id++;
        }
    }
}

static u8 ListMenuAddRedOutlineCursorObject(struct CursorStruct *cursor)
{
    struct CompressedSpriteSheet spriteSheet;
    struct SpritePalette spritePal;
    struct RedOutlineCursor *data;
    struct SpriteTemplate spriteTemplate;
    u8 taskId;

    spriteSheet.data = sSelectorOutlineGfx;
    spriteSheet.size = 0x100;
    spriteSheet.tag = cursor->tileTag;
    LoadCompressedSpriteSheet(&spriteSheet);

    if (cursor->palTag == SPRITE_INVALID_TAG)
    {
        LoadPalette(sRedArrowPal, (16 * cursor->palNum) + 0x100, 0x20);
    }
    else
    {
        spritePal.data = sRedArrowPal;
        spritePal.tag = cursor->palTag;
        LoadSpritePalette(&spritePal);
    }

    taskId = CreateTask(Task_RedOutlineCursor, 0);
    data = (void*) gTasks[taskId].data;

    data->tileTag = cursor->tileTag;
    data->palTag = cursor->palTag;
    data->subspriteTable.subspriteCount = ListMenuGetRedOutlineCursorSpriteCount(cursor->rowWidth, cursor->rowHeight);
    data->subspriteTable.subsprites = data->subspritesPtr = Alloc(data->subspriteTable.subspriteCount * 4);
    ListMenuSetUpRedOutlineCursorSpriteOamTable(cursor->rowWidth, cursor->rowHeight, data->subspritesPtr);

    spriteTemplate = gDummySpriteTemplate;
    spriteTemplate.tileTag = cursor->tileTag;
    spriteTemplate.paletteTag = cursor->palTag;

    data->spriteId = CreateSprite(&spriteTemplate, cursor->left + 120, cursor->top + 120, 0);
    SetSubspriteTables(&gSprites[data->spriteId], &data->subspriteTable);
    gSprites[data->spriteId].oam.priority = 0;
    gSprites[data->spriteId].subpriority = 0;
    gSprites[data->spriteId].subspriteTableNum = 0;

    if (cursor->palTag == SPRITE_INVALID_TAG)
    {
        gSprites[data->spriteId].oam.paletteNum = cursor->palNum;
    }

    return taskId;
}

static void ListMenuUpdateRedOutlineCursorObject(u8 taskId, u16 x, u16 y)
{
    struct RedOutlineCursor *data = (void*) gTasks[taskId].data;

    gSprites[data->spriteId].pos1.x = x + 120;
    gSprites[data->spriteId].pos1.y = y + 120;
}

static void ListMenuRemoveRedOutlineCursorObject(u8 taskId)
{
    struct RedOutlineCursor *data = (void*) gTasks[taskId].data;

    Free(data->subspritesPtr);

    if (data->tileTag != SPRITE_INVALID_TAG)
        FreeSpriteTilesByTag(data->tileTag);
    if (data->palTag != SPRITE_INVALID_TAG)
        FreeSpritePaletteByTag(data->palTag);

    DestroySprite(&gSprites[data->spriteId]);
    DestroyTask(taskId);
}

static void SpriteCallback_RedArrowCursor(struct Sprite *sprite)
{
    sprite->pos2.x = gSineTable[(u8)(sprite->data[0])] / 64;
    sprite->data[0] += 8;
}

static void Task_RedArrowCursor(u8 taskId)
{

}

static u8 ListMenuAddRedArrowCursorObject(struct CursorStruct *cursor)
{
    struct CompressedSpriteSheet spriteSheet;
    struct SpritePalette spritePal;
    struct RedArrowCursor *data;
    struct SpriteTemplate spriteTemplate;
    u8 taskId;

    spriteSheet.data = sRedArrowGfx;
    spriteSheet.size = 0x80;
    spriteSheet.tag = cursor->tileTag;
    LoadCompressedSpriteSheet(&spriteSheet);

    if (cursor->palTag == SPRITE_INVALID_TAG)
    {
        LoadPalette(sRedArrowPal, (16 * cursor->palNum) + 0x100, 0x20);
    }
    else
    {
        spritePal.data = sRedArrowPal;
        spritePal.tag = cursor->palTag;
        LoadSpritePalette(&spritePal);
    }

    taskId = CreateTask(Task_RedArrowCursor, 0);
    data = (void*) gTasks[taskId].data;

    data->tileTag = cursor->tileTag;
    data->palTag = cursor->palTag;

    spriteTemplate = sSpriteTemplate_RedArrowCursor;
    spriteTemplate.tileTag = cursor->tileTag;
    spriteTemplate.paletteTag = cursor->palTag;

    data->spriteId = CreateSprite(&spriteTemplate, cursor->left, cursor->top, 0);
    gSprites[data->spriteId].pos2.x = 8;
    gSprites[data->spriteId].pos2.y = 8;

    if (cursor->palTag == SPRITE_INVALID_TAG)
    {
        gSprites[data->spriteId].oam.paletteNum = cursor->palNum;
    }

    return taskId;
}

static void ListMenuUpdateRedArrowCursorObject(u8 taskId, u16 x, u16 y)
{
    struct RedArrowCursor *data = (void*) gTasks[taskId].data;

    gSprites[data->spriteId].pos1.x = x;
    gSprites[data->spriteId].pos1.y = y;
}

static void ListMenuRemoveRedArrowCursorObject(u8 taskId)
{
    struct RedArrowCursor *data = (void*) gTasks[taskId].data;

    if (data->tileTag != SPRITE_INVALID_TAG)
        FreeSpriteTilesByTag(data->tileTag);
    if (data->palTag != SPRITE_INVALID_TAG)
        FreeSpritePaletteByTag(data->palTag);

    DestroySprite(&gSprites[data->spriteId]);
    DestroyTask(taskId);
}
