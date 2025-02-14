@@MAIN DEBUG MENU==================================================================================
.set PRIMARY_ABILITY, 0
.set SECONDARY_ABILITY, 1
.set HIDDEN_ABILITY, 2

RyuDevSwitch::
	setvar VAR_TEMP_9, 100
	special RyuDevCheck
	switch VAR_RESULT
	case 69, Ryu_enableDevMode
	case 666, Ryu_SpecialChallengeActive
	release
	end

queryenabledev::
	.string "Enable Dev Mode?$"

Ryu_enableDevMode::
	msgbox queryenabledev, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq ryu_end
	setflag FLAG_RYU_DEV_MODE
	setflag FLAG_RYU_CHEATS_USED
	msgbox RyuNowDeveloper MSGBOX_SIGN
	end

Ryu_SpecialChallengeActive::
	msgbox RyuCantEnableDevBecauseChallenge
	release
	end

RyuCantEnableDevBecauseChallenge:
	.string "You cannot enable dev mode\nbecause you are participating\lin\pRyu's Challenge.$"
	
RyuNowDeveloper:
	.string "{PLAYER} is now a developer.$"

RyuDebugMenuBootstrap::
	goto RyuDebugMenuScript

RyuBetaMenuBootstrap::
	goto RyuDebug_ShowBetaMenu
	
RyuDebugMenuScript::
	msgbox RyuDebug_msg, MSGBOX_SIGN
	multichoicedefault 0, 0, MULTI_DEV_MENU_MAIN, 8, 0
	switch VAR_RESULT
	case 0, RyuDebug_UtilitiesMenu
	case 1, RyuDebug_Utility1
	case 2, RyuDebug_SpawningMenu
	case 3, RyuDebug_DevMonQuickStart
	case 4, RyuPokeEditMenu
	case 5, RyuDebug_enableWaystone
	case 6, RyuDebug_DisableDebug
	case 7, RyuDebug_ShowBetaMenu
	case 8, RyuDebug_cancel
	goto RyuDebug_cancel
	
RyuDebug_menu_nodevcheck::
	msgbox RyuDebug_msg MSGBOX_SIGN
	multichoicedefault 0, 0, MULTI_DEV_MENU_MAIN, 8, 0
	switch VAR_RESULT
	case 0, RyuDebug_UtilitiesMenu
	case 1, RyuDebug_Utility1
	case 2, RyuDebug_SpawningMenu
	case 3, RyuDebug_DevMonQuickStart
	case 4, RyuPokeEditMenu
	case 5, RyuDebug_enableWaystone
	case 6, RyuDebug_DisableDebug
	case 7, RyuDebug_ShowBetaMenu
	case 8, RyuDebug_cancel
	goto RyuDebug_cancel

RyuDebug_ShowBetaMenu::
	msgbox RDB_OpeningBetaMenu
	closemessage
	goto RDB_BetaMenuStandalone

RDB_OpeningBetaMenu:
	.string "Opening beta menu.$"


RyuDebug_UtilitiesMenu:
	scrollingmultichoice SCROLLINGMULTICHOICE_DEBUGUTILITIES, 0, 0, 9, 1
	switch VAR_RESULT
	case 0, RyuDebug_Phasing
	case 1, RyuDebug_Encounters
	case 2, RyuDebug_ViewTempVars
	case 3, RyuDebug_setTime
	case 4, RyuDebug_BattleSim
	case 5, RyuSetExpMultMenu
	case 6, RyuDebug_giveAllBadges
	case 7, RyuDebug_HealParty
	case 8, RyuDebug_SaveFileDebug
	case 9, RyuDebug_FillTheDex
	case 10, RDB_GiveOrTakeAllAchievements
	case 11, RDB_ShowModsMenu
	case 12, RyuDebug_EnableDexnav
	case 13, RDB_AlchemyInfo
	case 14, RDB_ToggleLevelCap
	case 15, RDB_EnableRealEstate
	case 16, RyuDebug_InitTimeSlide
	case 17, RyuDebug_ReadoutRGBTheme
	case 18, RyuDebug_CheckRyuChallengeStatus
	case 19, RyuDebug_CheckExactPlayTime
	case 20, RyuDebug_EnableVerboseUtility
	case 21, RyuDebug_TestDynamicObjects
	case 22, RyuDebug_ToggleBattleInfo
	case 23, RyuDebug_menu_nodevcheck
	goto RyuDebug_menu_nodevcheck

RyuDebug_ToggleBattleInfo::
	goto_if_set FLAG_RYU_BATTLE_INFO, RDB_TurnOffBattleInfo
	setflag FLAG_RYU_BATTLE_INFO
	msgbox RDB_BIT
	closemessage
	goto RyuDebug_UtilitiesMenu

RDB_TurnOffBattleInfo::
	clearflag FLAG_RYU_BATTLE_INFO
	msgbox RDB_BIT
	goto RyuDebug_UtilitiesMenu

RDB_BIT:
	.string "Battle Info screen toggled.$"

RyuDebug_TestDynamicObjects::
	callnative RyuTestDynamicObjectContents
	msgbox RDBTDO, MSGBOX_DEFAULT
	closemessage
	release
	end

RDBTDO:
	.string "There are {STR_VAR_1} Dynamic Object(s) active.$"

RDB_SpawnFollowerMultiMenu::
	msgbox RDB_SpawnWhichFollower
	scrollingmultichoice SMC_RYUFOLLOWERDEBUGSUBMENU, 0, 0, 6, 1
	switch VAR_RESULT
	case 0, RDB_SpawnMinnie
	case 1, RDB_SpawnShelly
	case 2, RDB_SpawnLanette
	case 3, RDB_SpawnDawn
	case 4, RDB_SpawnBrendan
	case 5, RDB_SpawnLeaf
	case 6, RDB_SpawnCourtney
	case 7, RDB_SpawnJoy
	case 8, RDB_SpawnMay
	case 9, RDB_SpawnLucy
	case 10, RDB_SpawnMom
	case 11, RDB_SpawnGladion
	case 12, RDB_SpawnLillie
	case 13, RDB_RETURNTOMAIN
	goto RDB_RETURNTOMAIN

RDB_SpawnWhichFollower:
	.string "Spawn which follower?$"

RDB_SpawnMinnie::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_MINNIE, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnShelly::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_SHELLY, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnLanette::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_LANETTE, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end
	
RDB_SpawnDawn::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_DAWN, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end
	
RDB_SpawnBrendan::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_BRENDAN, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end
	
RDB_SpawnLeaf::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_LEAF, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end
	
RDB_SpawnCourtney::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_COURTNEY, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end
	
RDB_SpawnJoy::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_JOY, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnBlaise::
	call RyuForceCancelFollowerForQuest
	createfollower OBJ_EVENT_GFX_MAGMA_MEMBER_M, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnTiana::
	call RyuForceCancelFollowerForQuest
	createfollower OBJ_EVENT_GFX_LASS, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnHexer::
	call RyuForceCancelFollowerForQuest
	createfollower OBJ_EVENT_GFX_HEX_MANIAC, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnMay::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_MAY, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnLucy::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_LUCY, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnMom::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_MOM, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnGladion::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_GLADION, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_SpawnLillie::
	call RyuForceCancelFollowerForQuest
	createfollower FOLLOWER_LILLIE, RyuFollowerSelectNPCScript, DIR_SOUTH
	goto RDB_ExplainSpawnFollower
	end

RDB_NONCOMBATFOLLOWER::
	goto RDB_SpawnFollowerMultiMenu

RDB_ExplainSpawnFollower::
	msgbox RDB_ExplainSpawnMsg
	closemessage
	goto RyuDebug_UtilitiesMenu

RDB_ExplainSpawnMsg:
	.string "You have spawned in a follower.$"
	
RDB_RETURNTOMAIN::
	msgbox RDB_RTM1
	closemessage
	goto RyuDebug_menu_nodevcheck

RDB_RTM1::
	.string "Returning to main menu.$"

RDB_EnableRealEstate::
	callnative SetPlayerRealtorStatus
	release
	end

RDB_ShowModsMenu::
	call RyuDifficultyModScript
	release
	end


RDB_APDebug:
	.string "You currently have {STR_VAR_2}/{STR_VAR_1} AP active.$"

RyuDebug_giveAllBadges::
	setflag FLAG_BADGE01_GET
	setflag FLAG_BADGE02_GET
	setflag FLAG_BADGE03_GET
	setflag FLAG_BADGE04_GET
	setflag FLAG_BADGE05_GET
	setflag FLAG_BADGE06_GET
	setflag FLAG_BADGE07_GET
	setflag FLAG_BADGE08_GET
	callnative RyuSetFactionStandingPokemonLeagueDebug
	playse SE_EXPMAX
	goto RyuDebug_UtilitiesMenu

RyuDebug_FillTheDex::
	special FillTheDex
	msgbox RDB_FillDex
	goto RyuDebug_UtilitiesMenu

RDB_FillDex:
	.string "Dex pages filled in.$"

RyuDebug_HealParty::
	special HealPlayerParty
	playse SE_EXPMAX
	goto RyuDebug_UtilitiesMenu

RyuDebug_money::
	addmoney 1000000000
	msgbox RyuDebug_money_text MSGBOX_DEFAULT
	return

RyuDebug_Encounters::
	goto_if_set FLAG_RYU_DEV_DISENC, RyuDebug_EnENC
	setflag FLAG_RYU_DEV_DISENC
	msgbox RyuEncDisabled
	goto RyuDebug_UtilitiesMenu
	
RyuDebug_EnENC::
	clearflag FLAG_RYU_DEV_DISENC
	msgbox RyuEncEnabled
	goto RyuDebug_UtilitiesMenu
	
RyuEncDisabled:
	.string "Dev Repel on$"
	
RyuEncEnabled:
	.string "Dev Repel off$"

RyuDebug_money_text:
	.string "Given ¥1b.$"

RyuDebug_DisableDebug::
	clearflag FLAG_RYU_DEV_MODE
	setflag FLAG_HIDE_103_TEST_TRAINER
	msgbox DevModeDisabled MSGBOX_DEFAULT
	releaseall
	end

Ryu_saveViewerEnabled:
	.string "First variable set to beacon the\nsave viewer.\pSave the game twice to make sure\nboth save banks are the same!$"

RyuDebug_Phasing::
	goto_if_set FLAG_RYU_COLLISION_OFF, Ryu_DisablePhasing
	setflag FLAG_RYU_COLLISION_OFF
	setflag FLAG_TOBY_TRAINER_SIGHT
	msgbox Ryu_CollisionOffMsg
	goto RyuDebug_UtilitiesMenu

Ryu_DisablePhasing::
	clearflag FLAG_RYU_COLLISION_OFF
	clearflag FLAG_TOBY_TRAINER_SIGHT
	msgbox Ryu_CollisionOnMsg
	goto RyuDebug_UtilitiesMenu

Ryu_CollisionOffMsg:
	.string "Entered the Ethereal plane.$"

Ryu_CollisionOnMsg:
	.string "Returned to Material plane.$"

DevModeDisabled:
	.string "{PLAYER} is no longer a developer.$"

RyuDebug_EnableDexnav::
	setflag FLAG_SYS_DEXNAV_GET
	release
	end

RDB_AlchemyInfo::
	callnative RyuDebug_CheckAlchemyStatus
	msgbox RDB_AlchemyInfoMsg
	closemessage
	release
	end

RDB_AlchemyInfoMsg:
	.string "Has Alchemy effect active? {STR_VAR_1}\n"
	.string "Current effect is {STR_VAR_2},\l"
	.string "with {STR_VAR_3} charges left.\p"
	.string "Player's alchemy level is {RYU_STR_1}.\n"
	.string "Player's Alchemy exp is {RYU_STR_2}.$"

RDB_ToggleLevelCap::
	msgbox RDB_LevelCapToggled
	goto_if_set FLAG_RYU_IGNORE_CAP, RDB_EnableCap
	setflag FLAG_RYU_IGNORE_CAP
	goto RyuDebug_UtilitiesMenu

RDB_EnableCap::
	clearflag FLAG_RYU_IGNORE_CAP
	goto RyuDebug_UtilitiesMenu

RDB_LevelCapToggled:
	.string "Level Cap Toggled.$"

RyuDebug_enableWaystone::
	additem ITEM_WAYSTONE, 1
	setflag FLAG_RYU_WAYSTONE_BYPASS
	setflag FLAG_VISITED_LITTLEROOT_TOWN
	setflag FLAG_VISITED_OLDALE_TOWN
	setflag FLAG_VISITED_DEWFORD_TOWN
	setflag FLAG_VISITED_LAVARIDGE_TOWN
	setflag FLAG_VISITED_FALLARBOR_TOWN
	setflag FLAG_VISITED_VERDANTURF_TOWN
	setflag FLAG_VISITED_PACIFIDLOG_TOWN
	setflag FLAG_VISITED_PETALBURG_CITY
	setflag FLAG_VISITED_SLATEPORT_CITY
	setflag FLAG_VISITED_MAUVILLE_CITY
	setflag FLAG_VISITED_RUSTBORO_CITY
	setflag FLAG_VISITED_FORTREE_CITY
	setflag FLAG_VISITED_LILYCOVE_CITY
	setflag FLAG_VISITED_MOSSDEEP_CITY
	setflag FLAG_VISITED_SOOTOPOLIS_CITY
	setflag FLAG_VISITED_EVER_GRANDE_CITY
	setflag FLAG_LANDMARK_BATTLE_FRONTIER
	setflag FLAG_LANDMARK_POKEMON_LEAGUE
	setflag FLAG_RYU_LANDMARK_MH
	setflag FLAG_RYU_LANDMARK_LCCDEPTSTORE
	setflag FLAG_LANDMARK_SNOWY_SHORE
	setflag FLAG_LANDMARK_AQUA_OFFICE
	clearflag FLAG_RYU_WAYSTONE_DISABLED
	return

RyuDebug_ViewTempVars::
	special RyuCheckTempVars
	msgbox RDB_TempVarsViewMsg
	goto RyuDebug_UtilitiesMenu

RDB_TempVarsViewMsg:
	.string "{STR_VAR_1}$"

RDB_GiveOrTakeAllAchievements::
	msgbox RDB_GiveTakeAchMsg, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RDB_GiveAllAch
	goto RDB_TakeAllAch

RDB_TakeAllAch::
	setvar VAR_TEMP_B, 0
	callnative Ryu_GiveOrTakeAllAchievments
	msgbox RDB_Finished
	release
	end

RDB_GiveAllAch::
	setvar VAR_TEMP_B, 69
	callnative Ryu_GiveOrTakeAllAchievments
	msgbox RDB_Finished
	release
	end

RDB_Finished:
	.string "Finished giving or taking ach's.$"

RDB_GiveTakeAchMsg::
	.string "Select yes to give\nor no to take all achievements.$"

RDB_Text:
	.string "Gained 4 badges.$"

RyuDebug_SaveFileDebug::
	special CheckSaveFileSize
	msgbox SaveFileStructMsg
	release
	end

SaveFileStructMsg:
	.string "SaveBlock1 size is {STR_VAR_1}/15872 bytes.\nSaveBlock2 size is {STR_VAR_2}/3968 bytes.\psize3 is {STR_VAR_3}\nsize4 is {RYU_STR_1}$"

exampletext:
	.string "This is standard text.\p"
	.string "{COLOR LIGHT_RED}{SHADOW RED}This is a partner speaking.\p"
	.string "{COLOR LIGHT_BLUE}{SHADOW BLUE}This is player speaking\p"
	.string " {COLOR LIGHT_GREEN}{SHADOW GREEN}(This is green narration text.\n"
	.string " There's a space at the beginning of\l"
	.string " newlines and new pages too.)$"

RDB_GiveRandomFrontierMons::
	compare VAR_TEMP_5, 0
	goto_if_eq GiveRandomFrontiermonsEnd
	subvar VAR_TEMP_5, 1
	goto RDB_GiveRandomFrontierMonsLoop

RDB_GiveRandomFrontierMonsLoop::
	random 735
	addvar VAR_RESULT, 97
	copyvar VAR_TEMP_6, VAR_RESULT 
	special RyuGiveFrontierMon
	goto RDB_GiveRandomFrontierMons

GiveRandomFrontiermonsEnd::
	msgbox RDB_GRFMD
	clearflag FLAG_RYU_PASSCODE_FRONTIER_MON_DEV
	closemessage
	release
	end

RDB_GRFMD:
	.string "Given 6 random frontier mons.$"

RDB_Readout:
	.string "It is currently: {STR_VAR_1}:{STR_VAR_2}::{STR_VAR_3}  $"

RyuDebug_setTime::
	gettime
	buffernumberstring 0, VAR_0x8000
	buffernumberstring 1, VAR_0x8001
	buffernumberstring 2, VAR_0x8002
	msgbox RDB_Readout
	closemessage
	msgbox settimequery MSGBOX_YESNO
	compare VAR_RESULT, 1
	goto_if_ne ryu_end
	fadescreen 1
	special StartWallClock
	waitstate
	gettime
	buffernumberstring 0, VAR_0x8000
	buffernumberstring 1, VAR_0x8001
	buffernumberstring 2, VAR_0x8002
	msgbox RDB_Readout_2
	goto RyuDebug_UtilitiesMenu

settimequery:
	.string "Would you like to change the time?$"

RDB_Readout_2:
	.string "Time was changed to: {STR_VAR_1}:{STR_VAR_2}:{STR_VAR_3}  $"

RyuDebug_GiveItems::
	msgbox RDB_GiveTMsQuery MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq RyuDebug_Init_GiveAllTMs
	msgbox AlchemyKitQuery MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq RyuDebug_GiveAlchemyKit
	msgbox ItemsKitQuery MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq RyuDebug_GiveItemsKit
	msgbox RyuDebug_MoneyMsg MSGBOX_YESNO
	compare VAR_RESULT, 1
	call_if_eq RyuDebug_money
	msgbox KeyItemsKitQuery MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq RyuDebug_GiveKeyItemsKit
	msgbox MegaStonesQuery, MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq RyuDebug_GiveMegaStones
	msgbox collAndRepel, MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq RyuDebug_DisableCollisionandencounters
	goto RyuDebug_menu_nodevcheck

RyuDebug_DisableCollisionandencounters::
	setflag FLAG_RYU_COLLISION_OFF
	setflag FLAG_RYU_DEV_DISENC
	return

RyuDebug_Init_GiveAllTMs::
	setvar VAR_TEMP_2, 482
	msgbox RyuDebug_GiveAllTMsBeginMsg MSGBOX_DEFAULT
	closemessage
	goto RyuDebug_GiveAllTMs
	
RyuDebug_GiveAllTMs::
	additem VAR_TEMP_2, 1
	addvar VAR_TEMP_2, 1
	goto RyuDebug_ReturnCallTMs
	
RyuDebug_ReturnCallTMs::
	compare VAR_TEMP_2, 546
	goto_if_eq RyuDebugGiveTMsDone
	goto RyuDebug_GiveAllTMs
	
RyuDebugGiveTMsDone::
	goto ryu_return

RyuDebug_GiveAlchemyKit::
	setvar VAR_RYU_PLAYER_ALCHEMY_SKILL, 5
	setvar VAR_RYU_PLAYER_ALCHEMY_SKILL_EXP, 5000
	additem ITEM_FRESH_WATER, 1000
	additem ITEM_TONIC_WATER, 1000
	additem ITEM_MINERAL_WATER, 1000
	additem ITEM_MUSCLE_POWDER, 1000
	additem ITEM_SMART_POWDER, 1000
	additem ITEM_STARDUST, 1000
	additem ITEM_TOUGH_POWDER, 1000
	additem ITEM_RAWST_BERRY, 1000
	additem ITEM_BRIGHT_POWDER, 1000
	additem ITEM_ORAN_BERRY, 1000
	additem ITEM_CHARCOAL, 1000
	additem ITEM_FIRE_STONE, 1000
	additem ITEM_SOFT_SAND, 1000
	additem ITEM_SNOWBALL, 1000
	additem ITEM_GEM_POWDER, 1000
	additem ITEM_STAR_PIECE, 1000
	setvar VAR_RYU_ALCHEMY_COPPER, 65500
	setvar VAR_RYU_ALCHEMY_SILVER, 65500
	setvar VAR_RYU_ALCHEMY_GOLD, 65500
	return

RyuDebug_GiveMegaStones::
	additem ITEM_ABOMASITE 1
	additem ITEM_ABSOLITE 1
	additem ITEM_AERODACTYLITE 1
	additem ITEM_AGGRONITE 1
	additem ITEM_ALAKAZITE 1
	additem ITEM_ALTARIANITE 1
	additem ITEM_AMPHAROSITE 1
	additem ITEM_AUDINITE 1
	additem ITEM_BANETTITE 1
	additem ITEM_BEEDRILLITE 1
	additem ITEM_BLASTOISINITE 1
	additem ITEM_BLAZIKENITE 1
	additem ITEM_CAMERUPTITE 1
	additem ITEM_CHARIZARDITE_X 1
	additem ITEM_CHARIZARDITE_Y 1
	additem ITEM_DIANCITE 1
	additem ITEM_GALLADITE 1
	additem ITEM_GARCHOMPITE 1
	additem ITEM_GARDEVOIRITE 1
	additem ITEM_GENGARITE 1
	additem ITEM_GLALITITE 1
	additem ITEM_GYARADOSITE 1
	additem ITEM_HERACRONITE 1
	additem ITEM_HOUNDOOMINITE 1
	additem ITEM_KANGASKHANITE 1
	additem ITEM_LATIASITE 1
	additem ITEM_LATIOSITE 1
	additem ITEM_LOPUNNITE 1
	additem ITEM_LUCARIONITE 1
	additem ITEM_MANECTITE 1
	additem ITEM_MAWILITE 1
	additem ITEM_MEDICHAMITE 1
	additem ITEM_METAGROSSITE 1
	additem ITEM_MEWTWONITE_X 1
	additem ITEM_MEWTWONITE_Y 1
	additem ITEM_PIDGEOTITE 1
	additem ITEM_PINSIRITE 1
	additem ITEM_SABLENITE 1
	additem ITEM_SALAMENCITE 1
	additem ITEM_SCEPTILITE 1
	additem ITEM_SCIZORITE 1
	additem ITEM_SHARPEDONITE 1
	additem ITEM_SLOWBRONITE 1
	additem ITEM_STEELIXITE 1
	additem ITEM_SWAMPERTITE 1
	additem ITEM_TYRANITARITE 1
	additem ITEM_VENUSAURITE 1
	return

RyuDebug_GiveItemsKit:
	additem ITEM_FULL_RESTORE, 1000
	additem ITEM_MAX_ELIXIR, 1000
	additem ITEM_MAX_REVIVE, 1000
	additem ITEM_HEART_SCALE, 1000
	additem ITEM_PP_MAX, 1000
	additem ITEM_RARE_CANDY, 1000
	additem ITEM_LUXURY_BALL, 1000
	additem ITEM_MASTER_BALL, 1000
	additem ITEM_MAX_REPEL, 1000
	additem ITEM_BIG_PEARL, 1000
	return

RyuDebug_GiveKeyItemsKit::
	additem ITEM_MEGA_BRACELET, 1
	additem ITEM_EXP_DRIVE, 1
	additem ITEM_FORECASTER, 1
	additem ITEM_IMPRINTER, 1
	additem ITEM_EXP_SHARE, 1
	additem ITEM_SOOT_SACK, 1
	additem ITEM_RECIPE_BOOK, 1
	additem ITEM_REAGENT_POUCH, 1
	additem ITEM_STATASSIST, 1
	additem ITEM_RAINBOW_PASS, 1
	return

MegaStonesQuery:
	.string "Would you like all Mega Stones?$"

RyuDebug_MoneyMsg:
	.string "Give Dev Mode money payout?$"

RDB_GiveTMsQuery:
	.string "Would you like all TMs?$"
	
RyuDebug_GiveAllTMsDoneMsg:
	.string "Finished giving starter kit.$"
	
RyuDebug_GiveAllTMsBeginMsg:
	.string "Giving all tms...$"

AlchemyKitQuery:
	.string "Do you want Alchemy items?$"

ItemsKitQuery:
	.string "Do you want adventuring items?$"

KeyItemsKitQuery:	
	.string "Do you want special Key Items?$"

RyuDebug_givemonsquery:
	.string "Would you like to be given\nall the Pokémon in the game?$"

RyuDebug_cancel::
	closemessage
	end

collAndRepel:
	.string "Turn off collision and encounters?$"

RyuDebug_msg:
	.string "What would you like to activate?$"
	
RyuDebug_Init_GiveAll::
	setvar VAR_TEMP_0, 265
	msgbox RyuDebug_GiveAllBeginMsg MSGBOX_DEFAULT
	goto RyuDebug_GiveAllMons
	
RyuDebug_GiveAllMons::
	givemon VAR_TEMP_0, 5, 0x0
	addvar VAR_TEMP_0, 1
	goto RyuDebug_ReturnCall
	
RyuDebug_ReturnCall::
	compare VAR_TEMP_0, 686
	goto_if_eq RyuDebugGivePokeDone
	goto RyuDebug_GiveAllMons
	
RyuDebugGivePokeDone::
	msgbox RyuDebug_GiveAllDoneMsg MSGBOX_DEFAULT
	release
	end
	
RyuDebug_GiveAllDoneMsg:
	.string "Finished Giving all mons.$"
	
RyuDebug_GiveAllBeginMsg:
	.string "The process will begin when you\nclose this message.\lIt will take 30 seconds to a minute\lto complete, and the game may appear\lto be frozen.\pIf music is still playing,\nit is not frozen. Enjoy!$"

RyuDebug_DevMonQuickStart::
	setflag FLAG_RYU_IGNORE_CAP
	clearflag FLAG_RYU_VANILLA_CAP
	setvar VAR_TEMP_A, NATURE_MODEST     
	setvar VAR_TEMP_B, SPECIES_MAGEARNA
	setvar VAR_TEMP_C, 0
	setvar VAR_TEMP_D, MON_GENDERLESS @@This is gender selection. Should be MON_FEMALE, MON_MALE, or MON_GENDERLESS.
	setvar VAR_TEMP_E, ITEM_EXPERT_BELT
	setvar VAR_TEMP_1, MOVE_MOONBLAST
	setvar VAR_TEMP_2, MOVE_SHADOW_BALL
	setvar VAR_TEMP_3, MOVE_FLASH_CANNON
	setvar VAR_TEMP_4, MOVE_THUNDERBOLT
	givepokedexflag SPECIES_MAGEARNA, TRUE @@for some reason, pokdex won't open unless you've seen at least one nat'l dex mon. This fixes that.
	callnative RyuGiveDevMon
	@@
	@@
	@@(this is to make it easy to tell between which mon you are editing)
	@@
	@@
	setvar VAR_TEMP_A, NATURE_ADAMANT
	setvar VAR_TEMP_B, SPECIES_MEGA_ABSOL
	setvar VAR_TEMP_C, 0
	setvar VAR_TEMP_D, MON_FEMALE
	setvar VAR_TEMP_E, ITEM_SCOPE_LENS
	setvar VAR_TEMP_1, MOVE_SLASH
	setvar VAR_TEMP_2, MOVE_NIGHT_SLASH
	setvar VAR_TEMP_3, MOVE_PSYCHO_CUT
	setvar VAR_TEMP_4, MOVE_STONE_EDGE
	givepokedexflag SPECIES_ABSOL, TRUE @@see above pokedex comment
	callnative RyuGiveDevMon
	special HealPlayerParty
	goto RyuDebug_QuickItemFlagStart


RyuDebug_QuickItemFlagStart::
	additem ITEM_WAYSTONE, 1
	call RyuDebug_enableWaystone
	special RyuSetUpSaveBlockStuff
	goto RyuDebug_SkipTutorial
	end

RyuDebug_SkipTutorial::
	goto_if_unset FLAG_RYU_DEV_MODE, ryu_end
	msgbox RyuDebug_GiveItemQuery MSGBOX_YESNO
	compare VAR_RESULT, 1
	call_if_eq RyuDebug_GiveItems
	release
	end

RyuDebug_GiveItemQuery:
	.string "Show item options?$"

RyuNewGameQuery:
	.string "Access QuickStart dialogues?$"

@@EV DEBUG MENU==================================================================================================================

RyuPokeEditMenu::
	multichoicedefault 0, 0, MULTI_DEV_MODE_POKEEDIT_MENU, 0, 0
	switch VAR_RESULT
	case 0, RD_MonDataView
	case 1, Ryu_ResetEVIV
	case 2, RyuSetIvEvDebugMenu
	case 3, RyuSetFriendshipDebugMenu
	case 4, RyuAccessDevMoveRelearner
	case 5, RyuChangeBossStatus
	case 6, RyuNatureSwapDevMode
	case 7, RyuAbilityChange
	case 8, RyuExitPokeEditMenu
	goto RyuExitPokeEditMenu

RyuAbilityChange::
	msgbox RyuIvEv_ChooseSlot
	call RyuBufferPartyMonNames
	multichoice 0, 0, MULTI_DYNAMIC_6, 1
	copyvar VAR_TEMP_8, VAR_RESULT
	specialvar VAR_TEMP_A, CheckIfSelectedSlotIsValid
	compare VAR_TEMP_A, FALSE
	goto_if_eq RyuPokeEditMenu
	closemessage
	msgbox GCMS_DMPG_Ability
	multichoicedefault 0, 6, MULTI_DEV_GIVE_MON_ABILITY, 0, 1
	copyvar VAR_TEMP_7, VAR_RESULT
	special SetMonAbility
	msgbox PKE_Done
	closemessage
	goto RyuPokeEditMenu

PKE_Done:
	.string "Ability Changed.$"

RyuChangeBossStatus::
	call RyuBufferPartyMonNames
	multichoice 0, 0, MULTI_DYNAMIC_6, 1
	copyvar VAR_0x8001, VAR_RESULT
	specialvar VAR_TEMP_A, CheckIfSelectedSlotIsValid
	compare VAR_TEMP_A, FALSE
	goto_if_eq RyuPokeEditMenu
	bufferpartymonnick 0, VAR_0x8001
	msgbox RCBS_Query, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RyuPokeEditMenu
	callnative RyuToggleBossStatus
	msgbox RCBS_Success
	goto RyuPokeEditMenu

RCBS_Query:	
	.string "Would you like to toggle {COLOR LIGHT_RED}{SHADOW RED}boss{COLOR DARK_GREY}{SHADOW LIGHT_GREY} status\nfor {STR_VAR_1}?$"

RCBS_Success:
	.string "Boss status successfully changed.$"

RD_MonDataView::
	setvar VAR_0x8001, 0
    special RyuFillStatsBuffers
	delay 30
    drawinfobox INFOBOXPOKEMONDATA
	bufferpartymonnick 1, 0
	msgbox Ryu_Stats
	removeinfobox
	special HealPlayerParty
	goto RyuPokeEditMenu
	
RyuExitPokeEditMenu::
	release
	end

Ryu_ResetEVIV::
	bufferdynamicmulti RIVEV1, RIVEV2, RIVEV3
	multichoice 0, 6, MULTI_DYNAMIC_3, 0
	copyvar VAR_0x8003, VAR_RESULT
	compare VAR_RESULT, 127
	goto_if_eq RyuPokeEditMenu
	msgbox RyuIvEv_ChooseSlot
	call RyuBufferPartyMonNames
	multichoice 0, 0, MULTI_DYNAMIC_6, 1
	copyvar VAR_0x8001, VAR_RESULT
	specialvar VAR_TEMP_A, CheckIfSelectedSlotIsValid
	compare VAR_TEMP_A, FALSE
	goto_if_eq RyuPokeEditMenu
	msgbox ResetEVIVQuery, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RyuPokeEditMenu
	callnative RyuResetIvEvs
	addvar VAR_TEMP_2, 1
	buffernumberstring 0, VAR_0x8001
	msgbox ResetDone
	goto RyuPokeEditMenu

RIVEV1:
	.string "Reset IV's$"

RIVEV2:
	.string "Reset EV's$"

RIVEV3:
	.string "Return$"

ResetEVIVQuery:
	.string "Perform reset operation?$"

ResetDone:
	.string "Reset Iv's or Ev's for slot {STR_VAR_1}.$"

RyuAccessDevMoveRelearner::
	msgbox RyuActivateRelearner MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RyuPokeEditMenu
	additem ITEM_HEART_SCALE, 1
	setflag FLAG_TEMP_4
	call FallarborTown_MoveRelearnersHouse_EventScript_ChooseMon
	goto RyuAskToUseTutorAgain

RyuAskToUseTutorAgain::
	msgbox retutor MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RyuPokeEditMenu
	goto RyuAccessDevMoveRelearner
	end

RyuActivateRelearner:
	.string "Activate the move relearner?$"

retutor:
	.string "Relearn another move?$"

RyuSetIvEvDebugMenu::
	msgbox RyuIvEv_SelectOP
	bufferdynamicmulti IVEVMENU_1, IVEVMENU_2, EVMENU_3
	multichoice 0, 3, MULTI_DYNAMIC_3, 1
	copyvar VAR_0x8003, VAR_RESULT @..................Which Mode
	compare VAR_RESULT, 2
	goto_if_ge RyuPokeEditMenu
	msgbox RyuIvEv_ChooseSlot
	call RyuBufferPartyMonNames
	multichoice 0, 0, MULTI_DYNAMIC_6, 1
	copyvar VAR_0x8001, VAR_RESULT @...................Which Slot
	specialvar VAR_TEMP_A, CheckIfSelectedSlotIsValid
	compare VAR_TEMP_A, FALSE
	goto_if_eq RyuIvEv_ExitQuery
	msgbox RyuIvEv_SetWhichStat
	bufferdynamicmulti RyuEVHP, RyuEVAtk, RyuEVDef, RyuEVSpeed, RyuEVSpAtk, RyuEVSpDef
	multichoice 0, 0, MULTI_DYNAMIC_6, 0
	copyvar VAR_0x8002, VAR_RESULT @...................Which stat
	closemessage
	compare VAR_RESULT, 127
	goto_if_eq RyuIvEv_DoMaximum
	msgbox RyuIvEv_ChooseValue
	numberentry 5, 10, 3
	copyvar VAR_0x8000, VAR_RESULT @...................The value to set
	callnative RyuSetSlotStatIVEV
	msgbox IvMenuDone
	closemessage
	call ShowMonDataInfoBox @..........................Should use gSpecialVar_0x8001 by default
	goto RyuIvEv_ExitQuery

RyuIvEv_DoMaximum::@...................user made no stat selection, since this is dev menu, assume user wants all stats.
	msgbox RyuIvEv_ChooseValue
	numberentry 5, 10, 3
	copyvar VAR_0x8000, VAR_RESULT @...................The value to set
	callnative RyuSetSlotStatIVEV
	msgbox IvMenuDone
	closemessage
	call ShowMonDataInfoBox @..........................Should use gSpecialVar_0x8001 by default
	goto RyuIvEv_ExitQuery


RyuIvEv_ExitQuery::
	msgbox RyuIvEv_Cancel, MSGBOX_YESNO
	closemessage
	compare VAR_RESULT, YES
	goto_if_eq RyuDebug_menu_nodevcheck
	goto RyuPokeEditMenu

RyuIvEv_Cancel:
	.string "Return to previous menu?$"

IVEVMENU_2:
	.string "Set IVs$"

IVEVMENU_1:
	.string "Set EVs$"

IvMenuDone:
	.string "Stats Changed.$"

RyuIvEv_ChooseValue:
	.string "Enter the new stat value.$"

RyuIvEv_SelectOP:
	.string "Select Operation.$"

RyuIvEv_ChooseSlot:
	.string "Choose a party slot.$"

RyuIvEv_SetWhichStat:
	.string "Modify which stat?$"

EvResetQuery:
	.string "Set all EV's to 0?$"
	
RyuEVHP:
	.string "HP$"

RyuEVAtk:
	.string "Attack$"

RyuEVDef:
	.string "Defense$"

RyuEVSpAtk:
	.string "SpcAtk$"

RyuEVSpDef:
	.string "SpcDef$"

RyuEVSpeed:
	.string "Speed$"
	
RyuSetFriendshipDebugMenu::
	callnative RyuMaxHappiness
	msgbox RyuSetFriendshipMsg MSGBOX_DEFAULT
	closemessage
	goto RyuPokeEditMenu
	
RyuSetFriendshipMsg:
	.string "Lead mon loves you now.$"
	
RyuSetExpMultMenu::
	special RyuBufferDifficultyValue
	msgbox RyuCurrentExpMult MSGBOX_DEFAULT
	bufferdynamicmulti RDIFF1, RDIFF2, RDIFF3, RDIFF4, RDIFF5, RDIFF6
	multichoicedefault 5, 0, MULTI_DYNAMIC_6, 0, 0
	compare VAR_RESULT, 4
	goto_if_eq RyuChangeExpMult
	compare VAR_RESULT, 5
	goto_if_eq RyuResetExpMult
	copyvar VAR_RYU_DIFFICULTY, VAR_RESULT
	goto RyuDebug_UtilitiesMenu

RDIFF1:
	.string "Easy$"

RDIFF2:
	.string "Normal$"

RDIFF3:
	.string "Hard$"

RDIFF4:
	.string "HARDCORE$"

RDIFF5:
	.string "Only Change Exp Multiplier$"

RDIFF6:
	.string "Revert to default$"

RyuChangeExpMult::
	msgbox RDIFF_ChangeExpInfo
	numberentry 0, 10, 1
	copyvar VAR_RYU_DEV_EXP_MULT, VAR_RESULT
	buffernumberstring 0, VAR_RYU_DEV_EXP_MULT
	msgbox RDB_ExpSetTo
	closemessage
	setflag FLAG_RYU_DEV_EXP_MULT_ENABLED
	goto RyuDebug_UtilitiesMenu

RyuResetExpMult::
	msgbox RDIFF_DefaultEXPInfo
	closemessage
	clearflag FLAG_RYU_DEV_EXP_MULT_ENABLED
	closemessage
	goto RyuDebug_UtilitiesMenu

RDB_ExpSetTo:
	.string "Your exp multiplier was set to {STR_VAR_1}.$"

RDIFF_ChangeExpInfo:
	.string "Choose a new value for your\nEXP multiplier.$"

RyuExitExpMult::
	msgbox ExpMultNoChange
	goto RyuDebug_UtilitiesMenu

ExpMultNoChange:
	.string "Difficulty not changed.$"

RDIFF_DefaultEXPInfo:
	.string "EXP multiplier will now be set\nto your difficulty's multiplier.$"

RyuCurrentExpMult:
	.string "The current difficulty is {COLOR LIGHT_RED}{SHADOW RED}{STR_VAR_1}$"

RyuDebug_SpawningMenu::
	bufferdynamicmulti RDBSP_1, RDBSP_2, RDBSP_3, RDBSP_4, RDBSP_5, RDBSP_6
	multichoice 0, 0, MULTI_DYNAMIC_6, 1
	switch VAR_RESULT
	case 0, FrontierMonDev
	case 1, DevModeMonGive
	case 2, DevModeItemGive
	case 3, RDB_SpawnFollowerMultiMenu
	case 4, RDB_GiveMoneyMenu
	case 5, RyuDebug_menu_nodevcheck
	goto RyuDebug_menu_nodevcheck

RDBSP_1:
	.string "Spawn Frontier Mon...$"

RDBSP_2:
	.string "Spawn Species ID...$"

RDBSP_3:
	.string "Spawn Item ID...$"

RDBSP_4:
	.string "Spawn Follower...$"

RDBSP_5:
	.string "Give Money...$"

RDBSP_6:
	.string "Return$"

FrontierMonDev::
	msgbox DMFMG
	numberentry 0, 9, 4
	compare VAR_RESULT, NUM_FRONTIER_MONS
	goto_if_ge RDB_CancelSpawn
	compare VAR_RESULT, 0
	goto_if_eq RDB_CancelSpawn
	copyvar VAR_TEMP_6, VAR_RESULT
	setflag FLAG_RYU_PASSCODE_FRONTIER_MON_DEV
	special RyuGiveFrontierMon
	msgbox FMSpawnMenuSuccess
	clearflag FLAG_RYU_PASSCODE_FRONTIER_MON_DEV
	closemessage
	goto RyuDebug_SpawningMenu

FMSpawnMenuSuccess:
	.string "Succesfully spawned FID {RYU_STR_3}.$"

DevModeMonGive::
	msgbox DMMG
	numberentry 0, 9, 3
	compare VAR_RESULT, 0
	goto_if_eq RDB_CancelSpawn
	compare VAR_RESULT, NUM_SPECIES
	goto_if_gt RDB_CancelSpawn
	copyvar VAR_TEMP_4, VAR_RESULT
	bufferspeciesname 0, VAR_TEMP_4
	getpartysize
	copyvar VAR_TEMP_8, VAR_RESULT
	setflag FLAG_RYU_DEVELOPER_POKEMON_MENU
	call GCMS_NatureMenu
	clearflag FLAG_RYU_DEVELOPER_POKEMON_MENU
	msgbox SpawnMenuSuccess
	goto RyuDebug_SpawningMenu

DevModeItemGive::
	msgbox DMIG
	numberentry 0, 9, 3
	compare VAR_RESULT, 0
	goto_if_eq RDB_CancelSpawn
	compare VAR_RESULT, ITEMS_COUNT
	goto_if_ge RDB_CancelSpawn
	additem VAR_RESULT, 1000
	msgbox SpawnMenuSuccess
	goto RyuDebug_SpawningMenu

RDB_GiveMoneyMenu::
	numberentry 0, 9, 9
	callnative RDB_DevModeGiveMoney
	msgbox RDB_GMM_Msg
	goto RyuDebug_menu_nodevcheck

RDB_CancelSpawn::
	msgbox RDB_SpawnCancelled
	closemessage
	goto RyuDebug_menu_nodevcheck

RDB_GMM_Msg:
	.string "Gave {PLAYER} ¥{STR_VAR_1}.$"

DMIG:
	.string "Enter Item ID.$"

DMMG:
	.string "Enter Species ID$"

DMFMG:
	.string "Enter Frontier Mon ID.$"

RDB_SpawnCancelled:
	.string "Invalid Entry.\nReturning to main menu$"

SpawnMenuSuccess:
	.string "Operation Finished.$"

RyuDebug_BattleSim::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_ne RDB_BattleSimFemale
	cleartrainerflag TRAINER_DOPPELGANGER_M
	trainerbattle_single TRAINER_DOPPELGANGER_M, BS_Intro, BS_Defeat
	waitstate
	release
	end

RDB_BattleSimFemale::
	cleartrainerflag TRAINER_DOPPELGANGER_F
	trainerbattle_single TRAINER_DOPPELGANGER_F, BS_Intro, BS_Defeat
	waitstate
	release
	end

BS_Defeat:
	.string "Simualtion over.$"

BS_Intro:
	.string "Starting Simulation.$"


RyuDebug_ReadoutRGBTheme:
	callnative printThemeColors
	msgbox rdbrr
	release
	end

rdbrr:
	.string "Refer to mGBA debug to see values.$"

RyuDebug_CheckRyuChallengeStatus::
	callnative RyuCheckSpecialChallengeStatus
	callnative RyuLoadPlayTimeChallenge
	msgbox RDB_SCS
	release
	end

RDB_SCS:
	.string "Finished play time: {RYU_STR_1}:{RYU_STR_2}:{RYU_STR_3}\p"
	.string "Your challenge took {RYU_STR_4} minutes.\p"
	.string "Speed options used:\p"
	.string "{STR_VAR_1}\p"
	.string "{COLOR LIGHT_GREEN}{SHADOW GREEN}({COLOR DARK_GREY}{SHADOW LIGHT_GREY} bs - {COLOR LIGHT_RED}{SHADOW RED}Bar Speed{COLOR DARK_GREY}{SHADOW LIGHT_GREY}, 1c - {COLOR LIGHT_RED}{SHADOW RED}100 cap{COLOR DARK_GREY}{SHADOW LIGHT_GREY},\n"
	.string " its - {COLOR LIGHT_RED}{SHADOW RED}instant transition{COLOR DARK_GREY}{SHADOW LIGHT_GREY},\n"
	.string " da - {COLOR LIGHT_RED}{SHADOW RED}disabled anims{COLOR LIGHT_GREEN}{SHADOW GREEN})$"

RyuDebug_CheckExactPlayTime::
	callnative RyuBufferLongPlayTimeString
	msgbox (RDB_TempVarsViewMsg)
	release
	end

RDB_VERBOSETOGGLED:
	.string "Verbose Mode Enabled.$"

RyuDebug_EnableVerboseUtility::
	goto_if_set FLAG_RYU_VERBOSE_MODE, RDB_DisableVerbose
	setflag FLAG_RYU_VERBOSE_MODE
	debugprint RDB_VERBOSETOGGLED, 120
	release
	end

RDB_DisableVerbose:
	clearflag FLAG_RYU_VERBOSE_MODE
	msgbox RDB_VBD
	release
	end

RDB_VBD:
	.string "Verbose mode disabled.$"

RyuScript_QuickNumberEntry::
	numberentry 15, 17, 3
	copyvar VAR_0x8001, VAR_RESULT
	return
