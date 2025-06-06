#pragma once
#define BT_EULER_DEFAULT_ZYX

// DX9PLUS Work
#include ".\..\Dark Basic Pro SDK\Shared\data\cdatac.h"
#include ".\..\Dark Basic Pro SDK\Shared\DBOFormat\DBOData.h"
#include "globstruct.h"
#include <stdio.h>
#include <vector>

#undef DARKSDK
#define DARKSDK
#define WIN32_LEAN_AND_MEAN

// Internal functions
void BULLETReceiveCoreDataPtr ( void );
void BULLETDestructor ( void );

// Initialisation commands
DARKSDK void		ODEStart								( void );
DARKSDK void		ODEUpdate								( void );
DARKSDK void		ODEUpdate								( float fManualStep );
DARKSDK void		ODEEnd									( void );
DARKSDK void		ODEFinalizeWorld						( void );
DARKSDK float		ODEProjectActivationWave				( float fDistanceWithinToActivate, float fSlices);
DARKSDK int			ODEFind									(int iID);

// Creation commands
DARKSDK void		ODECreateStaticSphere					( int iObjectNumber );
DARKSDK void		ODECreateStaticBox						( int iObjectNumber );
DARKSDK void		ODECreateStaticBox						( int iObjectNumber, int iLimbNumber );
DARKSDK void		ODECreateStaticCylinder				    ( int iObjectNumber );
DARKSDK void		ODECreateStaticCylinder				    ( int iObjectNumber, float fXPos, float fYPos, float fZPos, float fXSize, float fYSize, float fZSize, float fXRot, float fYRot, float fZRot );
DARKSDK void	    ODECreateTreeCylinder                   ( int iObjectNumber );
DARKSDK void		ODESetMeshFilename						( LPSTR pName );
DARKSDK void		ODESetOBJLoadingFilename				( LPSTR pName );
DARKSDK void		ODECreateStaticTerrainMesh				( int iObjectNumber );
DARKSDK void		ODECreateStaticTriangleMesh			    ( int iObjectNumber );
DARKSDK void		ODECreateStaticTriangleMesh			    ( int iObjectNumber, int iLimbNumber );
DARKSDK void		ODECreateStaticTriangleMesh			    ( int iObjectNumber, int iLimbNumber, int iCollisionScaling );
DARKSDK void        ODECreateStaticTriangleMesh             ( int iObjectNumber, int iLimbNumber, int iCollisionScaling, int iHullReductionMode );
DARKSDK void		ODECreateStaticTerrain					( int iObjectNumber, int iWidth, int iLength, LPSTR dwMemBlockPtr );
DARKSDK void		ODEUpdateStaticTerrain					( int iObjectNumber, int iWidth, int iLength, LPSTR dwMemBlockPtr, int iX1, int iZ1, int iX2, int iZ2 );
DARKSDK void		ODECreateStaticCapsule					( int iObjectNumber, float fCollisionScaling, float fCollisionScalingXZ);
DARKSDK void		ODECreateDynamicSphere					( int iObjectNumber, float fWeight, float fFriction, float fRestitution );
DARKSDK void		ODECreateDynamicBox					    ( int iObjectNumber );
DARKSDK void		ODECreateDynamicBox					    ( int iObjectNumber, int iLimbNumber, int iTipOverMode );
DARKSDK void		ODECreateDynamicBox					    ( int iObjectNumber, int iLimbNumber, int iTipOverMode, float fWeight, float fFriction, float fRestitution );
DARKSDK void		ODECreateDynamicCylinder				( int iObjectNumber, float fWeight, float fFriction, float fRestitution );
DARKSDK void		ODECreateDynamicTriangleMesh			( int iObjectNumber, float fWeight, float fFriction, float fRestitution, int iHullReductionMode );
DARKSDK void		ODECreateDynamicTriangleMesh			( int iObjectNumber, int iLimbNumber, int iCollisionScaling, int iHullReductionMode);
DARKSDK void		ODECreateDynamicCapsule				    ( int iObjectNumber, float fScaleModifier, float fRaised, float fWeight, float fFriction, float fCollisionScaling, float fCollisionScalingXZ);
DARKSDK void		ODEStartStaticObject					( int iObjectNumber );
DARKSDK void		ODEEndStaticObject						( int iObjectNumber, int iTerrainCollider );
DARKSDK void		ODEAddStaticObjectBox					( int iObjectNumber, int iStaticBoxObject, int iMaterialRefIndex );
DARKSDK void        ODECreateDynamicBoxManual               ( int iObjectNumber, float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize, float fXAngl, float fYAng, float fZAng);
DARKSDK int         ODECreateHingeSingle                    ( int iObjectNumber , float fX, float fY, float fZ, int iType, float fAngMin, float fAngMax );
DARKSDK int         ODECreateHingeDouble                    ( int iObjectNumA, int iObjectNumB, float fXa, float fYa, float fZa, float fXb, float fYb, float fZb, int iTypeA, int iTypeB, int noCollision );
DARKSDK int         ODECreateJointSingle                    ( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK int         ODECreateJointDouble                    ( int iObjectNumA, int iObjectNumB, float fXa, float fYa, float fZa, float fXb, float fYb, float fZb, int noCollision );
DARKSDK int         ODECreateSliderDouble                   ( int iObjectNumA, int iObjectNumB, float qXa, float qYa, float qZa, float qWa, float fXa, float fYa, float fZa, 
	                                                                                            float qXb, float qYb, float qZb, float qWb, float fXb, float fYb, float fZb, bool useLinearReferenceFrameA );
DARKSDK void		ODECreateTerrain						( unsigned int iVertexCount, unsigned int iIndexCount, float* pVertices, int* piIndices );

// GGTerrain
DARKSDK void		ODECreateGGTerrain						();

// Character Controller commands
DARKSDK void		ODECreateDynamicCharacterController	    ( int iObjectNumber, float fA, float fB, float fC );
DARKSDK void		ODESetDynamicCharacterController		( int iObjectNumber, float fA, float fA2, float fB, float fC, float fD, float fE, float fF );
DARKSDK void		ODEControlDynamicCharacterController	( int iObjectNumber, float fA, float fA2, float fB, float fC, float fDucking, float fPushAngle, float fPushForce, float fThrustUpwards );
DARKSDK DWORD		ODEGetDynamicCharacterControllerDidJump ( void );
DARKSDK DWORD		ODEGetCharacterControllerDucking		( int iObjectNumber );
DARKSDK int			ODEGetCharacterHitFloor				( void );
DARKSDK float		ODEGetCharacterFallDistance			( void );
DARKSDK void		ODESetWaterLine						( float fWaterLineY );
DARKSDK void		ODESetCharacterDirectionOverride(float angX, float angY, float angZ, float axisX, float axisY, float axisZ, float mag);
DARKSDK void		ODEGetCharacterWalkDirection(GGVECTOR3* out);
DARKSDK void		ODESwimmingLimitVertical(int limit);

// Ragdoll commands
DARKSDK void		BPhys_RagDollBegin(int objectID, float objTotalWeight);
DARKSDK int			BPhys_RagDollGetIDFromBoneObject(int objectID);
DARKSDK int			BPhys_RagDollAddBone(int startLimbID, int endLimbID, float diameter, int collisionGroup, int collisionMask);
DARKSDK int			BPhys_RagDollAddBone2(int startLimbID, int endLimbID, float diameter, float lengthmod, int collisionGroup, int collisionMask);
DARKSDK void		BPhys_RagDollBoneAddLimbID(int boneID, int limbID) ;
DARKSDK void		BPhys_RagDollAddHingeJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec2);
DARKSDK void		BPhys_RagDollAddTwistJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec3);
DARKSDK int			BPhys_RagdollGetBoneObjID(int ragdollID, int boneIndex);
DARKSDK void		BPhys_RagDollEnd();
DARKSDK bool		BPhys_RagDollApplyForce(int ragdollID,int iLimb,float fFX,float fFY,float fFZ,float fTX,float fTY,float fTZ,float fForce);
DARKSDK int			BPhys_RagdollExist(int ragdollID);
DARKSDK int			BPhys_DeleteRagdoll(int ragdollID);
DARKSDK int			BPhys_RagdollIsStatic(int ragdollID);
DARKSDK int			BPhys_RagdollSetStatic(int ragdollID, int isStatic);
DARKSDK int			BPhys_RagDollHideBones(int ragdollID);
DARKSDK int			BPhys_RagDollShowBones(int ragdollID);
DARKSDK void		BPhys_RagDollSetDamping(float linear, float angular);
DARKSDK void		BPhys_RagDollSetSleepingThresholds(float linear, float angular);
DARKSDK void		BPhys_RagDollSetDeactivationTime(float time);

// Destruction commands
DARKSDK void		ODEDestroyObject						( int iObjectNumber );

// Force command
DARKSDK void		ODESetLinearVelocity					( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODESetLinearVelocityXZWithGravity		( int iObjectNumber, float fX, float fZ, float fGravityForce );
DARKSDK void		ODESetLinearVelocityUsingWorldPosTarget	( int iObject, float fWorldX, float fWorldY, float fWorldZ);
DARKSDK void		ODEAddBodyForce							( int iObjectNumber, float fX, float fY, float fZ, float fPX, float fPY, float fPZ );
DARKSDK void		ODESetAngularVelocity					( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODESetAngularFactor						( int iObjectNumber );
DARKSDK void		ODEConstrainBodyMotion                  ( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODEConstrainBodyRotation                ( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODERemoveBodyConstraints                ( int iObjectNumber );
DARKSDK void		ODERemoveConstraint                     ( int iConstraintId );

// Ray commands
DARKSDK int			ODERayTerrainEx						( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType, bool bUpdateDebugObj );
DARKSDK int			ODERayTerrain						( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, bool bUpdateDebugObj);
DARKSDK int			ODERayForce							( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, float fForceValue );
DARKSDK int			ODERayEx							( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType, int iIgnoreObj1, int iIgnoreObj2);
DARKSDK int			ODERay								( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType);
DARKSDK float		ODEGetRayCollisionX					( );
DARKSDK float		ODEGetRayCollisionY					( );
DARKSDK float		ODEGetRayCollisionZ					( );
DARKSDK float		ODEGetRayNormalX					( );
DARKSDK float		ODEGetRayNormalY					( );
DARKSDK float		ODEGetRayNormalZ					( );
DARKSDK int			ODEGetRayObjectHit					( );

// Set commands
DARKSDK void		ODESetActive							( int iObjectNumber, int iMode );
DARKSDK void		ODESetBodyPosition						( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODESetBodyResponse						( int iObjectNumber, int iResponseMode );
DARKSDK void		ODESetBodyAngle							( int iObjectNumber, float fX, float fY, float fZ );
DARKSDK void		ODESetNoGravity                         ( int iObjectNumber, int iGravity );
DARKSDK void        ODESetBodyScaling                       ( int iObjectNumber, float fX, float fY, float fZ );

// properties
DARKSDK void		ODESetBodyMass							( int iObject, float fMass );
DARKSDK void		ODESetBodyFriction						( int iObject, float fFriction );
DARKSDK void		ODESetBodyDamping                       ( int iObject, float fDamp, float fAngleDamp );
DARKSDK void        ODESetHingeLimits                       ( int iConstraint, float fAngMin, float fAngMax, float fSoftness, float fBias, float fRelaxation );
DARKSDK void        ODESetHingeMotor                        ( int iConstraint, float fSpeed, float fAngle, float fForce );
DARKSDK void        ODESetSliderLimits                      ( int iConstraint, float fLowerLin, float fUpperLin, float fLowerAng, float fUpperAng );
DARKSDK void        ODESetSliderMotor                       ( int iConstraint, bool OnOff, float fForce, float fVelocity );

// world functions
DARKSDK void		ODESetWorldGravity						( float fX, float fY, float fZ , float fallspeed = 0 );

// Get commands
DARKSDK DWORD		ODEGetBodyLinearVelocityX				( int iObjectNumber );
DARKSDK DWORD		ODEGetBodyLinearVelocityY				( int iObjectNumber );
DARKSDK DWORD		ODEGetBodyLinearVelocityZ				( int iObjectNumber );
DARKSDK int			ODEGetBodyAttribValue					( int iObjectNumber );
DARKSDK bool        ODEGetBodyIsDynamic                     ( int iObjectNumber );
DARKSDK float       ODEGetHingeAngle                        ( int iConstraint   );
DARKSDK float       ODEGetSliderPosition                    ( int iConstraint   );

// Lua Collision support
DARKSDK int			ODEGetBodyNumCollisions                 ( int iObjectNumber );
DARKSDK void		ODEGetBodyCollisionDetails              ( int iObjectNumber, int iColNumber, int &iColObj, float &fX, float &fY, float &fZ, float &fImpulse );
DARKSDK int			ODEGetTerrainNumCollisions              ( int iObjectNumber );
DARKSDK void		ODEGetTerrainCollisionDetails           ( int iObjectNumber, int iColNumber, int &iLatest, float &fX, float &fY, float &fZ );
DARKSDK void		ODEAddBodyCollisionCheck                ( int iObjectNumber );
DARKSDK void		ODERemoveBodyCollisionCheck             ( int iObjectNumber );

// Debug Drawer
DARKSDK void		BPhys_SetDebugDrawerMode(int iMode, int iTransforms, int iConstraints);
DARKSDK int			BPhys_GetDebugDrawerMode();
DARKSDK void		BPhys_ClearDebugDrawData();
DARKSDK float*		BPhys_GetDynamicDebugDrawData(int& size);
DARKSDK float*		BPhys_GetStaticDebugDrawData(int& size);
DARKSDK void		BPhys_AddDebugSingleObject(int objectID);
DARKSDK void		BPhys_RemoveDebugSingleObject(int objectID);
DARKSDK void		BPhys_DrawDebugObjects();
DARKSDK int			BPhys_GetDebugObjectCount();











