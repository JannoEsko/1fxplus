// Copyright (C) 2001-2002 Raven Software
//
// bg_weapons.c - weapon data loading

#include "../qcommon/q_shared.h"
#include "bg_public.h"
#include "bg_local.h"
#include "g_local.h"

// names as they appear in the SOF2.wpn and inview files
char *bg_weaponNames[WP_MAX_WEAPONS] = {0};
char *bg_enumWeaponNames[WP_MAX_WEAPONS] = {0};
char *ammoNames[WP_MAX_AMMO] = {0};
char *modNames[WP_MAX_MODS] = {0};

weaponData_t weaponData[WP_MAX_WEAPONS];
ammoData_t ammoData[WP_MAX_AMMO];

void BG_InitializeWeaponsAndAmmo()
{
    bg_weaponNames[0] = "No Weapon";    // WP_NONE
    bg_weaponNames[1] = "Knife";        // WP_KNIFE
    bg_weaponNames[2] = "M1911A1";      // WP_M1911A1_PISTOL
    bg_weaponNames[3] = "US SOCOM";     // WP_US_SOCOM_PISTOL
    bg_weaponNames[4] = "Silver Talon"; // WP_SILVER_TALON
    bg_weaponNames[5] = "M590";         // WP_M590_SHOTGUN
    bg_weaponNames[6] = "Micro Uzi";    // WP_MICRO_UZI_SUBMACHINEGUN
    bg_weaponNames[7] = "M3A1";         // WP_M3A1_SUBMACHINEGUN
    bg_weaponNames[8] = "MP5";          // WP_MP5
    bg_weaponNames[9] = "USAS-12";      // WP_USAS_12_SHOTGUN
    bg_weaponNames[10] = "M4";          // WP_M4_ASSAULT_RIFLE
    bg_weaponNames[11] = "AK74";        // WP_AK74_ASSAULT_RIFLE
    bg_weaponNames[12] = "Sig 551";     // WP_SIG551
    bg_weaponNames[13] = "MSG90A1";     // WP_MSG90A1_SNIPER_RIFLE
    bg_weaponNames[14] = "M60";         // WP_M60_MACHINEGUN
    bg_weaponNames[15] = "MM1";         // WP_MM1_GRENADE_LAUNCHER
    bg_weaponNames[16] = "RPG7";        // WP_RPG7_LAUNCHER
    bg_weaponNames[17] = "M84";         // WP_M84_GRENADE
    bg_weaponNames[18] = "SMOHG92";     // WP_SMOHG92_GRENADE
    bg_weaponNames[19] = "ANM14";       // WP_ANM14_GRENADE
    bg_weaponNames[20] = "M15";         // WP_M15_GRENADE

    bg_enumWeaponNames[0] = "WP_NONE";
    bg_enumWeaponNames[1] = "WP_KNIFE";
    bg_enumWeaponNames[2] = "WP_M1911A1_PISTOL";
    bg_enumWeaponNames[3] = "WP_US_SOCOM_PISTOL";
    bg_enumWeaponNames[4] = "WP_SILVER_TALON";
    bg_enumWeaponNames[5] = "WP_M590_SHOTGUN";
    bg_enumWeaponNames[6] = "WP_MICRO_UZI_SUBMACHINEGUN";
    bg_enumWeaponNames[7] = "WP_M3A1_SUBMACHINEGUN";
    bg_enumWeaponNames[8] = "WP_MP5";
    bg_enumWeaponNames[9] = "WP_USAS_12_SHOTGUN";
    bg_enumWeaponNames[10] = "WP_M4_ASSAULT_RIFLE";
    bg_enumWeaponNames[11] = "WP_AK74_ASSAULT_RIFLE";
    bg_enumWeaponNames[12] = "WP_SIG551";
    bg_enumWeaponNames[13] = "WP_MSG90A1_SNIPER_RIFLE";
    bg_enumWeaponNames[14] = "WP_M60_MACHINEGUN";
    bg_enumWeaponNames[15] = "WP_MM1_GRENADE_LAUNCHER";
    bg_enumWeaponNames[16] = "WP_RPG7_LAUNCHER";
    bg_enumWeaponNames[17] = "WP_M84_GRENADE";
    bg_enumWeaponNames[18] = "WP_SMOHG92_GRENADE";
    bg_enumWeaponNames[19] = "WP_ANM14_GRENADE";
    bg_enumWeaponNames[20] = "WP_M15_GRENADE";

    ammoNames[0] = "Knife";         //  AMMO_KNIFE
    ammoNames[1] = "0.45 ACP";      //  AMMO_045
    ammoNames[2] = "5.56mm";        //  AMMO_556
    ammoNames[3] = "9mm";           //  AMMO_9
    ammoNames[4] = "12 gauge";      //  AMMO_12
    ammoNames[5] = "7.62mm";        //  AMMO_762
    ammoNames[6] = "40mm grenade";  //  AMMO_40
    ammoNames[7] = "RPG7";          //  AMMO_RPG7
    ammoNames[8] = "M15";           //  AMMO_M15
    ammoNames[9] = "M84";           //  AMMO_M84
    ammoNames[10] = "SMOHG92";      //  AMMO_SMOHG92
    ammoNames[11] = "ANM14";        //  AMMO_ANM14
    ammoNames[12] = "7.62mm belt";  //  AMMO_762_BELT
    ammoNames[13] = "9mm|mp5";      //  AMMO_9_MP5

    // Boe!Man 7/27/15: Check for 1fx. Client Additions, and if so, add the remaining weapons.
    if (g_enforce1fxAdditions.integer) {
        bg_weaponNames[21] = "M67";         // WP_M67_GRENADE
        bg_weaponNames[22] = "F1";          // WP_F1_GRENADE
        bg_weaponNames[23] = "L2A2";        // WP_L2A2_GRENADE
        bg_weaponNames[24] = "MDN11";       // WP_MDN11_GRENADE

        bg_enumWeaponNames[21] = "WP_M67_GRENADE";
        bg_enumWeaponNames[22] = "WP_F1_GRENADE";
        bg_enumWeaponNames[23] = "WP_L2A2_GRENADE";
        bg_enumWeaponNames[24] = "WP_MDN11_GRENADE";

        if(!Q_stricmp(g_realGametype.string, "hns")){
            // Boe!Man 11/9/15: Don't cross any hard-defined bit fields for ammo types. Re-use some to stay under the hard limit of MAX_AMMO.
            ammoNames[12] = "F1";       //  AMMO_F1
            ammoNames[13] = "MDN11";    //  AMMO_MDN11
        }
        ammoNames[14] = "M67";          //  AMMO_M67
        ammoNames[15] = "L2A2";         //  AMMO_L2A2
    }

    modNames[0] = "Unknown";
    modNames[1] = "Knife";
    modNames[2] = "M1911A1";
    modNames[3] = "US SOCOM";
    modNames[4] = "Silver Talon";
    modNames[5] = "M590";
    modNames[6] = "Micro Uzi";
    modNames[7] = "M3A1";
    modNames[8] = "MP5";
    modNames[9] = "USAS-12";
    modNames[10] = "M4";
    modNames[11] = "AK74";
    modNames[12] = "Sig 551";
    modNames[13] = "M1911A1";
    modNames[14] = "M60";
    modNames[15] = "MM1";
    modNames[16] = "RPG7";
    modNames[17] = "M84";
    modNames[18] = "SMOHG92";
    modNames[19] = "ANM14";
    modNames[20] = "M15";
    modNames[21] = "M67";
    modNames[22] = "F1";
    modNames[23] = "L2A2";
    modNames[24] = "MDN11";
    modNames[25] = "Water";
    modNames[26] = "Crushing entity";
    modNames[27] = "Telefrag";
    modNames[28] = "Falling";
    modNames[29] = "Sucicide";
    modNames[30] = "Team change";
    modNames[31] = "Target Laser";
    modNames[32] = "Trigger hurt entity";
    modNames[33] = "Trigger hurt entity (no suicide)";
    modNames[34] = "Car";
    modNames[35] = "Pop";
    modNames[36] = "Refresh";
    modNames[37] = "Plant up";
    modNames[38] = "Burn";
}


static const char* BG_GetRealAmmoName ( ammo_t ammoNum )
{
    static char name[64] = "";
    char* or;

    or = strchr ( ammoNames[ammoNum], '|' );
    if ( or )
    {
        Q_strncpyz ( name, ammoNames[ammoNum], or - ammoNames[ammoNum] + 1);
    }
    else
    {
        strcpy ( name, ammoNames[ammoNum] );
    }

    return name;
}

static qboolean BG_ParseAmmoStats(ammo_t ammoNum, void *group)
{
    char        tmpStr[256];
    ammoData_t *ammo;       // ammo

    ammo = &ammoData[ammoNum];
    memset(ammo, 0, sizeof(ammoData_t));

    ammo->name = (char*)G_StringAlloc ( BG_GetRealAmmoName ( ammoNum ) );
    Q_strlwr ( ammo->name );

    // Get the scale of the gore for this bullet
    trap_GPG_FindPairValue(group, "mp_goreScale||goreScale", "1", tmpStr, sizeof(tmpStr) );
    ammo->goreScale = atof ( tmpStr );

    // Max ammo will be filled in by the weapon parsing

    return qtrue;
}

qboolean BG_InitAmmoStats(void)
{
    void        *GP2, *topGroup;
    int         i;

    ammoData[AMMO_NONE].goreScale = 0.0f;
    ammoData[AMMO_NONE].name = "none";

    GP2 = trap_GP_ParseFile("ext_data/sof2.ammo" );
    if (!GP2)
    {
        return qfalse;
    }

    topGroup = trap_GP_GetBaseParseGroup(GP2);
    for ( i = 0; i < level.ammoMax; i ++ )
    {
        void*   topSubs;
        const char* realName;

        realName = BG_GetRealAmmoName ( i );

        topSubs = trap_GPG_GetSubGroups(topGroup);
        while(topSubs)
        {
            char name[256];
            trap_GPG_GetName(topSubs, name, sizeof(name));
            if (Q_stricmp(name, "ammo") != 0)
            {
                continue;
            }

            trap_GPG_FindPairValue(topSubs, "name", "", name, sizeof(name));
            if ( !Q_stricmp ( name, realName ) )
            {
                BG_ParseAmmoStats(i, topSubs );
                break;
            }

            topSubs = trap_GPG_GetNext(topSubs);
        }

        if ( !topSubs )
        {
            Com_Printf("BG_InitAmmoStats: Unknown ammo: %s\n", BG_GetRealAmmoName ( i ) );
        }
    }

    trap_GP_Delete(&GP2);

    return qtrue;
}

static qboolean BG_ParseAttackStats ( int weaponNum, attackData_t* attack, void *attacksub, qboolean pickupsDisabled )
{
    void*   sub;
    char    tmpStr[256];
    int     i;

    // No group is success.  This is to allow NULL to be passed
    if ( NULL == attacksub )
    {
        return qtrue;
    }

    // Zoom information
    trap_GPG_FindPairValue(attacksub, "action", "", tmpStr, sizeof(tmpStr));
    if ( !Q_stricmp ( tmpStr, "toggleZoom" ) )
    {
        weaponData_t *weapon;
        void         *value;
        int          zoomlvl;

        weapon = &weaponData[weaponNum];

        sub = trap_GPG_FindSubGroup(attacksub, "zoomFactors");
        if ( !sub )
        {
            return qfalse;
        }

        value = trap_GPG_GetPairs(sub);
        zoomlvl = 0;
        while(value)
        {
            trap_GPV_GetName ( value, weapon->zoom[zoomlvl].name, sizeof(weapon->zoom[zoomlvl].name) );
            trap_GPV_GetTopValue(value, tmpStr, sizeof(tmpStr) );

            weapon->zoom[zoomlvl].fov = atoi ( tmpStr );

            value = trap_GPV_GetNext ( value );
            zoomlvl ++;
        }

        return qtrue;
    }

    // Assign a melee attribute if there is one
    trap_GPG_FindPairValue(attacksub, "mp_melee||melee", "none", tmpStr, sizeof(tmpStr) );
    if ( Q_stricmp ( tmpStr, "none" ) )
    {
        Q_strlwr ( tmpStr );
        attack->melee = G_StringAlloc ( tmpStr );
    }

    trap_GPG_FindPairValue(attacksub, "name", "NONE", attack->name, sizeof(attack->name));
    trap_GPG_FindPairValue(attacksub, "hudIcon", "NONE", attack->icon, sizeof(attack->icon));

    if ( pickupsDisabled )
    {
        trap_GPG_FindPairValue(attacksub, "mp_ammoType_outfitting", "", tmpStr, sizeof(tmpStr));
        if ( !tmpStr[0] )
        {
            trap_GPG_FindPairValue(attacksub, "mp_ammoType||ammoType", "none", tmpStr, sizeof(tmpStr));
        }
    }
    else
    {
        trap_GPG_FindPairValue(attacksub, "mp_ammoType||ammoType", "none", tmpStr, sizeof(tmpStr));
    }

    attack->ammoIndex = AMMO_NONE;
    for (i = 0; i < level.ammoMax; i++)
    {
        if (0 == Q_stricmp(tmpStr, ammoNames[i]))
        {
            attack->ammoIndex = i;
            break;
        }
    }

#ifndef NDEBUG
    if (level.ammoMax == i)
    {
        Com_Printf("BG_ParseWeaponStats: Unknown ammo: %s\n", tmpStr);
    }
#endif // !NDEBUG

    // Parse the weapon animations
    trap_GPG_FindPairValue(attacksub, "mp_animFire", "TORSO_ATTACK_PISTOL", tmpStr, sizeof(tmpStr));
    attack->animFire = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue(attacksub, "mp_animFireZoomed", "", tmpStr, sizeof(tmpStr));
    attack->animFireZoomed = GetIDForString ( bg_animTable, tmpStr );

    trap_GPG_FindPairValue(attacksub, "mp_range||range", "8192", tmpStr, sizeof(tmpStr));
    attack->rV.range = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_radius||radius", "0", tmpStr, sizeof(tmpStr));
    attack->splashRadius = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_fireDelay||fireDelay", "0", tmpStr, sizeof(tmpStr));
    attack->fireDelay = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_clipSize||clipSize", "0", tmpStr, sizeof(tmpStr));
    attack->clipSize = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_fireAmount||fireAmount", "1", tmpStr, sizeof(tmpStr));
    attack->fireAmount = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_fireFromClip||fireFromClip", "1", tmpStr, sizeof(tmpStr));
    attack->fireFromClip = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_damage||damage", "0", tmpStr, sizeof(tmpStr));
    attack->damage = atoi(tmpStr);
    trap_GPG_FindPairValue(attacksub, "mp_inaccuracy||inaccuracy", "0", tmpStr, sizeof(tmpStr));
    attack->inaccuracy = (int)(atof(tmpStr)*1000.0f);
    trap_GPG_FindPairValue(attacksub, "mp_zoominaccuracy", "0", tmpStr, sizeof(tmpStr));
    attack->zoomInaccuracy = (int)(atof(tmpStr)*1000.0f);
    trap_GPG_FindPairValue(attacksub, "mp_maxInaccuracy||maxInaccuracy", "0", tmpStr, sizeof(tmpStr));
    attack->maxInaccuracy = (int)(atof(tmpStr)*1000.0f);
    trap_GPG_FindPairValue(attacksub, "mp_gore||gore", "YES", tmpStr, sizeof(tmpStr));
    attack->gore = (Q_stricmp(tmpStr, "YES") ? qfalse : qtrue);

    trap_GPG_FindPairValue(attacksub, "mp_extraClips", "0", tmpStr, sizeof(tmpStr));
    attack->extraClips = atoi(tmpStr);

    // max ammo is the combination of all guns that share the ammo
    ammoData[attack->ammoIndex].max += attack->clipSize * attack->extraClips;

    trap_GPG_FindPairValue(attacksub,"mp_kickAngles||kickAngles", "0 0 0 0 0 0", tmpStr, sizeof(tmpStr));
    sscanf( tmpStr, "%f %f %f %f %f %f",
            &attack->minKickAngles[0],
            &attack->maxKickAngles[0],
            &attack->minKickAngles[1],
            &attack->maxKickAngles[1],
            &attack->minKickAngles[2],
            &attack->maxKickAngles[2]  );

    if (0 == attack->inaccuracy)
    {
        trap_GPG_FindPairValue(attacksub, "mp_spread||spread", "0", tmpStr, sizeof(tmpStr));
        attack->inaccuracy = atof(tmpStr);
    }
    trap_GPG_FindPairValue(attacksub, "mp_pellets||pellets", "1", tmpStr, sizeof(tmpStr));
    attack->pellets = atof(tmpStr);
    attack->mod = (meansOfDeath_t)weaponNum;

    trap_GPG_FindPairValue(attacksub, "mp_lockFlashToBarrel||lockFlashToBarrel", "true", tmpStr, sizeof(tmpStr));
    if (0 == Q_stricmp(tmpStr, "false"))
    {
        attack->weaponFlags |= UNLOCK_MUZZLEFLASH;
    }
    // load effects, sounds
    trap_GPG_FindPairValue(attacksub, "muzzleFlash", "", attack->muzzleEffect, sizeof(attack->muzzleEffect));
    trap_GPG_FindPairValue(attacksub, "3rdPersonMuzzleFlash", "", attack->muzzleEffectInWorld, sizeof(attack->muzzleEffectInWorld));
    trap_GPG_FindPairValue(attacksub, "EjectBone", "", attack->ejectBone, sizeof(attack->ejectBone));
    trap_GPG_FindPairValue(attacksub, "ShellCasingEject", "", attack->shellEject, sizeof(attack->shellEject));
    trap_GPG_FindPairValue(attacksub, "TracerEffect", "", attack->tracerEffect, sizeof(attack->tracerEffect));

    // Some alt attacks have special bones they need their muzzle flashes attached to
    trap_GPG_FindPairValue ( attacksub, "mp_muzzleFlashBone", "", attack->muzzleEffectBone, sizeof(attack->muzzleEffectBone) );

    sub = trap_GPG_FindSubGroup(attacksub, "fireModes");
    if (sub)
    {
        int i;

        for ( i = 0; i < 5; i ++ )
        {
            trap_GPG_FindPairValue ( sub, va("mp_mode%i||mode%i", i, i ), "", tmpStr, sizeof(tmpStr) );
            if ( !tmpStr[0] )
            {
                continue;
            }

            if (0 == Q_stricmp("single", tmpStr))
                attack->weaponFlags |= (1<<WP_FIREMODE_SINGLE);
            else if (0 == Q_stricmp("auto", tmpStr))
                attack->weaponFlags |= (1<<WP_FIREMODE_AUTO);
            else if (0 == Q_stricmp("burst", tmpStr))
                attack->weaponFlags |= (1<<WP_FIREMODE_BURST);
            else
                attack->weaponFlags |= (1<<WP_FIREMODE_SINGLE);
        }
    }
    else
    {
        attack->weaponFlags |= (1<<WP_FIREMODE_SINGLE);
    }

    sub = trap_GPG_FindSubGroup(attacksub, "projectile");
    if (sub)
    {
        attack->weaponFlags |= PROJECTILE_FIRE;

        trap_GPG_FindPairValue(sub, "gravity", "1", tmpStr, sizeof(tmpStr));
        if (0 < atof(tmpStr))
            attack->weaponFlags |= PROJECTILE_GRAVITY;

        trap_GPG_FindPairValue(sub, "detonation", "0", tmpStr, sizeof(tmpStr));
        if (0 == Q_stricmp(tmpStr,"timer"))
            attack->weaponFlags |= PROJECTILE_TIMED;

        trap_GPG_FindPairValue(sub, "mp_bounce||bounce", "0", tmpStr, sizeof(tmpStr) );
        attack->bounceScale = atof ( tmpStr );

        switch ( weaponNum )
        {
            case WP_ANM14_GRENADE:
                // incediary grenade
                attack->weaponFlags |= PROJECTILE_DAMAGE_AREA;
                break;

            case WP_KNIFE:
                if ( attack->weaponFlags & PROJECTILE_GRAVITY )
                {
                    attack->weaponFlags &= ~PROJECTILE_GRAVITY;
                    attack->weaponFlags |= PROJECTILE_LIGHTGRAVITY;
                }
                break;
        }
        trap_GPG_FindPairValue(sub, "mp_speed||speed", "0", tmpStr, sizeof(tmpStr));
        attack->rV.velocity = atoi(tmpStr);
        trap_GPG_FindPairValue(sub, "mp_timer||timer", "10", tmpStr, sizeof(tmpStr));
        attack->projectileLifetime = (int)(atof(tmpStr) * 1000);

        // 'trail' effect
        trap_GPG_FindPairValue(sub, "mp_effect||effect", "", attack->tracerEffect, sizeof(attack->tracerEffect));
        trap_GPG_FindPairValue(sub, "model", "", attack->missileG2Model, sizeof(attack->missileG2Model));
        trap_GPG_FindPairValue(sub, "mp_explosionEffect||explosionEffect", "", attack->explosionEffect, sizeof(attack->explosionEffect));
        trap_GPG_FindPairValue(sub, "mp_explosionSound||explosionSound", "", attack->explosionSound, sizeof(attack->explosionSound));
    }

    return qtrue;
}

static qboolean BG_ParseWeaponStats(weapon_t weaponNum, void *group, qboolean pickupsDisabled )
{
    char         tmpStr[256];
    weaponData_t *weapon;

    weapon = &weaponData[weaponNum];
    memset(weapon, 0, sizeof(weaponData_t));

    weapon->classname = bg_weaponNames[weaponNum];
    trap_GPG_FindPairValue(group, "category", "0", tmpStr, sizeof(tmpStr));
    weapon->category = atoi(tmpStr);

    trap_GPG_FindPairValue(group, "safe", "false", tmpStr, sizeof(tmpStr));
    weapon->safe = !Q_stricmp(tmpStr, "true");

    trap_GPG_FindPairValue(group, "model", "", weapon->worldModel, sizeof(weapon->worldModel));

    trap_GPG_FindPairValue(group, "menuImage", "", weapon->menuImage, sizeof(weapon->menuImage));

    // Grab the animations
    trap_GPG_FindPairValue( group, "mp_animRaise", "TORSO_RAISE", tmpStr, sizeof(tmpStr) );
    weapon->animRaise = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue( group, "mp_animDrop", "TORSO_DROP", tmpStr, sizeof(tmpStr) );
    weapon->animDrop = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue( group, "mp_animIdle", "TORSO_IDLE_PISTOL", tmpStr, sizeof(tmpStr) );
    weapon->animIdle = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue( group, "mp_animIdleZoomed", "", tmpStr, sizeof(tmpStr) );
    weapon->animIdleZoomed = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue( group, "mp_animReload", "", tmpStr, sizeof(tmpStr) );
    weapon->animReload = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue( group, "mp_animReloadStart", "", tmpStr, sizeof(tmpStr) );
    weapon->animReloadStart = GetIDForString ( bg_animTable, tmpStr );
    trap_GPG_FindPairValue( group, "mp_animReloadEnd", "", tmpStr, sizeof(tmpStr) );
    weapon->animReloadEnd = GetIDForString ( bg_animTable, tmpStr );

    // primary attack
    BG_ParseAttackStats ( weaponNum, &weapon->attack[ATTACK_NORMAL], trap_GPG_FindSubGroup(group, "attack"), pickupsDisabled );

    // alternate attack
    BG_ParseAttackStats ( weaponNum, &weapon->attack[ATTACK_ALTERNATE], trap_GPG_FindSubGroup(group, "altattack"), pickupsDisabled );

    return qtrue;
}

qboolean BG_InitWeaponStats( qboolean pickupsDisabled )
{
    void        *GP2, *topGroup, *topSubs;
    char        name[256];
    int         i;

    // the custom wpn file stuff was detached from this func.
    // we might not have the full context needed to load all the weapons in the custom weaponfile.
    // so the chain is that we init everything with this, the default value, after which we load the custom additions on top.
    GP2 = trap_GP_ParseFile("ext_data/sof2.wpn");

    if (!GP2)
    {
        return qfalse;
    }

    topGroup = trap_GP_GetBaseParseGroup(GP2);
    topSubs = trap_GPG_GetSubGroups(topGroup);
    while(topSubs)
    {
        trap_GPG_GetName(topSubs, name, sizeof(name));
        if (Q_stricmp(name, "weapon") == 0)
        {
            trap_GPG_FindPairValue(topSubs, "name", "", name, sizeof(name));
            for(i=0;i<level.wpNumWeapons;i++)
            {
                if (Q_stricmp(bg_weaponNames[i], name) == 0)
                {
                    BG_ParseWeaponStats(i, topSubs, pickupsDisabled );
                    break;
                }
            }

#ifndef NDEBUG
            if (i == level.wpNumWeapons)
            {
                Com_Printf("BG_InitWeaponStats: Unknown weapon: %s\n", name);
            }
#endif // !NDEBUG
        }
        topSubs = trap_GPG_GetNext(topSubs);
    }

    trap_GP_Delete(&GP2);

    return qtrue;
}

////////////////////////////////////////////////////////////////////////////////////

TWeaponParseInfo    weaponParseInfo[WP_MAX_WEAPONS];
char                weaponLeftHand[MAX_QPATH];
char                weaponRightHand[MAX_QPATH];

static char *BG_BuildSideSurfaceList(void *group, char *pattern, char *sideSurfaces[])
{
    void        *value;
    char        *output, *data;
    char        fieldName[256], fieldValue[256];
    int         length;
    int         i;

    output = G_Alloc(0);
    length = strlen(pattern);
    i=0;

    value = trap_GPG_GetPairs(group);
    while((value)&&(i<MAX_SIDE_SURFACES))
    {
        trap_GPV_GetName(value, fieldName, sizeof(fieldName));
        if (Q_stricmpn(fieldName, pattern, length) == 0)
        {
            trap_GPV_GetTopValue(value, fieldValue, sizeof(fieldValue));
            data = G_AllocUnaligned(strlen(fieldValue)+1);
            strcpy(data, fieldValue);
            sideSurfaces[i]=data;
            i++;
        }
        value = trap_GPV_GetNext(value);
    }

    data = G_AllocUnaligned(1);
    *data = 0;

    return output;
}

static char *BG_BuildList(void *group, char *pattern)
{
    void        *value;
    char        *output, *data;
    char        fieldName[256], fieldValue[256];
    int         length;

    output = G_Alloc(0);
    length = strlen(pattern);

    value = trap_GPG_GetPairs(group);
    while(value)
    {
        trap_GPV_GetName(value, fieldName, sizeof(fieldName));
        if (Q_stricmpn(fieldName, pattern, length) == 0)
        {
            trap_GPV_GetTopValue(value, fieldValue, sizeof(fieldValue));
            data = G_AllocUnaligned(strlen(fieldValue)+1);
            strcpy(data, fieldValue);
        }
        value = trap_GPV_GetNext(value);
    }

    data = G_AllocUnaligned(1);
    *data = 0;

    return output;
}

#define     MAX_WEAPON_FILES    10

static void *weaponFrames[MAX_WEAPON_FILES];
static void *frameGroup[MAX_WEAPON_FILES];
static int  numWeaponFiles = 0;
static int  numInitialFiles = 0;

static  qboolean BG_OpenWeaponFrames(const char *name)
{
    weaponFrames[numWeaponFiles] = trap_GP_ParseFile(name);

    if (!weaponFrames)
    {
        return qfalse;
    }

    frameGroup[numWeaponFiles] = trap_GP_GetBaseParseGroup(weaponFrames[numWeaponFiles]);
    numWeaponFiles++;

    return qtrue;
}

static TNoteTrack *BG_FindNoteTracks(void *group)
{
    void        *sub;
    char        name[256];
    TNoteTrack  *head, *last, *current, *insert;

    head = last = insert = 0;

    sub = trap_GPG_GetSubGroups(group);
    while(sub)
    {
        trap_GPG_GetName(sub, name, sizeof(name));
        if (Q_stricmp(name, "notetrack") == 0)
        {
            current = (TNoteTrack *)G_Alloc(sizeof(*current));
            memset(current, 0, sizeof(*current));

            // last character is automatically 0 cuz of the memset
            trap_GPG_FindPairValue(sub, "note", "", current->mNote, sizeof(current->mNote));
            trap_GPG_FindPairValue(sub, "frame", "-1", name, sizeof(name));
            current->mFrame = atoi(name);

            last=insert=head;
            while(insert)
            {
                if(current->mFrame<insert->mFrame)
                {
                    break;
                }
                last=insert;
                insert=insert->mNext;
            }
            if(insert==head)
            {
                head=current;
            }
            else
            {
                last->mNext=current;
            }
            current->mNext=insert;
        }

        sub = trap_GPG_GetNext(sub);
    }

    return head;
}

static void BG_FindWeaponFrames(TAnimInfoWeapon *animInfo, int choice)
{
    void    *group;
    int     i;

    if (!numWeaponFiles || !animInfo->mAnim[choice])
    {
        animInfo->mNumFrames[choice] = -1;
        return;
    }

    for(i=0;i<numWeaponFiles;i++)
    {
        char  temp[256];

        group = trap_GPG_GetSubGroups ( frameGroup[i] );
        while ( group )
        {
            char* name;

            // Get the name and break it down to just the filename without
            // and extension
            trap_GPG_GetName ( group, temp, sizeof(temp) );
            name = COM_SkipPath ( temp );
            COM_StripExtension ( name, temp, sizeof(temp) );

            if ( Q_stricmp ( temp, animInfo->mAnim[choice] ) == 0 )
            {
                break;
            }

            group = trap_GPG_GetNext ( group );
        }

        if (group)
        {
            trap_GPG_FindPairValue(group, "startframe", "0", temp, sizeof(temp));
            animInfo->mStartFrame[choice] = atoi(temp);
            trap_GPG_FindPairValue(group, "duration", "0", temp, sizeof(temp));
            animInfo->mNumFrames[choice] = atoi(temp);
            trap_GPG_FindPairValue(group, "fps", "0", temp, sizeof(temp));
            animInfo->mFPS[choice] = atoi(temp);
            animInfo->mNoteTracks[choice] = BG_FindNoteTracks(group);
            return;
        }
    }

    animInfo->mNumFrames[choice] = -1;
}

static void BG_CloseWeaponFrames(int upTo)
{
    int     i;

    for(i=upTo; i < numWeaponFiles; i++)
    {
        if (weaponFrames[i])
        {
            trap_GP_Delete(&weaponFrames[i]);
        }
    }

    numWeaponFiles = upTo;
}


static qboolean BG_ParseAnimGroup(weapon_t weapon, void *animGroup)
{
    void            *sub;
    char            name[256];
    TAnimWeapon     *anim;
    TAnimInfoWeapon *info;
    char            value[256];
    int             i;
    char            temp[256];

    anim = (TAnimWeapon *)G_Alloc(sizeof(*anim));
    memset(anim, 0, sizeof(*anim));

    anim->mNext = weaponParseInfo[weapon].mAnimList;
    weaponParseInfo[weapon].mAnimList = anim;

    trap_GPG_FindPairValue(animGroup, "name", "", anim->mName, sizeof(anim->mName));
    trap_GPG_FindPairValue(animGroup, "mp_muzzle||muzzle", "", anim->mMuzzle, sizeof(anim->mMuzzle));

    sub = trap_GPG_GetSubGroups(animGroup);
    while(sub)
    {
        trap_GPG_GetName(sub, name, sizeof(name));
        if (Q_stricmp(name, "info") == 0)
        {
            info = (TAnimInfoWeapon *)G_Alloc(sizeof(*info));
            memset(info, 0, sizeof(*info));

            info->mNext = anim->mInfos;
            anim->mInfos = info;

            info->mNumChoices = 0;
            trap_GPG_FindPairValue(sub, "name", "", info->mName, sizeof(info->mName));
            trap_GPG_FindPairValue(sub, "type", "", info->mType, sizeof(info->mType));

            // Cache for later
            if ( !Q_stricmp ( info->mType, "weaponmodel" ) )
            {
                anim->mWeaponModelInfo = info;
            }

            // We first look for a multiplayer specific speed. If we don't
            // find a valid speed, use the single player speed instead.
            trap_GPG_FindPairValue(sub, "mp_speed||speed", "0", temp, sizeof(temp));
            info->mSpeed = atof(temp);
            if(!info->mSpeed)
            {
                trap_GPG_FindPairValue(sub, "mp_speed||speed", "1", temp, sizeof(temp));
                info->mSpeed = atof(temp);
            }
            trap_GPG_FindPairValue(sub, "lodbias", "0", temp, sizeof(temp));
            info->mLODBias = atoi(temp);

            for(i=0;i<=MAX_WEAPON_ANIM_CHOICES;i++)
            {
                if (i == 0)
                {
                    strcpy(temp, "anim||animNoLerp");
                }
                else
                {
                    Com_sprintf(temp, sizeof(temp), "anim%d||animNoLerp%d", i, i);
                }
                trap_GPG_FindPairValue(sub, temp, "", value, sizeof(value));
                if (value[0] && info->mNumChoices < MAX_WEAPON_ANIM_CHOICES)
                {
                    info->mAnim[info->mNumChoices] = (char *)G_Alloc(strlen(value)+1);
                    strcpy(info->mAnim[info->mNumChoices], value);

                    if (i == 0)
                    {
                        strcpy(temp, "transition");
                    }
                    else
                    {
                        Com_sprintf(temp, sizeof(temp), "transition%d", i);
                    }
                    trap_GPG_FindPairValue(sub, temp, "", value, sizeof(value));
                    if (value[0])
                    {
                        info->mTransition[info->mNumChoices] = (char *)G_Alloc(strlen(value)+1);
                        strcpy(info->mTransition[info->mNumChoices], value);
                    }

                    if (i == 0)
                    {
                        strcpy(temp, "end");
                    }
                    else
                    {
                        Com_sprintf(temp, sizeof(temp), "end%d", i);
                    }
                    trap_GPG_FindPairValue(sub, temp, "", value, sizeof(value));
                    if (value[0])
                    {
                        info->mEnd[info->mNumChoices] = (char *)G_Alloc(strlen(value)+1);
                        strcpy(info->mEnd[info->mNumChoices], value);
                    }

                    info->mNumChoices++;
                }
            }
        }

        sub = trap_GPG_GetNext(sub);
    }

    return qtrue;
}

static TBoltonWeapon *BG_ParseBolton(void *boltonGroup)
{
    TBoltonWeapon   *bolton;
    void            *sub;
    char            temp[256];

    bolton = (TBoltonWeapon *)G_Alloc(sizeof(*bolton));
    memset(bolton, 0, sizeof(*bolton));

    trap_GPG_FindPairValue(boltonGroup, "name", "", bolton->mName, sizeof(bolton->mName));
    trap_GPG_FindPairValue(boltonGroup, "model", "", bolton->mModel, sizeof(bolton->mModel));
    trap_GPG_FindPairValue(boltonGroup, "parent", "", bolton->mParent, sizeof(bolton->mParent));
    trap_GPG_FindPairValue(boltonGroup, "bolttobone", "", bolton->mBoltToBone, sizeof(bolton->mBoltToBone));

    trap_GPG_FindPairValue(boltonGroup, "frames", "", temp, sizeof(temp));
    BG_OpenWeaponFrames(temp);

    sub = trap_GPG_FindSubGroup(boltonGroup, "rightside");
    if (sub)
    {
        BG_BuildSideSurfaceList(sub, "surface", bolton->mRightSide);
    }

    sub = trap_GPG_FindSubGroup(boltonGroup, "joint");
    if (sub)
    {
        trap_GPG_FindPairValue(sub, "bone", "", bolton->mJointBone, sizeof(bolton->mJointBone));
        trap_GPG_FindPairValue(sub, "parentBone", "", bolton->mJointParentBone, sizeof(bolton->mJointParentBone));
        trap_GPG_FindPairValue(sub, "fwd", "", bolton->mJointForward, sizeof(bolton->mJointForward));
        trap_GPG_FindPairValue(sub, "right", "", bolton->mJointRight, sizeof(bolton->mJointRight));
        trap_GPG_FindPairValue(sub, "up", "", bolton->mJointUp, sizeof(bolton->mJointUp));
    }

    return bolton;
}

static qboolean BG_ParseWeaponGroup(TWeaponModel *weapon, void *weaponGroup)
{
    void            *sub, *hand;
    char            name[256];
    TOptionalWeapon *option;
    char            temp[256];

    trap_GPG_FindPairValue(weaponGroup, "name", "", weapon->mName, sizeof(weapon->mName));
    trap_GPG_FindPairValue(weaponGroup, "model", "", weapon->mModel, sizeof(weapon->mModel));

    trap_GPG_FindPairValue(weaponGroup, "frames", "", temp, sizeof(temp));
    BG_OpenWeaponFrames(temp);

    sub = trap_GPG_GetSubGroups(weaponGroup);
    while(sub)
    {
        trap_GPG_GetName(sub, name, sizeof(name));
        if (Q_stricmp(name, "buffer") == 0)
        {
            trap_GPG_FindPairValue(sub, "name", "", weapon->mBufferName, sizeof(weapon->mBufferName));
            trap_GPG_FindPairValue(sub, "model", "", weapon->mBufferModel, sizeof(weapon->mBufferModel));
            trap_GPG_FindPairValue(sub, "bolttobone", "", weapon->mBufferBoltToBone, sizeof(weapon->mBufferBoltToBone));
            trap_GPG_FindPairValue(sub, "mp_muzzle||muzzle", "", weapon->mBufferMuzzle, sizeof(weapon->mBufferMuzzle));
            trap_GPG_FindPairValue(sub, "mp_altmuzzle", "", weapon->mBufferAltMuzzle, sizeof(weapon->mBufferAltMuzzle));
        }
        else if (Q_stricmp(name, "hands") == 0)
        {
            hand = trap_GPG_FindSubGroup(sub, "left");
            if (hand)
            {
                trap_GPG_FindPairValue(hand, "bolttobone", "", weapon->mLeftHandsBoltToBone, sizeof(weapon->mLeftHandsBoltToBone));
            }
            hand = trap_GPG_FindSubGroup(sub, "right");
            if (hand)
            {
                trap_GPG_FindPairValue(hand, "bolttobone", "", weapon->mRightHandsBoltToBone, sizeof(weapon->mRightHandsBoltToBone));
            }
        }
        else if (Q_stricmp(name, "bolton") == 0)
        {
            weapon->mBolton = BG_ParseBolton(sub);
        }
        else if (Q_stricmp(name, "rightside") == 0)
        {
            BG_BuildSideSurfaceList(sub, "surface", weapon->mRightSideSurfaces);
        }
        else if (Q_stricmp(name, "leftside") == 0)
        {
            BG_BuildSideSurfaceList(sub, "surface", weapon->mLeftSideSurfaces);
        }
        else if (Q_stricmp(name, "front") == 0)
        {
            BG_BuildSideSurfaceList(sub, "surface", weapon->mFrontSurfaces);
        }
        else if (Q_stricmp(name, "optionalpart") == 0)
        {
            option = (TOptionalWeapon *)G_Alloc(sizeof(*option));
            memset(option, 0, sizeof(*option));
            trap_GPG_FindPairValue(sub, "name", "", option->mName, sizeof(option->mName));
            trap_GPG_FindPairValue(sub, "muzzle", "", option->mMuzzle, sizeof(option->mMuzzle));
            BG_BuildSideSurfaceList(sub, "surface", option->mSurfaces);
            option->mNext=weapon->mOptionalList;
            weapon->mOptionalList=option;
/*
            if(weapon->mOptionalList)
            {
                option->mNext=weapon->mOptionalList;
                weapon->mOptionalList=option;
            }
            else
            {
                weapon->mOptionalList=option;
            }       */
        }

        sub = trap_GPG_GetNext(sub);
    }

    return qtrue;
}

static qboolean BG_ParseWeapon(weapon_t weapon, void *group)
{
    void            *sub, *soundName, *surfaceCallbackName;
    void            *soundValue, *surfaceCallbackValue;
    char            onOffVal[256];
    char            name[256];
    int             i, j;
    TAnimWeapon     *anims;
    TAnimInfoWeapon *infos;
    char            temp[256];

    memset(&weaponParseInfo[weapon], 0, sizeof(TWeaponParseInfo));
    weaponParseInfo[weapon].mName = bg_weaponNames[weapon];
    trap_GPG_FindPairValue(group, "foreshorten", "0.0", temp, sizeof(temp));
    weaponParseInfo[weapon].mForeshorten = atof(temp);

    sub = trap_GPG_GetSubGroups(group);
    while(sub)
    {
        trap_GPG_GetName(sub, name, sizeof(name));

        if (Q_stricmp(name, "viewoffset") == 0)
        {
            trap_GPG_FindPairValue(sub, "forward", "0.0", temp, sizeof(temp));
            weaponParseInfo[weapon].mViewOffset[0] = atof(temp);
            trap_GPG_FindPairValue(sub, "right", "0.0", temp, sizeof(temp));
            weaponParseInfo[weapon].mViewOffset[1] = atof(temp);
            trap_GPG_FindPairValue(sub, "up", "0.0", temp, sizeof(temp));
            weaponParseInfo[weapon].mViewOffset[2] = atof(temp);
        }
        else if (Q_stricmp(name, "sounds") == 0)
        {
            soundName = trap_GPG_GetSubGroups(sub);
            for(i=0;soundName && (i < MAX_WEAPON_SOUNDS);i++)
            {
                trap_GPG_GetName(soundName, weaponParseInfo[weapon].mSoundNames[i], sizeof(weaponParseInfo[weapon].mSoundNames[i]));
                soundValue = trap_GPG_GetPairs(soundName);
                for(j=0;soundValue && (j<MAX_WEAPON_SOUND_SLOTS);j++)
                {
                    trap_GPV_GetTopValue(soundValue, weaponParseInfo[weapon].mSounds[i][j], sizeof(weaponParseInfo[weapon].mSounds[i][j]));
                    soundValue = trap_GPV_GetNext(soundValue);
                }

                soundName = trap_GPG_GetNext(soundName);
            }
        }
        else if (Q_stricmp(name, "surfaces") == 0)
        {
            surfaceCallbackName = trap_GPG_GetSubGroups(sub);
            for(i=0;surfaceCallbackName && (i < MAX_SURFACE_CALLBACKS);i++)
            {
                trap_GPG_GetName(surfaceCallbackName, weaponParseInfo[weapon].mSurfaceCallbacks[i].mName,
                    sizeof(weaponParseInfo[weapon].mSurfaceCallbacks[i].mName));
                surfaceCallbackValue = trap_GPG_GetPairs(surfaceCallbackName);
                for(j=0;surfaceCallbackValue && (j<MAX_CALLBACK_SURFACES);j++)
                {
                    trap_GPG_GetName(surfaceCallbackValue, weaponParseInfo[weapon].mSurfaceCallbacks[i].mOnOffSurfaces[j].mName,
                        sizeof(weaponParseInfo[weapon].mSurfaceCallbacks[i].mOnOffSurfaces[j].mName));
                    trap_GPV_GetTopValue(surfaceCallbackValue, onOffVal, sizeof(onOffVal));
                    assert(onOffVal);
                    weaponParseInfo[weapon].mSurfaceCallbacks[i].mOnOffSurfaces[j].mStatus=(!Q_stricmp(onOffVal,"on"))?1:0;
                    surfaceCallbackValue=trap_GPV_GetNext(surfaceCallbackValue);
                }
                surfaceCallbackName=trap_GPG_GetNext(surfaceCallbackName);
            }

        }
        else if (Q_stricmp(name, "weaponmodel") == 0)
        {
            BG_ParseWeaponGroup(&weaponParseInfo[weapon].mWeaponModel, sub);
        }
        else if (Q_stricmp(name, "anim") == 0)
        {
            BG_ParseAnimGroup(weapon, sub);
        }

        sub = trap_GPG_GetNext(sub);
    }

    anims = weaponParseInfo[weapon].mAnimList;
    while(anims)
    {
        infos = anims->mInfos;
        while(infos)
        {
            for(i=0;i<infos->mNumChoices;i++)
            {
                BG_FindWeaponFrames(infos, i);
            }
            infos = infos->mNext;
        }

        anims = anims->mNext;
    }

    BG_CloseWeaponFrames(numInitialFiles);

    return qtrue;
}

qboolean BG_ParseInviewFile( qboolean pickupsDisabled )
{
    void        *GP2, *topGroup, *topSubs, *group;
    char        name[256], temp[256];
    int         i;

    GP2 = NULL;

    if (strlen(g_inviewFile.string) > 0) {
        Com_Printf("Parsing inview from /1fx/weaponfiles/inviews/%s\n", g_inviewFile.string);
        GP2 = trap_GP_ParseFile(va("weaponfiles/inviews/%s", g_inviewFile.string));

        if (!GP2) {
            Com_Printf("Custom inview file parsing failed, reverting to default.\n");
        }

    }

    if (!GP2) {
        Com_Printf("Parsing inview from inview/sof2.inview.\n");
        GP2 = trap_GP_ParseFile("inview/sof2.inview");
    }
    
    if (!GP2)
    {
        return qfalse;
    }

    weaponLeftHand[0] = 0;
    weaponRightHand[0] = 0;

    topGroup = trap_GP_GetBaseParseGroup(GP2);
    topSubs = trap_GPG_GetSubGroups(topGroup);
    while(topSubs)
    {
        trap_GPG_GetName(topSubs, name, sizeof(name));
        if (Q_stricmp(name, "hands") == 0)
        {
            group = trap_GPG_FindSubGroup(topSubs, "left");
            if (group)
            {
                trap_GPG_FindPairValue(group, "model", "", weaponLeftHand, sizeof(weaponLeftHand));
                trap_GPG_FindPairValue(group, "frames", "", temp, sizeof(temp));
                if (BG_OpenWeaponFrames(temp))
                {
                    numInitialFiles++;
                }
            }
            group = trap_GPG_FindSubGroup(topSubs, "right");
            if (group)
            {
                trap_GPG_FindPairValue(group, "model", "", weaponRightHand, sizeof(weaponRightHand));
                trap_GPG_FindPairValue(group, "frames", "", temp, sizeof(temp));
                if (BG_OpenWeaponFrames(temp))
                {
                    numInitialFiles++;
                }
            }
        }
        else if (Q_stricmp(name, "hud") == 0)
        {
        }
        else if (Q_stricmp(name, "weapon") == 0)
        {
            trap_GPG_FindPairValue(topSubs, "name", "", name, sizeof(name));
            for(i=0;i<level.wpNumWeapons;i++)
            {
                if (Q_stricmp(bg_weaponNames[i], name) == 0)
                {
                    BG_ParseWeapon(i, topSubs);
                    break;
                }
            }

#ifndef NDEBUG
            if (i == level.wpNumWeapons)
            {
                Com_Printf("BG_InitWeapons: Unknown weapon: %s\n", name);
            }
#endif // !NDEBUG
        }

        topSubs = trap_GPG_GetNext(topSubs);
    }

    BG_CloseWeaponFrames(0);
    trap_GP_Delete(&GP2);

    BG_InitAmmoStats();

    return BG_InitWeaponStats( pickupsDisabled );
}

TAnimWeapon *BG_GetInviewAnim(int weaponIdx,const char *animKey,int *animIndex)
{
    TAnimWeapon         *animWeapon;

    (*animIndex)=0;
    animWeapon=weaponParseInfo[weaponIdx].mAnimList;
    while((animWeapon!=0)&&(Q_stricmp(animWeapon->mName,animKey)))
    {
        animWeapon=animWeapon->mNext;
        (*animIndex)++;
    }
    if(!animWeapon)
    {
        return(0);
    }
    return(animWeapon);
}

TAnimWeapon *BG_GetInviewAnimFromIndex(int weaponIdx,int animIndex)
{
    TAnimWeapon     *animWeapon;
    int             i=0;

    animWeapon=weaponParseInfo[weaponIdx].mAnimList;
    while((animWeapon!=0)&&(i!=animIndex))
    {
        animWeapon=animWeapon->mNext;
        i++;
    }
    if(!animWeapon)
    {
        return(0);
    }
    return(animWeapon);
}

TAnimInfoWeapon *BG_GetInviewModelAnim(int weaponIdx,const char *modelKey,const char *animKey)
{
    TAnimWeapon         *animWeapon;
    TAnimInfoWeapon     *animInfoWeapon;
    animWeapon=weaponParseInfo[weaponIdx].mAnimList;
    while((animWeapon!=0)&&(Q_stricmp(animWeapon->mName,animKey)))
    {
        animWeapon=animWeapon->mNext;
    }
    if(!animWeapon)
    {
        return(0);
    }
    animInfoWeapon=animWeapon->mInfos;
    while((animInfoWeapon!=0)&&(Q_stricmp(animInfoWeapon->mType,modelKey)))
    {
        animInfoWeapon=animInfoWeapon->mNext;
    }
    if(!animInfoWeapon)
    {
        return(0);
    }
    return(animInfoWeapon);
}

/*
===============
BG_WeaponHasAlternateAmmo

Returns qtrue if the given weapon has ammo for its alternate attack
===============
*/
qboolean BG_WeaponHasAlternateAmmo ( int weapon )
{
    // No valid ammo index means no alternate ammo
    if ( weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex == AMMO_NONE )
    {
        return qfalse;
    }

    // If the alternate attack doesnt deplete ammo then it doesnt use it
    if ( !weaponData[weapon].attack[ATTACK_ALTERNATE].fireAmount )
    {
        return qfalse;
    }

    // If the alternate ammo is the same as the primary ammo then
    // the primary is good enough
    if ( weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex ==
         weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex )
    {
        return qfalse;
    }

    // Yup, alternates have ammo
    return qtrue;
}

/*
===============
BG_FindFireMode

Finds the firemode for the given weapon using the given default
===============
*/
int BG_FindFireMode ( weapon_t weapon, attackType_t attack, int firemode )
{
    int i;

    if ( !weapon )
    {
        return WP_FIREMODE_NONE;
    }

    for ( i=0; i <= WP_FIREMODE_SINGLE; i++ )
    {
        if( firemode >= WP_FIREMODE_MAX )
        {
            firemode = WP_FIREMODE_NONE + 1;
        }

        if( weaponData[weapon].attack[ATTACK_NORMAL].weaponFlags&(1<<firemode))
        {
            break;
        }
        else
        {
            firemode++;
        }
    }

    assert ( firemode < WP_FIREMODE_MAX );

    return firemode;
}

/*
===============
BG_CalculateBulletEndpoint

Calculates the end point of a bullet based on the given inaccuracy and range
===============
*/
void BG_CalculateBulletEndpoint ( vec3_t muzzlePoint, vec3_t fireAngs, float inaccuracy, float range, vec3_t end, int *seed )
{
    float   fGaussianX = 0;
    float   fGaussianY = 0;
    vec3_t  dir;
    vec3_t  fwd;
    vec3_t  up;
    vec3_t  right;

    AngleVectors ( fireAngs, fwd, right, up );

    // No inaccuracy so just extend it forward by the range
    if ( inaccuracy <= 0.0f )
    {
        VectorMA (muzzlePoint, range, fwd, end);
        return;
    }

    // Gaussian spread should keep it a bit less random looking
    while ( 1 )
    {
        float fGaussian;
        float f1;
        float f2;

        f1 = (float)((unsigned int)Q_rand ( seed ) % 15000) / 15000.0f;
        f2 = (float)((unsigned int)Q_rand ( seed ) % 15000) / 15000.0f;
        fGaussianX = (f1-0.5f) + (f2-0.5f);

        f1 = (float)((unsigned int)Q_rand ( seed ) % 15000) / 15000.0f;
        f2 = (float)((unsigned int)Q_rand ( seed ) % 15000) / 15000.0f;
        fGaussianY = (f1-0.5f) + (f2-0.5f);

        fGaussian = fGaussianX * fGaussianX + fGaussianY * fGaussianY;

        if ( fGaussian < 1 )
        {
            break;
        }
    }

    VectorMA ( fwd, 0.05f * inaccuracy * fGaussianX, right, dir );
    VectorMA ( dir, 0.05f * inaccuracy * fGaussianY, up, dir );

    VectorMA (muzzlePoint, range, dir, end);
}

/*
===============
BG_GetMaxAmmo

Returns the max ammo a client can hold for the given ammo index
===============
*/
int BG_GetMaxAmmo ( const playerState_t* ps, int ammoIndex )
{
    int         ammo;
    weapon_t    weapon;

    if ( ammoIndex == AMMO_NONE )
    {
        return 0;
    }

    for ( ammo = 0, weapon = WP_KNIFE; weapon < level.wpNumWeapons; weapon ++ )
    {
        if ( !(ps->stats[STAT_WEAPONS] & (1<<weapon)) )
        {
            if ( weapon != ps->stats[STAT_OUTFIT_GRENADE] )
            {
                continue;
            }
        }

        if ( weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex == ammoIndex )
        {
            ammo += (weaponData[weapon].attack[ATTACK_NORMAL].extraClips + 1) * weaponData[weapon].attack[ATTACK_NORMAL].clipSize;
            ammo -= ps->clip[ATTACK_NORMAL][weapon];
        }

        if ( BG_WeaponHasAlternateAmmo ( weapon ) )
        {
            if ( weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex == ammoIndex )
            {
                ammo += (weaponData[weapon].attack[ATTACK_ALTERNATE].extraClips + 1) * weaponData[weapon].attack[ATTACK_ALTERNATE].clipSize;
                ammo -= ps->clip[ATTACK_ALTERNATE][weapon];
            }
        }
    }

    return ammo;
}
