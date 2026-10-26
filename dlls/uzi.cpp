/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "skill.h"
#include "weapons.h"
#include "player.h"

enum uzi_e {
	UZI_IDLE1 = 0,
	UZI_IDLE2,
	UZI_IDLE3,
	UZI_RELOAD,
	UZI_DEPLOY,
	UZI_SHOOT,
	UZI_DEPLOY2,
	UZI_UNUSED7,
	UZI_AKIMBO_PULL,
	UZI_AKIMBO_IDLE,
	UZI_AKIMBO_RELOAD_RIGHT,
	UZI_AKIMBO_RELOAD_LEFT,
	UZI_AKIMBO_RELOAD_BOTH,
	UZI_AKIMBO_FIRE_LEFT,
	UZI_AKIMBO_FIRE_RIGHT,
	UZI_AKIMBO_FIRE_BOTH,
	UZI_AKIMBO_DEPLOY,
};

class CUzi : public CConfigurableWeapon
{
public:
	void PrecacheDefaultModelSounds() override;
	int WeaponId() const override { return WEAPON_UZI; }
	bool GetItemInfo(ItemInfo *p) override;
	WeaponParameters GetDefaultParameters() const override;
	int AddDuplicate( CBasePlayerWeapon *pOriginal ) override;
};

LINK_WEAPON_TO_CLASS( weapon_uzi, CUzi )
LINK_ENTITY_TO_CLASS( weapon_uziakimbo, CUzi ) // Link to single uzi until akimbo is implemented

//=========================================================
//=========================================================

// Picking up a second Uzi turns the carried one into akimbo (alt mode)
int CUzi::AddDuplicate( CBasePlayerWeapon *pOriginal )
{
	if( !pOriginal->m_inAltMode )
	{
		pOriginal->m_inAltMode = true;
		CBasePlayerWeapon::AddDuplicate( pOriginal );
		return true;
	}
	return CBasePlayerWeapon::AddDuplicate( pOriginal );
}

void CUzi::PrecacheDefaultModelSounds()
{
	PRECACHE_SOUND( "weapons/uzi/reload1.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload2.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload3.wav" );

	PRECACHE_SOUND( "weapons/uzi/deploy.wav" );
	PRECACHE_SOUND( "weapons/uzi/deploy1.wav" );
	PRECACHE_SOUND( "weapons/uzi/akimbo_pull2.wav" );
}

bool CUzi::GetItemInfo( ItemInfo *p )
{
	p->iSlot = 1;
	p->iPosition = 3;

	return true;
}

WeaponParameters CUzi::GetDefaultParameters() const
{
	WeaponParameters params;

	params.initialAmmoAmount = 32;
	params.maxClip = 64;
	params.ammoName = "9mm";

	params.worldModel = "models/w_uzi.mdl";
	params.viewModel = "models/v_uzi.mdl";
	params.playerModel = "models/p_uzi.mdl";
	params.playerAnimExt = "mp5";
	params.priority = 15;

	params.deploy.animIndex = UZI_DEPLOY;
	params.deploy.duration = 1.28f;

	params.idleAnims.main = WeaponParameters::IdleAnimArray{
		WeaponParameters::IdleAnim{UZI_IDLE1, 0.6f, 2.8f},
		WeaponParameters::IdleAnim{UZI_IDLE2, 0.1f, 6.73f},
		WeaponParameters::IdleAnim{UZI_IDLE3, 0.3f, 5.0f},
	};

	// Primary fire
	params.fire.fireType = WeaponParameters::Fire::BULLETS;
	params.fire.damageInfo.main.damage = ::GetSkillValueRange("plr_uzi");
	params.fire.anims.main = {UZI_SHOOT};

	params.fire.sound = {
		CHAN_WEAPON,
		{"weapons/uzi/shoot1.wav", "weapons/uzi/shoot2.wav", "weapons/uzi/shoot3.wav"},
		1.0f,
		ATTN_NORM,
		IntRange(94, 109)
	};

	params.fire.spread.SetStaticSpread(false, VECTOR_CONE_3DEGREES);
	params.fire.cycleTime = 0.1f;
	params.fire.allowUnderwater = false;
	params.fire.tracerFreq = 2;

	params.fire.autoAimDegree = AUTOAIM_5DEGREES;
	params.fire.muzzleFlash = true;
	params.fire.weaponVolume = NORMAL_GUN_VOLUME;
	params.fire.weaponFlash = NORMAL_GUN_FLASH;

	params.fire.clientPunchPitch = FloatRange(-2.0f, 2.0f);
	params.fire.shellOffsetForward = 20;
	params.fire.shellOffsetUp = -12;
	params.fire.shellOffsetSide = 4;
	params.fire.shellModel = "models/shell.mdl";
	params.fire.shellSound = TE_BOUNCE_SHELL;
	//

	// Akimbo mode (alt mode values)
	params.deploy.animIndex.alt = UZI_AKIMBO_DEPLOY;
	params.deploy.duration.alt = 1.17f;

	params.idleAnims.Materialize(true, false) = WeaponParameters::IdleAnimArray{
		WeaponParameters::IdleAnim{UZI_AKIMBO_IDLE, 1.0f, 0.1f},
	};

	params.fire.anims.Materialize(true, false) = {UZI_AKIMBO_FIRE_BOTH};
	params.fire.bulletCount.alt = 2;
	params.fire.ammoPerFire.alt = 2;

	params.reload.animIndex.Materialize(true, false) = {WeaponParameters::ReloadAnim(UZI_AKIMBO_RELOAD_BOTH)};
	params.reload.duration.alt = 2.02f;
	//

	// Secondary attack switches between one Uzi and two Uzi (akimbo)
	params.secondaryFireType = SecondaryFireType::SWITCH_MODE;
	params.altMode.animIndex.main = UZI_AKIMBO_PULL;
	params.altMode.animIndex.alt = UZI_DEPLOY2;
	params.altMode.attackDelay.main = 1.87f;
	params.altMode.attackDelay.alt = 1.08f;
	//

	params.reload.animIndex = {WeaponParameters::ReloadAnim(UZI_RELOAD)};
	params.reload.duration = 2.5f;
	params.reload.ammoCount = 32; // a reload loads 32 rounds (akimbo needs two reloads to fill 64)

	params.dropAmmo.classname = "ammo_9mmclip";

	return std::move(params);
}
