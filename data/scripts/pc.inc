EventScript_PC:: @ 8271D92
	lockall
	setvar VAR_0x8004, 0
	special DoPCTurnOnEffect
	compare VAR_RYU_DIFFICULTY, DIFF_NOMERCY
	call_if_eq PC_NoMercyMode
	playse SE_PC_ON
	msgbox Text_BootUpPC, MSGBOX_DEFAULT
	goto EventScript_PCMainMenu
	end

PC_NoMercyMode::
	msgbox nomercysave, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_ne ryu_return
	msgbox nomercysavingwait
	callnative(RyuForceSaveTheGame)
	closemessage
	msgbox nomercysavingcomplete
	closemessage
	return

nomercysave:
	.string "Would you like to save the game?$"

nomercysavingwait:
	.string "Now saving the game.\nPlease wait!$"

nomercysavingcomplete:
	.string "Game successfully saved.$"

EventScript_PCMainMenu:: @ 8271DAC
	goto_if_set FLAG_RYU_DOING_RYU_CHALLENGE, PC_ChallengeMode
	message gText_WhichPCShouldBeAccessed
	waitmessage
	special ScriptMenu_CreatePCMultichoice
	waitstate
	goto EventScript_AccessPC
	end

PC_ChallengeMode::
	msgbox challengemodepc
	closemessage
	release
	end

challengemodepc:
	.string "You cannot use this PC\ndue to Ryu's Challenge.$"


EventScript_AccessPC:: @ 8271DBC
	switch VAR_RESULT
	case 0, EventScript_AccessPokemonStorage
	case 1, EventScript_AccessPlayersPC
	case 2, EventScript_AccessHallOfFame
	case 3, EventScript_TurnOffPC
	case MULTI_B_PRESSED, EventScript_TurnOffPC
	end

EventScript_AccessPlayersPC:: @ 8271DF9
	playse SE_PC_LOGIN
	modflag GET ECONOMY_MODE
	compare VAR_RESULT, TRUE
	call_if_eq RyuEconomyModeItemStorage
	msgbox gText_AccessedPlayersPC, MSGBOX_DEFAULT
	special PlayerPC
	waitstate
	goto EventScript_PCMainMenu
	end

EventScript_AccessPokemonStorage:: @ 8271E0E
	modflag GET ECONOMY_MODE
	compare VAR_RESULT, TRUE
	call_if_eq RyuEconomyModePokemonStorage
	getpartysize
	compare VAR_RESULT, 0
	goto_if_eq EventscriptPC_NoMons
	playse SE_PC_LOGIN
	call_if_unset FLAG_SYS_PC_LANETTE, EventScript_AccessSomeonesPC
	call_if_set FLAG_SYS_PC_LANETTE, EventScript_AccessLanettesPC
	msgbox gText_StorageSystemOpened, MSGBOX_DEFAULT
	special ShowPokemonStorageSystemPC
	waitstate
	goto EventScript_PCMainMenu
	end

EventscriptPC_NoMons::
	playse SE_HAZURE
	msgbox nomons
	releaseall
	end

nomons:
	.string "You don't have any pokemon.$"

EventScript_AccessSomeonesPC:: @ 8271E35
	msgbox gText_AccessedSomeonesPC, MSGBOX_DEFAULT
	return

EventScript_AccessLanettesPC:: @ 8271E3E
	msgbox gText_AccessedLanettesPC, MSGBOX_DEFAULT
	return

EventScript_TurnOffPC:: @ 8271E47
	setvar VAR_0x8004, 0
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	end

EventScript_AccessHallOfFame:: @ 8271E54
	goto_if_unset FLAG_SYS_GAME_CLEAR, EventScript_TurnOffPC
	playse SE_PC_LOGIN
	special AccessHallOfFamePC
	waitstate
	goto EventScript_AccessPC
	end
