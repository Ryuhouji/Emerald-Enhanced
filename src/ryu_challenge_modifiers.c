#include "ryu_challenge_modifiers.h"
#include "event_data.h"
#include "random.h"
#include "battle.h"


/////////////////////////////////////////////////////////////////General challenge mod functions

void SetModFlag(u32 id)
{
    if(id > 63)
        return;
    gSaveBlock1Ptr->challengeFlags[id / 8] |= 1 << (id % 8);
}

bool32 GetModFlag(u32 id)
{
    u32 ret = 69;
    if(id > 63)
        return FALSE;
    return !!((gSaveBlock1Ptr->challengeFlags[id / 8] >> (id % 8)) & 1);
}

void ClearModFlag(u32 id)
{
    if(id > 63)
        return;
    gSaveBlock1Ptr->challengeFlags[id / 8] &= ~(1 << (id % 8));
}

void ToggleModFlag(u32 id)
{
    if (id > 63){
        return;
    }
    if (GetModFlag(id) == FALSE){
        SetModFlag(id);
    }
    else {
        ClearModFlag(id);
    }
}

void ResetModFlags(void)
{
    memset(gSaveBlock1Ptr->challengeFlags, 0, NUM_CHALLENGE_FLAG_BYTES);
}


//////////////////////////////////////////////////////////////////Nuzlocke

void SetNuzlockeFlag(u32 id)
{
    if(id > MAPSEC_NONE)
        return;
    gSaveBlock1Ptr->nuzlockeMapsecs[id / 8] |= 1 << (id % 8);
    
}

bool32 GetNuzlockeFlag(u32 id)
{
    if(id > MAPSEC_NONE)
        return FALSE;

    return !!((gSaveBlock1Ptr->nuzlockeMapsecs[id / 8] >> (id % 8)) & 1);
}

void RemoveNuzlockeFlag(u32 id)
{
    if(id > MAPSEC_NONE)
        return;
        
    gSaveBlock1Ptr->nuzlockeMapsecs[id / 8] &= ~(1 << (id % 8));
}

void ResetNuzlockeFlags(void)
{
    memset(gSaveBlock1Ptr->nuzlockeMapsecs, 0, NUM_NUZLOCKE_MAPSEC_BYTES);
}

//////////////////////////////////////////////////////////////////End Nuzlocke
//////////////////////////////////////////////////////////////////Revelation mods (per minute)

const u16 randomStatuses[] = {
    STATUS1_BURN,
    STATUS1_FREEZE,
    STATUS1_PARALYSIS,
    STATUS1_POISON,
    STATUS1_TOXIC_POISON,
    STATUS1_SLEEP
};

void TryRevelationModPenalties(void)
{
    u8 partySlot = (Random() % CalculatePlayerPartyCount());
    u16 deathcount = 1;
    u8 ppcount = 4;
    u8 plaguestatus = randomStatuses[Random() % ARRAY_COUNT(randomStatuses)];

    if ((GetModFlag(PLAGUE_MOD)) || (GetModFlag(REVELATION_MOD)))
    {
        if (Random() % 100 < 13){
            SetMonData(&gPlayerParty[partySlot], MON_DATA_STATUS, &plaguestatus);
        }
    }

    if ((GetModFlag(FAMINE_MOD)) || (GetModFlag(REVELATION_MOD)))
    {
        u8 ppslot = (Random() % 4);
        if (ppslot > 4){ppslot = 4;}
        if (Random() % 100 < 20){
            SetMonData(&gPlayerParty[partySlot], MON_DATA_PP1 + ppslot, &ppcount);
        }
    }

    if ((GetModFlag(DEATH_MOD)) || (GetModFlag(REVELATION_MOD)))
    {
        if (Random() % 100 < 10)
        {
            SetMonData(&gPlayerParty[partySlot], MON_DATA_HP, &deathcount);
        }
    }

    if ((GetModFlag(WAR_MOD)) || (GetModFlag(REVELATION_MOD)))
    {
        if (Random() % 100 < 50)
        {
            u16 curHP = GetMonData(&gPlayerParty[partySlot], MON_DATA_HP);
            curHP /= 2;
            if ((curHP < 1) || (curHP > 2000)){
                curHP = 1;
            }
            SetMonData(&gPlayerParty[partySlot], MON_DATA_HP, &curHP);
        }
    }

}

/////////////////////////////////////////////////////////////////////////////////End revelation

////////////////////////General list for mod names/////////////////////////////////////////////

const u8 gRyuChallengeModifierNames[17][16] = {
    _("Nuzlocke"),
    _("Monotype"),
    _("Adv.Mono"),
    _("Plague"),
    _("Famine"),
    _("Death"),
    _("War"),
    _("Revelation"),
    _("Gen 1 Mode"),
    _("Late Evolve"),
    _("Magneto"),
    _("Lazy"),
    _("Sickly"),
    _("Greedy Trainers"),
    _("Technician"),
    _("Economy Mode"),
    _("No Evolve")
};