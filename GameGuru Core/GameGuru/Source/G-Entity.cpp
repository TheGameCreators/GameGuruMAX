//----------------------------------------------------
//--- GAMEGURU - G-Entity
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"
#include "CObjectsC.h"

#ifdef WICKEDENGINE
#include ".\..\..\Guru-WickedMAX\wickedcalls.h"
#include ".\\..\..\\Guru-WickedMAX\\GPUParticles.h"
using namespace GPUParticles;
#endif

#ifdef WICKEDENGINE
#include "GGRecastDetour.h"
extern GGRecastDetour g_RecastDetour;
#endif

// Globals
std::vector<int> g_iDestroyedEntitiesList;

// 
//  ENTITY GAME CODE
// 

void entity_init_overwritefireratesettings (void)
{
	// when all entities loaded, some contain override settings for weapons,
	// so ensure they are used after the weapon default settings are loaded
	// so new weapons added to level and tested reflect overrides
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		t.entid = t.entityelement[t.e].bankindex;
		if (t.entid > 0)
		{
			//  update gun/flak settings from latest entity properties
			t.tgunid_s = t.entityprofile[t.entid].isweapon_s;
			entity_getgunidandflakid ();
			if (t.tgunid > 0)
			{
				// entity properties should only edit first primary gun settings (so we dont mess up enhanced weapons)
				int firemode = 0;
				g.firemodes[t.tgunid][firemode].settings.damage = t.entityelement[t.e].eleprof.damage;
				g.firemodes[t.tgunid][firemode].settings.accuracy = t.entityelement[t.e].eleprof.accuracy;
				g.firemodes[t.tgunid][firemode].settings.reloadqty = t.entityelement[t.e].eleprof.reloadqty;
				g.firemodes[t.tgunid][firemode].settings.iterate = t.entityelement[t.e].eleprof.fireiterations;
				g.firemodes[t.tgunid][firemode].settings.range = t.entityelement[t.e].eleprof.range;
				g.firemodes[t.tgunid][firemode].settings.dropoff = t.entityelement[t.e].eleprof.dropoff;
				g.firemodes[t.tgunid][firemode].settings.usespotlighting = t.entityelement[t.e].eleprof.usespotlighting;
			}
		}
	}
}

void entity_init ( void )
{
	// all entities initialised, and any old destroy list items cleared
	g_iDestroyedEntitiesList.clear();
	 
	//  pre-create element data (load from eleprof)
	timestampactivity(0,"Configure entity instances for use");
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		entity_configueelementforuse ( );
	}

	//  activate all entities and perform any pre-test game setup
	timestampactivity(0,"Configure entity attachments and AI obstacles");
	g.entityattachmentindex=0;

	//PE: Could we use collisionmode == 0 and only create it as a box ? and not all faces.
	//PE: t.tobstype=t.entityprofile[t.entid].forcesimpleobstacle = true;
	//PE: This takes 30 sec. and take 400 MB. mem in FatherIsland, perhaps another faster way could be made.
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if (  t.entid>0 ) 
		{
			//  Activate entity
			t.entityelement[t.e].active=1;
			t.tobj=t.entityelement[t.e].obj;
			if (  t.tobj>0 ) 
			{
				//  if object exists
				if (  ObjectExist(t.tobj) == 1 ) 
				{
					//  Create attachents for entity
					entity_createattachment ( );
					//  Reset AI Obstacle Center references (used later by physics placement)
					t.entityelement[t.e].abscolx_f=-1;
					t.entityelement[t.e].abscolz_f=-1;
					t.entityelement[t.e].abscolradius_f=-1;
					//  Create AI obstacles for all static entities
					if (  t.entityprofile[t.entid].ismarker == 0 ) 
					{
						bool bSceneStatic = false;
						if ( t.entityelement[t.e].staticflag == 1 ) bSceneStatic = true;

						// leelee, sometimes want to make dynamic immobile entities AI obstacles!! (exploding crate in Escape level)
						// if ( t.entityelement[t.e].staticflag == 0 && t.entityelement[t.e].eleprof.isimmobile == 1 ) bSceneStatic = true;
						if ( bSceneStatic == true && t.entityprofile[t.entid].collisionmode != 11 && t.entityprofile[t.entid].collisionmode != 12 ) 
						{
							t.tfullheight=1;
							t.tcontainerid=0;
							if (  t.entityprofile[t.entid].collisionmode >= 50 && t.entityprofile[t.entid].collisionmode<60 ) 
							{
								t.ttreemode=t.entityprofile[t.entid].collisionmode-50;
								//  dont need to setup ai for multiplayer since it doesnt use any ai - unless coop mode!
								if ( g.aidisabletreeobstacles == 0 && (t.game.runasmultiplayer == 0 || ( g.mp.coop  ==  1 && t.entityprofile[t.entid].ismultiplayercharacter  ==  0 ) ) ) 
								{
									darkai_setup_tree ( );
								}
							}
							else
							{
								//  dont need to setup ai for multiplayer since it doesnt use any ai
								if (  t.game.runasmultiplayer == 0 || ( g.mp.coop  ==  1 && t.entityprofile[t.entid].ismultiplayercharacter  ==  0 ) ) 
								{
									if (g.aidisableobstacles == 0 && t.entityprofile[t.entid].collisionmode != 11 && t.entityprofile[t.entid].collisionmode != 12)
									{
										darkai_setup_entity();
									}
								}
							}
						}
					}
					//  ensure all transparent static objects are removed from 'intersect all' consideration
					t.tokay=0;
					if (  t.entityelement[t.e].staticflag == 1 ) 
					{
						if (  t.entityprofile[t.entid].canseethrough == 1 ) 
						{
							t.tokay=1;
						}
					}
					if (  t.entityprofile[t.entid].ischaracter == 0 ) 
					{
						if (  t.entityprofile[t.entid].collisionmode == 11  )  t.tokay = 1;
					}
					if (  t.tokay == 1 ) 
					{
						SetObjectCollisionProperty (  t.entityelement[t.e].obj,1 );
					}
					//  ensure all transparency modes set for each entity
					if (  t.entityprofile[t.entid].ismarker == 0 ) 
					{
						#ifdef WICKEDENGINE
						//PE: Wicked material can overwrite objects settings.
						if (t.entityelement[t.e].eleprof.WEMaterial.MaterialActive) {
							WickedSetEntityId(t.entid);
							WickedSetElementId(t.e);
							SetObjectTransparency(t.entityelement[t.e].obj, t.entityelement[t.e].eleprof.WEMaterial.bTransparency[0]);
							WickedSetEntityId(-1);
							WickedSetElementId(0);
						}
						else 
						#endif
						{
							int iNeverFive = t.entityelement[t.e].eleprof.transparency;
							if (iNeverFive == 5) iNeverFive = 6;
							#ifdef WICKEDENGINE
							WickedSetEntityId(t.entid);
							WickedSetElementId(t.e);
							SetObjectTransparency(t.entityelement[t.e].obj, iNeverFive);
							WickedSetEntityId(-1);
							WickedSetElementId(0);
							#else
							SetObjectTransparency(t.entityelement[t.e].obj, iNeverFive);
							#endif
						}
					}
					// ensure correct zdepth when game level starts
					entity_preparedepth(t.entid, t.tobj);
				}
			}
		}
	}
}

void entity_bringnewentitiestolife (bool bAllNewOnes)
{
	// scan if any new entities are characters, if so, scan for adding them to character array
	bool bNewEntityIsCharacter = false;
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		t.entid = t.entityelement[t.e].bankindex;
		if (t.entid > 0)
		{
			if (t.entityprofile[t.entid].isebe == 0)
			{
				if (t.entityelement[t.e].active == 2)
				{
					t.tobj = t.entityelement[t.e].obj;
					if (t.tobj > 0)
					{
						//  if object exists
						if (ObjectExist(t.tobj) == 1)
						{
							// Only redo script for characters (as they were wiped out)
							if (t.entityprofile[t.entid].ischaracter == 1) 
							{
								bNewEntityIsCharacter = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (bNewEntityIsCharacter == true)
	{
		extern void darkai_refresh_characters(bool);
		darkai_refresh_characters(true);
	}

	// new entities have active=2
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid = t.entityelement[t.e].bankindex;
		if ( t.entid>0 ) 
		{
			if ( t.entityprofile[t.entid].isebe == 0 )
			{
				if ( t.entityelement[t.e].active == 2 ) 
				{
					t.tobj = t.entityelement[t.e].obj;
					if ( t.tobj>0 ) 
					{
						//  if object exists
						if ( ObjectExist(t.tobj) == 1 ) 
						{
							// Only redo script for characters (as they were wiped out)
							if ( t.entityprofile[t.entid].ischaracter==1 || bAllNewOnes == true )
							{
								//  Launch the entity AI
								lua_loadscriptin ( );
								//  Launch init script
								lua_initscript ( );
								//  configure new entity for action
								entity_configueelementforuse ( );
								//  Create attachents for entity
								entity_createattachment ( );
							}
						}
					}
				}
			}
		}
	}

	// 161115 - now restore ALL entities back to actve=1 (now new entities determined)
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if ( t.entid>0 ) 
		{
			if ( t.entityprofile[t.entid].isebe == 0 )
			{
				if ( t.entityelement[t.e].active == 2 || t.entityelement[t.e].active == 3 || t.entityelement[t.e].active == 4 ) 
				{
					// show attachment
					if (  t.entityelement[t.e].attachmentobj>0 ) 
					{
						if (  ObjectExist(t.entityelement[t.e].attachmentobj) == 1 ) 
						{
							ShowObject (  t.entityelement[t.e].attachmentobj );
						}
					}

					// 171115 - only [3 with character] requires characters to be restored to clones
					if (  t.entityelement[t.e].active == 3 ) 
					{
						t.tte = t.e; 
						entity_converttoclone ( );
					}

					// restore active flag for in-game use
					t.entityelement[t.e].active = 1;
				}
			}
		}
	}
}

#ifdef WICKEDENGINE
float g_fActivationWaveDistance = 0.0f;
#endif

void entity_initafterphysics ( void )
{
	//  Handle spawn entities
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if (  t.entid>0 ) 
		{
			if (  t.entityelement[t.e].eleprof.spawnatstart == 0 ) 
			{
				t.entityelement[t.e].active=0;
				entity_lua_hide ( );
				entity_lua_collisionoff ( );
			}
			else
			{
				entity_lua_collisionon ( );
			}
		}
	}

	// special wave to activate physics in a sequence
	#ifdef WICKEDENGINE
	g_fActivationWaveDistance = 0.0f;
	#endif
}

void entity_refreshelementforuse ( void )
{
	// reset entity flags when entity is new (renewed, i.e. load game)

	// internal entity speed modulator defaults at 1.0
	t.entityelement[t.e].speedmodulator_f=1.0;

	// reset explosion state
	t.entityelement[t.e].explodefusetime=0;

	// reset ragdoll state
	t.entityelement[t.e].ragdollified=0;
}

void entity_configueelementforuse ( void )
{
	//  Spawn values
	t.entityelement[t.e].spawn.atstart=t.entityelement[t.e].eleprof.spawnatstart;
	t.entityelement[t.e].spawn.max=t.entityelement[t.e].eleprof.spawnmax;
	t.entityelement[t.e].spawn.delay=t.entityelement[t.e].eleprof.spawndelay;
	t.entityelement[t.e].spawn.qty=t.entityelement[t.e].eleprof.spawnqty;
	t.entityelement[t.e].spawn.upto=t.entityelement[t.e].eleprof.spawnupto;
	t.entityelement[t.e].spawn.afterdelay=t.entityelement[t.e].eleprof.spawnafterdelay;
	t.entityelement[t.e].spawn.whendead=t.entityelement[t.e].eleprof.spawnwhendead;
	t.entityelement[t.e].spawn.delayrandom=t.entityelement[t.e].eleprof.spawndelayrandom;
	t.entityelement[t.e].spawn.qtyrandom=t.entityelement[t.e].eleprof.spawnqtyrandom;
	t.entityelement[t.e].spawn.vel=t.entityelement[t.e].eleprof.spawnvel;
	t.entityelement[t.e].spawn.velrandom=t.entityelement[t.e].eleprof.spawnvelrandom;
	t.entityelement[t.e].spawn.angle=t.entityelement[t.e].eleprof.spawnangle;
	t.entityelement[t.e].spawn.anglerandom=t.entityelement[t.e].eleprof.spawnanglerandom;
	t.entityelement[t.e].spawn.life=t.entityelement[t.e].eleprof.spawnlife;
	if (  t.entityelement[t.e].spawn.atstart == 0 && t.entityelement[t.e].spawn.max == 0 ) 
	{
		t.entityelement[t.e].spawn.max=1;
		if (  t.entityelement[t.e].spawn.afterdelay == 0 && t.entityelement[t.e].spawn.whendead == 0 ) 
		{
			t.entityelement[t.e].spawn.afterdelay=1;
		}
		if (  t.entityelement[t.e].spawn.qty == 0  )  t.entityelement[t.e].spawn.qty = 1;
		if (  t.entityelement[t.e].spawn.upto == 0  )  t.entityelement[t.e].spawn.upto = 1;
	}

	//  Configure health from strength
	if (  t.entityelement[t.e].eleprof.strength>0 ) 
	{
		if (strcmp(t.entityelement[t.e].eleprof.aimain_s.Get(), "animals\\bird.lua") == 0)
		{
			// Birds will freeze when health reaches 0. We can remove this if script is updated to handle death.
			t.entityelement[t.e].health = 99999999;

			// Birds are also interfering with ally enemy detection (and cannot be killed at the moment, so no point in them having their allegiance changed)
			t.entityelement[t.e].eleprof.iCharAlliance = 2;//Neutral
		}
		else
		{
			t.entityelement[t.e].health = t.entityelement[t.e].eleprof.strength;
		}
	}
	else
	{
		t.entityelement[t.e].health=1;
	}

	//  Resolve default weapon gun ids
	if (  t.entityelement[t.e].eleprof.hasweapon_s != "" ) 
	{
		t.findgun_s = Lower( t.entityelement[t.e].eleprof.hasweapon_s.Get() ) ; 
		gun_findweaponindexbyname ( );
		t.entityelement[t.e].eleprof.hasweapon=t.foundgunid;
		if (  t.foundgunid>0 && t.entityprofile[t.entid].isammo == 0  )  t.gun[t.foundgunid].activeingame = 1;
	}
	else
	{
		t.entityelement[t.e].eleprof.hasweapon=0;
	}

	// Reset general flags when entity is newified
	entity_refreshelementforuse();
}

void entity_freeragdoll ( void )
{
	if (  t.entityelement[t.tte].ragdollified == 1 ) 
	{
		t.tphyobj=t.entityelement[t.tte].obj ; ragdoll_destroy ( );
		t.entityelement[t.tte].ragdollified=0;
	}
}

#ifdef WICKEDENGINE
void entity_resetlimbtwists(sObject* pObject, int e)
{
	// reset any animation motion within the object
	if (pObject)
	{
		if (pObject->ppFrameList)
		{
			if (pObject->dwSpineCenterLimbIndex > 0)
			{
				sFrame* pFrame = pObject->ppFrameList[pObject->dwSpineCenterLimbIndex];
				if (pFrame)
				{
					WickedCall_SetBip01Position(pObject, pFrame, 0, 0, 0);
				}
			}
		}
		pObject->bSpineTrackerMoving = false;

		// find this character
		int entid = t.entityelement[e].bankindex;
		t.ttte = e;
		entity_find_charanimindex_fromttte();
		if (t.tcharanimindex > 0)
		{
			t.charanimstate = t.charanimstates[t.tcharanimindex];
			// reset the spine twists
			t.charanimstate.spineRightAndLeft = 0;
			t.charanimstate.spineYAdjust = 0;
			t.charanimstate.spineZAdjust = 0;
			int iFrameIndex = t.entityprofile[entid].spine2;
			if (iFrameIndex > 0)
			{
				sFrame* pFrameOfLimb = pObject->ppFrameList[iFrameIndex];
				if (pFrameOfLimb)
				{
					WickedCall_RotateLimb(pObject, pFrameOfLimb, t.charanimstate.spineRightAndLeft, t.charanimstate.spineYAdjust, t.charanimstate.spineZAdjust);
				}
			}
			// reset head
			t.charanimstate.neckRightAndLeft = 0;
			t.charanimstate.neckUpAndDown = 0;
			if (t.charanimstate.ccpo.settings.iNeckBone > 0)
			{
				sFrame* pFrameOfLimb = pObject->ppFrameList[t.charanimstate.ccpo.settings.iNeckBone];
				if (pFrameOfLimb)
				{
					WickedCall_RotateLimb(pObject, pFrameOfLimb, t.charanimstate.neckRightAndLeft, t.charanimstate.neckUpAndDown, 0);
				}
			}
			// put back to main array (probably redundant as will be reconstructed when next in game level)
			t.charanimstates[t.tcharanimindex] = t.charanimstate;
		}
	}
}
#endif

void entity_free ( void )
{
	// free any ragdoll plus objects
	for (int iSpareRagdollPlusPhyObj = g.ragdollplussystemobjstart; iSpareRagdollPlusPhyObj <= g.ragdollplussystemobjfinish; iSpareRagdollPlusPhyObj++)
	{
		if (ObjectExist(iSpareRagdollPlusPhyObj) == 1)
		{
			ODEDestroyObject(iSpareRagdollPlusPhyObj);
			DeleteObject(iSpareRagdollPlusPhyObj);
		}
	}

	if (ObjectExist(g.ragdollplussystemdebugobj) == 1)
	{
		DeleteObject(g.ragdollplussystemdebugobj);
	}
	
	// close down game entities
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		t.obj=t.entityelement[t.e].obj;
		if (  t.entid>0 && t.obj>0 ) 
		{
			if ( ObjectExist(t.obj) == 1 ) 
			{
				// ensure all ragdolls/dynamic entities are restored
				if ( t.entityprofile[t.entid].ismarker == 0 && t.entityprofile[t.entid].isebe == 0 ) 
				{
					// and restore object for the editor
					t.tte = t.e ; entity_converttoinstance ( );
					PositionObject (  t.obj,t.entityelement[t.e].x,t.entityelement[t.e].y,t.entityelement[t.e].z );
					RotateObject (  t.obj,t.entityelement[t.e].rx,t.entityelement[t.e].ry,t.entityelement[t.e].rz );
					t.tentid=t.entid ; t.tte=t.e ; t.tobj=t.obj ; entity_resettodefaultanimation ( );
					ShowObject (  t.obj );

					#ifdef WICKEDENGINE
					sObject* pObject = GetObjectData(t.obj);
					if (t.entityprofile[t.entid].ischaracter == 1)
					{
						// reset any animation motion within the object
						entity_resetlimbtwists(pObject, t.e);
						// new ragdoll plus wipes pivot, need this restoring
						RotateLimb(t.obj, 0, 0, 0, 0);
						RotateObject (t.obj, t.entityelement[t.e].rx, t.entityelement[t.e].ry, t.entityelement[t.e].rz);
						// pivot character to face right way
						ResetObjectPivot(t.obj);
						RotateObject(t.obj, 0, 180, 0); 
						FixObjectPivot(t.obj);
					}
					else
					{
						// simply reset the animation frame if not a character object
						t.tte = t.e; entity_resettodefaultanimation();
					}
					// and ensure wicked updates the object 
					pObject->bAnimUpdateOnce = true;
					WickedCall_UpdateObject(pObject);
					// also force the new frame (skipping the slerp as we only have one update once pass here)
					WickedCall_InstantObjectFrameUpdate(pObject);
					#endif
				}
			}
			entity_freeattachment ( );
			t.entityelement[t.e].active=0;
			t.entityelement[t.e].ragdollifiedplusphyobj = 0;
			t.entityelement[t.e].ragdollplusactivate = 0;
			t.entityelement[t.e].ragdollplusweapontypeused = 0;
		}
		// release clonse sounds before leave
		if ( t.entityelement[t.e].soundset>0  )  deleteinternalsound(t.entityelement[t.e].soundset);
		if ( t.entityelement[t.e].soundset<0  )  DeleteAnimation (  abs(t.entityelement[t.e].soundset) );
		t.entityelement[t.e].soundset=0;
		if ( t.entityelement[t.e].soundset1>0  )  deleteinternalsound(t.entityelement[t.e].soundset1);
		if ( t.entityelement[t.e].soundset1<0  )  DeleteAnimation (  abs(t.entityelement[t.e].soundset1) );
		t.entityelement[t.e].soundset1=0;
		if ( t.entityelement[t.e].soundset2>0  )  deleteinternalsound(t.entityelement[t.e].soundset2);
		t.entityelement[t.e].soundset2=0;
		if ( t.entityelement[t.e].soundset3>0  )  deleteinternalsound(t.entityelement[t.e].soundset3);
		t.entityelement[t.e].soundset3=0;
		t.entityelement[t.e].soundset4 = 0;
		if (t.entityelement[t.e].soundset5 > 0)  deleteinternalsound(t.entityelement[t.e].soundset5);
		t.entityelement[t.e].soundset5 = 0;
		if (t.entityelement[t.e].soundset6 > 0)  deleteinternalsound(t.entityelement[t.e].soundset6);
		t.entityelement[t.e].soundset6 = 0;
		#ifdef WICKEDENGINE
		// reset any animation motion within the object
		sObject* pObject = GetObjectData(t.obj);
		entity_resetlimbtwists(pObject, t.e);
		#endif
	}
}

void entity_reset_defaults(void)
{
	t.entityelement[t.e].lipset.clear();
	t.entityelement[t.e].lipset1.clear();
	t.entityelement[t.e].lipset2.clear();
	t.entityelement[t.e].lipset3.clear();
	t.entityelement[t.e].lipset4.clear();
	t.entityelement[t.e].climbgravity = 0.0f;
	t.entityelement[t.e].lastfootfallsound = 0;
	t.entityelement[t.e].lastfootfallframeindex = -1;
	t.entityelement[t.e].iHasParentIndex = 0;
	t.entityelement[t.e].detectedlimbhit = 0;
	t.entityelement[t.e].beenmoved = 0;
	t.entityelement[t.e].underground = 0;
	t.entityelement[t.e].mp_rotateTimer = 0;
	t.entityelement[t.e].mp_rotateType = 0;
	t.entityelement[t.e].mp_isLuaChar = 0;
	t.entityelement[t.e].mp_lastUpdateSent = 0;
	t.entityelement[t.e].mp_updateOn = 0;
	t.entityelement[t.e].mp_coopLastTimeSwitchedTarget = 0;
	t.entityelement[t.e].mp_coopControlledByPlayer = 0;
	t.entityelement[t.e].mp_killedby = 0;
	t.entityelement[t.e].mp_networkkill = 0;
	t.entityelement[t.e].overpromptuse3D = false;
	t.entityelement[t.e].overprompt3dX = 0;
	t.entityelement[t.e].overprompt3dY = 0;
	t.entityelement[t.e].overprompt3dZ = 0;
	t.entityelement[t.e].overprompt3dAY = 0;
	t.entityelement[t.e].overprompt3dFaceCamera = false;
	t.entityelement[t.e].overprompttimer = 0;
	t.entityelement[t.e].overprompt_s = "";
	t.entityelement[t.e].editorlock = 0;
	t.entityelement[t.e].ragdollplusactivate = 0;
	t.entityelement[t.e].ragdollplusweapontypeused = 0;
	t.entityelement[t.e].ragdollifiedplusphyobj = 0;
	t.entityelement[t.e].ragdollifiedforcelimb = 0;
	t.entityelement[t.e].ragdollifiedforcevalue_f = 0.0f;
	t.entityelement[t.e].ragdollifiedforcez_f = 0.0f;
	t.entityelement[t.e].ragdollifiedforcey_f = 0.0f;
	t.entityelement[t.e].ragdollifiedforcex_f = 0.0f;
	t.entityelement[t.e].ragdollified = 0;
	t.entityelement[t.e].particleemitterid = 0;
	t.entityelement[t.e].characterSoundBankNumber = 0;
	t.entityelement[t.e].abscolradius_f = 0.0f;
	t.entityelement[t.e].abscolz_f = 0.0f;
	t.entityelement[t.e].abscolx_f = 0.0f;
	t.entityelement[t.e].hasbeenbatched = 0;
	t.entityelement[t.e].donotreflect = 0;
	t.entityelement[t.e].speedmodulator_f = 1.0f;
	t.entityelement[t.e].limbhurta_f = 0.0f;
	t.entityelement[t.e].limbhurt = 0;
	t.entityelement[t.e].plrvisible = 0;
	t.entityelement[t.e].isclone = 0;
	t.entityelement[t.e].consumed = 0;
	//t.entityelement[t.e].delaydestroytimer = 0;
	//t.entityelement[t.e].delaydestroy = 0;
	t.entityelement[t.e].distance = 0;
	t.entityelement[t.e].soundplaying = 0;
	t.entityelement[t.e].entityacc = 0;
	//t.entityelement[t.e].objectivecurrentrange = 0;
	//t.entityelement[t.e].objectiveradarrange = 0;
	//t.entityelement[t.e].isanobjective = 0;
	//t.entityelement[t.e].cullimmobile = 0;
	//t.entityelement[t.e].positionchanged = 0;
	//t.entityelement[t.e].camrotz = 0;
	//t.entityelement[t.e].camroty = 0;
	//t.entityelement[t.e].camrotx = 0;
	//t.entityelement[t.e].offsetanglez = 0.0f;
	//t.entityelement[t.e].offsetangley = 0.0f;
	//t.entityelement[t.e].offsetanglex = 0.0f;
	//t.entityelement[t.e].camoffsetz = 0;
	//t.entityelement[t.e].camoffsety = 0;
	//t.entityelement[t.e].camoffsetx = 0;
	//t.entityelement[t.e].camuse = 0;
	//t.entityelement[t.e].isimmune = 0;
	//t.entityelement[t.e].shotbyammo_s = "";
	//t.entityelement[t.e].shotby = 0;
	//t.entityelement[t.e].radardistance = 0;
	//t.entityelement[t.e].onradar = 0;
	//t.entityelement[t.e].usefade = 0;
	//t.entityelement[t.e].cullstate = 0;
	t.entityelement[t.e].ishidden = 0;
	t.entityelement[t.e].entitydammult_f = 0.0f;
	//t.entityelement[t.e].floorpositiony = 0.0f;
	//t.entityelement[t.e].hideshadow = 0;
	//t.entityelement[t.e].isaltammo = 0;
	//t.entityelement[t.e].nobulletcol = 0;
	//t.entityelement[t.e].lastdamagez_f = 0.0f;
	//t.entityelement[t.e].lastdamagey_f = 0.0f;
	//t.entityelement[t.e].lastdamagex_f = 0.0f;
	//t.entityelement[t.e].reserve5 = 0;
	//t.entityelement[t.e].reserve4 = 0;
	//t.entityelement[t.e].reserve3 = 0;
	//t.entityelement[t.e].reserve2 = 0;
	//t.entityelement[t.e].reserve1 = 0;
	t.entityelement[t.e].explodefusetime = 0;
	t.entityelement[t.e].destroyme = 0;
	t.entityelement[t.e].usingphysicsnow = 0;
	t.entityelement[t.e].doorobsactive = 0;
	t.entityelement[t.e].videotexture = 0;
	t.entityelement[t.e].alttextureused = 0;
	t.entityelement[t.e].soundistalking = 0;
	t.entityelement[t.e].soundlooping = 0;
	t.entityelement[t.e].soundisnonthreedee = 0;
	t.entityelement[t.e].soundset6 = 0;
	t.entityelement[t.e].soundset5 = 0;
	t.entityelement[t.e].soundset4 = 0;
	t.entityelement[t.e].soundset3 = 0;
	t.entityelement[t.e].soundset2 = 0;
	t.entityelement[t.e].soundset1 = 0;
	t.entityelement[t.e].soundset = 0;
	//t.entityelement[t.e].blockedtoz = 0;
	//t.entityelement[t.e].blockedtoy = 0;
	//t.entityelement[t.e].blockedtox = 0;
	//t.entityelement[t.e].blockedby = 0;
	//t.entityelement[t.e].invincibleactive = 0;
	//t.entityelement[t.e].collisionactive = 0;
	t.entityelement[t.e].activated = 0;
	t.entityelement[t.e].whoactivated = 0;
	t.entityelement[t.e].collected = 0;
	//t.entityelement[t.e].delaydir = 0;
	//t.entityelement[t.e].delaydamagez_f = 0.0f;
	//t.entityelement[t.e].delaydamagey_f = 0.0f;
	//t.entityelement[t.e].delaydamagex_f = 0.0f;
	//t.entityelement[t.e].delayimpact = 0;
	//t.entityelement[t.e].delaydamage = 0;
	//t.entityelement[t.e].delaydamagesource = 0;
	//t.entityelement[t.e].delaydamagetimer = 0.0f;
	//t.entityelement[t.e].delaydamagecount = 0.0f;
	//t.entityelement[t.e].shotdamagetype = 0;
	//t.entityelement[t.e].shotdamagesource = 0;
	//t.entityelement[t.e].shotdamage = 0;
	t.entityelement[t.e].beenkilled = 0;
	//t.entityelement[t.e].lifecode = 0;
	t.entityelement[t.e].briefimmunity = 0;
	t.entityelement[t.e].health = 0;
	//t.entityelement[t.e].floatvalue = 0.0f;
	//t.entityelement[t.e].floatrate = 0;
	//t.entityelement[t.e].spinvalue = 0.0f;
	//t.entityelement[t.e].spinrate = 0;
	t.entityelement[t.e].animframeupdate = 0;
	t.entityelement[t.e].destanimframe = 0.0f;
	t.entityelement[t.e].animonce = 0;
	t.entityelement[t.e].animframe = 0.0f;
	t.entityelement[t.e].animtime = 0;
	t.entityelement[t.e].animdo = 0;
	t.entityelement[t.e].animdir = 0;
	t.entityelement[t.e].animset = 0;
	//t.entityelement[t.e].decalsizey = 0.0f;
	//t.entityelement[t.e].decalsizex = 0.0f;
	//t.entityelement[t.e].decalslotused = 0;
	//t.entityelement[t.e].decalloop = 0.0f;
	//t.entityelement[t.e].decalmode = 0;
	//t.entityelement[t.e].decalindex = 0;
	t.entityelement[t.e].plrdist = 0.0f;
	//t.entityelement[t.e].crouchprofile = 0;
	//t.entityelement[t.e].nofloorlogic = 0;
	//t.entityelement[t.e].raycastlaststate = 0;
	//t.entityelement[t.e].raycastcount = 0;
	//t.entityelement[t.e].priorityduration = 0;
	//t.entityelement[t.e].priorityai = 0;
	//t.entityelement[t.e].logictimestamp = 0;
	//t.entityelement[t.e].logiccountburst = 0;
	//t.entityelement[t.e].logiccount = 0.0f;
	//t.entityelement[t.e].limbslerp = 0;
	t.entityelement[t.e].colb = 0;
	t.entityelement[t.e].colg = 0;
	t.entityelement[t.e].colr = 0;
	#ifdef WICKEDENGINE
	t.entityelement[t.e].soundset5 = 0;
	t.entityelement[t.e].soundset6 = 0;
	t.entityelement[t.e].floorposy = -90000.0f;
	t.entityelement[t.e].delay_floorposy = -90000.0f;
	#else
	t.entityelement[t.e].floorposy = 0;
	t.entityelement[t.e].delay_floorposy = 0;
	#endif
	t.entityelement[t.e].dry = 0.0f;
	t.entityelement[t.e].hoverfactoroverride = 0;
	t.entityelement[t.e].nogravity = 0;
	//t.entityelement[t.e].norotate = 0;
	t.entityelement[t.e].scalez = 0.0f;
	t.entityelement[t.e].scaley = 0.0f;
	t.entityelement[t.e].scalex = 0.0f;
	t.entityelement[t.e].rz = 0.0f;
	t.entityelement[t.e].ry = 0.0f;
	t.entityelement[t.e].rx = 0.0f;
	t.entityelement[t.e].quatmode = 0;
	t.entityelement[t.e].quatx = 0.0f;
	t.entityelement[t.e].quaty = 0.0f;
	t.entityelement[t.e].quatz = 0.0f;
	t.entityelement[t.e].quatw = 1.0f;
	t.entityelement[t.e].z = 0.0f;
	t.entityelement[t.e].y = 0.0f;
	t.entityelement[t.e].x = 0.0f;
	t.entityelement[t.e].lastx = 0.0f;
	t.entityelement[t.e].lasty = 0.0f;
	t.entityelement[t.e].lastz = 0.0f;
	t.entityelement[t.e].customlastx = 0.0f;
	t.entityelement[t.e].customlasty = 0.0f;
	t.entityelement[t.e].customlastz = 0.0f;
	//t.entityelement[t.e].attachmentblobobj = 0;
	t.entityelement[t.e].attachmentobjfirespotlimb = 0;
	//t.entityelement[t.e].attachmentweapontype = 0;
	t.entityelement[t.e].attachmentbaseobj = 0;
	t.entityelement[t.e].attachmentobj = 0;
	t.entityelement[t.e].obj = 0;
	t.entityelement[t.e].staticflag = 0;
	t.entityelement[t.e].profileobj = 0;
	t.entityelement[t.e].bankindex = 0;
	t.entityelement[t.e].maintype = 0;
	//t.entityelement[t.e].dormant = 0;
	t.entityelement[t.e].active = 0;
	//t.entityelement[t.e].fakeplrindex = 0;
	//t.entityelement[t.e].servercontrolled = 0;
	t.entityelement[t.e].editorfixed = 0;
	//t.entityelement[t.e].lastdamagepaused = 0;
	//t.entityelement[t.e].lastdamagetimer = 0;
	//t.entityelement[t.e].etimerpaused = 0;
	t.entityelement[t.e].etimer = 0;
	t.entityelement[t.e].ttarget = 0;
	//t.entityelement[t.e].aimovez = 0.0f;
	//t.entityelement[t.e].aimovex = 0.0f;
	//t.entityelement[t.e].aimoved = 0;
	//t.entityelement[t.e].eyeoffset = 0;
	//t.entityelement[t.e].spineraim = 0.0f;
	//t.entityelement[t.e].spiner = 0.0f;
	t.entityelement[t.e].spine = 0;
	//t.entityelement[t.e].curvcmz = 0;
	//t.entityelement[t.e].curvcmy = 0;
	//t.entityelement[t.e].curvcmx = 0;
	t.entityelement[t.e].isflak = 0;
	//t.entityelement[t.e].enterwater = 0;
	t.entityelement[t.e].animspeedmod = 1.0f;
	t.entityelement[t.e].dc_obj[0] = 0;
	t.entityelement[t.e].dc_obj[1] = 0;
	t.entityelement[t.e].dc_obj[2] = 0;
	t.entityelement[t.e].dc_obj[3] = 0;
	t.entityelement[t.e].dc_obj[4] = 0;
	t.entityelement[t.e].dc_obj[5] = 0;
	t.entityelement[t.e].dc_obj[6] = 0;
	t.entityelement[t.e].dc_entid[0] = 0;
	t.entityelement[t.e].dc_entid[1] = 0;
	t.entityelement[t.e].dc_entid[2] = 0;
	t.entityelement[t.e].dc_entid[3] = 0;
	t.entityelement[t.e].dc_entid[4] = 0;
	t.entityelement[t.e].dc_entid[5] = 0;
	t.entityelement[t.e].dc_entid[6] = 0;
	t.entityelement[t.e].draw_call_obj = 0;
	t.entityelement[t.e].dc_merged = false;
	#ifdef WICKEDENGINE
	// wipe out relational data!
	t.entityelement[t.e].eleprof.iObjectLinkID = 0;
	for (int i = 0; i < 10; i++)
	{
		t.entityelement[t.e].eleprof.iObjectRelationships[i] = 0;
		t.entityelement[t.e].eleprof.iObjectRelationshipsData[i] = 0;
		t.entityelement[t.e].eleprof.iObjectRelationshipsType[i] = 0;
	}
	#endif
}
void entity_delete ( void )
{
	//  delete all entities
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		if (  t.e <= ArrayCount(t.entityelement) ) 
		{
			// find and delete entity element obj
			t.obj=t.entityelement[t.e].obj;
			if (  t.obj>0 ) 
			{
				if (  ObjectExist(t.obj) == 1 ) 
				{
					DeleteObject (  t.obj );
				}
			}

			//  delete character creator object
			#ifdef VRTECH
			///t.ccobjToDelete = t.obj;
			///characterkit_deleteEntity ( );
			#else
			t.ccobjToDelete = t.obj;
			characterkit_deleteEntity ( );
			#endif

			t.entid = t.entityelement[t.e].bankindex;
			//PE: Clear entityprofile.
			//if (t.entityprofile[t.entid].ismarker == 0)

			t.entityelement[t.e].bankindex=0;
			t.entityelement[t.e].obj=0;

			//PE: as we are going to reuse the array in next level , reset everything.
			entity_reset_defaults(); //PE: takes t.e
		}
	}

	//PE: Alle objects has been removed reset counters.
	g.entityviewendobj = 0;
	//PE: Create new entities from beginning
	g.entityviewcurrentobj = g.entityviewstartobj;

}

void entity_pauseanimations ( void )
{
	Dim (  t.storeanimspeeds,g.entityelementlist  );
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.obj=t.entityelement[t.e].obj;
		if (  t.obj>0 ) 
		{
			if (  ObjectExist(t.obj) == 1 ) 
			{
				t.storeanimspeeds[t.e]=GetSpeed(t.obj);
				SetObjectSpeed (  t.obj,0 );
			}
		}
	}
}

void entity_resumeanimations ( void )
{
	if (t.storeanimspeeds.size() <= 0) return; //PE: Crash if pause not called before this.

	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.obj=t.entityelement[t.e].obj;
		if (  t.obj>0 ) 
		{
			if (  ObjectExist(t.obj) == 1 ) 
			{
				//PE: Resume without pause. got a crash here.
				int speed = 100;
				if (t.e < t.storeanimspeeds.size())
				{
					speed = t.storeanimspeeds[t.e];
				}
				else
				{
					speed = GetSpeed(t.obj);
				}
				SetObjectSpeed (  t.obj, speed );
			}
		}
	}
	UnDim (  t.storeanimspeeds );
}

#ifdef WICKEDENGINE
bool g_bOnlyOneRagdollPlusPerCycleForPerformance = false;
#endif

void entity_loop ( void )
{
	// so avoid ALL physics activating at once and slowing the level start,
	// we pace it out from the camera position outward
	#ifdef WICKEDENGINE
	if (g_fActivationWaveDistance < 999999)
	{
		float fSlice = 50.0f;
		g_fActivationWaveDistance = ODEProjectActivationWave (g_fActivationWaveDistance, fSlice);
	}
	#endif

	//  Handle all entities in level 
	#ifdef WICKEDENGINE
	g_bOnlyOneRagdollPlusPerCycleForPerformance = true;
	#endif
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		// 011016 - scenes with LARGE number of static entities hitting perf hard
		if ( t.entityelement[t.e].staticflag == 1 && t.entityelement[t.e].eleprof.phyalways == 0 ) continue;
		// NOTE: Determine essential tasks static needs (i.e. plrdist??)

		// only handle DYNAMIC entities
		t.entid=t.entityelement[t.e].bankindex;
		if ( t.entid>0 ) 
		{
			//  Entity object
			t.tobj=t.entityelement[t.e].obj;

			#ifdef WICKEDENGINE
			extern bool g_bShowRecastDetourDebugVisuals;
			if (t.entityprofile[t.entid].ischaracter == 1)
			{
				sObject* pObject = GetObjectData(t.tobj);
				float fObjectOriginalRotated = t.entityelement[t.e].ry;
				int iTempObjectForTest = g.ragdollplussystemdebugobj;
				if (t.entityelement[t.e].ragdollplusactivate > 0)
				{
					// ragdoll plus triggered
					if (t.entityelement[t.e].ragdollplusactivate < 10) t.entityelement[t.e].ragdollplusactivate += 10;

					// see if we can fit a death animation into the scene
					bool bDropAnyWeaponNow = false;
					bool bGoToRagdollNow = false;
					GGVECTOR3 vecMinOrig = pObject->collision.vecMin;
					GGVECTOR3 vecMaxOrig = pObject->collision.vecMax;
					GGVECTOR3 vecCenterOrig = pObject->collision.vecCentre;
					int iSpareRagdollPlusPhyObj = t.entityelement[t.e].ragdollifiedplusphyobj;
					if (iSpareRagdollPlusPhyObj == 0)
					{
						// only alow one creation per frame cycle
						if (g_bOnlyOneRagdollPlusPerCycleForPerformance == false)
							continue;

						// show available floor area according to nav mesh (simple for now, need smartypants one for any angle and area shape)
						GGVECTOR3 vecCurrentPos = GGVECTOR3(ObjectPositionX(t.tobj), ObjectPositionY(t.tobj), ObjectPositionZ(t.tobj));
						GGVECTOR3 vecNearestPt = GGVECTOR3(0, 0, 0);
						// ensure starting position is INSIDE a navmesh
						bool bBeInsideNavMesh = false;
						bool bNoSpaceBehindOrInFrontCharacterInNavMesh = false;
						while (bBeInsideNavMesh == false)
						{
							if (g_RecastDetour.isWithinNavMeshEx(vecCurrentPos.x, vecCurrentPos.y, vecCurrentPos.z, (float*)&vecNearestPt, true) == true)
							{
								// good, we are on the nav mesh
								bBeInsideNavMesh = true;

								// check if there is navmesh behind character (can choose better anim later if so)
								GGVECTOR3 vecBehindOrInFrontPos = vecCurrentPos;
								float fWhoShotMeX = CameraPositionX(0);// this should be who shot this character, not necessarily the player!!
								float fWhoShotMeZ = CameraPositionZ(0);// this should be who shot this character, not necessarily the player!!
								float fDX = fWhoShotMeX - vecCurrentPos.x;
								float fDZ = fWhoShotMeZ - vecCurrentPos.z;
								float fDA = atan2(fDX, fDZ);
								for (float fTraceOutward = 5.0f; fTraceOutward < 40.0f; fTraceOutward += 5.0f)
								{
									if (bNoSpaceBehindOrInFrontCharacterInNavMesh == false)
									{
										float fDistanceToStepCheckBack = -fTraceOutward; // behind
										vecBehindOrInFrontPos.x = NewXValue(vecCurrentPos.x, GGToDegree(fDA), fDistanceToStepCheckBack);
										vecBehindOrInFrontPos.z = NewZValue(vecCurrentPos.z, GGToDegree(fDA), fDistanceToStepCheckBack);
										if (g_RecastDetour.isWithinNavMeshEx(vecBehindOrInFrontPos.x, vecBehindOrInFrontPos.y, vecBehindOrInFrontPos.z, (float*)&vecNearestPt, true) == false)
										{
											// seems behind the character is NO navmesh!
											bNoSpaceBehindOrInFrontCharacterInNavMesh = true;
										}
										fDistanceToStepCheckBack = fTraceOutward; // in front
										vecBehindOrInFrontPos.x = NewXValue(vecCurrentPos.x, GGToDegree(fDA), fDistanceToStepCheckBack);
										vecBehindOrInFrontPos.z = NewZValue(vecCurrentPos.z, GGToDegree(fDA), fDistanceToStepCheckBack);
										if (g_RecastDetour.isWithinNavMeshEx(vecBehindOrInFrontPos.x, vecBehindOrInFrontPos.y, vecBehindOrInFrontPos.z, (float*)&vecNearestPt, true) == false)
										{
											// seems behind the character is NO navmesh!
											bNoSpaceBehindOrInFrontCharacterInNavMesh = true;
										}
									}
								}
							}
							else
							{
								// if out of navmesh, we can assume obstacles abound!!
								bNoSpaceBehindOrInFrontCharacterInNavMesh = true;

								// not exactly on nav mesh, are we near one
								if (g_RecastDetour.isWithinNavMesh(vecCurrentPos.x, vecCurrentPos.y, vecCurrentPos.z) == true)
								{
									// move current starting pos for expanding box to point inside nav mesh
									GGVECTOR3 vecDir = vecNearestPt - vecCurrentPos;
									GGVec3Normalize(&vecDir, &vecDir);
									float fMargin = 11.0f; // pushes well into the nav mesh which must account for character breadth
									vecCurrentPos = vecNearestPt + (vecDir * fMargin);
									bBeInsideNavMesh = true;
								}
								else
								{
									// no nav mesh, so no restrictions
									break;
								}
							}
						}
						GGMATRIX matRotY;
						GGMatrixRotationY(&matRotY, GGToRadian(-fObjectOriginalRotated));
						GGMATRIX matInvRotY;
						GGMatrixRotationY(&matInvRotY, GGToRadian(fObjectOriginalRotated));
						GGVECTOR3 vecNavMeshMin = vecCurrentPos;
						GGVECTOR3 vecNavMeshMax = vecCurrentPos;
						if (bBeInsideNavMesh == true)
						{
							// first find best center position to start box growth
							int iBestSpoke = -1;
							float fSpokeDist = 0.0f;
							GGVECTOR3 vecNewPosAlongSpoke = GGVECTOR3(0, 0, 0);
							for (int iSpokes = 0; iSpokes < 8; iSpokes++)
							{
								float fAngle = iSpokes * 45.0f;
								float fDestX = NewXValue(0, fAngle, 1.0f);
								float fDestZ = NewZValue(0, fAngle, 1.0f);
								GGVECTOR3 vecAt = vecCurrentPos;
								int iStepExtent = 20;// 100;
								for (int iStep = 0; iStep < iStepExtent; iStep++)
								{
									vecAt.x += fDestX;
									vecAt.z += fDestZ;
									bool bWeCanHaveThisNow = false;
									if (g_RecastDetour.isWithinNavMeshEx(vecAt.x, vecAt.y, vecAt.z, (float*)&vecNearestPt, true) == false)
									{
										// left nav mesh, stop here
										if (iStep > fSpokeDist)
										{
											bWeCanHaveThisNow = true;
										}
										else
										{
											break;
										}
									}
									if (iStep >= (iStepExtent - 2)) bWeCanHaveThisNow = true;
									if (bWeCanHaveThisNow == true)
									{
										iBestSpoke = iSpokes;
										fSpokeDist = iStep;
										vecNewPosAlongSpoke = vecCurrentPos + ((vecAt - vecCurrentPos) / 2);
										break;
									}
								}
							}
							if (iBestSpoke >= 0)
							{
								// new best current position for ideal box expansion
								vecCurrentPos = vecNewPosAlongSpoke;
							}
							// expand box to find max size area here (account for fObjectOriginalRotated)
							float fFoundAreaSizeX, fFoundAreaSizeZ;
							GGVECTOR3 vecLocaMin, vecLocaMax;
							for (int iAlignModeTry = 0; iAlignModeTry < 2; iAlignModeTry++)
							{
								vecLocaMin = GGVECTOR3(0, 0, 0);
								vecLocaMax = GGVECTOR3(0, 0, 0);
								bool bExpanding = true;
								bool bExpandingMinX = true;
								bool bExpandingMaxX = true;
								bool bExpandingMinZ = true;
								bool bExpandingMaxZ = true;
								float fStep = 2.0f;
								while (bExpanding)
								{
									if (bExpandingMinX == true)
									{
										for (int stepz = vecLocaMin.z; stepz <= vecLocaMax.z; stepz += 5)
										{
											GGVECTOR3 vecWorld = GGVECTOR3(vecLocaMin.x - fStep, vecLocaMin.y, stepz);
											GGVec3TransformCoord(&vecWorld, &vecWorld, &matInvRotY);
											vecWorld += vecCurrentPos;
											if (g_RecastDetour.isWithinNavMeshEx(vecWorld.x, vecWorld.y, vecWorld.z, (float*)&vecNearestPt, true) == false)
											{
												bExpandingMinX = false;
											}
										}
										if (bExpandingMinX == true) vecLocaMin.x -= fStep;
									}
									if (bExpandingMinZ == true)
									{
										for (int stepx = vecLocaMin.x; stepx <= vecLocaMax.x; stepx += 5)
										{
											GGVECTOR3 vecWorld = GGVECTOR3(stepx, vecLocaMin.y, vecLocaMin.z - fStep);
											GGVec3TransformCoord(&vecWorld, &vecWorld, &matInvRotY);
											vecWorld += vecCurrentPos;
											if (g_RecastDetour.isWithinNavMeshEx(vecWorld.x, vecWorld.y, vecWorld.z, (float*)&vecNearestPt, true) == false)
											{
												bExpandingMinZ = false;
											}
										}
										if (bExpandingMinZ == true) vecLocaMin.z -= fStep;
									}
									if (bExpandingMaxX == true)
									{
										for (int stepz = vecLocaMin.z; stepz <= vecLocaMax.z; stepz += 5)
										{
											GGVECTOR3 vecWorld = GGVECTOR3(vecLocaMax.x + fStep, vecLocaMin.y, stepz);
											GGVec3TransformCoord(&vecWorld, &vecWorld, &matInvRotY);
											vecWorld += vecCurrentPos;
											if (g_RecastDetour.isWithinNavMeshEx(vecWorld.x, vecWorld.y, vecWorld.z, (float*)&vecNearestPt, true) == false)
											{
												bExpandingMaxX = false;
											}
										}
										if (bExpandingMaxX == true) vecLocaMax.x += fStep;
									}
									if (bExpandingMaxZ == true)
									{
										for (int stepx = vecLocaMin.x; stepx <= vecLocaMax.x; stepx += 5)
										{
											GGVECTOR3 vecWorld = GGVECTOR3(stepx, vecLocaMin.y, vecLocaMax.z + fStep);
											GGVec3TransformCoord(&vecWorld, &vecWorld, &matInvRotY);
											vecWorld += vecCurrentPos;
											if (g_RecastDetour.isWithinNavMeshEx(vecWorld.x, vecWorld.y, vecWorld.z, (float*)&vecNearestPt, true) == false)
											{
												bExpandingMaxZ = false;
											}
										}
										if (bExpandingMaxZ == true) vecLocaMax.z += fStep;
									}
									fFoundAreaSizeX = vecLocaMax.x - vecLocaMin.x;
									fFoundAreaSizeZ = vecLocaMax.z - vecLocaMin.z;
									if (fFoundAreaSizeX >= 100.0f) { bExpandingMinX = false; bExpandingMaxX = false; }
									if (fFoundAreaSizeZ >= 100.0f) { bExpandingMinZ = false; bExpandingMaxZ = false; }
									if (bExpandingMinX == false && bExpandingMaxX == false && bExpandingMinZ == false && bExpandingMaxZ == false)
										bExpanding = false;
								}
								if (fabs(fFoundAreaSizeX + fFoundAreaSizeZ) < 100.0f)
								{
									// means this volume of floor is less than the needed area, try again with an aligned rotation
									fObjectOriginalRotated = (int)((fObjectOriginalRotated + 45.0f) / 90.0f);
									fObjectOriginalRotated = (fObjectOriginalRotated * 90.0f);
									GGMatrixRotationY(&matRotY, GGToRadian(-fObjectOriginalRotated));
									GGMatrixRotationY(&matInvRotY, GGToRadian(fObjectOriginalRotated));
								}
								else
								{
									// happy with this floor size
									break;
								}
							}
							if (ObjectExist(iTempObjectForTest) == 1)
							{
								DeleteObject(iTempObjectForTest);
							}
							MakeObjectBox (iTempObjectForTest, fFoundAreaSizeX, 5, fFoundAreaSizeZ);
							GGVECTOR3 vecCenter = (vecLocaMax - vecLocaMin) / 2;
							GGVECTOR3 vecWorld = vecLocaMin + vecCenter;
							GGVec3TransformCoord(&vecWorld, &vecWorld, &matInvRotY);
							vecWorld += vecCurrentPos;
							vecNavMeshMin = vecLocaMin + vecCurrentPos; // we use these unrotated for 'within box calc' :)
							vecNavMeshMax = vecLocaMax + vecCurrentPos;
							PositionObject (iTempObjectForTest, vecWorld.x, ObjectPositionY(t.tobj) + 2.5f, vecWorld.z);
							RotateObject(iTempObjectForTest, 0, fObjectOriginalRotated, 0);
							if (g_bShowRecastDetourDebugVisuals == false)
							{
								HideObject(iTempObjectForTest);
							}
						}
						// create death database
						struct sDeathListType
						{
							float fScore;
							cStr pName;
							float iStartFrame;
							float iEndFrame;
						};
						std::vector<sDeathListType> pDeathList;
						pDeathList.clear();
						if (pObject->pAnimationSet)
						{
							// get animation slot data
							extern std::vector<sAnimSlotStruct> g_pAnimSlotList;
							g_pAnimSlotList.clear();
							animsystem_buildanimslots(t.tobj);

							// go through all animations
							for (int slot = 0; slot < g_pAnimSlotList.size(); slot++)
							{
								LPSTR pName = g_pAnimSlotList[slot].pName;
								if (pName && strnicmp(pName, "death", 5) == NULL)
								{
									// death anim found, add to list
									sDeathListType item;
									item.pName = Lower(pName);
									item.iStartFrame = g_pAnimSlotList[slot].fStart;
									item.iEndFrame = g_pAnimSlotList[slot].fFinish;
									pDeathList.push_back(item);
								}
							}
						}
						// sort database by preference and reject early choices
						LPSTR pPreferredKeyword = "front";
						if (t.entityelement[t.e].ragdollplusactivate == 11) pPreferredKeyword = "front";
						if (t.entityelement[t.e].ragdollplusactivate == 12) pPreferredKeyword = "back";
						if (t.entityelement[t.e].ragdollplusactivate == 13) pPreferredKeyword = "right";
						if (t.entityelement[t.e].ragdollplusactivate == 14) pPreferredKeyword = "left";
						if (t.entityelement[t.e].ragdollplusactivate == 15) pPreferredKeyword = "back";
						if (t.entityelement[t.e].ragdollplusactivate == 16) pPreferredKeyword = "";
						// if detect wall in front or behind character and BACK used, specify wall specific
						if (t.entityelement[t.e].ragdollplusactivate == 11)
						{
							// scan to see if a solid obstacle behind
							if (bNoSpaceBehindOrInFrontCharacterInNavMesh == true)
							{
								// choose the special back animation!
								pPreferredKeyword = "front_fall_wall";
							}
						}
						if (t.entityelement[t.e].ragdollplusactivate == 12 || t.entityelement[t.e].ragdollplusactivate == 15)
						{
							// scan to see if a solid obstacle behind
							if (bNoSpaceBehindOrInFrontCharacterInNavMesh == true)
							{
								// choose the special back animation!
								pPreferredKeyword = "back_fall_wall";
							}
							else
							{
								// if killed by shotgun type, and going back, special force back preferred
								if (t.entityelement[t.e].ragdollplusweapontypeused == 2) // 1- pierce, 2-shotgun shell
								{
									pPreferredKeyword = "back_fall_slope";
								}
							}
						}
						std::vector<sDeathListType> pDeathListPreferred;
						pDeathListPreferred.clear();
						bool pDone[32];
						memset(pDone, 0, sizeof(pDone));
						if (pDeathList.size() > 0)
						{
							for (int preferences = 0; preferences < 2; preferences++)
							{
								int iMaxDeathChoices = pDeathList.size();
								if (iMaxDeathChoices > 30) iMaxDeathChoices = 30;
								for (int deathindex = 0; deathindex < iMaxDeathChoices; deathindex++)
								{
									bool bAddToPrefList = false;
									if (preferences == 0)
									{
										// add specific ones preferred (i.e. back for when shot from the front)
										if (strstr (pDeathList[deathindex].pName.Get(), pPreferredKeyword) > 0)
										{
											bAddToPrefList = true;
										}
									}
									if (preferences == 1)
									{
										// add remaining
										bAddToPrefList = true;
									}
									if (bAddToPrefList == true && pDone[deathindex] == false)
									{
										sDeathListType item;
										item.pName = cStr(pDeathList[deathindex].pName.Get());
										item.iStartFrame = pDeathList[deathindex].iStartFrame;
										item.iEndFrame = pDeathList[deathindex].iEndFrame;
										pDeathListPreferred.push_back(item);
										pDone[deathindex] = true;
									}
								}
							}
						}

						// now choose a shape that represents the final resting place of the death anim
						float fBestScore = -1.0f;
						int iPreferredAnimChosen = -1;
						GGVECTOR3 vecBestSize = GGVECTOR3(0, 0, 0);
						GGVECTOR3 vecBestCenterOffset = GGVECTOR3(0, 0, 0);
						GGVECTOR3 vecBestShiftInObjectSpace = GGVECTOR3(0, 0, 0);
						GGVECTOR3 vecObjPos = GGVECTOR3(ObjectPositionX(t.tobj), ObjectPositionY(t.tobj), ObjectPositionZ(t.tobj));
						GGVECTOR3 vecNavMeshFloorSize = vecNavMeshMax - vecNavMeshMin;
						if (strlen(pPreferredKeyword) > 0)
						{
							for (int preflist = 0; preflist < pDeathListPreferred.size(); preflist++)
							{
								// get a death choice
								float fFinalFrameOfChosenDeath = pDeathListPreferred[preflist].iEndFrame;
								// work out how this fits as a shape into floor area
								float fBiasTowardsX = (vecNavMeshFloorSize.x / vecNavMeshFloorSize.z);
								float fBiasTowardsZ = (vecNavMeshFloorSize.z / vecNavMeshFloorSize.x);
								CalculateObjectFrameBounds(t.tobj, fFinalFrameOfChosenDeath);
								GGVECTOR3 vecCenter = pObject->collision.vecCentre;
								float xx = vecCenter.x;
								float yy = vecCenter.y;
								float zz = vecCenter.z;
								GGVECTOR3 vecMin = pObject->collision.vecMin;
								GGVECTOR3 vecMax = pObject->collision.vecMax;
								// try to fit this into the floor area box
								GGVECTOR3 vecCenterOffset = GGVECTOR3(xx, yy, zz);
								GGVECTOR3 vecSize = vecMax - vecMin;
								GGVECTOR3 vecTopLeft = vecCenterOffset - (vecSize / 2);
								GGVECTOR3 vecTopLeftOriginal = vecTopLeft;
								GGVECTOR3 vecDiff = vecObjPos - vecCurrentPos;
								GGVec3TransformCoord(&vecDiff, &vecDiff, &matRotY);
								float fDistToCreep = 200; //floor area box can be quite some way off in cramped spots!!
								for (int iEachSide = 0; iEachSide < 4; iEachSide++)
								{
									for (int iStep = 0; iStep < fDistToCreep; iStep++)
									{
										if (iEachSide == 0)
										{
											vecTopLeft.x += 1;
											GGVECTOR3 vecUnrotatedWorld = vecCurrentPos + vecDiff + vecTopLeft;
											if (vecUnrotatedWorld.x >= vecNavMeshMin.x)
											{
												// now inside on left X
												break;
											}
										}
										if (iEachSide == 1)
										{
											vecTopLeft.z += 1;
											GGVECTOR3 vecUnrotatedWorld = vecCurrentPos + vecDiff + vecTopLeft;
											if (vecUnrotatedWorld.z >= vecNavMeshMin.z)
											{
												// now inside on top Z
												break;
											}
										}
										if (iEachSide == 2)
										{
											vecTopLeft.x -= 1;
											GGVECTOR3 vecUnrotatedWorld = vecCurrentPos + vecDiff + vecTopLeft + vecSize;
											if (vecUnrotatedWorld.x <= vecNavMeshMax.x)
											{
												// now inside on right X
												break;
											}
										}
										if (iEachSide == 3)
										{
											vecTopLeft.z -= 1;
											GGVECTOR3 vecUnrotatedWorld = vecCurrentPos + vecDiff + vecTopLeft + vecSize;
											if (vecUnrotatedWorld.z <= vecNavMeshMax.z)
											{
												// now inside on bottom Z
												break;
											}
										}
									}
								}
								// give the final position of the shape as a score to use later
								float fScore = 0.0f;
								GGVECTOR3 vecUnrotatedShapeWorld1 = vecCurrentPos + vecDiff + vecTopLeft;
								GGVECTOR3 vecUnrotatedShapeWorld2 = vecCurrentPos + vecDiff + vecTopLeft + vecSize;
								if (vecUnrotatedShapeWorld1.x >= vecNavMeshMin.x)
								{
									fScore += 1 * fBiasTowardsZ;
								}
								else
								{
									fScore -= (((vecNavMeshMin.x - vecUnrotatedShapeWorld1.x) / vecSize.x) * fBiasTowardsZ);
								}
								if (vecUnrotatedShapeWorld2.x <= vecNavMeshMax.x)
								{
									fScore += 1 * fBiasTowardsZ;
								}
								else
								{
									fScore -= (((vecUnrotatedShapeWorld2.x - vecNavMeshMax.x) / vecSize.x) * fBiasTowardsZ);
								}
								if (vecUnrotatedShapeWorld1.z >= vecNavMeshMin.z)
								{
									fScore += 1 * fBiasTowardsX;
								}
								else
								{
									fScore -= (((vecNavMeshMin.z - vecUnrotatedShapeWorld1.z) / vecSize.z) * fBiasTowardsX);
								}
								if (vecUnrotatedShapeWorld2.z <= vecNavMeshMax.z)
								{
									fScore += 1 * fBiasTowardsX;
								}
								else
								{
									fScore -= (((vecUnrotatedShapeWorld2.z - vecNavMeshMax.z) / vecSize.z) * fBiasTowardsX);
								}
								if (fScore > fBestScore)
								{
									fBestScore = fScore;
									vecBestSize = vecSize;
									vecBestShiftInObjectSpace = vecCenterOffset + (vecTopLeft - vecTopLeftOriginal);
									vecBestCenterOffset = vecCenterOffset;
									iPreferredAnimChosen = preflist;
								}
								pDeathListPreferred[preflist].fScore = fScore;
							}
						}
						if (iPreferredAnimChosen >= 0)
						{
							float fThresholdForMovingTooFarX = 22.5f;
							float fThresholdForMovingTooFarZ = 25.0f;
							if (fabs(vecBestShiftInObjectSpace.x) > fThresholdForMovingTooFarX || fabs(vecBestShiftInObjectSpace.z) > fThresholdForMovingTooFarZ)
							{
								iPreferredAnimChosen = -1;
							}
						}
						if (iPreferredAnimChosen >= 0)
						{
							// if no floor area truncation, can use preferred anim
							if (vecNavMeshFloorSize.x == 100 && vecNavMeshFloorSize.z == 100)
							{
								// no worries about space, so can have ideal animation based on shot trajectory
								if (bNoSpaceBehindOrInFrontCharacterInNavMesh == true)
									iPreferredAnimChosen = 0;
								else
									iPreferredAnimChosen = rand() % 3;
							}
							// ensure size is not bigger than detected floor (so no silly physicsness)
							if (vecBestSize.x > vecNavMeshFloorSize.x) vecBestSize.x = vecNavMeshFloorSize.x;
							if (vecBestSize.y > 20.0f) vecBestSize.y = 20.0f;
							if (vecBestSize.z > vecNavMeshFloorSize.z) vecBestSize.z = vecNavMeshFloorSize.z;
							// find spare physics obj for ragdoll plus
							int iSpareRagdollPlusPhyObj = g.ragdollplussystemobjstart;
							for (; iSpareRagdollPlusPhyObj < g.ragdollplussystemobjfinish; iSpareRagdollPlusPhyObj++)
							{
								if (ObjectExist(iSpareRagdollPlusPhyObj) == 0)
									break;
							}
							// update new shape box bounds at new position that sits inside floor box
							if (ObjectExist(iSpareRagdollPlusPhyObj) == 1)
							{
								ODEDestroyObject(iSpareRagdollPlusPhyObj);
								DeleteObject(iSpareRagdollPlusPhyObj);
							}
							t.entityelement[t.e].ragdollifiedplusphyobj = iSpareRagdollPlusPhyObj;
							MakeObjectBox (iSpareRagdollPlusPhyObj, vecBestSize.x, vecBestSize.y, vecBestSize.z);
							GGVECTOR3 vecBestShiftInWorldSpace;
							GGVec3TransformCoord(&vecBestShiftInWorldSpace, &vecBestShiftInObjectSpace, &matInvRotY);
							vecObjPos += vecBestShiftInWorldSpace;
							float fRaiseForTheFall = 20.0f;
							PositionObject (iSpareRagdollPlusPhyObj, vecObjPos.x, vecObjPos.y + fRaiseForTheFall, vecObjPos.z);
							RotateObject(iSpareRagdollPlusPhyObj, 0, fObjectOriginalRotated, 0);
							if (g_bShowRecastDetourDebugVisuals == false)
							{
								HideObject(iSpareRagdollPlusPhyObj);
							}
							if (t.entityelement[t.e].usingphysicsnow == 1)
							{
								ODEDestroyObject(t.tobj);
								t.entityelement[t.e].usingphysicsnow = 0;
							}
							ODECreateDynamicBox(iSpareRagdollPlusPhyObj, -1, 13);
							ODESetBodyFriction(iSpareRagdollPlusPhyObj, 55);

							// completely end this character
							t.smoothanim[t.tobj].playflag = 0;
							t.entityelement[t.e].destroyme = 0;
							t.entityelement[t.e].active = 0;
							t.entityelement[t.e].health = 0;
							t.entityelement[t.e].lua.flagschanged = 2;

							// remove pivot and readjust char object (undo this when restore from test level)
							RotateLimb(t.tobj, 0, 0, 180, 0);
							ResetObjectPivot(t.tobj);

							// play the chosen animation
							PlayObject (t.tobj, pDeathListPreferred[iPreferredAnimChosen].iStartFrame, pDeathListPreferred[iPreferredAnimChosen].iEndFrame);
							t.smoothanim[t.tobj].fn = pDeathListPreferred[iPreferredAnimChosen].iEndFrame;
							t.smoothanim[t.tobj].usefulTimer = timeGetTime();
							SetObjectSpeed(t.tobj, 25.0f);

							// created for this frame 
							g_bOnlyOneRagdollPlusPerCycleForPerformance = false;
						}
						else
						{
							// absolutely no animation would work, defer to ragdoll method
							bGoToRagdollNow = true;
						}
						// restore original bounds after above per-frame calcs
						pObject->collision.vecMin = vecMinOrig;
						pObject->collision.vecMax = vecMaxOrig;
						pObject->collision.vecCentre = vecCenterOrig;
					}
					else
					{
						// steal orientation from shape box finding a nice floor
						if (ObjectExist(iSpareRagdollPlusPhyObj) == 1)
						{
							sObject* pObject = GetObjectData(t.tobj);
							float fCurrentFrame = WickedCall_GetObjectFrame(pObject);
							if (fCurrentFrame >= pObject->fAnimFrameEnd)
							{
								// stop animation
								StopObject(t.tobj);

								// perhaps transition to ragdoll for final settlement
								t.entityelement[t.e].ragdollplusactivate = 99; // special mode - ignore ragdoll!!
								t.entityelement[t.e].ragdollplusweapontypeused = 0;
								bGoToRagdollNow = true;
							}
							else
							{
								// after one second of death anim, can release weapon
								if (timeGetTime() - t.smoothanim[t.tobj].usefulTimer > 500)
								{
									bDropAnyWeaponNow = true;
								}

								// use the last frame of the death anim to anchor for final resting position
								float fInitialWaitBeforeCopyPhysicsBoxAngle = 10; //ms
								if (timeGetTime() > t.smoothanim[t.tobj].usefulTimer + fInitialWaitBeforeCopyPhysicsBoxAngle)
								{
									SetObjectToObjectOrientation(t.tobj, iSpareRagdollPlusPhyObj);
								}
								float fInitialWaitBeforeCopyPhysicsBoxFalling = 20; //ms
								if (timeGetTime() > t.smoothanim[t.tobj].usefulTimer + fInitialWaitBeforeCopyPhysicsBoxFalling)
								{
									float fDX = ObjectPositionX(iSpareRagdollPlusPhyObj) - t.entityelement[t.e].x;
									float fDY = (ObjectPositionY(iSpareRagdollPlusPhyObj) - (ObjectSizeY(iSpareRagdollPlusPhyObj) / 2)) - t.entityelement[t.e].y;
									float fDZ = ObjectPositionZ(iSpareRagdollPlusPhyObj) - t.entityelement[t.e].z;
									fDX /= 20.0f; fDY /= 10.0f; fDZ /= 20.0f;
									float fInitialWaitBeforeCopyPhysicsBoxFalling = 350; //ms
									if (timeGetTime() > t.smoothanim[t.tobj].usefulTimer + fInitialWaitBeforeCopyPhysicsBoxFalling)
									{
										// we slerp to the physics object center - all axis
										t.entityelement[t.e].x += fDX;
										t.entityelement[t.e].y += fDY;
										t.entityelement[t.e].z += fDZ;
									}
									else
									{
										// we slerp to the physics object center - just X and Z while physics settles
										t.entityelement[t.e].x += fDX;
										t.entityelement[t.e].z += fDZ;
									}
									PositionObject (t.tobj, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z);
								}

								// keep gun attached to hand during death anim (no longer can use the regular call)
								int iObjStore = t.tobj;
								entity_controlattachments ();
								t.tobj = iObjStore;
							}
						}
					}
					pObject->collision.vecMin = vecMinOrig;
					pObject->collision.vecMax = vecMaxOrig;
					pObject->collision.vecCentre = vecCenterOrig;

					// can drop weapon after death anim is underway
					if ( bDropAnyWeaponNow == true || bGoToRagdollNow == true )
					{
						// make attachment object a physics object so it drops in better place (either immediate if ragdoll, or at end of anim if death anim - see above)
						t.tattobj = t.entityelement[t.e].attachmentobj;
						if (t.tattobj > 0)
						{
							// and ensure it does not bury into surface by raising it
							if (ODEFind(t.tattobj) == 0)
							{
								ODECreateDynamicBox (t.tattobj, -1, 1);
								ODESetLinearVelocity(t.tattobj, 0, 20.0f, 0);
							}
						}
					}
					if (bGoToRagdollNow == true)
					{
						// delete any assist physics objects and start ragdoll
						int iSpareRagdollPlusPhyObj = t.entityelement[t.e].ragdollifiedplusphyobj;
						if (ObjectExist(iSpareRagdollPlusPhyObj) == 1)
						{
							ODEDestroyObject(iSpareRagdollPlusPhyObj);
							DeleteObject(iSpareRagdollPlusPhyObj);
						}
						t.entityelement[t.e].ragdollifiedplusphyobj = 0;
						if (t.entityelement[t.e].ragdollplusactivate != 99 )
						{
							// pretty horrible - ragdoll overhaul much needed!!
							// create the ragdoll
							t.ttte = t.e;
							ragdoll_setcollisionmask (t.entityelement[t.ttte].eleprof.colondeath);
							t.tphye = t.ttte; t.tphyobj = t.entityelement[t.ttte].obj; 
							ragdoll_create();
							// prep ragdolified vars for zero force, just collapse
							t.entityelement[t.ttte].ragdollified = 1;
							t.entityelement[t.ttte].ragdollifiedforcex_f = 0.8f;
							t.entityelement[t.ttte].ragdollifiedforcey_f = 0.0f;
							t.entityelement[t.ttte].ragdollifiedforcez_f = 0.8f;
							t.entityelement[t.ttte].ragdollifiedforcevalue_f = 0.0f;
							t.entityelement[t.ttte].ragdollifiedforcelimb = 0;
						}
						t.entityelement[t.e].ragdollplusactivate = 0;
						t.entityelement[t.e].ragdollplusweapontypeused = 0;
					}
				}
			}
			#endif

			// Entity Prompt Local
			if ( t.entityelement[t.e].overprompttimer>0 ) 
			{
				if ( ObjectExist(t.tobj) == 1 ) 
				{
					#ifdef VRTECH
					if ( Timer()>(int)t.entityelement[t.e].overprompttimer ) 
					{
						if ( t.entityelement[t.e].overpromptuse3D == false ) 
							t.entityelement[t.e].overprompttimer=0;
						else
							lua_hideperentity3d ( t.e );
					}
					else
					{
						if ( t.entityelement[t.e].overpromptuse3D == false )
						{
							if ( GetInScreen(t.tobj) == 1 ) 
							{
								t.t_s=t.entityelement[t.e].overprompt_s ; t.twidth=getbitmapfontwidth(t.t_s.Get(),1)/2;
								pastebitmapfont(t.t_s.Get(),GetScreenX(t.tobj)-t.twidth,GetScreenY(t.tobj),1,255);
							}
						}
						else
						{
							lua_updateperentity3d ( t.e, t.entityelement[t.e].overprompt_s.Get(), t.entityelement[t.e].overprompt3dX, t.entityelement[t.e].overprompt3dY, t.entityelement[t.e].overprompt3dZ, t.entityelement[t.e].overprompt3dAY, t.entityelement[t.e].overprompt3dFaceCamera );
						}
					}
					#else
					if (  Timer()>(int)t.entityelement[t.e].overprompttimer ) 
					{
						t.entityelement[t.e].overprompttimer=0;
					}
					else
					{
						if (  GetInScreen(t.tobj) == 1 ) 
						{
							t.t_s=t.entityelement[t.e].overprompt_s ; t.twidth=getbitmapfontwidth(t.t_s.Get(),1)/2;
							pastebitmapfont(t.t_s.Get(),GetScreenX(t.tobj)-t.twidth,GetScreenY(t.tobj),1,255);
						}
					}
					#endif
				}
			}

			// if ragdoll and has force, apply it repeatedly
			if ( t.tobj>0 ) 
			{
				if (t.entityelement[t.e].ragdollified == 1 && t.entityelement[t.e].ragdollifiedforcevalue_f > 1.0)
				{
					if (BPhys_RagDollApplyForce (t.tobj, t.entityelement[t.e].ragdollifiedforcelimb, 0, 0, 0, t.entityelement[t.e].ragdollifiedforcex_f, t.entityelement[t.e].ragdollifiedforcey_f, t.entityelement[t.e].ragdollifiedforcez_f, t.entityelement[t.e].ragdollifiedforcevalue_f) == true)
					{
						t.entityelement[t.e].ragdollifiedforcevalue_f = t.entityelement[t.e].ragdollifiedforcevalue_f*0.75;
						if (t.entityelement[t.e].ragdollifiedforcevalue_f <= 1.0)
						{
							t.entityelement[t.e].ragdollifiedforcevalue_f = 0;
						}
					}
				}
			}

			//  obtain distance from camera/player
			entity_controlrecalcdist ( );
			if (t.entityelement[t.e].lua.flagschanged == 0)
			{
				if (abs(t.entityelement[t.e].plrdist - t.dist_f) > 10)
				{
					t.entityelement[t.e].lua.flagschanged = 1;
				}
			}
			t.entityelement[t.e].plrdist=t.dist_f;

			// control immunity for entities
			if ( t.entityelement[t.e].briefimmunity > 0 )
			{
				t.entityelement[t.e].briefimmunity--;
			}
			else
			{
				// when not immune, check if characters are underwater and damage them as a result
				if (t.entityprofile[t.entid].ischaracter == 1)
				{
					if (t.entityelement[t.e].y + 65.0f < t.terrain.waterliney_f)
					{
						// damage from drowning (means we do not need handling in ALL character scripts)
						// NOTE: Future feature could switch off the default drown so some charactes can swim and go underwater..
						t.tdamage = 10;
						t.tdamageforce = 0;
						t.brayx1_f = t.entityelement[t.e].x;
						t.brayy1_f = t.entityelement[t.e].y+10.0f;
						t.brayz1_f = t.entityelement[t.e].z;
						t.brayx2_f = t.entityelement[t.e].x;
						t.brayy2_f = t.entityelement[t.e].y;
						t.brayz2_f = t.entityelement[t.e].z;
						t.tdamagesource = 0;
						t.ttte = t.e; entity_applydamage(); t.e = t.ttte;
					}
				}
			}

			// in all active states, must repell player to avoid penetration
			if ( t.tobj > 0 ) 
			{
				if ( (t.entityprofile[t.entid].ischaracter == 1 && t.entityprofile[t.entid].collisionmode != 22) || t.entityprofile[t.entid].collisionmode == 21 )
				{
					if ( t.entityelement[t.e].health>0 && t.entityelement[t.e].usingphysicsnow == 1 ) 
					{
						bool bThirdPersonPlayer = false;
						if ( t.playercontrol.thirdperson.enabled == 1 && t.playercontrol.thirdperson.charactere == t.e ) bThirdPersonPlayer = true;
						if ( bThirdPersonPlayer == false )
						{
							t.tplrproxx_f=ObjectPositionX(t.aisystem.objectstartindex)-ObjectPositionX(t.tobj);
							t.tplrproyy_f=ObjectPositionY(t.aisystem.objectstartindex)-ObjectPositionY(t.tobj);
							t.tplrproxz_f=ObjectPositionZ(t.aisystem.objectstartindex)-ObjectPositionZ(t.tobj);
							t.tplrproxd_f=Sqrt(abs(t.tplrproxx_f*t.tplrproxx_f)+abs(t.tplrproyy_f*t.tplrproyy_f)+abs(t.tplrproxz_f*t.tplrproxz_f));
							t.tplrproxa_f=atan2deg(t.tplrproxx_f,t.tplrproxz_f);
							if (  t.tplrproxd_f<t.entityprofile[t.entid].fatness ) 
							{
								//t.playercontrol.pushforce_f = 1.0f;
								float fDepthOfPen = t.entityprofile[t.entid].fatness - t.tplrproxd_f;
								if (fDepthOfPen > 1.0f)
								{
									t.playercontrol.pushforce_f += fDepthOfPen / 100.0f;
									t.playercontrol.pushangle_f = t.tplrproxa_f;
								}
							}
						}
					}
				}
			}

			// Handle when entity limb flinch hurt system
			if ( t.entityelement[t.e].limbhurt>0 && t.entityelement[t.e].health>0 ) 
			{
				//  known limbs
				t.headlimbofcharacter=t.entityprofile[t.entityelement[t.e].bankindex].headlimb;
				t.spine2limbofcharacter=t.entityprofile[t.entityelement[t.e].bankindex].spine2;
				//  determine which segment the limb belongs
				t.tsegmenttoflinch=0;
				if (  t.entityelement[t.e].limbhurt == t.headlimbofcharacter  )  t.tsegmenttoflinch = 1;
				//  degrade flinch value until finished
				t.tsmoothspeed_f=3.0/g.timeelapsed_f;
				t.entityelement[t.e].limbhurta_f=CurveValue(0,t.entityelement[t.e].limbhurta_f,t.tsmoothspeed_f);
				if (  abs(t.entityelement[t.e].limbhurta_f)<1.0 ) 
				{
					t.entityelement[t.e].limbhurta_f=0;
					t.entityelement[t.e].limbhurt=0;
				}
				//  modify character limbs based on segment hurt
				if (  t.tobj>0 ) 
				{
					if (  ObjectExist(t.tobj) == 1 ) 
					{
						if (  t.tsegmenttoflinch == 0 ) 
						{
							if (  t.spine2limbofcharacter>0 ) 
							{
								if (  LimbExist(t.tobj,t.spine2limbofcharacter) == 1 ) 
								{
									RotateLimb (  t.tobj,t.spine2limbofcharacter,t.entityelement[t.e].limbhurta_f/3.0,t.entityelement[t.e].limbhurta_f*-1,0 );
								}
							}
						}
						if (  t.tsegmenttoflinch == 1 ) 
						{
							if (  t.headlimbofcharacter>0 ) 
							{
								if (  LimbExist(t.tobj,t.headlimbofcharacter) == 1 ) 
								{
									RotateLimb (  t.tobj,t.headlimbofcharacter,t.entityelement[t.e].limbhurta_f,LimbAngleY(t.tobj,t.headlimbofcharacter),LimbAngleZ(t.tobj,t.headlimbofcharacter) );
								}
							}
						}
					}
				}
			}

			// if entity using non-3d sound, needs to update based on camera position
			// (can also be used for moving entities that LoopSound ( later) )
			if ( t.entityelement[t.e].soundisnonthreedee == 1 ) 
			{
				t.entityelement[t.e].soundisnonthreedee=0;
				if ( t.entityelement[t.e].soundset>0 ) 
				{
					PositionSound (  t.entityelement[t.e].soundset,CameraPositionX(0),CameraPositionY(0),CameraPositionZ(0) );
					if ( SoundPlaying(t.entityelement[t.e].soundset) == 1 ) 
					{
						t.entityelement[t.e].soundisnonthreedee=1;
					}
				}
				if ( t.entityelement[t.e].soundset1>0 ) 
				{
					PositionSound (  t.entityelement[t.e].soundset1,CameraPositionX(0),CameraPositionY(0),CameraPositionZ(0) );
					if (  SoundPlaying(t.entityelement[t.e].soundset1) == 1 ) 
					{
						t.entityelement[t.e].soundisnonthreedee=1;
					}
				}
				if (  t.entityelement[t.e].soundset2>0 ) 
				{
					PositionSound (  t.entityelement[t.e].soundset2,CameraPositionX(0),CameraPositionY(0),CameraPositionZ(0) );
					if (  SoundPlaying(t.entityelement[t.e].soundset2) == 1 ) 
					{
						t.entityelement[t.e].soundisnonthreedee=1;
					}
				}
				if (  t.entityelement[t.e].soundset3>0 ) 
				{
					PositionSound (  t.entityelement[t.e].soundset3,CameraPositionX(0),CameraPositionY(0),CameraPositionZ(0) );
					if (  SoundPlaying(t.entityelement[t.e].soundset3) == 1 ) 
					{
						t.entityelement[t.e].soundisnonthreedee=1;
					}
				}
				if (t.entityelement[t.e].soundset5 > 0)
				{
					PositionSound(t.entityelement[t.e].soundset5, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
					if (SoundPlaying(t.entityelement[t.e].soundset5) == 1)
					{
						t.entityelement[t.e].soundisnonthreedee = 1;
					}
				}
				if (t.entityelement[t.e].soundset6 > 0)
				{
					PositionSound (t.entityelement[t.e].soundset6, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
					if (SoundPlaying(t.entityelement[t.e].soundset6) == 1)
					{
						t.entityelement[t.e].soundisnonthreedee = 1;
					}
				}
			}

			// character creator object
			if ( t.entityprofile[t.entid].ischaractercreator == 1 ) 
			{
				t.tccobj = g.charactercreatorrmodelsoffset+((t.e*3)-t.characterkitcontrol.offset);
				if ( ObjectExist(t.tccobj) == 1 ) 
				{
					// only glue head if enemy is visible
					t.tconstantlygluehead=0;
					if ( t.tobj>0 ) { if ( GetVisible(t.tobj)==1 ) { t.tconstantlygluehead=1; } } 
					if ( t.game.runasmultiplayer == 1 ) 
					{
						// deal with multiplayer issues - if ( its me, ) only show me when im dead
						if ( t.characterkitcontrol.showmyhead == 1 && t.e == t.mp_playerEntityID[g.mp.me] ) 
						{
							t.tconstantlygluehead=1;
						}
						// if other players are dead and transitioning to a new spawn postion
						for ( t.ttemploop = 0 ; t.ttemploop <= MP_MAX_NUMBER_OF_PLAYERS; t.ttemploop++ )
						{
							if ( t.ttemploop != g.mp.me ) 
							{
								#ifdef PHOTONMP
								 int iAlive = PhotonGetPlayerAlive(t.ttemploop);
								#else
								 int iAlive = SteamGetPlayerAlive(t.ttemploop);
								#endif
								if ( t.e == t.mp_playerEntityID[t.ttemploop] && t.mp_forcePosition[t.ttemploop]>0 && iAlive == 1 ) 
								{
									t.tconstantlygluehead=0;
								}
							}
						}
					}
					// if head is flagged to by glued, attach to body now
					if ( t.tconstantlygluehead == 1 ) 
					{
						// NOTE; re-searching for head limb is a performance hit
						t.tSourcebip01_head=getlimbbyname(t.entityelement[t.e].obj, "Bip01_Head");
						if ( t.tSourcebip01_head>0 ) 
						{
							//Dave - fix to heads being backwards for characters when switched off (3000 units away)
							float tdx = CameraPositionX(0) - ObjectPositionX(t.entityelement[t.e].obj);
							float tdy = CameraPositionY(0) - ObjectPositionY(t.entityelement[t.e].obj);
							float tdz = CameraPositionZ(0) - ObjectPositionZ(t.entityelement[t.e].obj);
							float tdist = sqrt ( tdx*tdx + tdy*tdy + tdz*tdz );
							t.te = t.e; entity_getmaxfreezedistance ( );
							if ( tdist > t.maximumnonefreezedistance )
							{
								YRotateObject (  t.tccobj, ObjectAngleY(t.entityelement[t.e].obj)-180 );
							}
							else
							{
								YRotateObject (  t.tccobj, 0 );								
							}							
							GlueObjectToLimbEx (  t.tccobj,t.entityelement[t.e].obj,t.tSourcebip01_head,2 );
						}
					}
					else
					{
						//  else unglue and hide the head
						UnGlueObject (  t.tccobj );
						PositionObject (  t.tccobj,100000,100000,100000 );
					}
				}
			}

			// handle particle emitter entity (for when in game)
			#ifdef WICKEDENGINE
			entity_updateparticleemitter(t.e);
			entity_updateautoflatten(t.e);
			#endif

			// flag to destroy entity dead (can be set from LUA command or explosion trigger)
			if ( t.entityelement[t.e].destroyme == 1 ) 
			{
				// mark as destroyed officially
				t.entityelement[t.e].destroyme = 0;
				entity_adddestroyevent(t.e);

				// remove entity from game play
				t.entityelement[t.e].eleprof.phyalways = 0;
				t.entityelement[t.e].active = 0;
				t.entityelement[t.e].health = 0;
				t.entityelement[t.e].lua.flagschanged = 2;
				if ( t.game.runasmultiplayer == 1 ) 
				{
					mp_addDestroyedObject ( );
				}
				t.obj=t.entityelement[t.e].obj;

				// do not hide if ragdollified
				if (t.entityelement[t.e].ragdollified == 0)
				{
					if (t.obj > 0)
					{
						if (ObjectExist(t.obj) == 1)
						{
							HideObject (t.obj);
						}
					}
				}

				// attempt to remove collision object
				entity_lua_collisionoff ( );

				// possible remove character
				entity_lua_findcharanimstate ( );
				if (  t.tcharanimindex != -1 ) 
				{
					//  deactivate DarkA.I for this dead entity
					darkai_killai ( );

					//  Convert object back to instance and hide it
					darkai_character_remove ( );
					t.charanimstates[t.tcharanimindex] = t.charanimstate;
				}
				else
				{
					//  can still have non-character ragdoll (zombie), so remove ragdoll if so
					t.tphyobj=t.obj ; ragdoll_destroy ( );
				}

				// find and remove if in any inventory
				for (int inventoryindex = 0; inventoryindex < t.inventoryContainers.size(); inventoryindex++)
				{
					for (int n = 0; n < t.inventoryContainer[inventoryindex].size(); n++)
					{
						if (t.inventoryContainer[inventoryindex][n].e == t.e)
						{
							t.inventoryContainer[inventoryindex].erase(t.inventoryContainer[inventoryindex].begin() + n);
							break;
						}
					}
				}
			}
		}
	}

	//  handle explosion triggers in separate loop as they call
	//  other subroutines that use E and other entity calls (i.e. physics_explodesphere)
	for ( t.ee = 1 ; t.ee <= g.entityelementlist; t.ee++ )
	{
		t.eentid=t.entityelement[t.ee].bankindex;
		if ( t.eentid>0 ) 
		{
			//  flag to explode entity after a fused amount of time
			if ( t.entityelement[t.ee].explodefusetime>0 ) 
			{
				if ( Timer()>t.entityelement[t.ee].explodefusetime ) 
				{
					t.entityelement[t.ee].explodefusetime = -1;
					// explode from beneath
					t.tdamage=t.entityelement[t.ee].eleprof.explodedamage;
					t.tdamageforce=0;
					t.brayx1_f=ObjectPositionX(t.entityelement[t.ee].obj)+GetObjectCollisionCenterX(t.entityelement[t.ee].obj);
					t.brayy1_f=(ObjectPositionY(t.entityelement[t.ee].obj)+GetObjectCollisionCenterY(t.entityelement[t.ee].obj))-100;
					t.brayz1_f=ObjectPositionZ(t.entityelement[t.ee].obj)+GetObjectCollisionCenterZ(t.entityelement[t.ee].obj);
					t.brayx2_f=t.brayx1_f;
					t.brayy2_f=(t.brayy1_f+100);
					t.brayz2_f=t.brayz1_f;
					t.tdamagesource=0;
					t.ttte = t.ee ; entity_applydamage() ; t.ee=t.ttte;
					// create a huge bang
					t.entityelement[t.ee].destroyme=1;
					#ifdef WICKEDENGINE
					// customize the barrel explosion in "projectiletypes\common\explode"
					t.tProjectileName_s = "";
					t.tProjectileResult = WEAPON_PROJECTILERESULT_EXPLODE;
					for (int w = 0; w < t.WeaponProjectileBase.size(); w++)
					{
						if (strstr (t.WeaponProjectileBase[w].name_s.Get(), "common\\explode") > 0)
						{
							t.tProjectileResult = WEAPON_PROJECTILERESULT_CUSTOM;
							t.tProjectileName_s = t.WeaponProjectileBase[w].name_s.Get();
							t.tProjectileResultExplosionImageID = t.WeaponProjectileBase[w].explosionImageID;
							t.tProjectileResultLightFlag = t.WeaponProjectileBase[w].explosionLightFlag;
							t.tProjectileResultSmokeImageID = t.WeaponProjectileBase[w].explosionSmokeImageID;
							t.tProjectileResultSparksCount = t.WeaponProjectileBase[w].explosionSparksCount;
							t.tProjectileResultSize = t.WeaponProjectileBase[w].explosionSize;
							t.tProjectileResultSmokeSize = t.WeaponProjectileBase[w].explosionSmokeSize;
							t.tProjectileResultSparksSize = t.WeaponProjectileBase[w].explosionSparksSize;
							break;
						}
					}
					#else
					t.tProjectileName_s = "";
					t.tProjectileResult = WEAPON_PROJECTILERESULT_EXPLODE;
					#endif
					t.tx_f=t.entityelement[t.ee].x ; t.ty_f=t.entityelement[t.ee].y ; t.tz_f=t.entityelement[t.ee].z;
					t.tDamage_f = t.entityelement[t.ee].eleprof.explodedamage; 
					t.tradius_f = 300;
					t.tSourceEntity = t.ee;
					// provide the explosion sound (as it cannot come from projectile)
					t.tSoundID=0;
					#define TENCLONESOUNDSFOREXPLOSIONS 10
					for ( t.tscanexp = 1 ; t.tscanexp <= TENCLONESOUNDSFOREXPLOSIONS; t.tscanexp++ )
					{
						if (t.tscanexp > 0 && SoundExist(g.explodesoundoffset + t.tscanexp) == 0)
						{
							CloneSound(g.explodesoundoffset + t.tscanexp, g.explodesoundoffset);
						}
						if ( SoundExist(g.explodesoundoffset+t.tscanexp) == 1 ) 
						{
							if ( SoundPlaying(g.explodesoundoffset+t.tscanexp) == 0 ) 
							{
								t.tSoundID=g.explodesoundoffset+t.tscanexp;
							}
						}
					}
					weapon_projectileresult_make ( );
				}
			}
		}
	}
}

//PE: No speed difference with normal usage.
//#define SPEEDTESTOLDSYSTEM 
//PE: All animations (master objects) will now animate independent of fps.
//PE: Now allow "static" with animation to use AnimSpeed per object.
void entity_loopanim ( void )
{
	// In game or editor, must control entity animation speed (machine indie)#
	static int currentsynccount = 0;
	currentsynccount++;

	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.entid = t.entityelement[t.e].bankindex;

		if (t.entid <= 0) {
			#ifdef WICKEDENGINE
			//PE: Also control objects in properties.
			extern bool bImGuiInTestGame;
			if (t.game.gameisexe == 0 && t.gridentityinzoomview == t.e && !bImGuiInTestGame)
			{
				t.entid = t.gridentity; //t.entityelement[t.gridentityinzoomview].bankindex;
			}
			else
			#endif
				continue;
		}

		t.tparentobj = g.entitybankoffset + t.entid;

		// 011016 - scenes with LARGE number of static entities hitting perf hard
		//PE: @Lee if the same master object is used without any per object animspeed changes , it will be like normal.
		//PE: Only if animspeed is changed (per object) it will create a new clone , so should not bring down fps that bad.
#ifdef SPEEDTESTOLDSYSTEM
		if (t.entityelement[t.e].staticflag == 1 && t.entityelement[t.e].eleprof.phyalways == 0)
			continue;
#else
		if (t.entityelement[t.e].staticflag == 1 && t.entityelement[t.e].eleprof.phyalways == 0)
		{
			//Quickly skip entry, but still allow custom anim on static objects.
			if ( t.entityelement[t.e].eleprof.animspeed == t.entityprofile[t.entid].animspeed) {

				//PE: We still need to set speed on master object. once each sync.
				if ( t.entityprofile[t.entid].synccount != currentsynccount && t.entityprofile[t.entid].ischaracter == 0 && t.entityelement[t.e].isclone == 0 && ObjectExist(t.tparentobj) == 1) {

					t.entityprofile[t.entid].synccount = currentsynccount; //Only update one time per sync.
					if (GetNumberOfFrames(t.tparentobj) > 0)
					{
						t.tanimspeed_f = t.entityprofile[t.entid].animspeed;
						if (ObjectExist(t.tparentobj) == 1) 
						{
							#ifdef WICKEDENGINE
							SetObjectSpeed(t.tparentobj, t.tanimspeed_f);
							#else
							SetObjectSpeed(t.tparentobj, g.timeelapsed_f*t.tanimspeed_f);
							#endif
						}
					}
				}
				continue;
			}
		}
#endif
		// NOTE: Determine essential tasks static needs (i.e. plrdist??)
		// only handle DYNAMIC entities 
		if ( t.entid>0 ) 
		{
			//t.tparentobj=g.entitybankoffset+t.entid;

			#ifdef WICKEDENGINE
			//PE: Add decal support here.
			if (t.entityprofile[t.entid].bIsDecal)
			{
				t.tobj = t.entityelement[t.e].obj;
				if (t.tobj > 0)
				{

					float fNextFrame = g.timeelapsed_f*t.entityelement[t.e].fDecalSpeed;

					t.entityelement[t.e].fDecalFrame = t.entityelement[t.e].fDecalFrame + fNextFrame;

					if (t.entityelement[t.e].fDecalFrame < 0) t.entityelement[t.e].fDecalFrame = 0;
					if (t.entityelement[t.e].fDecalFrame > (t.entityprofile[t.entid].iDecalRows*t.entityprofile[t.entid].iDecalColumns))
						t.entityelement[t.e].fDecalFrame = 0;

					if (ObjectExist(t.tobj) == 1)
					{
						LockVertexDataForLimbCore(t.tobj, 0, 1);
						SetVertexDataNormals(0, 0, 1, 0);
						SetVertexDataNormals(1, 0, 1, 0);
						SetVertexDataNormals(2, 0, 1, 0);
						SetVertexDataNormals(3, 0, 1, 0);
						SetVertexDataNormals(4, 0, 1, 0);
						SetVertexDataNormals(5, 0, 1, 0);
						UnlockVertexData();
						SetObjectUVManually(t.tobj, t.entityelement[t.e].fDecalFrame, t.entityprofile[t.entid].iDecalRows, t.entityprofile[t.entid].iDecalColumns);
					}


					//Just for testing, this is done in lua.
					//float camx = CameraPositionX();
					//float camy = CameraPositionY();
					//float camz = CameraPositionZ();
					//PointObject(t.tobj, camx, camy, camz);
					//XRotateObject(t.tobj, 0); ZRotateObject(t.tobj, 0);
					//YRotateObject(t.tobj, ObjectAngleY(t.tobj)+ 180);

					/*
					sObject* pObject = g_ObjectList[t.tobj];
					if (pObject)
					{
						//PE: SetObjectCull(t.tobj, 1); Dont work.
						//PE: iCullMode need to be zero in wicked ?
						for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
						{
							if (pObject->ppMeshList[iMesh]) pObject->ppMeshList[iMesh]->iCullMode = 0;
						}
						WickedCall_SetObjectCullmode(pObject);
					}
					*/
				}
			}
			else if (t.entityprofile[t.entid].ischaracter == 0)
			#else
			if ( t.entityprofile[t.entid].ischaracter == 0 ) 
			#endif
			{
				// but not for characters which have their own speed control
				t.tobj=t.entityelement[t.e].obj;
				if ( t.tobj>0 ) 
				{
					if ( ObjectExist(t.tobj) == 1 && ObjectExist(t.tparentobj) == 1 ) 
					{
						if ( GetNumberOfFrames(t.tparentobj)>0 ) 
						{
							// 120417 - now modulate anim speed with script controlled modulation
							float fFinalAnimSpeed = t.entityelement[t.e].eleprof.animspeed * t.entityelement[t.e].animspeedmod;

							// Detect if entity instance speed different from base default, and if so, need CLONE!
							if ( t.entityelement[t.e].isclone == 0 ) 
							{
								if ( fFinalAnimSpeed != t.entityprofile[t.entid].animspeed ) 
								{
									// Entity must be unique to allow different speed from parent
									t.tte = t.e ; entity_converttoclone ( );
									//Start animation.
									t.tobj = t.entityelement[t.e].obj;
									if (GetNumberOfFrames(t.tobj) > 0)
									{
										//  allow first animation
										if ((t.game.set.ismapeditormode == 0 || t.game.gameisexe == 1) && t.entityprofile[t.entid].startanimingame > 0 && t.entityprofile[t.entid].animmax > 0) 
										{ 
											//PE:
											t.q = t.entityprofile[t.entid].startanimingame - 1;
											if (t.q >= 0)
												LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
										}
										else if (t.entityprofile[t.entid].animmax > 0 && t.entityprofile[t.entid].playanimineditor > 0)
										{
											SetObjectFrame(t.tobj, 0); LoopObject(t.tobj); StopObject(t.tobj);
											t.q = t.entityprofile[t.entid].playanimineditor - 1;
											if (t.q >= 0)
												LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
										}
										else if (t.entityprofile[t.entid].playanimineditor < 0)
										{
											// uses name instead of index, the negative is the ordinal into the animset
											extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
											entity_loop_using_negative_playanimineditor(t.e, t.tobj, t.entityprofile[t.entid].playanimineditor_name);
										}
									}
								}
							}
							bool isWicked = false;
							#ifdef WICKEDENGINE
							//PE: Wicked is always clone.
							isWicked = true;
							#endif
							if ( t.entityelement[t.e].isclone == 1 || isWicked )
							{
								// Control animation speed of cloned object
								t.tanimspeed_f = fFinalAnimSpeed * t.entityelement[t.e].speedmodulator_f;
								#ifdef WICKEDENGINE
								SetObjectSpeed (t.tobj, t.tanimspeed_f);
								#else
								SetObjectSpeed (t.tobj, g.timeelapsed_f*t.tanimspeed_f);
								#endif
							}
							else
							{
								//PE: This is never called, (continue; above).
								//  Control animation speed of parent object associated with instance
								t.tanimspeed_f = t.entityprofile[t.entid].animspeed;
								if (ObjectExist(t.tparentobj) == 1)
								{
									#ifdef WICKEDENGINE
									SetObjectSpeed (t.tparentobj, t.tanimspeed_f);
									#else
									SetObjectSpeed (t.tparentobj, g.timeelapsed_f*t.tanimspeed_f);
									#endif
								}
							}
							//  if animation in progress (handle any transitioning)
							if ( t.entityelement[t.e].lua.animating == 1 ) 
							{
								smoothanimupdate(t.tobj);
							}
						}
					}
				}
			}

			// also handle entity footfall sounds from loopanim
			#ifdef WICKEDENGINE
			t.tobj = t.entityelement[t.e].obj;
			if (t.tobj > 0)
			{
				float fCurrentFrame = GetFrame(t.tobj);
				sObject* pObject = GetObjectData(t.tobj);
				if (pObject)
				{
					sAnimationSet* pAnimSet = pObject->pAnimationSet;
					while (pAnimSet)
					{
						int leftorright = 0;
						int iFootFallKeyFrame = (int)pAnimSet->fAnimSetStep1;
						float fDistanceFromFrame = fabs(fCurrentFrame - iFootFallKeyFrame);
						if (iFootFallKeyFrame > 0 && fCurrentFrame >= iFootFallKeyFrame && fDistanceFromFrame < 5.0f && iFootFallKeyFrame != t.entityelement[t.e].lastfootfallframeindex)
						{
							// okay to use this footfall (left one)
							leftorright = 0;
						}
						else
						{
							// if no luck, try the right foot
							iFootFallKeyFrame = (int)pAnimSet->fAnimSetStep2;
							fDistanceFromFrame = fabs(fCurrentFrame - iFootFallKeyFrame);
							if (iFootFallKeyFrame > 0 && fCurrentFrame >= iFootFallKeyFrame && fDistanceFromFrame < 5.0f && iFootFallKeyFrame != t.entityelement[t.e].lastfootfallframeindex)
							{
								// okay to use this footfall (right one)
								leftorright = 1;
							}
							else
							{
								// else try the final step (any one)
								iFootFallKeyFrame = (int)pAnimSet->fAnimSetStep3;
								fDistanceFromFrame = fabs(fCurrentFrame - iFootFallKeyFrame);
								leftorright = 0;
							}
						}
						if (iFootFallKeyFrame > 0 && fCurrentFrame >= iFootFallKeyFrame && fDistanceFromFrame < 5.0f && iFootFallKeyFrame != t.entityelement[t.e].lastfootfallframeindex)
						{
							// ensure this footfall frame not triggered again until another one gets triggered
							t.entityelement[t.e].lastfootfallframeindex = iFootFallKeyFrame;

							// choose footfall sound for character
							int iFootFallType = -1;

							// above or below water line
							if (t.entityelement[t.e].y > t.terrain.waterliney_f + 36 || t.hardwareinfoglobals.nowater != 0)
							{
								// this arb value is CHANGED if a capsule (character) when it touches physics!
								iFootFallType = ODEGetBodyAttribValue (t.tobj);
							}
							else
							{
								if (t.entityelement[t.e].y > t.terrain.waterliney_f - 33)
								{
									// footfall water wading sound
									iFootFallType = 17;
								}
								else
								{
									// underwater sound for character
									iFootFallType = 18;
								}
							}

							// manage trigger of footfall sound effects
							if (iFootFallType != -1)
							{
								// play footfall sound effect at character position
								sound_footfallsound (iFootFallType, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z, leftorright, &t.entityelement[t.e].lastfootfallsound);
							}
						}

						// next anim
						pAnimSet = pAnimSet->pNext;
					}
				}
			}
			#else
			int iFootFallMax = t.entityprofile[t.entid].footfallmax;
			if (iFootFallMax > 0)
			{
				t.tobj = t.entityelement[t.e].obj;
				if (t.tobj > 0)
				{
					for (int iFootFallIndex = 0; iFootFallIndex < iFootFallMax; iFootFallIndex++)
					{
						float fCurrentFrame = GetFrame(t.tobj);
						int iFootFallKeyFrame = t.entityfootfall[t.entid][iFootFallIndex].leftfootkeyframe;
						float fDistanceFromFrame = fabs(fCurrentFrame - iFootFallKeyFrame);
						if (fCurrentFrame >= iFootFallKeyFrame && fDistanceFromFrame < 5.0f && iFootFallKeyFrame != t.entityelement[t.e].lastfootfallframeindex)
						{
							// okay to use this footfall (left one)
						}
						else
						{
							// if no luck, try the right foot
							iFootFallKeyFrame = t.entityfootfall[t.entid][iFootFallIndex].rightfootkeyframe;
							float fDistanceFromFrame = fabs(fCurrentFrame - iFootFallKeyFrame);
						}
						if (fCurrentFrame >= iFootFallKeyFrame && fDistanceFromFrame < 5.0f && iFootFallKeyFrame != t.entityelement[t.e].lastfootfallframeindex)
						{
							// ensure this footfall frame not triggered again until another one gets triggered
							t.entityelement[t.e].lastfootfallframeindex = iFootFallKeyFrame;

							// choose footfall sound for character
							int iFootFallType = -1;

							// above or below water line
							if (t.entityelement[t.e].y > t.terrain.waterliney_f + 36 || t.hardwareinfoglobals.nowater != 0)
							{
								iFootFallType = ODEGetBodyAttribValue (t.tobj);
							}
							else
							{
								if (t.entityelement[t.e].y > t.terrain.waterliney_f - 33)
								{
									// Footfall water wading sound
									iFootFallType = 17;
								}
								else
								{
									// underwater sound for character
									iFootFallType = 18;
								}
							}

							//  Manage trigger of footfall sound effects
							if (iFootFallType != -1)
							{
								// play footfall sound effect at character position
								sound_footfallsound (iFootFallType, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z, 0, &t.entityelement[t.e].lastfootfallsound);
							}
						}
					}
				}
			}
			#endif
		}
	}
}

void entity_controlrecalcdist ( void )
{
	//  Distance between player camera and entity
	t.tobj=t.entityelement[t.e].obj;
	if (  t.tobj>0 && (t.entityelement[t.e].active != 0 || t.entityelement[t.e].eleprof.phyalways != 0) ) 
	{
		if (  ObjectExist(t.tobj)  ==  1 ) 
		{
			t.distx_f=CameraPositionX(0)-ObjectPositionX(t.tobj);
			t.disty_f=CameraPositionY(0)-ObjectPositionY(t.tobj);
			t.distz_f=CameraPositionZ(0)-ObjectPositionZ(t.tobj);
			t.dist_f=Sqrt(abs(t.distx_f*t.distx_f)+abs(t.disty_f*t.disty_f)+abs(t.distz_f*t.distz_f));
			t.diffangle_f=atan2deg(t.distx_f,t.distz_f);
			if (  t.diffangle_f<0  )  t.diffangle_f = t.diffangle_f+360;
		}
		else
		{
			t.dist_f=9999999;
		}
	}
	else
	{
		t.dist_f=9999999;
	}
return;

}

void entity_getmaxfreezedistance ( void )
{
	if (  t.entityelement[t.te].eleprof.phyalways != 0 ) 
	{
		//  always active characters NEVER freeze at distance
		t.maximumnonefreezedistance=MAXNEVERFREEZEDISTANCE;
	}
	else
	{
		//  distance at which logic is frozen
		t.maximumnonefreezedistance=MAXFREEZEDISTANCE;
	}
	//  AI that is attacking another player counts as on
	if (  t.game.runasmultiplayer  ==  1 ) 
	{
		if (  t.entityelement[t.te].mp_updateOn  ==  1  )  t.maximumnonefreezedistance = MAXNEVERFREEZEDISTANCE;
	}
}

void entity_updatepos ( void )
{
	// takes te - but not tv# as already dealth with when moved entity X Y Z
	t.tobj=t.entityelement[t.te].obj;

	// move entity using physics
	if ( t.entityelement[t.te].usingphysicsnow == 1 ) 
	{
		#ifdef WICKEDENGINE
		if(t.entityprofile[t.entityelement[t.te].bankindex].ischaracter == 1 && t.entityelement[t.te].eleprof.disableascharacter == 0)
		{
			// old legacy system was glitchy, pulling back real XZ and causing huge jumps into the air
			// simpler system gives priority to navmesh guide movement and rays to find surface, 
			// then move physics capsule to push dynamic stuff out of the way (no longer a controller)
			ODESetLinearVelocityUsingWorldPosTarget (t.tobj, t.entityelement[t.te].x, t.entityelement[t.te].y, t.entityelement[t.te].z);
		}
		else
		{
			// non-characters subjecty to simpler physics forces
			float fNewDistanceX = t.entityelement[t.te].x - ObjectPositionX(t.tobj);
			float fNewDistanceZ = t.entityelement[t.te].z - ObjectPositionZ(t.tobj);
			float fNewDistanceTotal = Sqrt(fabs(fNewDistanceX * fNewDistanceX) + fabs(fNewDistanceZ * fNewDistanceZ));
			if (fNewDistanceTotal <= 0) return;
			// work out normalized increment
			float fForceToApplyX = fNewDistanceX / fNewDistanceTotal;
			float fForceToApplyZ = fNewDistanceZ / fNewDistanceTotal;
			// apply the specified speed
			float fSpeed = fNewDistanceTotal;
			float fSpeedMax = 25.0f * ((t.entityelement[t.te].eleprof.speed + 0.0) / 100.0);
			if (fSpeed > fSpeedMax) fSpeed = fSpeedMax;
			fForceToApplyX *= (fSpeed * 15.0f);
			fForceToApplyZ *= (fSpeed * 15.0f);

			if (t.entityelement[t.te].nogravity == 1)
			{
				// special case of non character entity with gravity off (pickupable objects)
				float fNoGravY = t.entityelement[t.te].y - ObjectPositionY(t.tobj);
				if (fabs(fNoGravY) > 0.0f)
				{
					if (fNoGravY > fSpeedMax) fNoGravY = fSpeedMax;
					if (fNoGravY < -fSpeedMax) fNoGravY = -fSpeedMax;
					fNoGravY *= 30.0f; // keep it in eye view when look up and down 15.0f;
				}

				ODESetLinearVelocity(t.tobj, fForceToApplyX, fNoGravY * 2, fForceToApplyZ);
			}
			else
			{
				// apply force to physics object to get to new XZ position (eventually)
				ODESetLinearVelocityXZWithGravity(t.tobj, fForceToApplyX, fForceToApplyZ, t.tvgravity_f);
			}
		}
		#else
		//  control physics object (entity-driven)
		t.tvx_f=t.entityelement[t.te].x-ObjectPositionX(t.tobj);
		t.tvz_f=t.entityelement[t.te].z-ObjectPositionZ(t.tobj);
		t.tvd_f=Sqrt(abs(t.tvx_f*t.tvx_f)+abs(t.tvz_f*t.tvz_f));

		// cap relative to speed of entity
		t.tentspeed_f = (t.entityelement[t.te].eleprof.speed+0.0)/100.0;
		float fDistCap = 25.0f * t.tentspeed_f;

		//  ensure it is capped for max physics object movement
		if ( t.tvd_f > 0.0f )
		{
			t.tvx_f=t.tvx_f/t.tvd_f;
			t.tvz_f=t.tvz_f/t.tvd_f;
		}
		if ( t.tvd_f > fDistCap ) t.tvd_f = fDistCap;
		t.tvx_f=t.tvx_f*t.tvd_f;
		t.tvz_f=t.tvz_f*t.tvd_f;
		t.tvx_f=t.tvx_f*15.0;
		t.tvz_f=t.tvz_f*15.0;

		// entity speed modifier
		t.tentinvspeed_f=100.0/(t.entityelement[t.te].eleprof.speed+0.0);

		// special method for characters to climb 'stairs/ramps'
		if ( t.entityelement[t.te].climbgravity > 0.0f )
		{
			// find ground Y of XZ position, then apply physics velocity to get object to that location
			bool bClimbNeeded = true;
			float fStepUp = 20.0f;
			float fStepDown = 5.0f;
			// advance 'step scan' a little so can anticipate a staircase (stuff to climb)
			float fAdvanceX = NewXValue ( 0, t.entityelement[t.te].ry, 12.0f );
			float fAdvanceZ = NewZValue ( 0, t.entityelement[t.te].ry, 12.0f );
			int iCollisionMode = (1<<(0)) | (1<<(1)); //COL_TERRAIN | COL_OBJECT | COL_OBJECT_DYNAMIC;
			if ( ODERayTerrainEx ( t.entityelement[t.te].x + fAdvanceX, t.entityelement[t.te].y + fStepUp, t.entityelement[t.te].z + fAdvanceZ, t.entityelement[t.te].x + fAdvanceX, t.entityelement[t.te].y - fStepDown, t.entityelement[t.te].z + fAdvanceZ, iCollisionMode, false ) == 1 )
			{
				t.entityelement[t.te].y = ODEGetRayCollisionY();
			}
			else
			{
				// detected no floor in step down test, revert to gravity eventually!
				bClimbNeeded = false;
			}
			float fRiseY = t.entityelement[t.te].y - ObjectPositionY(t.tobj);

			// rise AI to climb stairs/ramp
			if ( fRiseY > 1.5f ) 
			{
				// need substantial upward rise to keep climbing to avoid setting stuck on impassable obstructions!
				t.entityelement[t.te].climbgravity = 1.0f;
			}
			else
			{
				// if not climbing, and up against resistence, stop climb mode (eventually)
				bClimbNeeded = false;
			}
			if ( fRiseY > 0.001f )
			{
				// climbing phyics
				fRiseY=fRiseY*25.0f;
				ODESetLinearVelocity ( t.tobj, t.tvx_f, fRiseY, t.tvz_f );
			}
			if ( bClimbNeeded == false )
			{
				// allows for subtle flat spots during climb, but deactivates if no upward movement after a while
				//t.entityelement[t.te].climbgravity -= 0.05f; //100417 - many enemies and this takes AGES to get to zero, so AI keeps reseting with no physics movement (just slow grav countdown and lots of XZ math movement!)
				t.entityelement[t.te].climbgravity -= g.timeelapsed_f * 2.0f;
				if ( t.entityelement[t.te].climbgravity < 0.0f ) 
					t.entityelement[t.te].climbgravity = 0.0f;
			}
		}
		if ( t.entityelement[t.te].climbgravity == 0.0f )
		{
			// accelerate physics to reach entity X Y Z quickly
			int entid = t.entityelement[t.te].bankindex;
			if (t.tvd_f > 24.0*t.tentspeed_f) // use this to align AI bot position with physics object
			{
				// basically stops a physics object getting ahead of AI entity position
				
				// no velocity while we adjst dest to current stood location (possible vibrate fix)
				ODESetLinearVelocity ( t.tobj,0,0,0 );

				// ensure no new velocities applied while correct physics/entity alignment
				t.tvx_f = 0; t.tvz_f = 0;

				// ensure entity does not get too far from physics object
				t.entityelement[t.te].x=ObjectPositionX(t.tobj);
				t.entityelement[t.te].z=ObjectPositionZ(t.tobj);

				// if AI bot, must reset its position to last valid pos (where phyiscs object is)
				if ( g.charanimindex > 0 )
				{
					// otherwise it separates from its object terribly FAR!
					AISetEntityPosition ( t.tobj,t.entityelement[t.te].x,t.entityelement[t.te].y,t.entityelement[t.te].z );
				}

				// if character finds themselves stopped by something, they can try a climb state
				if ( t.entityprofile[entid].ischaracter == 1 )
				{
					t.entityelement[t.te].climbgravity = 1.0f;
				}

				// in all cases, register dynamic obstruction with LUA var
				if ( t.entityelement[t.te].lua.dynamicavoidance == 0 )
				{
					t.entityelement[t.te].lua.dynamicavoidancestuckclock += 0.5f;
					if ( t.entityelement[t.te].lua.dynamicavoidancestuckclock > 1.0f )
					{
						t.entityelement[t.te].lua.dynamicavoidance = 1;
					}
				}
				t.entityelement[t.te].lua.flagschanged=1;
			}
			else
			{
				// apply normal velocity for entity movement (but gravity force for characters)
				if ( t.entityprofile[entid].ischaracter == 1 )
				{
					// 100317 - special hover mode tracks surface below entity using raycast on physics
					float fHoverFactor = t.entityprofile[entid].hoverfactor;
					if ( fHoverFactor > 0.0f )
					{
						if ( t.entityelement[t.te].hoverfactoroverride > 0.0f )
						{
							// allows hover factor value to be changed in LUA script
							fHoverFactor = t.entityelement[t.te].hoverfactoroverride;
						}
						int iCollisionMode = (1<<(0)) | (1<<(1)); //COL_TERRAIN | COL_OBJECT | COL_OBJECT_DYNAMIC;
						if ( ODERayTerrainEx ( t.entityelement[t.te].x, t.entityelement[t.te].y + 75.0f, t.entityelement[t.te].z, t.entityelement[t.te].x, t.entityelement[t.te].y - 75.0f, t.entityelement[t.te].z, iCollisionMode, false ) == 1 )
						{
							t.entityelement[t.te].y = ODEGetRayCollisionY();
							float tvy = (t.entityelement[t.te].y + fHoverFactor) - ObjectPositionY(t.tobj);
							ODESetLinearVelocity ( t.tobj, t.tvx_f, tvy, t.tvz_f );
						}
						else
						{
							ODESetLinearVelocityXZWithGravity ( t.tobj, t.tvx_f, t.tvz_f, t.tvgravity_f/5.0f );
						}
					}
					else
					{
						ODESetLinearVelocityXZWithGravity ( t.tobj, t.tvx_f, t.tvz_f, t.tvgravity_f );
					}
				}
				else
				{
					#ifdef VRTECH
					if ( t.entityelement[t.te].nogravity == 1 )
					{
						// special case of non character entity with gravity off (pickupable objects)
						float fNoGravY = t.entityelement[t.te].y - ObjectPositionY(t.tobj);
						if ( fabs(fNoGravY) > 0.0f )
						{
							if ( fNoGravY > fDistCap ) fNoGravY = fDistCap;
							if ( fNoGravY < -fDistCap ) fNoGravY = -fDistCap;
							fNoGravY *= 30.0f; // keep it in eye view when look up and down 15.0f;
						}
						ODESetLinearVelocity ( t.tobj, t.tvx_f*2, fNoGravY*2, t.tvz_f*2 );
					}
					else
					{
						// default
						ODESetLinearVelocity ( t.tobj,t.tvx_f,t.tvgravity_f*5.0,t.tvz_f );
					}
					#else
					ODESetLinearVelocity ( t.tobj,t.tvx_f,t.tvgravity_f*5.0,t.tvz_f );
					#endif
				}
			}
		}
		#endif
	}
	else
	{
		if (t.entityelement[t.te].nogravity == 0 && t.entityelement[t.te].collected == 0)
		{
			// non physics objects stick with the floor
			t.tterrainfloorposy_f = BT_GetGroundHeight (t.terrain.TerrainID, t.entityelement[t.te].x, t.entityelement[t.te].z);
			t.entityelement[t.te].y = t.tterrainfloorposy_f;
		}
		else
		{
			// no gravity allows entities to be in the sky (birds and blimps)
		}
		PositionObject ( t.tobj, t.entityelement[t.te].x, t.entityelement[t.te].y, t.entityelement[t.te].z );
	}
}

void entity_determinedamagemultiplier ( void )
{
	t.tdamagemultiplier_f=1.0f;
	/* 190316 - removed head damage hard coding, now in scripts
	//  Check if limb is in any headlimb range (takes ttentid,bulletraylimbhit,tobj)
	if (  t.bulletraylimbhit>0 ) 
	{
		if (  t.ttentid>0 ) 
		{
			if (  t.bulletraylimbhit >= t.entityprofile[t.ttentid].headframestart && t.bulletraylimbhit <= t.entityprofile[t.ttentid].headframefinish ) 
			{
				if (  t.tobj>0 ) 
				{
					if (  ObjectExist(t.tobj) == 1 ) 
					{
						if (  t.entityelement[t.ttte].health>0 ) 
						{
							//  work out if object has limbs
							PerformCheckListForLimbs (  t.tobj );
							t.tlimbmax=ChecklistQuantity()-1;
							//  small visual indicator the headshot worked!
							//  but not if a character creator entity
							if (  t.entityprofile[t.ttentid].ischaractercreator  ==  0 ) 
							{
								for ( t.tlmb = t.entityprofile[t.ttentid].headframestart ; t.tlmb<=  t.entityprofile[t.ttentid].headframefinish; t.tlmb++ )
								{
									if (  t.tobj <= t.tlimbmax ) 
									{
										RotateLimb (  t.tobj,t.tlmb,Rnd(80)-40,Rnd(80)-40,0 );
									}
								}
							}
						}
					}
				}
				t.tdamagemultiplier_f=4.0f;
			}
		}
	}
	return;
	*/
}

void entity_determinegunforce ( void )
{
	//  bulletraytype (1-pierce, 2-shotgun pellets)
	t.bulletraytype=g.firemodes[t.gunid][g.firemode].settings.damagetype;
	#ifdef WICKEDENGINE
	t.tforce_f = g.firemodes[t.gunid][g.firemode].settings.force*2.0 * t.playercontrol.fWeaponDamageMultiplier;
	if (  t.gun[t.gunid].settings.ismelee == 2 || g.firemodes[t.gunid][g.firemode].settings.usemeleedamageonly > 0)
	{
		//  100415 - added separate force for melee attacks
		t.tforce_f=g.firemodes[t.gunid][g.firemode].settings.meleeforce*2.0 * t.playercontrol.fMeleeDamageMultiplier;
	}
	#else
	t.tforce_f = g.firemodes[t.gunid][g.firemode].settings.force*2.0;
	if (t.gun[t.gunid].settings.ismelee == 2)
	{
		//  100415 - added separate force for melee attacks
		t.tforce_f = g.firemodes[t.gunid][g.firemode].settings.meleeforce*2.0;
	}
	#endif
	else
	{
		//  regular bullet type force modifiers
		if (  t.bulletraytype == 2  )  t.tforce_f = t.tforce_f*2;
		if (  t.bulletraytype == 1 ) 
		{
			if (  t.tforce_f>40  )  t.tforce_f = 40;
		}
	}
	t.tforce_f=t.tforce_f*20.0;
return;

}

void entity_find_charanimindex_fromttte ( void )
{
	t.tcharanimindex=0;
	if ( t.ttte>0 ) 
	{
		for ( t.ttcharanimindex = 1 ; t.ttcharanimindex <= g.charanimindexmax; t.ttcharanimindex++ )
		{
			if ( t.charanimstates[t.ttcharanimindex].e == t.ttte ) 
			{
				t.tcharanimindex=t.ttcharanimindex; 
				break;
			}
		}
	}
}

void entity_adddestroyevent(int e)
{
	bool bUniqueForList = true;
	for (int n = 0; n < g_iDestroyedEntitiesList.size(); n++)
	{
		if (g_iDestroyedEntitiesList[n] == e)
		{
			bUniqueForList = false;
			break;
		}
	}
	if (bUniqueForList == true)
	{
		g_iDestroyedEntitiesList.push_back(e);
	}
}

void entity_applydamage ( void )
{
	if ( t.entityelement[t.ttte].obj <= 0 ) return;
	if ( ObjectExist ( t.entityelement[t.ttte].obj ) == 0 ) return;

	int iHealthBefore = t.entityelement[t.ttte].health;
	#ifdef WICKEDENGINE
	// wicked allows wicked amounts of damage ;)
	#else
	#ifdef VRTECH
	// 090419 - special VR mode also disables concepts of being damaged
	if ( g.vrqcontrolmode != 0 ) return;//g.gvrmode == 3 ) return;
	#endif
	#endif

	//  if entity being damaged is protagonist
	if (  t.tskiplayerautoreject == 0 ) 
	{
		if (  t.playercontrol.thirdperson.enabled == 1 ) 
		{
			if (  t.ttte == t.playercontrol.thirdperson.charactere ) 
			{
				//  this is the player that was hit, is player damage instead
				t.testore=t.te ; t.te=t.ttte;
				physics_player_takedamage ( );
				t.te=t.testore;
				return;
			}
		}
	}

	//  early exit
	if (  t.entityelement[t.ttte].active == 0  )  return;

	//  no damage if immobile and not a character (collectables)
	if (  t.tallowanykindofdamage == 0 ) 
	{
		// 010216 - if 'isimmobile' was set in FPE, it cannot be reduced to zero health
		//if (  t.entityprofile[t.ttentid].isimmobile == 1 && t.entityprofile[t.ttentid].ischaracter == 0  )  return;
		t.ttentid=t.entityelement[t.ttte].bankindex;
		if (  t.entityprofile[t.ttentid].isimmobile == 1 && t.entityprofile[t.ttentid].ischaracter == 0  )
		{
			// code at the end will cap any reduction to 1, this keeps COLLECTABLES alive but allows scripts to detect damage on ISIMMOBILE entities
		}
	}

	//  magnify damage if player has superpowers
	if (  t.tdamagesource == 2 ) 
	{
		if (  t.player[t.plrid].powers.level != 100 ) 
		{
			t.tpowerratio_f=(t.player[t.plrid].powers.level+0.0)/100.0;
			t.tdamage=(t.tdamage+0.0)*t.tpowerratio_f;
			t.tdamageforce=(t.tdamageforce+0.0)*t.tpowerratio_f;
		}
	}

	//  if damaging ai in mp, you will take the aggro of the enemy
	if (  t.game.runasmultiplayer == 1 && g.mp.coop  ==  1 && g.mp.ignoreDamageToEntity  ==  0 ) 
	{
		t.ttentid=t.entityelement[t.ttte].bankindex;
		if (  t.ttentid>0 ) 
		{
			if (  (t.entityprofile[t.ttentid].ischaracter  ==  1 || t.entityelement[t.ttte].mp_isLuaChar  ==  1) && t.entityelement[t.ttte].mp_coopControlledByPlayer  !=  g.mp.me && t.entityprofile[t.entid].ismultiplayercharacter  ==  0 ) 
			{
				if (  t.entityelement[t.ttte].mp_coopControlledByPlayer  ==  -1 ) 
				{
					t.tsteamplayeralive = 0;
				}
				else
				{
					t.tsteamplayeralive = SteamGetPlayerAlive(t.entityelement[t.ttte].mp_coopControlledByPlayer);
				}
				if (  Timer() - t.entityelement[t.ttte].mp_coopLastTimeSwitchedTarget > 5000 || t.tsteamplayeralive  ==  0 ) 
				{
					t.entityelement[t.ttte].mp_coopControlledByPlayer = g.mp.me;
					t.entityelement[t.ttte].mp_updateOn = 1;
					mp_sendlua (  MP_LUA_TakenAggro,t.ttte,g.mp.me );
					t.entityelement[t.ttte].mp_coopLastTimeSwitchedTarget = Timer()+5000;
				}
			}
		}
	}

	//  takes ttte, tdamage, tdamageforce, tdamagesource
	//  takes brayx1# to brayz2#
	//  tdamagesource = 0;self 1;bullet 2;explosion
	t.tobj=t.entityelement[t.ttte].obj;
	//  work out force
	if (  t.tdamageforce>0 ) 
	{
		//  force given
		t.tforce_f=t.tdamageforce;
		//  stop force going overboard in multiplayer or if character
		t.tokay=0;
		if (  t.entityprofile[t.entityelement[t.ttte].bankindex].ismultiplayercharacter == 1  )  t.tokay = 1;
		if (  t.game.runasmultiplayer == 1 && g.mp.coop == 1 && t.entityprofile[t.entityelement[t.ttte].bankindex].ismultiplayercharacter == 1  )  t.tokay = 1;
		if (  t.tokay == 1 ) 
		{
			if (  t.tforce_f>150.0  )  t.tforce_f = 150.0;
		}
	}
	else
	{
		//  no force (possibly provided by ODERayTerrain Ex (  HIT elsewhere such as bulletray subroutine) )
		t.tforce_f=0.0f;
	}

	//  find character or non-character
	int istorecharindex = t.tcharanimindex;
	entity_find_charanimindex_fromttte ( );
	t.ttentid=t.entityelement[t.ttte].bankindex;
	if ( t.tcharanimindex == 0 && t.tforce_f>0 && t.entityprofile[t.ttentid].ragdoll == 0 ) 
	{
		//  NON-CHARACTER ENTITY (used for explosion forces)
		t.tdx_f=(t.brayx2_f-t.brayx1_f)*(t.tforce_f/90.0);
		t.tdy_f=(t.brayy2_f-t.brayy1_f)*(t.tforce_f/90.0);
		t.tdz_f=(t.brayz2_f-t.brayz1_f)*(t.tforce_f/90.0);
		ODEAddBodyForce (  t.tobj,t.tdx_f,t.tdy_f,t.tdz_f,0,0,0 );
		ODESetAngularVelocity (  t.tobj,Rnd(600)-300,Rnd(200)-100,Rnd(600)-300 );
	}
	#ifdef WICKEDENGINE
	if (t.tcharanimindex > 0)
	{
		// any damage implied on a character instantly pushes it out of dormancy
		t.charanimstates[t.tcharanimindex].dormant = 0;
	}
	#endif
	t.tcharanimindex = istorecharindex;

	//  work out damage and see if entity gets destroyed
	entity_determinedamagemultiplier ( );
	t.tdamage=t.tdamage*t.tdamagemultiplier_f;

	//  apply damage locally if not multiplayer
	if (  t.game.runasmultiplayer == 0 ) 
	{
		t.entityelement[t.ttte].health=t.entityelement[t.ttte].health-t.tdamage;
	}
	else
	{
		//  Multiplayer, first checks if it is a player, if it is, we send the damage to them to apply
		//  If not, we appply it and inform everyone else
		if (  t.entityprofile[t.entityelement[t.ttte].bankindex].ismultiplayercharacter == 1 ) 
		{
			for ( int tpindex = 0 ; tpindex<=  MP_MAX_NUMBER_OF_PLAYERS-1; tpindex++ )
			{
				if (  t.mp_playerEntityID[tpindex]  ==  t.ttte && tpindex  !=  g.mp.me && SteamGetPlayerAlive(tpindex)  ==  1 ) 
				{
					t.tSteamForce_f = t.tforce_f;
					if (  t.tSteamForce_f  ==  150  )  t.tSteamForce_f  =  300;
					if (  g.mp.ignoreDamageToEntity  ==  0 ) 
					{
						t.tsteamlastdamagesentcounter = t.tsteamlastdamagesentcounter + 1;
						//  13032015 0XX - Team Multiplayer
						if (  g.mp.team  ==  0 || g.mp.friendlyfireoff  ==  0 || t.mp_team[tpindex]  !=  t.mp_team[g.mp.me] ) 
						{
							// Ignore setentityhealth lua message if it is a player
							//if ( t.tallowanykindofdamage == 0 )
							//{
								SteamApplyPlayerDamage (  tpindex,t.tdamage, t.brayx2_f-t.brayx1_f, t.brayy2_f-t.brayy1_f, t.brayz2_f-t.brayz1_f, t.tSteamForce_f, t.bulletraylimbhit );
							//}
						}
					}
				}
			}
		}
		else
		{
			//  it is not a player, so we can apply damage to it
			if (  t.entityelement[t.ttte].health > 0 ) 
			{
				t.entityelement[t.ttte].health=t.entityelement[t.ttte].health-t.tdamage;
				if (  g.mp.ignoreDamageToEntity  ==  0 ) 
				{
					if (  t.entityelement[t.ttte].health  <= 0 ) 
					{
						//  for coop, we count ai kills and not player kills
						if (  g.mp.coop  ==  1 ) 
						{
							t.tttentid=t.entityelement[t.ttte].bankindex;
							if (  t.tttentid > 0 ) 
							{
								if (  t.entityprofile[t.tttentid].ischaracter  ==  1 || t.entityelement[t.ttte].mp_isLuaChar  ==  1 ) 
								{
									mp_IKilledAnAI ( );
								}
							}
						}
						++t.tempsteamdestroycount;
						mp_destroyentity ( );
					}
				}
			}
		}
	}
	t.entityelement[t.ttte].lua.flagschanged=1;

	// 010216 - special case for ISIMMOBILE FPE that are not characters, they cannot get to zero health (COLLECTABLES)
	if (  t.tallowanykindofdamage == 0 ) 
	{
		t.ttentid=t.entityelement[t.ttte].bankindex;
		if (  t.entityprofile[t.ttentid].isimmobile == 1 && t.entityprofile[t.ttentid].ischaracter == 0  )
		{
			if ( t.entityelement[t.ttte].health <= 0 )
				t.entityelement[t.ttte].health = 1;
		}
	}

	#ifdef WICKEDENGINE
	// preexit not used in MAX
	#else
	// when health drops to zero, and have a preexit LUA function for this entity, keep 
	// entity alive long enough to run the logic in the preexit function
	if ( t.entityelement[t.ttte].health <= 0 ) 
	{
		// this is only set to zero if the init call cleared it ready for preexit usage, otherwise ignore
		if ( t.entityelement[t.ttte].eleprof.aipreexit == 0 )
		{
			t.entityelement[t.ttte].health = 1;
			t.entityelement[t.ttte].eleprof.aipreexit = 1;
		}
	}
	if ( t.entityelement[t.ttte].eleprof.aipreexit == 1 ) t.entityelement[t.ttte].health = 1;
	if ( t.entityelement[t.ttte].eleprof.aipreexit == 2 ) t.entityelement[t.ttte].health = 0;
	#endif

	#ifdef WICKEDENGINE
	// used when switch off auto matic destroy so can play anim before ragdoll, etc (or for damage invulnerability)
	if (t.entityelement[t.ttte].briefimmunity != 0)
		if (t.entityelement[t.ttte].health <= 0)
			t.entityelement[t.ttte].health = 1;
	#endif

	// a system to allow the ragdoll force to be retained for MAX
	//bool bAllowRagdollForceToBeRecorded = false;

	// when health drops to zero
	if (t.entityelement[t.ttte].health <= 0)
	{
		// counting any entity damage at or below zero a destroy event (for counting XP)
		//PE: Only add this one time or XP just keep increasing. Like "rats" would triggere this constantly even if dead.
		if(iHealthBefore > 0)
			entity_adddestroyevent(t.ttte);

		//  if explodble, have a delayed reaction
		if (t.entityelement[t.ttte].eleprof.explodable != 0)
		{
			if (t.entityelement[t.ttte].explodefusetime == 0)
			{
				if (t.tdamagesource == 2)
				{
					//  explosion is time delayed
					#ifdef WICKEDENGINE
					t.entityelement[t.ttte].explodefusetime = Timer() + 350 + ( rand() % 250 );
					#else
					t.entityelement[t.ttte].explodefusetime = Timer() + 250;
					#endif
				}
				else
				{
					//  explosion is instant
					t.entityelement[t.ttte].explodefusetime = Timer();
				}
			}
		}
		else
		{
			//  reset health to zero
			t.entityelement[t.ttte].health = 0;
		}

		// 010616 - May be a third person character, no ragdoll means find death animation and play
		int iThirdPersonCharacter = 0;
		if (t.playercontrol.thirdperson.enabled == 1)
			if (t.playercontrol.thirdperson.charactere == t.ttte)
				iThirdPersonCharacter = t.playercontrol.thirdperson.characterindex;

		//  if character
#ifdef WICKEDENGINE
		int iStoreCharAnimIndex = t.tcharanimindex;
		entity_find_charanimindex_fromttte();
		int iCharacterIndexToUse = t.tcharanimindex;
		t.tcharanimindex = iStoreCharAnimIndex;
#else
		int iCharacterIndexToUse = t.tcharanimindex;
#endif
		t.tapplyragdollforce = 0;
		if (iThirdPersonCharacter > 0) iCharacterIndexToUse = iThirdPersonCharacter;
		if (t.entityelement[t.ttte].eleprof.explodable != 0) iCharacterIndexToUse = 0;
		if (iCharacterIndexToUse > 0)
		{
			//  CHARACTER
			if (iThirdPersonCharacter == 0)
			{
				int iStoreCharIndex = t.tcharanimindex;
				t.tcharanimindex = iCharacterIndexToUse;
				darkai_killai ();
				t.tcharanimindex = iStoreCharIndex;
			}

			//  if dead, trigger impact death
			t.tdx_f = ObjectPositionX(t.tobj) - t.twhox_f;
			t.tdz_f = ObjectPositionZ(t.tobj) - t.twhoz_f;
			t.tda_f = atan2deg(t.tdx_f, t.tdz_f);
			t.relativeangle_f = WrapValue(ObjectAngleY(t.tobj) - t.tda_f);
			t.impacting = 5;
			#ifdef WICKEDENGINE
			extern bool g_bForceRagdoll;
			if (g_bForceRagdoll == true)
			{
				// trigger a forced ragdoll event
				t.impacting = 6;
			}
			else
			{
				if (t.relativeangle_f >= 315 || t.relativeangle_f < 45)  t.impacting = 1;
				if (t.relativeangle_f >= 45 && t.relativeangle_f < 135)  t.impacting = 3;
				if (t.relativeangle_f >= 135 && t.relativeangle_f < 225)  t.impacting = 2;
				if (t.relativeangle_f >= 225 && t.relativeangle_f < 315)  t.impacting = 4;
			}
			#else
			if (t.relativeangle_f >= 315 - 22 || t.relativeangle_f < 45 + 22)
			{
				t.impacting = 1;
			}
			else
			{
				if (t.relativeangle_f >= 45 + 22 && t.relativeangle_f < 135 - 22)  t.impacting = 3;
				if (t.relativeangle_f >= 135 - 22 && t.relativeangle_f < 225 + 22)  t.impacting = 2;
				if (t.relativeangle_f >= 225 + 22 && t.relativeangle_f < 315 - 22)  t.impacting = 4;
			}
			#endif

			//  cannot use state engine - use instant animation for this
			if (t.charanimstates[iCharacterIndexToUse].playcsi != g.csi_limbo)
			{
				if (t.charanimstates[iCharacterIndexToUse].playcsi > 0 && t.charanimstates[iCharacterIndexToUse].playcsi >= t.csi_crouchidle[t.charanimstates[iCharacterIndexToUse].weapstyle] && t.charanimstates[iCharacterIndexToUse].playcsi <= t.csi_crouchgetup[t.charanimstates[iCharacterIndexToUse].weapstyle])
				{
					//  die crouched
					if (t.impacting == 1)  t.charanimstates[iCharacterIndexToUse].playcsi = t.csi_crouchimpactfore[t.charanimstates[iCharacterIndexToUse].weapstyle];
					if (t.impacting == 2)  t.charanimstates[iCharacterIndexToUse].playcsi = t.csi_crouchimpactback[t.charanimstates[iCharacterIndexToUse].weapstyle];
					if (t.impacting == 3)  t.charanimstates[iCharacterIndexToUse].playcsi = t.csi_crouchimpactleft[t.charanimstates[iCharacterIndexToUse].weapstyle];
					if (t.impacting == 4)  t.charanimstates[iCharacterIndexToUse].playcsi = t.csi_crouchimpactright[t.charanimstates[iCharacterIndexToUse].weapstyle];
				}
				else
				{
					//  die stood
					if (t.impacting == 1)  t.charanimstates[iCharacterIndexToUse].playcsi = g.csi_unarmedimpactfore;
					if (t.impacting == 2)  t.charanimstates[iCharacterIndexToUse].playcsi = g.csi_unarmedimpactback;
					if (t.impacting == 3)  t.charanimstates[iCharacterIndexToUse].playcsi = g.csi_unarmedimpactleft;
					if (t.impacting == 4)  t.charanimstates[iCharacterIndexToUse].playcsi = g.csi_unarmedimpactright;
				}
				if (t.impacting == 5)  t.charanimstates[iCharacterIndexToUse].playcsi = g.csi_unarmeddeath;
				t.smoothanim[t.tobj].transition = 0;
			}

			// only for regular characters
			if (iThirdPersonCharacter == 0)
			{
				// wipe out health
				t.entityelement[t.charanimstates[iCharacterIndexToUse].e].health = 0;

				// reset spin e twist and neck twist
				t.charanimstates[iCharacterIndexToUse].spineAiming = 0.0f;
				t.charanimstates[iCharacterIndexToUse].neckAiming = 0.0f;
				int iCharObj = t.charanimstates[iCharacterIndexToUse].obj;
				if (iCharObj > 0)
				{
					int iEntID = t.entityelement[t.charanimstates[iCharacterIndexToUse].e].bankindex;
					int iFrameIndex = t.entityprofile[iEntID].spine2;
					if (iFrameIndex > 0)
					{
						sObject* pCharObject = GetObjectData(iCharObj);
						sFrame* pFrameOfLimb = pCharObject->ppFrameList[iFrameIndex];
						if (pFrameOfLimb)
						{
							WickedCall_RotateLimb(pCharObject, pFrameOfLimb, 0, 0, 0);
						}
					}
				}

				//  setting main to 0 so the main lua won't be called for this object
				t.entityelement[t.ttte].eleprof.aimain = 0;

				//  Prepare character for eventual fade out
				if (t.entityprofile[t.ttentid].ragdoll == 1)
				{
					t.charanimstates[iCharacterIndexToUse].timetofadeout = Timer() + AICORPSETIME;
					t.charanimstates[iCharacterIndexToUse].fadeoutvalue_f = 1.0;
				}
				else
				{
					// if not ragdoll, used own die anim, remove right away
					// 071018 - caused legacy nonragdoll characters not to play their exit animations
					// t.entityelement[t.ttte].destroyme = 1;
				}
			}

			//  Convert to clone so can operate independent of parent object
			t.tte = t.ttte; entity_converttoclone ();

			//  Ragdoll for characters is now optional
			if (t.entityprofile[t.ttentid].ragdoll == 1)
			{
				//  create ragdoll and stop any further manipulation of the object
				#ifdef WICKEDENGINE
				if (t.entityelement[t.ttte].ragdollplusactivate == 0)
				{
					t.entityelement[t.ttte].ragdollplusactivate = t.impacting;
					// could use: t.bulletraytype; // 1-pierce, 2-shotgun shell
					if (t.tdamageforce > 1000)
						t.entityelement[t.ttte].ragdollplusweapontypeused = 2;
					else
						t.entityelement[t.ttte].ragdollplusweapontypeused = 1;
				}
				#else
				ragdoll_setcollisionmask (t.entityelement[t.ttte].eleprof.colondeath);
				t.tphye = t.ttte; t.tphyobj = t.entityelement[t.ttte].obj; ragdoll_create ();
				t.tapplyragdollforce = 1;
				#endif
			}

			#ifdef WICKEDENGINE
			// this is now done later when death anim finished
			#else
			// and make attachment object a physics object
			t.tattobj = t.entityelement[t.ttte].attachmentobj;
			if (t.tattobj > 0)  ODECreateDynamicBox (t.tattobj, -1, 1);
			#endif

			// only for regular characters
			if (iThirdPersonCharacter == 0)
			{
				//  Ensure character control ceases at this (instantly for ragdoll / anim death delays this assignment)
				if (t.entityprofile[t.ttentid].ragdoll == 1)
				{
					t.charanimstates[iCharacterIndexToUse].e = 0;
				}
			}
		}
		else
		{
			//  NON-CHARACTER, but can still have ragdoll flagged (like Zombies)
			if (t.entityprofile[t.ttentid].ragdoll == 1)
			{
				// can only ragdoll clones not instances
				t.tte = t.ttte; entity_converttoclone ();

				// create ragdoll and stop any further manipulation of the object
				ragdoll_setcollisionmask (t.entityelement[t.ttte].eleprof.colondeath);
				t.tphye = t.ttte; t.tphyobj = t.entityelement[t.ttte].obj; ragdoll_create ();
				t.tapplyragdollforce = 1;

				// and make attachment object a physics object
				t.tattobj = t.entityelement[t.ttte].attachmentobj;
				if (t.tattobj > 0)
				{
					// and ensure it does not bury into surface by raising it
					if (ODEFind(t.tattobj) == 0)
					{
						ODECreateDynamicBox (t.tattobj, -1, 1);
					}
				}

				// and ensure entity is destroyed (active to zero)
				//t.entityelement[t.ttte].destroyme = 1; // tried this but hide the object and killed the ragdoll
				t.entityelement[t.ttte].active = 0;
				t.entityelement[t.ttte].health = 0;
				t.entityelement[t.ttte].lua.flagschanged = 2;
			}
		}

		//  multiplayer undocumented stuff
		if (t.game.runasmultiplayer == 1)
		{
			if (g.mp.ignoreDamageToEntity == 1)
			{
				if (t.tapplyragdollforce == 1)
				{
					t.tapplyragdollforce = 0;
					t.entityelement[t.ttte].ragdollified = 1;
				}
			}
		}

		// and apply bullet directional force (tforce#=from gun settings)
		//bAllowRagdollForceToBeRecorded = true;
	}

	bool bAllowRagdollForceToBeRecorded = false;
	#ifdef WICKEDENGINE
	// as bullet hits can be interceded with an animation, and then a tru destroy to create the ragdoll, 
	// for MAX< record the ragdoll force from the original hit so we can apply when we finally become a ragdoll :)
	if (t.tforce_f > 0.0f && t.bulletraylimbhit != -1)
	{
		//if (t.tapplyragdollforce == 1)
		if (t.entityelement[t.ttte].ragdollified == 0 )
		{
			bAllowRagdollForceToBeRecorded = true;
		}
	}
	else
	{
		// but retains last good ragdoll force values if called again but with no force (see soldier attack behavior in MAX)
	}
	#endif

	// apply bullet directional force if all the elements in the equation are good
	if (bAllowRagdollForceToBeRecorded == true )
	{
		//if ( t.tapplyragdollforce == 1 ) 
		{
			t.entityelement[t.ttte].ragdollified=1;
			t.entityelement[t.ttte].ragdollifiedforcex_f=(t.brayx2_f-t.brayx1_f)*0.8;
			t.entityelement[t.ttte].ragdollifiedforcey_f=(t.brayy2_f-t.brayy1_f)*1.2;
			t.entityelement[t.ttte].ragdollifiedforcez_f=(t.brayz2_f-t.brayz1_f)*0.8;
			if ( t.game.runasmultiplayer == 0 ) 
			{
				t.entityelement[t.ttte].ragdollifiedforcevalue_f=t.tforce_f*8000.0;
			}
			else
			{
				t.tsteamcoopforcemulti_f = 8000.0;
				if ( g.mp.coop == 1 ) 
				{
					if ( t.entityprofile[t.entityelement[t.ttte].bankindex].ismultiplayercharacter == 1 ) 
					{
						if ( t.tforce_f > 300.0  )  t.tforce_f  =  300.0;
						t.tsteamcoopforcemulti_f = 2000.0;
					}
				}
				t.entityelement[t.ttte].ragdollifiedforcevalue_f=t.tforce_f*t.tsteamcoopforcemulti_f;
			}
			t.entityelement[t.ttte].ragdollifiedforcelimb=t.bulletraylimbhit;
			t.bulletraylimbhit=-1;
		}
	}
}

void entity_gettruecamera ( void )
{
	//  True camera position
	if (  t.playercontrol.thirdperson.enabled == 1 ) 
	{
		t.tcamerapositionx_f=t.playercontrol.thirdperson.storecamposx;
		t.tcamerapositiony_f=t.playercontrol.thirdperson.storecamposy;
		t.tcamerapositionz_f=t.playercontrol.thirdperson.storecamposz;
	}
	else
	{
		t.tcamerapositionx_f=CameraPositionX(t.terrain.gameplaycamera);
		t.tcamerapositiony_f=CameraPositionY(t.terrain.gameplaycamera);
		t.tcamerapositionz_f=CameraPositionZ(t.terrain.gameplaycamera);
	}
}

void entity_gettrueplayerpos(void)
{
	//  True camera position
	if (t.playercontrol.thirdperson.enabled == 1)
	{
		t.tcamerapositionx_f = ObjectPositionX(t.aisystem.objectstartindex);
		t.tcamerapositiony_f = ObjectPositionY(t.aisystem.objectstartindex);
		t.tcamerapositionz_f = ObjectPositionZ(t.aisystem.objectstartindex);
	}
	else
	{
		t.tcamerapositionx_f = CameraPositionX(t.terrain.gameplaycamera);
		t.tcamerapositiony_f = CameraPositionY(t.terrain.gameplaycamera);
		t.tcamerapositionz_f = CameraPositionZ(t.terrain.gameplaycamera);
	}
}

void entity_hasbulletrayhit(void)
{
	// bulletray is x1#,y1#,z1#,x2#,y2#,z2#,bulletrayhit,gunrange#
	t.brayx1_f = t.x1_f; t.brayy1_f = t.y1_f; t.brayz1_f = t.z1_f;
	t.brayx2_f = t.x2_f; t.brayy2_f = t.y2_f; t.brayz2_f = t.z2_f;
	t.bulletrayhit = 0; t.bulletraylimbhit = -1; t.tttriggerdecalimpact = 0;
	t.tfoundentityindexhit = -1;
	t.tmaterialvalue = -1;

	// first cast a ray at any terrain
	#ifdef WICKEDENGINE
	GGVECTOR3 vecRayHitNormal = GGVECTOR3(0, 0, 0);
	#endif
	if (ODERayTerrain(t.brayx1_f, t.brayy1_f, t.brayz1_f, t.brayx2_f, t.brayy2_f, t.brayz2_f, false) == 1)
	{
		//  and shorten the ray if we hit terra firma!
		t.brayx2_f = ODEGetRayCollisionX();
		t.brayy2_f = ODEGetRayCollisionY();
		t.brayz2_f = ODEGetRayCollisionZ();
		#ifdef WICKEDENGINE
		// get extra collision data, need to know surface normal
		vecRayHitNormal = GGVECTOR3(ODEGetRayNormalX(), ODEGetRayNormalY(), ODEGetRayNormalZ());
		// LEELEE = need to get TERRAIN MATERIAL ID HERE TOO!!
		t.tttriggerdecalimpact = 10;
		#else
		// trigger dust flume at coords
		t.tttriggerdecalimpact = 10;
		#endif
	}
	
	// Character creator can override the limb hit, to make the cc head report the head limb of the main character
	t.ccLimbHitOverride = false;

	// create a ray and check for object hit (first intersectall command simply fills a secondary range of objects)
	#ifndef WICKEDENGINE
	if(g.lightmappedobjectoffset >= g.lightmappedobjectoffsetfinish)
		t.ttt = IntersectAll(87000, 87000 + g.merged_new_objects - 1, 0, 0, 0, 0, 0, 0, -123);
	else
		t.ttt = IntersectAll(g.lightmappedobjectoffset, g.lightmappedobjectoffsetfinish, 0, 0, 0, 0, 0, 0, -123);
	// check if character creator characters are in the game
	if (t.characterkitcontrol.gameHasCharacterCreatorIn == 1)
	{
		// 010715 - will even detect GLUED OBJECTS thanks to new code in IntersectAll (  )
		t.ttt = IntersectAll(g.charactercreatorrmodelsoffset + ((t.characterkitcontrol.minEntity * 3) - t.characterkitcontrol.offset), g.charactercreatorrmodelsoffset + ((t.characterkitcontrol.maxEntity * 3) - t.characterkitcontrol.offset), 0, 0, 0, 0, 0, 0, -124);
	}
	if (g.firemodes[t.gunid][0].settings.detectcoloff == 1)
	{
		// 111215 - '-125' is ignore collision property where entity was CollisionOff(e)
		t.ttt = IntersectAll(0, 0, 0, 0, 0, 0, 0, 0, -125); 
	}
	#endif
	// if TPP, can ignore entity used as player
	int iIgnoreOneEntityObj = 0;
	if ( t.playercontrol.thirdperson.enabled == 1 )
	{
		// 220217 - cannot shoot self with weapon!
		iIgnoreOneEntityObj = t.entityelement[t.playercontrol.thirdperson.charactere].obj;
	}
	t.thitvalue=IntersectAll(g.entityviewstartobj,g.entityviewendobj,t.brayx1_f,t.brayy1_f,t.brayz1_f,t.brayx2_f,t.brayy2_f,t.brayz2_f,iIgnoreOneEntityObj);
	if ( t.thitvalue>0 ) 
	{
		if (t.thitvalue > 0)
		{
			if (ObjectExist(t.thitvalue) == 1)
			{
				for (t.tte = 1; t.tte <= g.entityelementlist; t.tte++)
				{
					if (t.entityelement[t.tte].obj == t.thitvalue)
					{
						t.tfoundentityindexhit = t.tte; break;
					}
				}
			}
		}
		#ifdef WICKEDENGINE
		// allow for any limb to be detected at this point
		#else
		// check if it was a character creator object hit and find the main body object
		if ( t.characterkitcontrol.gameHasCharacterCreatorIn  ==  1 ) 
		{
			if (  t.thitvalue >= g.charactercreatorrmodelsoffset+((t.characterkitcontrol.minEntity*3)-t.characterkitcontrol.offset) && t.thitvalue  <=  g.charactercreatorrmodelsoffset+((t.characterkitcontrol.maxEntity*3)-t.characterkitcontrol.offset) ) 
			{
				t.toldthitvalue=t.thitvalue;
				t.tTheE = ((t.thitvalue - g.charactercreatorrmodelsoffset) + t.characterkitcontrol.offset) / 3;
				t.thitvalue = t.entityelement[t.tTheE].obj;
				//Character creator can override the limb hit, to make the cc head report the head limb of the main character
				t.ccLimbHitOverride = true;
				t.ccLimbHitOverrideLimb  = getlimbbyname(t.entityelement[t.tTheE].obj, "Bip01_Head");
			}
		}
		#endif
		// record object number we hit
		t.tsteamLastHit=t.thitvalue;
		t.bulletrayhit=t.thitvalue;
		//  first check if object uses 'physics collision' over 'geometry collision'
		t.tcollisionwithphysics=0;
		if (t.tfoundentityindexhit != -1)
		{
			t.tentid = t.entityelement[t.tfoundentityindexhit].bankindex;
		}
		if (t.tfoundentityindexhit != -1)
		{
			if (  t.entityprofile[t.tentid].collisionoverride == 1 || t.entityprofile[t.tentid].collisionmode == 11 )
			{
				if (  t.entityprofile[t.tentid].collisionmode == 11 ) 
				{
					t.tcollisionwithphysics=2;
				}
				else
				{
					if ( ODERayTerrainEx(t.brayx1_f,t.brayy1_f,t.brayz1_f,t.brayx2_f,t.brayy2_f,t.brayz2_f,2,false) == 1 ) 
					{
						t.tcollisionwithphysics=1;
					}
					else
					{
						t.tcollisionwithphysics=2;
					}
				}
			}
		}
		if ( t.tcollisionwithphysics>0 ) 
		{
			if ( t.tcollisionwithphysics == 1 ) 
			{
				// found hit woth physics shapes instead of geometry
				t.brayx2_f=ODEGetRayCollisionX();
				t.brayy2_f=ODEGetRayCollisionY();
				t.brayz2_f=ODEGetRayCollisionZ();
				#ifdef WICKEDENGINE
				vecRayHitNormal = GGVECTOR3(ODEGetRayNormalX(), ODEGetRayNormalY(), ODEGetRayNormalZ());
				#endif
				t.bulletraylimbhit=0;
				if (t.tfoundentityindexhit != -1)
				{
					t.tmaterialvalue = t.entityprofile[t.tentid].materialindex;
				}
			}
			else
			{
				// hit geometry but missed physics shape, no collision (foliage banana tree)
				t.bulletrayhit=-1;
				t.bulletraylimbhit=-1;
			}
		}
		else
		{
			// shorten ray to reflect hit coordinate
			t.brayx2_f=ChecklistFValueA(6);
			t.brayy2_f=ChecklistFValueB(6);
			t.brayz2_f=ChecklistFValueC(6);

			#ifdef WICKEDENGINE
			vecRayHitNormal = GGVECTOR3(ChecklistFValueA(7), ChecklistFValueB(7), ChecklistFValueC(7));
			#endif

			// get limb we hit (for flinch effect when we hit enemy limb)
			t.tlimbhit=ChecklistValueB(1);

			// return material index and use to trigger decal
			#ifdef WICKEDENGINE
			// ChecklistValueA 9 not reliable, get material from entity properties!
			if (t.tfoundentityindexhit != -1)
			{
				t.tmaterialvalue = t.entityprofile[t.tentid].materialindex;
			}
			else
			{
				t.tmaterialvalue = ChecklistValueA(9);
			}
			#else
			t.tmaterialvalue = ChecklistValueA(9);
			#endif

			// check if we hit character creator head and adjust limbhit to the head of the character
			// simpler head shot detection (gun and other things can get in the way)
			#ifdef WICKEDENGINE
			if (t.tfoundentityindexhit != -1)
			{
				if (t.entityprofile[t.tentid].ischaracter == 1)
				{
					float fHitY = ChecklistFValueB(6);
					float fPercentageThatIsNotHead = 0.8f;
					if (fHitY > ObjectPositionY(t.thitvalue) + ObjectSizeY(t.thitvalue, 1) * fPercentageThatIsNotHead)
					{
						// head
						t.tlimbhit = getlimbbyname(t.thitvalue, "Bip01_Head");
					}
					else
					{
						fPercentageThatIsNotHead = 0.35f;
						if (fHitY > ObjectPositionY(t.thitvalue) + ObjectSizeY(t.thitvalue, 1) * fPercentageThatIsNotHead)
						{
							// pelvis
							t.tlimbhit = getlimbbyname(t.thitvalue, "Bip01_Pelvis");
						}
						else
						{
							// foot
							t.tlimbhit = getlimbbyname(t.thitvalue, "Bip01_R_Foot");
						}
					}
					t.tmaterialvalue = t.entityprofile[t.tentid].materialindex;
				}
			}
			#endif

			// reset and assign detectedlimbhit flag for script
			if ( t.tfoundentityindexhit != -1 ) 
			{
				t.entityelement[t.tfoundentityindexhit].detectedlimbhit = 0;
				t.entityelement[t.tfoundentityindexhit].lua.flagschanged = 1;
			}
			if (  t.tlimbhit>0 ) 
			{
				if (  ObjectExist(t.bulletrayhit) == 1 ) 
				{
					if (  LimbExist(t.bulletrayhit,t.tlimbhit) == 1 ) 
					{
						// record which limb we hit
						t.bulletraylimbhit = t.tlimbhit;

						// 201115 - also record limb hit within entity (so LUA can do stuff with the info)
						if ( t.tfoundentityindexhit != -1 ) 
						{
							t.entityelement[t.tfoundentityindexhit].detectedlimbhit = t.tlimbhit;
							t.entityelement[t.tfoundentityindexhit].lua.flagschanged = 1;
						}
					}
				}
			}
		}
		if ( t.tmaterialvalue >= 0 ) t.tttriggerdecalimpact = 10+t.tmaterialvalue;
	}

	// ensure material index never goes negative
	if ( t.tmaterialvalue < 0 ) t.tmaterialvalue = 0;

	//  calculate increment along ray
	t.tbix_f=t.brayx2_f-t.brayx1_f;
	t.tbiy_f=t.brayy2_f-t.brayy1_f;
	t.tbiz_f=t.brayz2_f-t.brayz1_f;
	t.trange_f=Sqrt(abs(t.tbix_f*t.tbix_f)+abs(t.tbiy_f*t.tbiy_f)+abs(t.tbiz_f*t.tbiz_f));
	t.tbix_f=t.tbix_f/t.trange_f;
	t.tbiy_f=t.tbiy_f/t.trange_f;
	t.tbiz_f=t.tbiz_f/t.trange_f;

	//  if bullet ray passed waterlevel, create a splash at intersection
	if (  t.hardwareinfoglobals.nowater == 0 ) 
	{
		#ifdef WICKEDENGINE
		if (t.decalglobal.splashdecalrippleid != 0 && ((t.brayy1_f > t.terrain.waterliney_f && t.brayy2_f < t.terrain.waterliney_f) || (t.brayy1_f<t.terrain.waterliney_f && t.brayy2_f>t.terrain.waterliney_f)))
		#else
		if (t.decalglobal.splashdecalrippleid != 0 && t.terrain.TerrainID > 0 && ((t.brayy1_f > t.terrain.waterliney_f && t.brayy2_f < t.terrain.waterliney_f) || (t.brayy1_f<t.terrain.waterliney_f && t.brayy2_f>t.terrain.waterliney_f)))
		#endif
		{
			//  calculate coordate where ray hit water plane
			t.tperc_f=(t.brayy1_f-t.terrain.waterliney_f)/abs(t.tbiy_f);
			t.tbx_f=t.brayx1_f+(t.tbix_f*t.tperc_f);
			t.tby_f=t.brayy1_f+(t.tbiy_f*t.tperc_f);
			t.tbz_f=t.brayz1_f+(t.tbiz_f*t.tperc_f);
			//  check if this coord ABOVE terrain Floor (  )
			t.tgroundheight_f=BT_GetGroundHeight(t.terrain.TerrainID,t.tbx_f,t.tbz_f);
			if (  t.tby_f>t.tgroundheight_f ) 
			{
				//  trigger water splash at coords
				g.decalx=t.tbx_f ; g.decaly=t.tby_f+0.5 ; g.decalz=t.tbz_f; t.tInScale_f = 1;
				decal_triggerwatersplash ( );
				//  play splash sound
				t.tmatindex=17; 
				#ifdef WICKEDENGINE
				t.tsoundtrigger = t.material[t.tmatindex].matsound_id[matSound_LandHard][0];
				t.tvol_f = 75;
				#else
				t.tsoundtrigger = t.material[t.tmatindex].impactid;
				t.tvol_f = 6;
				#endif
				t.tspd_f=(t.material[t.tmatindex].freq*1.5)+Rnd(t.material[t.tmatindex].freq)*0.5;
				t.tsx_f=g.decalx ; t.tsy_f=g.decaly ; t.tsz_f=g.decalz;
				material_triggersound ( 0 );
				t.tsoundtrigger=0;
			}
		}
	}

	//  check if we hit a character
	if (  t.bulletrayhit>0 ) 
	{
		if (  ObjectExist(t.bulletrayhit) == 1 ) 
		{
			// Find which entity this is
			t.bulletrayhite=-1 ; t.bulletrayhitentid=-1;
			t.tobj=t.bulletrayhit;
			if ( t.tfoundentityindexhit != -1 ) 
			{
				t.tte=t.tfoundentityindexhit ; t.bulletrayhite=t.tte;
				t.bulletrayhitentid=t.entityelement[t.tte].bankindex;
			}
			else
			{
				if ( t.tobj>0 ) 
				{
					if ( ObjectExist(t.tobj) == 1 ) 
					{
						for ( t.tte = 1 ; t.tte <= g.entityelementlist; t.tte++ )
						{
							if ( t.entityelement[t.tte].obj == t.tobj ) 
							{
								t.bulletrayhite=t.tte ; t.bulletrayhitentid=t.entityelement[t.tte].bankindex ; break;
							}
						}
					}
				}
			}

			// Check if this object is a character
			if ( t.bulletrayhitentid != -1 ) 
			{
				t.px_f=t.x1_f ; t.py_f=t.y1_f ; t.pz_f=t.z1_f;
				entity_determinegunforce ( );
				darkai_ischaracterhit ( );
				if ( t.darkaifirerayhitcharacter == 0 ) 
				{
					// also make sure it's not a beast (ragdoll)
					t.tokay = 1 ; if ( t.entityprofile[t.bulletrayhitentid].ragdoll == 1 )  t.tokay = 0;
					if ( t.tokay == 1 ) 
					{
						// create a ray of force along bullet tragectory (to disturb non-character objects)
						if ( ODERayForce(t.brayx1_f,t.brayy1_f,t.brayz1_f,t.brayx2_f,t.brayy2_f,t.brayz2_f,t.tforce_f*0.25) == 1 ) 
						{
							// and knock dynamic physics objects if force is applied to them
						}
					}
				}
				if ( t.darkaifirerayhitcharacter == 1 ) 
				{
					// trigger limb flinch system (limbhurt and limbhurta#)
					if ( t.bulletraylimbhit != -1 && t.bulletrayhite != -1 ) 
					{
						t.tte=t.bulletrayhite;
						if (  t.entityelement[t.tte].limbhurt <= 0 ) 
						{
							t.entityelement[t.tte].limbhurt=t.bulletraylimbhit;
							//  determine if entity facing away from plr
							t.tdx_f=ObjectPositionX(t.tobj)-CameraPositionX(0);
							t.tdz_f=ObjectPositionZ(t.tobj)-CameraPositionZ(0);
							t.tangley_f=atan2deg(t.tdx_f,t.tdz_f);
							t.tdiffhurtangle_f=t.tangley_f-ObjectAngleY(t.tobj);
							if ( t.tdiffhurtangle_f<-180  )  t.tdiffhurtangle_f = t.tdiffhurtangle_f+360;
							if ( t.tdiffhurtangle_f>180  )  t.tdiffhurtangle_f = t.tdiffhurtangle_f-360;
							if ( abs(t.tdiffhurtangle_f)<90.0 ) 
							{
								// bend forward
								t.entityelement[t.tte].limbhurta_f=8+Rnd(8);
							}
							else
							{
								// bend back
								t.entityelement[t.tte].limbhurta_f=(8+Rnd(8))*-1;
							}
						}
					}
					// cause blood splat (if violent)
					if ( t.bulletrayhite != -1 ) 
					{
						#ifdef WICKEDENGINE
						// Wicked is wicked :)
						//PE: But still allow unselect blood effect on some objects :)
						if (t.entityelement[t.bulletrayhite].eleprof.isviolent != 0)
						{
							t.tttriggerdecalimpact = 2;
						}

						#else
						if ( t.entityelement[t.bulletrayhite].eleprof.isviolent != 0 && g.quickparentalcontrolmode != 2) 
						{
							// 100317 - only if material index not specified (or materialindex=0) (16=flesh)
							if ( t.tttriggerdecalimpact <= 10 || t.tttriggerdecalimpact == 16)
							{
								t.tttriggerdecalimpact = 2;
							}
						}
						#endif
					}
					t.bulletrayhit=0;
				}
			}
			else
			{
				//  did not hit entity, could be we hit a lightmapped static object
				t.bulletrayhite=0;
			}

			//  determine which entity we hit (if not character which is already handled)
			//  for things such as Zombies and other entities in the level
			if ( t.bulletrayhit > 0 && t.bulletrayhite != -1 ) 
			{
				// apply some damage
				t.tdamagesource = 1;
				#ifdef WICKEDENGINE
				t.tdamage = 0;
				if (g.firemodes[t.gunid][g.firemode].settings.damage > 0)
				{
					// ensure DamageMultiplier does not wipe out minimum damage
					t.tdamage = (float)g.firemodes[t.gunid][g.firemode].settings.damage * t.playercontrol.fWeaponDamageMultiplier;
					if (t.playercontrol.fWeaponDamageMultiplier > 0 && t.tdamage < 1) t.tdamage = 1;
				}
				if (t.gun[t.gunid].settings.ismelee == 2 || g.firemodes[t.gunid][g.firemode].settings.usemeleedamageonly > 0)
				{
					if (g.firemodes[t.gunid][0].settings.meleedamage > 0)
					{
						// ensure DamageMultiplier does not wipe out minimum damage
						t.tdamage = (float)g.firemodes[t.gunid][0].settings.meleedamage * t.playercontrol.fMeleeDamageMultiplier;
						if (t.playercontrol.fMeleeDamageMultiplier > 0 && t.tdamage < 1) t.tdamage = 1;
					}
				}
				#else
				t.tdamage = g.firemodes[t.gunid][g.firemode].settings.damage;
				if (t.gun[t.gunid].settings.ismelee == 2) t.tdamage = g.firemodes[t.gunid][0].settings.meleedamage;
				#endif
				entity_hitentity ( t.bulletrayhite, t.bulletrayhit );
			}
		}
	}

	#ifdef WICKEDENGINE
	// if hitting a material, leave a bullethole (not for melee combat 'weapon')
	if (!strstr(t.gun[t.gunid].name_s.Get(), "gloves_unarmed") && t.tttriggerdecalimpact >= 10 && t.tttriggerdecalimpact != 16)
	{
		if (t.bulletrayhite == 0 || t.entityelement[t.bulletrayhite].staticflag == 1)
		{
			int iMaterialIndex = t.tttriggerdecalimpact - 10;
			bulletholes_add(iMaterialIndex, t.brayx2_f, t.brayy2_f, t.brayz2_f, vecRayHitNormal.x, vecRayHitNormal.y, vecRayHitNormal.z);
		}
	}
	#endif

	// trigger decal at impact coordinate
	entity_triggerdecalatimpact ( t.brayx2_f, t.brayy2_f, t.brayz2_f );
}

void entity_hitentity ( int e, int obj )
{
	int iStoreE = t.ttte; t.ttte = e;
	if ( t.entityelement[t.ttte].health > 0 ) 
	{
		//  turn into clone in case we need to animate it (could be in distance)
		if ( GetNumberOfFrames(obj) > 0 ) 
		{
			entity_converttoclone ( );
		}

		// determine and set damage force
		entity_determinegunforce ( );
		t.ttentid=t.entityelement[t.ttte].bankindex;
		if (  t.entityprofile[t.ttentid].ischaracter == 1 || t.entityprofile[t.ttentid].ragdoll == 1 ) 
		{
			t.tdamageforce=t.tforce_f;
		}
		else
		{
			t.tdamageforce=0;
		}

		// apply the damage
		entity_applydamage ( );

		// check if we hit an organic or custom character (animal, zombie, etc)
		t.ttentid = t.entityelement[t.ttte].bankindex;

		// cause blood splat
		if ( t.entityprofile[t.ttentid].materialindex == 6 || ( t.entityprofile[t.ttentid].materialindex == 0 && t.entityprofile[t.ttentid].ischaracter == 1 ) ) 
		{
			if ( t.entityelement[t.ttte].eleprof.isviolent != 0 && g.quickparentalcontrolmode != 2 )
			{
				t.tttriggerdecalimpact=2;
			}
		}

		//  cause blood splat on steam multiplayer char
		if ( t.game.runasmultiplayer == 1 ) 
		{
			if ( t.entityprofile[t.ttentid].ismultiplayercharacter == 1 ) 
			{
				if ( t.entityelement[t.ttte].eleprof.isviolent != 0 && g.quickparentalcontrolmode != 2 )
				{
					t.tttriggerdecalimpact=2;
				}
			}
		}
	}
	t.ttte = iStoreE;
}

void entity_triggerdecalatimpact ( float fX, float fY, float fZ )
{
	// trigger decal at impact coordinate
	// 111215 - and only if ignorematerial flag not set by GUNSPEC (for interactive HUD 'weapons')
	if ( t.tttriggerdecalimpact>0 && (t.gunid==0 || g.firemodes[t.gunid][0].settings.ignorematerial == 0) )
	{
		//  trigger decal animation at coords
		g.decalx=fX; g.decaly=fY+0.5 ; g.decalz=fZ;
		if ( t.tttriggerdecalimpact >= 10 ) decal_triggermaterialdecal ( );
		if ( t.tttriggerdecalimpact == 1 ) decal_triggerimpact ( );
		if ( t.tttriggerdecalimpact == 2 ) 
		{
			if ( t.playercontrol.startviolent != 0 && g.quickparentalcontrolmode != 2 ) 
			{
				for ( t.iter = 1 ; t.iter <= 3+Rnd(1); t.iter++ )
				{
					decal_triggerbloodsplat ( );
				}
			}
		}

		// play material impact sound
		t.tmatindex = 0 ; if (  t.tttriggerdecalimpact >= 10  )  t.tmatindex = t.tttriggerdecalimpact-10;
		#ifdef WICKEDENGINE
		t.tsoundtrigger = t.material[t.tmatindex].matsound_id[matSound_LandHard][0];
		#else
		t.tsoundtrigger = t.material[t.tmatindex].impactid;
		#endif
		t.tspd_f=t.material[t.tmatindex].freq;
		t.tsx_f=g.decalx ; t.tsy_f=g.decaly ; t.tsz_f=g.decalz;
		t.tvol_f = 100.0f ; material_triggersound ( 0 );
		t.tsoundtrigger=0;
	}
}

void entity_createattachment ( void )
{
	// Single player character must HOLD the weapon before attaching it
	t.tischaracterholdingweapon=0;
	if ( t.entityprofile[t.entid].ischaracter == 1 && t.entityelement[t.e].eleprof.hasweapon>0 ) 
	{
		t.tischaracterholdingweapon=1;
	}

	// Load all VWEAPS for each entity that wants weapon attachments
	t.entid=t.entityelement[t.e].bankindex;
	if ( (t.tischaracterholdingweapon == 1 || t.entityprofile[t.entid].ismultiplayercharacter == 1) && t.entityelement[t.e].obj>0 ) 
	{
		// Make attachment if warranted
		if ( ObjectExist(t.entityelement[t.e].obj) == 1 && t.entityelement[t.e].attachmentobj == 0 ) 
		{
			if ( t.entityprofile[t.entid].firespotlimb>-1 ) 
			{
				// all vweaps (that are active)
				for ( t.tgindex = 1 ; t.tgindex <= g.gunmax; t.tgindex++ )
				{
					if ( t.gun[t.tgindex].activeingame == 1 ) 
					{
						t.tweaponname_s=t.gun[t.tgindex].name_s;
						if ( t.tweaponname_s != "" ) 
						{
							// entity has this gun in their hands
							#ifdef WICKEDENGINE
							t.tthasweapon_s = Lower(t.entityelement[t.e].eleprof.hasweapon_s.Get());
							#else
							t.tthasweapon_s = Lower(t.entityprofile[t.entid].hasweapon_s.Get());
							#endif
							if ( t.tthasweapon_s == t.tweaponname_s.Lower() ) 
							{
								// go and load this gun (attached to calling entity instance)
								++g.entityattachmentindex;
								t.ttobj=g.entityattachmentsoffset+g.entityattachmentindex;
								if (  ObjectExist(t.ttobj) == 1  )  DeleteObject (  t.ttobj );

								// replaced X file load with optional DBO convert/load
								t.tfile_s="gamecore\\guns\\";
								t.tfile_s += t.tweaponname_s+"\\vweap.x";
								deleteOutOfDateDBO(t.tfile_s.Get());
								if ( cstr(Lower(Right(t.tfile_s.Get(),2))) == ".x"  )  {t.tdbofile_s = Left(t.tfile_s.Get(),Len(t.tfile_s.Get())-2); t.tdbofile_s += ".dbo"; } else t.tdbofile_s = "";
								if (FileExist(t.tfile_s.Get()) == 0 && FileExist(t.tdbofile_s.Get()) == 0)
								{
									// can use new designation for weapons held (for both player and character in new system)
									t.tfile_s = "";
									t.tdbofile_s = "gamecore\\guns\\";
									t.tdbofile_s += t.tweaponname_s + "\\weapon.dbo";
								}
								if (FileExist(t.tfile_s.Get()) == 1 || FileExist(t.tdbofile_s.Get()) == 1)
								{
									if ( FileExist(t.tdbofile_s.Get()) == 1 ) 
									{
										t.tfile_s=t.tdbofile_s;
										t.tdbofile_s="";
									}
									LoadObject ( t.tfile_s.Get(), t.ttobj );
									#ifdef WICKEDENGINE
									#else
									SetObjectFilter (  t.ttobj,2 );
									if ( Len(t.tdbofile_s.Get()) > 1 ) 
									{
										if ( FileExist( t.tdbofile_s.Get()) == 0 ) 
										{
											// unnecessary now as LoadObject auto creates DBO file!
											SaveObject ( t.tdbofile_s.Get(), t.ttobj );
										}
										if (  FileExist(t.tdbofile_s.Get()) == 1 ) 
										{
											DeleteObject (  t.ttobj );
											LoadObject (  t.tdbofile_s.Get(),t.ttobj );
											SetObjectFilter (  t.ttobj,2 );
											t.tfile_s=t.tdbofile_s;
										}
									}
									#endif
								}
								else
								{
									MakeObjectTriangle (  t.ttobj,0,0,0,0,0,0,0,0,0 );
								}
								t.entityelement[t.e].attachmentobj=t.ttobj;

								#ifdef WICKEDENGINE
								SetObjectDiffuseEx(t.ttobj, 0xFFFFFFFF, 0);
								#else
								//  Apply object settings
								SetObjectTransparency (  t.ttobj,1 );
								SetObjectCollisionOff (  t.ttobj );
								SetObjectMask (  t.ttobj, 1 );
								EnableObjectZDepth(t.ttobj); // PE:

								//  VWEAP is NOT part of collision universe (prevents rocket hitting launcher)
								SetObjectCollisionProperty (  t.ttobj,1 );

								// fixes issue of some models not being able to detect with intersectall
								SetObjectDefAnim ( t.ttobj, t.entityprofile[t.entid].ignoredefanim );
								#endif

								// VWEAP can choose own texture
								t.tvweaptex_s=t.gun[t.tgindex].vweaptex_s;
								if ( Len(t.tvweaptex_s.Get())<2  )  t.tvweaptex_s = "gun";

								// apply texture to vweap
								if ( g.gdividetexturesize == 0 ) 
								{
									t.texuseid=loadinternaltexture("effectbank\\reloaded\\media\\white_D.dds");
								}
								else
								{
									sprintf ( t.szwork , "gamecore\\guns\\%s\\%s_D.dds" , t.tweaponname_s.Get() , t.tvweaptex_s.Get() );
									t.texuseid=loadinternaltexture(t.szwork);
									if (t.texuseid == 0) {
										sprintf(t.szwork, "gamecore\\guns\\%s\\%s_color.dds", t.tweaponname_s.Get(), t.tvweaptex_s.Get());
										t.texuseid = loadinternaltexture(t.szwork);
									}
								}
								TextureObject ( t.ttobj, 0, t.texuseid );

								#ifdef WICKEDENGINE
								#else
								TextureObject ( t.ttobj,1,loadinternaltexture("effectbank\\reloaded\\media\\blank_O.dds") );
								sprintf ( t.szwork , "gamecore\\guns\\%s\\%s_N.dds" ,t.tweaponname_s.Get() , t.tvweaptex_s.Get() );
								t.texuseid=loadinternaltexture(t.szwork);
								if (t.texuseid == 0) 
								{
									sprintf(t.szwork, "gamecore\\guns\\%s\\%s_normal.dds", t.tweaponname_s.Get(), t.tvweaptex_s.Get());
									t.texuseid = loadinternaltexture(t.szwork);
								}
								TextureObject (  t.ttobj,2,t.texuseid );
								sprintf ( t.szwork ,  "gamecore\\guns\\%s\\%s_S.dds" ,t.tweaponname_s.Get() , t.tvweaptex_s.Get()  );
								t.texuseid=loadinternaltexture(t.szwork);
								if (t.texuseid == 0) 
								{
									//PE: we need a gray_d for a little less specular on guns.
									//PE: blank_medium_S.DDS dont work yet.
									t.texuseid = loadinternaltexture("effectbank\\reloaded\\media\\white_D.dds");
								}			

								TextureObject ( t.ttobj,3,t.texuseid );
								TextureObject ( t.ttobj,4,t.terrain.imagestartindex );
								TextureObject ( t.ttobj,5,g.postprocessimageoffset+5 );
								TextureObject ( t.ttobj,6,loadinternaltexture("effectbank\\reloaded\\media\\blank_I.dds") );

								// Apply entity shader to vweap model
								#ifdef VRTECH
								t.teffectid=loadinternaleffect("effectbank\\reloaded\\entity_basic.fx");
								#else
								t.teffectid=loadinternaleffect("effectbank\\reloaded\\character_static.fx");
								#endif
								SetObjectEffect (  t.ttobj,t.teffectid );
								#endif

								// Find firespot for this vweap
								t.entityelement[t.e].attachmentobjfirespotlimb=0;
								PerformCheckListForLimbs (  t.ttobj );
								for ( t.tc = 1 ; t.tc <= ChecklistQuantity(); t.tc++ )
								{
									if ( cstr(Lower(ChecklistString(t.tc))) == "firespot" ) 
									{
										t.entityelement[t.e].attachmentobjfirespotlimb=t.tc-1;
										t.tc=ChecklistQuantity()+1;
									}
								}

								// no need to continue looking thrugh guns
								t.tgindex=g.gunmax; break;
							}
						}
					}
				}
			}
		}
	}
}

void entity_freeattachment ( void )
{
	if (  t.entityelement[t.e].attachmentobj>0 ) 
	{
		if (  ObjectExist(t.entityelement[t.e].attachmentobj) == 1 ) 
		{
			HideObject (  t.entityelement[t.e].attachmentobj );
		}
	}
}

void entity_controlattachments ( void )
{
	// ensure attachments are updated and visible
	t.tcharacterobj=t.entityelement[t.e].obj;
	t.tobj=t.entityelement[t.e].attachmentobj;
	if ( t.tobj>0 && t.tcharacterobj>0 ) 
	{
		//  Added check for visibility, as attachment will be invisible once it's been picked up
		if ( ObjectExist(t.tobj) == 1 && ObjectExist(t.tcharacterobj) == 1 ) //&& GetVisible(t.tobj) == 1 ) so weapon not flicker when show during equip anim
		{
			//  manual position of gun attachment
			#ifdef WICKEDENGINE
			// holds onto gun longer so physics kicks in when death anim ends
			if (t.entityelement[t.e].beenkilled == 0)
			#else
			if (t.entityelement[t.e].beenkilled == 0 && (t.entityelement[t.e].active != 0 || t.entityelement[t.e].eleprof.phyalways != 0))
			#endif
			{
				t.limbpx_f=ObjectPositionX(t.entityelement[t.e].obj);
				t.limbpy_f=ObjectPositionY(t.entityelement[t.e].obj);
				t.limbpz_f=ObjectPositionZ(t.entityelement[t.e].obj);
				if ( t.entityelement[t.e].obj>0 ) 
				{
					if ( ObjectExist(t.entityelement[t.e].obj) == 1 ) 
					{
						t.tentid=t.entityelement[t.e].bankindex;
						if ( t.entityprofile[t.tentid].firespotlimb >= 0 ) 
						{
							if ( LimbExist(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb) == 1 ) 
							{
								// position of entity hand where gun rests
								#ifdef WICKEDENGINE
								// animation system moved over to wicked, so only wicked knows where firespot limb is right now!
								sObject* pObject = GetObjectData(t.entityelement[t.e].obj);
								float fQuatX, fQuatY, fQuatZ, fQuatW;
								int iGunID = t.entityelement[t.e].eleprof.hasweapon;
								float fHandOffX = t.gun[iGunID].handposx_f;
								float fHandOffY = t.gun[iGunID].handposy_f;
								float fHandOffZ = t.gun[iGunID].handposz_f;
								float fHandRotX = t.gun[iGunID].handrotx_f;
								float fHandRotY = t.gun[iGunID].handroty_f;
								float fHandRotZ = t.gun[iGunID].handrotz_f;

								/* this is ultra hacky, should come from model or gunspec, not from hard coding!!!
								// weapon type (LB: optimize to store weapon type (pistol/rifle/etc)
								LPSTR animsystem_getweapontype (LPSTR,LPSTR);
								LPSTR pWeaponName = t.entityelement[t.e].eleprof.hasweapon_s.Get();
								int gunid = t.entityelement[t.e].eleprof.hasweapon;
								LPSTR pWeaponType = animsystem_getweapontype(pWeaponName,t.gun[gunid].animsetoverride.Get());
								int iIsAPistol = 0;
								int iIsAShotgun = 0;
								if (stricmp(pWeaponType, "") == NULL)
								{
									iIsAPistol = 1;
								}
								else
								{
									if (stricmp(pWeaponName, "enhanced\\R870") == NULL)
									{
										iIsAShotgun = 1;
									}
								}
								// male and female
								if (iIsAPistol == 0)
								{
									// adjustment to lift rifle a little for all
									fHandRotX += 10.0f;

									// if not a shotgun (rifle), move down a touch
									if (iIsAShotgun == 0)
									{
										fHandOffY -= 2.0f;
									}
								}
								// just female
								if (t.entityprofile[t.tentid].characterbasetype == 1)
								{
									fHandRotX += 6.0f;
									if (iIsAPistol == 1)
									{
										// and extra tweak for pistol pose (-rifle is okay)
										fHandRotY += 10.0f;
									}
									else
									{
										// a bit more for female rifle and shotgun
										fHandOffY -= 1.0f;
									}
								}
								*/
								//PE: Optimizing this is hitting TransformComponent::GetLocalMatrix() heavy.
								//PE: use t.entityelement[t.e] to store last entry.
								//PE: This dont change unless t.limbpx_f,t.limbpy_f,t.limbpz_f has changed from last frame.
								if (t.entityelement[t.e].firespotlimb_lastx != t.limbpx_f ||
									t.entityelement[t.e].firespotlimb_lasty != t.limbpy_f ||
									t.entityelement[t.e].firespotlimb_lastz != t.limbpz_f)
								{
									//LB: This is one frame behind, need a Wicked GLUE operation for attached objects me thinks!
									WickedCall_GetLimbDataEx(pObject, t.entityprofile[t.tentid].firespotlimb, true, fHandOffX, fHandOffY, fHandOffZ, GGToRadian(fHandRotX), GGToRadian(fHandRotY), GGToRadian(fHandRotZ), &t.limbpx_f, &t.limbpy_f, &t.limbpz_f, &fQuatX, &fQuatY, &fQuatZ, &fQuatW);
									RotateObjectQuat(t.tobj, fQuatX, fQuatY, fQuatZ, fQuatW);

									// scale now from gunspec
									float fHandscale = t.gun[iGunID].handscale_f;
									ScaleObject(t.tobj, fHandscale, fHandscale, fHandscale);

									// store limb offset position to ensure we only call if needed
									t.entityelement[t.e].firespotlimb_lastx = t.limbpx_f;
									t.entityelement[t.e].firespotlimb_lasty = t.limbpy_f;
									t.entityelement[t.e].firespotlimb_lastz = t.limbpz_f;
								}
								#else
								// expensive process, so only do when close enough to see one-frame lag
								// be aware that this ADVANCES THE ANIM FRAME ONE STEP
								CalculateObjectBoundsEx (  t.entityelement[t.e].obj,1 );
								t.limbpx_f=LimbPositionX(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb);
								t.limbpy_f=LimbPositionY(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb);
								t.limbpz_f=LimbPositionZ(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb);
								t.limbax_f=LimbDirectionX(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb);
								t.limbay_f=LimbDirectionY(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb);
								t.limbaz_f=LimbDirectionZ(t.entityelement[t.e].obj,t.entityprofile[t.tentid].firespotlimb);
								#ifdef VRTECH
								RotateObject (  t.tobj,t.limbax_f,t.limbay_f,t.limbaz_f );
								ScaleObject (  t.tobj,85,85,85 );
								#else
								float fWeaponScale = 85.0f;
								fWeaponScale *= (t.entityprofile[t.entid].handfirespotsize/100.0f);
								RotateObject ( t.tobj, t.limbax_f, t.limbay_f, t.limbaz_f );
								ScaleObject ( t.tobj, fWeaponScale, fWeaponScale, fWeaponScale );
								#endif
								#endif
							}
						}
					}
				}

				PositionObject (  t.tobj,t.limbpx_f,t.limbpy_f,t.limbpz_f );
				
				//ShowObject (  t.tobj ); visiblility handled elsewhere

				//PE: fix t.entityelement[t.e].attachmentobj;
				//PE: https://forum.game-guru.com/thread/219491.
				EnableObjectZDepth(t.tobj);
			}
		}
	}
}

void entity_monitorattachments ( void )
{
	//  handle when player picks up ammo from dead enemies. Assumes ammo pool 1 is the default pool we want to update
	if (  t.playercontrol.thirdperson.enabled == 0 ) 
	{
		t.tobj=t.entityelement[t.e].attachmentobj;
		if (  t.tobj>0 ) 
		{
			if (  ObjectExist(t.tobj) == 1 && GetVisible(t.tobj) == 1 ) 
			{
				if (  t.entityelement[t.e].health <= 0 && t.entityelement[t.e].eleprof.cantakeweapon != 0 ) 
				{
					t.txDist_f = CameraPositionX(0) - ObjectPositionX(t.tobj);
					t.tyDist_f = CameraPositionY(0) - ObjectPositionY(t.tobj);
					t.tzDist_f = CameraPositionZ(0) - ObjectPositionZ(t.tobj);
					t.ttD_f = t.txDist_f * t.txDist_f + t.tyDist_f * t.tyDist_f + t.tzDist_f * t.tzDist_f;
					if (  t.ttD_f < 100 * 100 ) 
					{
						//  collect whole weapon, not just ammo
						t.weaponindex=t.entityelement[t.e].eleprof.hasweapon;
						if (  t.weaponindex>0 ) 
						{
							//  if weapon not yet part of players inventory, prompt to press E
							t.gotweapon=0;
							for ( t.ws = 1 ; t.ws<=  10; t.ws++ )
							{
								if (  t.weaponslot[t.ws].got == t.weaponindex  )  t.gotweapon = t.ws;
							}
							int iPressedE = KeyState(g.keymap[18]);
							if ( g.gxbox == 1 ) 
							{
								if ( JoystickFireD() == 1 )  
									iPressedE = 1;
							}
							if (  t.gotweapon == 0 && iPressedE == 0 ) 
							{
								//  prompt user to press E
								t.luaglobal.scriptprompttime=Timer()+200;
								if ( g.gxbox == 0 )
									t.luaglobal.scriptprompt_s="Press E to pick up weapon";
								else
									t.luaglobal.scriptprompt_s="Press Y Button to pick up weapon";
							}
							else
							{
								//  got weapon already, so collect ammo
								t.tqty=t.entityelement[t.e].eleprof.quantity;
								physics_player_addweapon ( );
								t.luaglobal.scriptprompttime=Timer()+3000;
								t.luaglobal.scriptprompt_s="Collected ammo";
								ODEDestroyObject (  t.tobj );
								HideObject (  t.tobj );
							}
						}
					}
				}
			}
		}
	}
}

void entity_converttoclone ( void )
{
	#ifdef WICKEDENGINE
	// wicked handles instances inside engine, no need to have clone/instance here
	#else
	// takes tte
	if ( t.entityelement[t.tte].isclone == 0 ) 
	{
		t.tttentid = t.entityelement[t.tte].bankindex;
		if ( t.entityprofile[t.tttentid].isebe == 0 )
		{
			t.tobj=t.entityelement[t.tte].obj;
			if (  t.tobj>0 ) 
			{
				if (  ObjectExist(t.tobj) == 1 ) 
				{
					t.tstorevis=GetVisible(t.tobj);
					DeleteObject ( t.tobj );
					t.ttsourceobj=g.entitybankoffset+t.tttentid;
					#ifdef WICKEDENGINE
					WickedSetElementId(t.tte);
					WickedSetEntityId(t.tttentid);
					#endif
					if (  t.tentityconverttoclonenotshared == 1 ) 
					{
						//  need a freshly created when third person character needs new shader applied
						CloneObject (  t.tobj,t.ttsourceobj,0 );
						SetObjectEffect (  t.tobj,0 );
					}
					else
					{
						CloneObject (  t.tobj,t.ttsourceobj,1 );
					}
					#ifdef WICKEDENGINE
					WickedSetEntityId(-1);
					#endif

					//  restore any radius settings the original object might have had
					SetSphereRadius (  t.tobj,-1 );
					t.entityelement[t.tte].isclone=1;
					t.tentid=t.entityelement[t.tte].bankindex;
					entity_prepareobj ( );
					entity_positionandscale ( );
					if (  t.tstorevis == 0  )  HideObject (  t.tobj );
					if ( t.entityprofile[t.tttentid].addhandlelimb == 0 )
					{
						// 301115 - override parent LOD distance with LODModifier
						entity_calculateentityLODdistances ( t.tttentid, t.tobj, t.entityelement[t.tte].eleprof.lodmodifier );
					}
				}
			}
		}
	}
	#endif
}

void entity_converttoclonetransparent ( void )
{
	#ifdef WICKEDENGINE
	// wicked handles instances inside engine, no need to have clone/instance here
	#else
	// used in IDE editor to show locked entities
	entity_converttoclone ( );
	SetAlphaMappingOn ( t.tobj, 110 ); // special semi-transparent mode with draw first rendering
	DisableObjectZWrite ( t.tobj );
	#endif
}

bool entity_isuniquespecularoruv ( int ee )
{
	bool bUnique = false; 
	if ( ee != -1  ) 
	{
		// specular can be overridden per entity
		if ( t.entityelement[ee].eleprof.specularperc != 100.0f ) 
			bUnique = true;

		// UV data scroll/scale can be overridden per entity
		int eentid = t.entityelement[ee].bankindex;
		if ( t.entityprofile[eentid].uvscrollu != 0.0f 
		||   t.entityprofile[eentid].uvscrollv != 0.0f 
		||   t.entityprofile[eentid].uvscaleu != 1.0f 
		||   t.entityprofile[eentid].uvscalev != 1.0f )
			bUnique = true;
	}
	return bUnique;
}

void entity_converttoinstance ( void )
{
	// takes tte
	#ifdef WICKEDENGINE
	// really needed this when I had ragdoll objects, call it if object has been ragdollified
	if (t.entityelement[t.tte].ragdollified == 1)
	#else
	if (t.entityelement[t.tte].isclone == 1 ) 
	#endif
	{
		t.tobj=t.entityelement[t.tte].obj;
		if ( t.tobj>0 ) 
		{
			// 101216 - if entity is given unique specular, must be a clone to take effect
			// 020217 - quit early if cannot make this an instance
			bool bUniqueSpecularOrUV = entity_isuniquespecularoruv ( t.tte );
			if ( ObjectExist(t.tobj) == 1 && bUniqueSpecularOrUV == false ) 
			{
				//  first remove any ragdoll
				entity_freeragdoll ();

				//  then delete clone and recreate as instance
				t.tstorevis=GetVisible(t.tobj);
				DeleteObject (  t.tobj );
				t.ttsourceobj=g.entitybankoffset+t.entityelement[t.tte].bankindex;

				#ifdef WICKEDENGINE
				WickedSetElementId(t.tte);
				WickedSetEntityId(t.entityelement[t.tte].bankindex);
				#endif

				#ifdef WICKEDENGINE
				// always clone for now (instance work during performance opt)
				CloneObject (t.tobj, t.ttsourceobj, 1);
				#else
				#ifdef VRTECH
				if ( t.entityprofile[t.entityelement[t.tte].bankindex].cpuanims==0 && t.entityprofile[t.entityelement[t.tte].bankindex].ischaracter == 0 )
				#else
				if ( t.entityprofile[t.entityelement[t.tte].bankindex].cpuanims==0 )
				#endif
					InstanceObject (t.tobj, t.ttsourceobj);
				else
					CloneObject (t.tobj, t.ttsourceobj, 1);
				#endif

				#ifdef WICKEDENGINE
				WickedSetElementId(0);
				WickedSetEntityId(-1);
				#endif

				// restore any radius settings the original object might have had
				SetSphereRadius (  t.tobj,-1 );
				t.entityelement[t.tte].isclone=0;
				t.tentid = t.entityelement[t.tte].bankindex;
				entity_prepareobj ( );
				entity_positionandscale ( );
				if (  t.tstorevis == 0  )  HideObject (  t.tobj );
				#ifdef VRTECH
				///characterkit_convertCloneInstance ( );
				#else
				characterkit_convertCloneInstance ( );
				#endif
				if ( t.entityprofile[t.tentid].addhandlelimb == 0 )
				{
					// 301115 - override parent LOD distance with LODModifier
					entity_calculateentityLODdistances ( t.tentid, t.tobj, t.entityelement[t.tte].eleprof.lodmodifier );
				}
			}
		}
	}
}

void entity_createobj ( void )
{
	//  takes OBJ, TUPDATEE, TENDIT
	t.sourceobj=g.entitybankoffset+t.tentid;
	if (  ObjectExist(t.sourceobj) == 1 ) 
	{
		if (  t.tupdatee != -1  )  t.entityelement[t.tupdatee].profileobj = t.sourceobj;

		#ifdef WICKEDENGINE
		// wicked handles instances inside engine, so always clone
		bool bCreateAsClone = true;
		#else
		EnableObjectZWrite (  t.sourceobj );
		//  Create new object
		bool bUniqueSpecular = entity_isuniquespecularoruv ( t.tupdatee );

		//PE: We also need it to be a clone in Classic for head to work.
		bool bCreateAsClone = false;
		if ( t.entityprofile[t.tentid].ismarker != 0 || t.entityprofile[t.tentid].cpuanims != 0 || t.entityprofile[t.gridentity].isebe != 0 || bUniqueSpecular == true ) bCreateAsClone = true;
		if ( t.entityprofile[t.tentid].ischaractercreator == 1 ) bCreateAsClone = true; // needed to keep head attached!
		#endif

		#ifdef WICKEDENGINE
		extern bool bUseInstancing;
		extern int iUseMasterObjectID;
		extern bool bNextObjectMustBeClone;
		//PE: InstanceObject - If this object has a custom "Materials" always create as clone.
		if (t.tupdatee != -1)
		{
			if (t.entityelement[t.tupdatee].eleprof.bCustomWickedMaterialActive) bCreateAsClone = true;
			WickedSetElementId(t.tupdatee);
			WickedSetEntityId(t.entityelement[t.tupdatee].bankindex);
			iUseMasterObjectID = t.sourceobj;

			bUseInstancing = true; //PE: Need to exclude animated/cpp/ profile wematerial!= entityele wematerial.
			if (bNextObjectMustBeClone) bUseInstancing = false;
			//if (t.entityprofile[t.tentid].ismarker != 0 || t.entityprofile[t.tentid].cpuanims != 0 || t.entityprofile[t.gridentity].isebe != 0 ) bUseInstancing = false;
			if (t.entityprofile[t.tentid].ismarker != 0 || t.entityprofile[t.tentid].cpuanims != 0 || t.entityprofile[t.tentid].isebe != 0) bUseInstancing = false;
			if (t.entityprofile[t.tentid].ischaractercreator == 1) bUseInstancing = false;
			if (t.entityprofile[t.tentid].animmax > 0)  bUseInstancing = false;

			//if (memcmp(&t.entityprofile[t.tentid].WEMaterial,&t.entityelement[t.tupdatee].eleprof.WEMaterial,sizeof(WickedMaterial)) != 0) bUseInstancing = false;

			extern int active_tools_obj;
			extern int active_tools_entity_index;
			extern sObject* g_selected_editor_object;
			//PE: Cursor always real clone , if changing materials ...
			//g_selected_editor_object == g_ObjectList[t.obj]
			if (t.widget.pickedEntityIndex == t.tupdatee || t.obj == 70000 || t.gridentityobj == t.obj || active_tools_obj == t.obj || active_tools_entity_index == t.tupdatee || t.tentitytoselect == t.tupdatee)
			{
				bUseInstancing = false;
			}
			else if(bUseInstancing)
			{
				if (t.entityelement[t.tupdatee].eleprof.WEMaterial.MaterialActive)
				{
					sObject* pObject = g_ObjectList[t.sourceobj];
					if (pObject && pObject->iMeshCount < MAXMESHMATERIALS) //PE: Max 100 custom materials available.
					{
						entityeleproftype mastereleprof;
						//PE: Create a mastereleprof from master object, just like it would look in entityelement[t.tupdatee].eleprof.
						//PE: We need to do this as entity_prepareobj is called after the cloneobj is actually created.
						//PE: This will make that many more object is included in instancing.
						Wicked_Copy_Material_To_Grideleprof((void*)pObject, 0, &mastereleprof);
						WickedMaterial *Master_WEMaterial = NULL;
						//Master_WEMaterial = &t.entityprofile[t.tentid].WEMaterial;
						Master_WEMaterial = &mastereleprof.WEMaterial;
						DWORD dwMeshCount = pObject->iMeshCount;
						for (int iMesh = 0; iMesh < (int)dwMeshCount; iMesh++)
						{
							//PE: Surface is different , and could match entityprofile only ?
							if (Master_WEMaterial->surfaceMapName[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.surfaceMapName[iMesh] &&
								t.entityprofile[t.tentid].WEMaterial.surfaceMapName[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.surfaceMapName[iMesh])
							{
								bUseInstancing = false;
								break;
							}

							if (Master_WEMaterial->baseColorMapName[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.baseColorMapName[iMesh] ||
								Master_WEMaterial->normalMapName[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.normalMapName[iMesh])
							{
								bUseInstancing = false;
								break;
							}
							if (Master_WEMaterial->displacementMapName[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.displacementMapName[iMesh] ||
								Master_WEMaterial->emissiveMapName[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.emissiveMapName[iMesh])
							{
								bUseInstancing = false;
								break;
							}

							//PE: bDoubleSided different first set later. so check both.
							if (Master_WEMaterial->bDoubleSided[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.bDoubleSided[iMesh] &&
								t.entityprofile[t.tentid].WEMaterial.bDoubleSided[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.bDoubleSided[iMesh] )
							{
								bUseInstancing = false;
								break;
							}

							if (Master_WEMaterial->bTransparency[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.bTransparency[iMesh] ||
								Master_WEMaterial->fRenderOrderBias[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fRenderOrderBias[iMesh] ||
								Master_WEMaterial->bPlanerReflection[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.bPlanerReflection[iMesh] ||
								Master_WEMaterial->bCastShadows[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.bCastShadows[iMesh] ||
								Master_WEMaterial->fReflectance[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fReflectance[iMesh])
							{
								bUseInstancing = false;
								break;
							}

							//PE: fMetallness different first set later. so check both.
							if (Master_WEMaterial->fMetallness[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fMetallness[iMesh] &&
								t.entityprofile[t.tentid].WEMaterial.fMetallness[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fMetallness[iMesh])
							{
								bUseInstancing = false;
								break;
							}
							//PE: fRoughness different first set later. so check both.
							if (Master_WEMaterial->fRoughness[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fRoughness[iMesh] &&
								t.entityprofile[t.tentid].WEMaterial.fRoughness[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fRoughness[iMesh])
							{
								bUseInstancing = false;
								break;
							}

							if (
								Master_WEMaterial->dwBaseColor[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.dwBaseColor[iMesh] ||
								Master_WEMaterial->dwEmmisiveColor[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.dwEmmisiveColor[iMesh] ||
								Master_WEMaterial->fNormal[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fNormal[iMesh] ||
								Master_WEMaterial->fAlphaRef[iMesh] != t.entityelement[t.tupdatee].eleprof.WEMaterial.fAlphaRef[iMesh])
							{
								bUseInstancing = false;
								break;
							}

						}
					}
					else
					{
						bUseInstancing = false;
					}
				}
			}
		}

		#endif

		if ( bCreateAsClone == true )
		{
			CloneObject (  t.obj,t.sourceobj,1 );
			if (  t.tupdatee != -1  )  t.entityelement[t.tupdatee].isclone = 1;
		}
		else
		{
			InstanceObject (  t.obj,t.sourceobj );
			if (  t.tupdatee != -1  )  t.entityelement[t.tupdatee].isclone = 0;
		}
		#ifdef WICKEDENGINE
		iUseMasterObjectID = 0;
		bUseInstancing = false;
		WickedSetEntityId(-1);
		WickedSetElementId(0);
		#endif

		//LB: incorporate overrideanimset into object creation step (during editing/loading/etc)
		if (t.obj > 0)
		{
			LPSTR pOverrideAnimSet = t.entityelement[t.tupdatee].eleprof.overrideanimset_s.Get();
			if (strlen(pOverrideAnimSet) > 1) // "" = default to weapon type, "-" = default to object anim
			{
				// replace actual object animations
				sObject* pObject = GetObjectData(t.obj);
				AppendObject(pOverrideAnimSet, t.obj, 0);
				WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);
			}
		}

		#ifdef WICKEDENGINE
		if (t.entityprofile[t.tentid].islightmarker == 1)
		{
			sObject* pObject = g_ObjectList[t.obj];
			if (pObject)
				WickedCall_SetObjectCastShadows(pObject, false);
			t.entityprofile[t.tentid].castshadow = -1;
		}
		#endif
		//  restore any radius settings the original object might have had
		SetSphereRadius (  t.obj,-1 );

		//  ensure new object ONLY interacts with main camera and shadow camera
		//  (until postprocess masks kick in)
		if (  t.entityprofile[t.tentid].ismarker != 0 ) 
		{
			SetObjectMask (  t.obj,1 );
		}
		else
		{
			SetObjectMask (  t.obj,1+(1<<31) );
		}

		//  initially prep any objects with animation
		if (  GetNumberOfFrames(t.obj)>0 ) 
		{
			SetObjectFrame (  t.obj,0  ); LoopObject (  t.obj  ); StopObject (  t.obj );
		}

		//  allow first animation
		if (t.entityprofile[t.tentid].startanimingame > 0 && t.entityprofile[t.tentid].animmax>0 ) { //PE:

			t.q = t.entityprofile[t.tentid].startanimingame - 1;
			LoopObject(t.sourceobj, t.entityanim[t.tentid][t.q].start, t.entityanim[t.tentid][t.q].finish);
			if (GetNumberOfFrames(t.obj) > 0)
			{
				LoopObject(t.obj, t.entityanim[t.tentid][t.q].start, t.entityanim[t.tentid][t.q].finish);
				StopObject(t.obj);
			}
			StopObject(t.sourceobj);
		}
		else if (  t.entityprofile[t.tentid].animmax>0 && t.entityprofile[t.tentid].playanimineditor>0 ) 
		{
			// animation chosen
			t.q=t.entityprofile[t.tentid].playanimineditor-1;

			// play through "parent object" (ONE OFF)
			LoopObject ( t.sourceobj, t.entityanim[t.tentid][t.q].start,t.entityanim[t.tentid][t.q].finish );

			// 060217 - and clone object if so
			if ( GetNumberOfFrames(t.obj) > 0 ) 
			{
				LoopObject ( t.obj, t.entityanim[t.tentid][t.q].start, t.entityanim[t.tentid][t.q].finish );
			}
		}
		else if (t.entityprofile[t.tentid].playanimineditor < 0)
		{
			// uses name instead of index, the negative is the ordinal into the animset
			extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
			entity_loop_using_negative_playanimineditor(t.tupdatee, t.obj, t.entityprofile[t.tentid].playanimineditor_name);
		}

		//  SetObject (  properties )
		t.tobj=t.obj ; t.tte=t.tupdatee ; entity_prepareobj ( );

		// prepare correct depth mode
		entity_preparedepth(t.tentid, t.tobj);

		//  check if a character creator entity
		#ifdef VRTECH
		#else
		if (  t.entityprofile[t.tentid].ischaractercreator  ==  1 ) 
		{
			t.tccSetTentid = 1;
			characterkit_addEntityToMap ( );
		}
		#endif

		if (  t.tupdatee != -1  ) 
		{
			if ( t.entityprofile[t.tentid].addhandlelimb == 0 )
			{
				// 301115 - override parent LOD distance with LODModifier
				entity_calculateentityLODdistances ( t.tentid, t.tobj, t.entityelement[t.tupdatee].eleprof.lodmodifier );
			}
		}
	}
	else
	{
		//  debug sphere when object not found
		MakeObjectCube (  t.obj,25 );
		SetObjectCollisionOff (  t.obj );
		SetAlphaMappingOn (  t.obj,100 );
	}
}

void entity_updatelightobjtype (int obj, int spotlighting)
{
	if (obj > 0)
	{
		if (LimbExist(obj, 3) == 1)
		{
			if (spotlighting == 0)
			{
				// point light
				ShowLimb(obj, 0);
				ShowLimb(obj, 1);
				ShowLimb(obj, 2);
				HideLimb(obj, 3);
			}
			else
			{
				// spot light
				HideLimb(obj, 0);
				HideLimb(obj, 1);
				HideLimb(obj, 2);
				ShowLimb(obj, 3);
			}
		}
	}
}

void entity_updatelightobj ( int e, int obj )
{
	// adjusts object to show correct shape and color for this light
	if (e > 0 && obj > 0)
	{
		int entid = t.entityelement[e].bankindex;
		//PE: While we have it as a cursor object, entid = 0 so use t.gridentity.
		if (entid == 0 && t.refreshgrideditcursor == 1 && e < t.entityelement.size() && t.gridentity > 0)
			entid = t.gridentity;

		if (entid > 0 && t.entityprofile[entid].ismarker == 2)
		{
			// change color based on color
			SetObjectDiffuse(obj, t.entityelement[e].eleprof.light.color);
			SetObjectEmissive(obj, Rgb(0, 0, 0));

			// change shape based on type
			int spotlighting = t.entityelement[e].eleprof.usespotlighting;
			entity_updatelightobjtype(obj, spotlighting);
		}
	}
}

void entity_preparedepth( int entid, int obj)
{
	if (t.entityprofile[entid].zdepth == 0)
	{
		// simply clears zdepth but keeps layer
		DisableObjectZDepthEx (obj, 1);
	}
	else
	{
		if (t.entityprofile[entid].zdepth == 2)
		{
			// new zdepth mode which DOES move the render order (same render order as weapons so cannot penetrate geometry)
			DisableObjectZDepthEx (obj, 0);
		}
		else
		{
			// normal zdepth handling
			EnableObjectZDepth (obj);
		}
	}
}

int iInstancedTotal = 0;
void entity_prepareobj ( void )
{
	//  takes tte, tobj and tentid
	//  called after entity object clone or instance created (during _entity_createobj and also _entity_converttoclone)
	if (  ObjectExist(t.tobj) == 1 ) 
	{
		//  ensure new object ONLY interacts with main camera and shadow camera
		//  (until postprocess masks kick in)
		if (  t.entityprofile[t.tentid].ismarker != 0 ) 
		{
			SetObjectMask (  t.tobj,1 );
		}
		else
		{
			SetObjectMask (  t.tobj,1+(1<<31) );
		}

		// specific object mask settings
		if ( t.tte > 0 ) visuals_updatespecificobjectmasks ( t.tte, t.tobj );

		//  object properties
		if ( t.entityprofile[t.tentid].ismarker != 0 && t.entityprofile[t.tentid].ismarker != 11 )
		{
			#ifdef WICKEDENGINE
			if (t.entityprofile[t.tentid].ismarker == 2)
			{
				SetObjectCull(t.tobj, 1);
			}
			#else
			//  special setup for marker objects
			if ( t.entityprofile[t.tentid].ismarker != 2 )
			{
				// 160616 - but only if not light markers which are now real 3D models for better grabbing
				SetObjectTransparency ( t.tobj,2 );
				SetObjectCull ( t.tobj,1 );
				DisableObjectZDepth ( t.tobj );
				DisableObjectZWrite ( t.tobj );
				DisableObjectZRead ( t.tobj );
			}
			#endif
		}
		else
		{
			if (t.entityprofile[t.tentid].cullmode >= 0)
			{
				#ifdef WICKEDENGINE
				// For Wicked, cull mode controlled per-mesh with parent default as normal
				//PE: Prefer WEMaterial over old cullmode
				bool bUseWEMaterial = false;
				if (t.entityprofile[t.tentid].WEMaterial.MaterialActive)
				{
					WickedSetEntityId(t.tentid);
					if(t.tte > 0)
						WickedSetElementId(t.tte);
					else
						WickedSetElementId(0);
					sObject* pObject = g_ObjectList[t.tobj];
					if (pObject)
					{
						bUseWEMaterial = true;
						for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
						{
							sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
							if (pMesh)
							{
								// set properties of mesh
								WickedSetMeshNumber(iMeshIndex);
								bool bDoubleSided = WickedDoubleSided();
								if (bDoubleSided)
								{
									pMesh->bCull = false;
									pMesh->iCullMode = 0;
									WickedCall_SetMeshCullmode(pMesh);
								}
								else
								{
									pMesh->iCullMode = 1;
									pMesh->bCull = true;
									WickedCall_SetMeshCullmode(pMesh);
								}
							}
						}
					}
					WickedSetEntityId(-1);
					WickedSetElementId(0);
				}

				if (!bUseWEMaterial)
					SetObjectCull(t.tobj, 1);
				#else
				if (t.entityprofile[t.tentid].cullmode != 0)
				{
					SetObjectCull(t.tobj, 0);
				}
				else
				{
					SetObjectCull(t.tobj, 1);
				}
				#endif
			}
		}

		//  object animation
		entity_resettodefaultanimation ( );

		//  object rotation and scale
		if (  t.entityprofile[t.tentid].fixnewy != 0 ) 
		{
			RotateObject (  t.tobj,0,t.entityprofile[t.tentid].fixnewy,0 );
			FixObjectPivot (  t.tobj );
		}
		if (  t.entityprofile[t.tentid].scale != 0  )  ScaleObject (  t.tobj,t.entityprofile[t.tentid].scale,t.entityprofile[t.tentid].scale,t.entityprofile[t.tentid].scale );

		// 091115 - after scaling, ensure LOD is a reflection of overall object size (so LARGE buildings not instantly go to LOD2)
		if (  t.entityprofile[t.tentid].ismarker == 0 ) 
		{
			// 051115 - only if not using limb visibility for hiding decal arrow
			if ( t.entityprofile[t.tentid].addhandlelimb==0 )
			{
				//  set LOD levels for object
				entity_calculateentityLODdistances ( t.tentid, t.tobj, 0 );
			}
		}

		// no collision and full alpha multiplier
		SetObjectCollisionOff ( t.tobj );
		#ifdef WICKEDENGINE
		if (t.entityelement[t.tte].eleprof.WEMaterial.MaterialActive)
		{
			WickedSetEntityId(t.tentid);
			WickedSetElementId(t.tte);
		}
		if (!t.entityprofile[t.tentid].bIsDecal)
			SetAlphaMappingOn(t.tobj, 100);
		WickedSetEntityId(-1);
		WickedSetElementId(0);
		#else
		SetAlphaMappingOn(t.tobj, 100);
		#endif

		// set transparency mode (after 'set alpha mapping on' as it messes with transparency flag)
		if ( t.entityprofile[t.tentid].ismarker == 0 ) 
		{
			//PE: Wicked material can overwrite objects settings.
			#ifdef WICKEDENGINE
			//LB: always prepare object with TextureMesh!
			if (1)//t.entityelement[t.tte].eleprof.WEMaterial.MaterialActive) 
			{
				// LB: need to restore ALL WEMaterial settings here when preparing the object
				WickedSetEntityId(t.tentid);
				WickedSetElementId(t.tte);
				// LB: apply WEMaterial to all meshes of this object, not just the first one
				// LB: Setting object transparency defaults here (so not everything is transparent), but the TextureMesh can then set per-mesh transparency :)
				SetObjectTransparency(t.tobj, t.entityelement[t.tte].eleprof.WEMaterial.bTransparency[0]);
				sObject* pObject = g_ObjectList[t.tobj];
				for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
				{
					sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
					if (pMesh)
					{
						// set properties of mesh
						WickedSetMeshNumber(iMeshIndex);

						// sets ALL properties of each mesh from WEMaterial
						if (pMesh->bInstanced && pMesh->wickedmaterialindex == 0 && pMesh->master_wickedmaterialindex > 0 )
						{
							//PE: No need to texture Instanced objects.
							iInstancedTotal++;
						}
						else
						{
							// from WE materials
							WickedCall_TextureMesh(pMesh);

							// and must restore mesh transparency flag
							bool bTransparent = WickedGetTransparent();
							pMesh->bTransparency = bTransparent;
						}
					}
				}
				WickedSetEntityId(-1);
				WickedSetElementId(0);
			}
			else
			#endif
			{
				#ifdef WICKEDENGINE
				WickedSetEntityId(t.tentid);
				WickedSetElementId(t.tte);
				SetObjectTransparency(t.tobj, t.entityprofile[t.tentid].transparency);
				WickedSetEntityId(-1);
				WickedSetElementId(0);
				#else
				SetObjectTransparency(t.tobj, t.entityprofile[t.tentid].transparency);
				#endif
				if (t.entityprofile[t.tentid].transparency >= 2)
				{
					//  set Z write for transparency mode 2 and above
					//Dave - commented this out because otherwise grass is drawn over things like gates
					//DisableObjectZWrite (  t.tobj );
				}
			}
		}
		else
		{
			#ifdef WICKEDENGINE
			sObject* pObject = g_ObjectList[t.tobj];
			if (pObject)
			{
				WickedCall_TextureObject(pObject, NULL);
			}
			#endif
		}

		// handle zdepth mode of this entity
		entity_preparedepth(t.tentid, t.tobj);

		// 281116 - set specular (new internal property that ties to 'SpecularOverride' shader constant)
		// BUT we will use TTE so can get the per entity ELEPROF state when creating object!
		// also calls this in lightmapper just after cloned parent obj 9see Lightmaping.cpp)
		if ( t.entityprofile[t.tentid].ismarker == 0 && t.tte > 0 )
		{
			SetObjectSpecularPower ( t.tobj, t.entityelement[t.tte].eleprof.specularperc / 100.0f );
		}


		// apply the scrolls cale uv data values for the shader use later on
		if ( t.entityprofile[t.tentid].uvscrollu != 0.0f 
		||   t.entityprofile[t.tentid].uvscrollv != 0.0f 
		||   t.entityprofile[t.tentid].uvscaleu != 1.0f 
		||   t.entityprofile[t.tentid].uvscalev != 1.0f )
		{
			SetObjectScrollScaleUV ( t.tobj, t.entityprofile[t.tentid].uvscrollu, t.entityprofile[t.tentid].uvscrollv, t.entityprofile[t.tentid].uvscaleu, t.entityprofile[t.tentid].uvscalev );
		}

		// Set art flags for object (can use 32 bit flags here eventually)
		DWORD dwArtFlags = 0;
		if ( t.entityprofile[t.tentid].invertnormal == 1 ) dwArtFlags = 1;
		if ( t.entityprofile[t.tentid].preservetangents == 1 ) dwArtFlags |= 1<<1;
		SetObjectArtFlags ( t.tobj, dwArtFlags, 0.0f );

		#ifdef WICKEDENGINE
		//PE: Emulate old classic shaders, using settings.
		cstr sEffectLower = Lower(t.entityprofile[t.tentid].effect_s.Get());
		if (sEffectLower == "effectbank\\reloaded\\decal_animate1_additive.fx")
		{
			//PE: AvengingEagle's Light Effects.
			DisableObjectZWrite(t.tobj); //Additive blending.
		}

		if (t.entityprofile[t.tentid].bIsDecal)
		{

			void SetupDecalObject(int obj, int elementID);
			SetupDecalObject(t.tobj, t.tte);
		}

		#endif

		#ifdef WICKEDENGINE
		if (t.entityprofile[t.tentid].ismarker == 0)
		{
			int iAutoFlattenMode = t.entityprofile[t.tentid].autoflatten;
			if(iAutoFlattenMode != 0)
			{
				if (t.entityelement[t.tte].eleprof.iFlattenID != -1)
					entity_updateautoflatten(t.tte, t.tobj);
				else
					entity_autoFlattenWhenAdded(t.tte, t.tobj);
			}
		}
		if (t.entityprofile[t.tentid].ismarker == 2)
		{
			entity_updatelightobj(t.tte, t.tobj);
		}
		//PE: Start any particle effects.
		if (t.entityprofile[t.tentid].ismarker == 10)
		{
			if(t.entityelement[t.tte].obj == 0 )
				t.entityelement[t.tte].obj = t.tobj;
			entity_updateparticleemitter(t.tte);
		}
		#endif
	}
}

void entity_calculateentityLODdistances ( int tentid, int tobj, int iModifier )
{
	float fLODModifier = (100+iModifier)/100.0f;
	if ( t.entityprofile[tentid].lod1distance==0 )
	{
		// default LOD distances a product of scale of object
		float fRelativeScale = ObjectSize ( tobj, 1 ) / 100.0f;
		SetObjectLOD ( tobj, 1, 400.0f * fRelativeScale * fLODModifier );
		SetObjectLOD ( tobj, 2, 800.0f * fRelativeScale * fLODModifier );
	}
	else
	{
		// otherwise its specified by the FPE
		SetObjectLOD ( tobj, 1, (float)t.entityprofile[tentid].lod1distance * fLODModifier );
		SetObjectLOD ( tobj, 2, (float)t.entityprofile[tentid].lod2distance * fLODModifier );
	}
}

void entity_setupcharobjsettings ( void )
{
	// unique extra setup for character objects, takes charanimstate, obj
	if ( t.obj>0 ) 
	{
		if ( ObjectExist(t.obj) == 1 ) 
		{
			PositionObject ( t.obj,t.entityelement[t.charanimstate.e].x,t.entityelement[t.charanimstate.e].y,t.entityelement[t.charanimstate.e].z );
			RotateObject ( t.obj,t.entityelement[t.charanimstate.e].rx,t.entityelement[t.charanimstate.e].ry,t.entityelement[t.charanimstate.e].rz );
			ScaleObject ( t.obj,100+t.entityelement[t.charanimstate.e].scalex,100+t.entityelement[t.charanimstate.e].scaley,100+t.entityelement[t.charanimstate.e].scalez );
		}
	}
}

void entity_resettodefaultanimation ( void )
{
	// takes tte, tobj, tentid
	if ( t.tte != -1 ) 
	{
		bool isWicked = false;
		#ifdef WICKEDENGINE
		//PE: Wicked is always clone.
		isWicked = true;
		#endif
		if ( t.entityelement[t.tte].isclone == 1 || isWicked )
		{
			// CLONE
			if ( GetNumberOfFrames(t.tobj)>0 ) 
			{
				SetObjectFrame ( t.tobj, 0 ); 
				LoopObject ( t.tobj ); 
				StopObject ( t.tobj );
			}
			if ( t.entityelement[t.tte].staticflag == 1 ) 
			{
				// do not animate if marked as static
				#ifdef WICKEDENGINE
				//PE: Allow static animation in wicked editor. if set in fpe.
				if (t.entityprofile[t.tentid].animmax > 0 && t.entityprofile[t.tentid].playanimineditor > 0)
				{
					t.q = t.entityprofile[t.tentid].playanimineditor - 1;
					LoopObject(t.tobj, t.entityanim[t.tentid][t.q].start, t.entityanim[t.tentid][t.q].finish);
				}
				else if (t.entityprofile[t.tentid].playanimineditor < 0)
				{
					// uses name instead of index, the negative is the ordinal into the animset
					extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
					entity_loop_using_negative_playanimineditor(t.tte, t.tobj, t.entityprofile[t.tentid].playanimineditor_name);
				}
				#endif
			}
			else
			{		
				if ( t.entityprofile[t.tentid].animmax>0 && t.entityprofile[t.tentid].playanimineditor>0 ) 
				{
					t.q=t.entityprofile[t.tentid].playanimineditor-1;
					LoopObject ( t.tobj,t.entityanim[t.tentid][t.q].start,t.entityanim[t.tentid][t.q].finish );
				}
				else if (t.entityprofile[t.tentid].startanimingame > 0 && t.entityprofile[t.tentid].animmax > 0)
				{
					t.q = t.entityprofile[t.tentid].startanimingame - 1;
					LoopObject(t.tobj, t.entityanim[t.tentid][t.q].start, t.entityanim[t.tentid][t.q].finish);
					StopObject(t.tobj);
				}
				else if (t.entityprofile[t.tentid].playanimineditor < 0)
				{
					// uses name instead of index, the negative is the ordinal into the animset
					extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
					entity_loop_using_negative_playanimineditor(t.tte, t.tobj, t.entityprofile[t.tentid].playanimineditor_name);
				}
			}
		}
		else
		{
			// INSTANCE - no self-animation
		}
	}
}

void entity_positionandscale ( void )
{
	// takes tobj,tte,tentid
	PositionObject (  t.tobj,t.entityelement[t.tte].x,t.entityelement[t.tte].y,t.entityelement[t.tte].z );
	RotateObject (  t.tobj,t.entityelement[t.tte].rx,t.entityelement[t.tte].ry,t.entityelement[t.tte].rz );
	ScaleObject (  t.tobj,100+t.entityelement[t.tte].scalex,100+t.entityelement[t.tte].scaley,100+t.entityelement[t.tte].scalez );
	//PE: Particle scale bug fix.
	//if (t.entityprofile[t.entityelement[t.tte].bankindex].ismarker == 10)
	//{
	//	float fScale = t.entityprofile[t.entityelement[t.tte].bankindex].scale;
	//	ScaleObject (t.tobj, fScale, fScale, fScale);
	//}
	//LB: object is rotated AFTER being created, so ensure particle knows rotation of entity
	if (t.entityprofile[t.entityelement[t.tte].bankindex].ismarker == 10)
		entity_updateparticleemitter(t.tte);

	ShowObject (  t.tobj );

	// ensure all transparent static objects are removed from 'intersect all' consideration
	t.tokay=0;
	if (  t.entityelement[t.tte].staticflag == 1 ) 
	{
		if (  t.entityprofile[t.entityelement[t.tte].bankindex].canseethrough == 1 ) 
		{
			t.tokay=1;
		}
	}
	if (  t.entityprofile[t.entityelement[t.tte].bankindex].ischaracter == 0 ) 
	{
		if (  t.entityprofile[t.entityelement[t.tte].bankindex].collisionmode == 11  )  t.tokay = 1;
	}
	if (  t.tokay == 1 ) 
	{
		SetObjectCollisionProperty (  t.tobj,1 );
	}
	return;
}

#ifdef WICKEDENGINE

#define STOPPROBELIGHTLEAK

XMFLOAT3 Hit[6];
float RayAroundObject(int obj,float x, float y, float z)
{
	if (obj <= 0.0) return 0.0f;
	if (!ObjectExist(obj)) return 0.0f;

	sObject* pObject = g_ObjectList[obj];
	if (!pObject) return 0.0f;
	
	int iVisible = GetVisible(obj);
	HideObject(obj);

	XMFLOAT3 ObjectCenter = { 0,0,0 }; //GetRealCenterToGridEntity();
	XMFLOAT3 ObjectSize = { 0,0,0 };
	XMFLOAT3 fObjCenter = { x,y,z };
	float fLowetDist = 99999.0f;
	Hit[0] = { 0,0,0 }; // <-  
	Hit[1] = { 0,0,0 }; // /|\  
	Hit[2] = { 0,0,0 }; // ->  
	Hit[3] = { 0,0,0 }; // \|/  
	Hit[4] = { 0,0,0 }; //  / 
	Hit[5] = { 0,0,0 }; //  \  

	for (int i = 0; i < 6; i++)
	{
		if (i == 0)
		{
			ObjectSize.x = (pObject->collision.vecMin.x - pObject->collision.vecMax.x) * 0.5;  //offset from center.
			ObjectSize.y = pObject->collision.vecMin.y;
			ObjectSize.z = 0.0f;
		}
		else if (i == 1)
		{
			ObjectSize.x = 0.0f;
			ObjectSize.y = pObject->collision.vecMin.y;
			ObjectSize.z = (pObject->collision.vecMin.z - pObject->collision.vecMax.z) * 0.5; //offset from center.
		}
		else if (i == 2)
		{
			ObjectSize.x = fabs((pObject->collision.vecMin.x - pObject->collision.vecMax.x) * 0.5);  //offset from center.
			ObjectSize.y = pObject->collision.vecMin.y;
			ObjectSize.z = 0.0f;
		}
		else if (i == 3)
		{
			ObjectSize.x = 0.0f;
			ObjectSize.y = pObject->collision.vecMin.y;
			ObjectSize.z = fabs((pObject->collision.vecMin.z - pObject->collision.vecMax.z) * 0.5); //offset from center.
		}
		else if (i == 4)
		{
			ObjectSize.x = (pObject->collision.vecMin.x - pObject->collision.vecMax.x) * 0.5;  //offset from center.
			ObjectSize.y = pObject->collision.vecMin.y;
			ObjectSize.z = (pObject->collision.vecMin.z - pObject->collision.vecMax.z) * 0.5; //offset from center.
		}
		else if (i == 5)
		{
			ObjectSize.x = fabs((pObject->collision.vecMin.x - pObject->collision.vecMax.x) * 0.5);  //offset from center.
			ObjectSize.y = pObject->collision.vecMin.y;
			ObjectSize.z = fabs((pObject->collision.vecMin.z - pObject->collision.vecMax.z) * 0.5); //offset from center.
		}

		XMFLOAT3 fObjPos = { x + ObjectCenter.x + (ObjectSize.x) , y + ObjectCenter.y ,z + ObjectCenter.z + (ObjectSize.z) };
		XMFLOAT3 fObjRayPos = { x + ObjectCenter.x - (ObjectSize.x) , y + ObjectCenter.y ,z + ObjectCenter.z - (ObjectSize.z) };
		XMVECTOR vectorSub = XMVectorSubtract(XMLoadFloat3(&fObjPos), XMLoadFloat3(&fObjCenter));
		XMVECTOR rayDirection = XMVector3Normalize(vectorSub);
		XMFLOAT3 f3Dir;
		XMStoreFloat3(&f3Dir, rayDirection);
		float fHitX, fHitY, fHitZ, fdist = 99999.0;
		if (WickedCall_SentRay(fObjRayPos.x, fObjRayPos.y, fObjRayPos.z, f3Dir.x, 0.0f, f3Dir.z, &fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, NULL, GGRENDERLAYERS_NORMAL))
		{
			XMFLOAT3 fObjHit = { fHitX, fHitY, fHitZ };

			float fdist = sqrt((fObjPos.x - fObjHit.x) * (fObjPos.x - fObjHit.x) +
				(fObjPos.z - fObjHit.z) * (fObjPos.z - fObjHit.z));
			if (fdist < fLowetDist)
				fLowetDist = fdist;

			Hit[i] = fObjHit;
		}
	}
	if (iVisible) ShowObject(obj);
	if (fLowetDist < 99990.0f)
		return fLowetDist;
	return(0.0f);
}

void entity_placeprobe(int obj, float fLightProbeScale)
{
	return; // don't place static probes 

	// NOTE: Can only have 16 probes before lighting goes nuts - find out why!
	if (obj <= 0) return;
	sObject* pObject = g_ObjectList[obj];

	// work out object center
	XMFLOAT3 fObjCenter;
	if (pObject)
	{
		// position of probe relative to light object (nice if we could HIDE the light when env maps being created (for editor))
		float fShiftDownToMissLightObject = -20.0f;
		fObjCenter = { pObject->position.vecPosition.x, pObject->position.vecPosition.y + fShiftDownToMissLightObject, pObject->position.vecPosition.z };
	}

	float fSize = 180.0f;

	//PE: Probe create light leak, when they are bigger then a room and reach to the other side.
	//PE: try to raycast around light to find sides and center at that point.
	#ifdef STOPPROBELIGHTLEAK
	if (pObject)
	{
		fSize = RayAroundObject(obj, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z);
		if (fSize > 0)
		{
			//Try a new center.
			if (Hit[0].x > 0 && Hit[2].x > 0)
			{
				XMVECTOR vectorSub = XMVectorSubtract(XMLoadFloat3(&Hit[2]), XMLoadFloat3(&Hit[0]));
				XMFLOAT3 fObjOffset;
				fObjOffset.x = XMVectorGetX(vectorSub) * 0.5;
				fObjOffset.y = XMVectorGetY(vectorSub) * 0.5;
				fObjOffset.z = XMVectorGetZ(vectorSub) * 0.5;

				float fSize2 = RayAroundObject(obj, Hit[0].x + fObjOffset.x, pObject->position.vecPosition.y, Hit[0].z-fObjOffset.z);
				if (fSize2 > 0 && fSize2 > fSize)
				{
					fSize = fSize2;
					fObjCenter = { Hit[0].x + fObjOffset.x, pObject->position.vecPosition.y, Hit[0].z - fObjOffset.z };
				}
			}
			if (Hit[1].z > 0 && Hit[3].z > 0)
			{
				XMVECTOR vectorSub = XMVectorSubtract(XMLoadFloat3(&Hit[3]), XMLoadFloat3(&Hit[1]));
				XMFLOAT3 fObjOffset;
				fObjOffset.x = XMVectorGetX(vectorSub) * 0.5;
				fObjOffset.y = XMVectorGetY(vectorSub) * 0.5;
				fObjOffset.z = XMVectorGetZ(vectorSub) * 0.5;

				float fSize2 = RayAroundObject(obj, Hit[0].x + fObjOffset.x, pObject->position.vecPosition.y, Hit[0].z - fObjOffset.z);
				if (fSize2 > 0 && fSize2 > fSize)
				{
					fSize = fSize2;
					fObjCenter = { Hit[0].x + fObjOffset.x, pObject->position.vecPosition.y, Hit[0].z - fObjOffset.z };
				}
			}

			if (Hit[3].z > 0 && Hit[1].z == 0)
			{
				//Try moving away from the wall and see if we get better results.
				XMFLOAT3 fObjOffset;
				fObjOffset.x = 0;
				fObjOffset.y = 0;
				fObjOffset.z = 30;
				float fSize2 = RayAroundObject(obj, fObjCenter.x + fObjOffset.x, pObject->position.vecPosition.y, fObjCenter.z - fObjOffset.z);
				if (fSize2 > 0 && fSize2 > fSize)
				{
					fSize = fSize2;
					fObjCenter = { fObjCenter.x + fObjOffset.x, pObject->position.vecPosition.y, fObjCenter.z - fObjOffset.z };
				}
				fObjOffset.x = 0;
				fObjOffset.y = 0;
				fObjOffset.z = 60.0f; //Try fixed 100 offset.
				fSize2 = RayAroundObject(obj, fObjCenter.x + fObjOffset.x, pObject->position.vecPosition.y, fObjCenter.z - fObjOffset.z);
				if (fSize2 > 0 && fSize2 > fSize)
				{
					fSize = fSize2;
					fObjCenter = { fObjCenter.x + fObjOffset.x, pObject->position.vecPosition.y, fObjCenter.z - fObjOffset.z };
				}
			}
		}
		//PE: If walls are rotate around probe, we cant currently rotate the box , so tak out a little.
		if (fSize > 50.0) fSize -= 7.0;
		if (fSize <= 0 || fSize > 180.0f)
		{
			fSize = 180.0f;
		}
	}
	#endif

	if (pObject)
	{
		// LB: modulate size of prove with scale passed in (controlled in light properties)
		fSize *= fLightProbeScale;

		//PE: Add remove if already exists.
		cStr name = cStr(obj);
		WickedCall_CreateReflectionProbe(fObjCenter.x, fObjCenter.y, fObjCenter.z, name.Get(), fSize);
	}
}

void entity_deleteprobe(int obj)
{
	if (obj <= 0) return;
	sObject* pObject = g_ObjectList[obj];
	if (pObject)
	{
		cStr name = cStr(obj);
		WickedCall_DeleteReflectionProbe(name.Get());
	}
}
#endif

void entity_updateentityobj ( void )
{
	//  special mode which intercepts non-static entities and replaces with blanks
	if ( t.lightmapper.onlyloadstaticentitiesduringlightmapper == 1 ) 
	{
		//  eliminate entities that NEVER get lightmapped
		if (  t.entityelement[t.tupdatee].staticflag == 0 ) 
		{
			t.entityelement[t.tupdatee].bankindex=0;
			t.entityelement[t.tupdatee].obj=0;
		}
	}

	// Create/replace/remove olay object to reflect (olayindex,ti)
	t.tentid=t.entityelement[t.tupdatee].bankindex;
	if (  t.tentid != 0 ) 
	{
		t.obj=t.entityelement[t.tupdatee].obj;
		if (  t.obj>0 ) 
		{
			if (  ObjectExist(t.obj) == 1  )  DeleteObject (  t.obj );
			t.entityelement[t.tupdatee].obj=0;

			//LB: this ensures the deleted OBJ ID can be reused immediately so they entity element OBJ ID is the same when refreshed (modifying media while running level)
			g.entityviewcurrentobj = t.obj;
		}
		if (  t.entityelement[t.tupdatee].obj == 0 ) 
		{
			//  find free object
			t.obj=g.entityviewcurrentobj;
			if (  ObjectExist(t.obj) == 1 ) 
			{
				while ( ObjectExist(t.obj)==1 ) ++t.obj;
				g.entityviewcurrentobj=t.obj;
			}
			if (  g.entityviewcurrentobj>g.entityviewendobj ) 
			{
				g.entityviewendobj=g.entityviewcurrentobj;
			}
			g.editorresourcecounterpacer=1;

			//  create object
			entity_createobj ( );
			t.tobj=t.obj ; t.tte=t.tupdatee ; entity_positionandscale ( );
			t.entityelement[t.tupdatee].obj=t.obj;

			// also punch terrain to flatten area where entity is placed
			if ( g.gridlayershowsingle == 0 ) 
			{
				#ifdef WICKEDENGINE
				// auto flatten is dynamic
				#else
				// only when not in INSIDE edit mode
				if ( t.gridedit.autoflatten == 1 && t.entityprofile[t.tentid].ismarker == 0 ) 
				{
					t.terrain.terrainpainteroneshot=1;
					t.terrain.X_f=ObjectPositionX(t.obj);
					t.terrain.Y_f=ObjectPositionZ(t.obj);
					t.terrain.shapeA_f=ObjectAngleY(t.obj);
					t.terrain.shapeWidth_f=ObjectSizeX(t.obj,1);
					t.terrain.shapeLong_f=ObjectSizeZ(t.obj,1);
					t.terrain.shapeHeight_f=ObjectPositionY(t.obj);
				}
				#endif
			}

			/*
			// if entity is a light, has a probe
			#ifdef WICKEDENGINE
			int entid = t.entityelement[t.tupdatee].bankindex;
			if (entid > 0)
			{
				if (t.entityprofile[entid].ismarker == 2)
				{
					float fLightProbeScale = t.entityelement[t.tupdatee].eleprof.light.fLightHasProbe;
					if ( fLightProbeScale > 0 )
						entity_placeprobe(t.entityelement[t.tupdatee].obj, fLightProbeScale);
					else
						entity_deleteprobe(t.entityelement[t.tupdatee].obj);
				}
			}
			#endif
			*/
		}
	}
	if ( t.tentid == 0 ) 
	{
		t.obj=t.entityelement[t.tupdatee].obj;
		if ( t.obj>0 ) 
		{
			if ( ObjectExist(t.obj) == 1 ) DeleteObject ( t.obj );
		}
		t.entityelement[t.tupdatee].obj=0;
	}
}
