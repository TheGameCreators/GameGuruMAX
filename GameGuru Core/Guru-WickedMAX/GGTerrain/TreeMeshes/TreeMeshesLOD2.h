#include "birch_trunk_LOD2.h"
#include "birch_branches_LOD2.h"
#include "cactus_var1_LOD2.h"
#include "cactus_var2_LOD2.h"
#include "cactus_var3_LOD2.h"
#include "cactus_var4_LOD2.h"
#include "dead pine tree_trunk_LOD2.h"
#include "drypine_trunk_LOD2.h"
#include "drypine_branches_LOD2.h"
#include "italian pine_trunk_LOD2.h"
#include "italian pine_branches_LOD2.h"
#include "jungletree1_trunk_LOD2.h"
#include "jungletree1_branches_LOD2.h"
#include "jungletree2_trunk_LOD2.h"
#include "jungletree2_branches_LOD2.h"
#include "jungletree3a_trunk_LOD2.h"
#include "jungletree3a_branches_LOD2.h"
#include "jungletree3b_trunk_LOD2.h"
#include "jungletree3b_branches_LOD2.h"
#include "jungletree4a_trunk_LOD2.h"
#include "jungletree4a_branches_LOD2.h"
#include "jungletree4b_trunk_LOD2.h"
#include "jungletree4b_branches_LOD2.h"
#include "jungletree5a_trunk_LOD2.h"
#include "jungletree5a_branches_LOD2.h"
#include "jungletree5b_trunk_LOD2.h"
#include "jungletree5b_branches_LOD2.h"
#include "jungletree6a_trunk_LOD2.h"
#include "jungletree6a_branches_LOD2.h"
#include "jungletree6b_trunk_LOD2.h"
#include "jungletree6b_branches_LOD2.h"
#include "kentia palm_trunk_LOD2.h"
#include "kentia palm_branches_LOD2.h"
#include "palm_trunk_LOD2.h"
#include "palm branches_LOD2.h"
#include "pine_trunk_LOD2.h"
#include "pine_branches_LOD2.h"
#include "scotspine1_trunk_LOD2.h"
#include "scotspine1_branches_LOD2.h"
#include "scotspine2_trunk_LOD2.h"
#include "scotspine2_branches_LOD2.h"
#include "scotspine dead_trunk_LOD2.h"
#include "snow fir2_trunk_LOD2.h"
#include "snow fir2_branches_LOD2.h"
#include "snow fir3_trunk_LOD2.h"
#include "snow fir3_branches_LOD2.h"
#include "snow fir_trunk_LOD2.h"
#include "snow fir_branches_LOD2.h"
#include "snow pine_trunk_LOD2.h"
#include "snow pine_branches_LOD2.h"
#include "snow pine tall2_trunk_LOD2.h"
#include "snow pine tall2_branches_LOD2.h"
#include "snow pine tall_trunk_LOD2.h"
#include "snow pine tall_branches_LOD2.h"
#include "sparse pine_trunk_LOD2.h"
#include "sparse pine_branches_LOD2.h"
#include "vine tree large_trunk_LOD2.h"
#include "vine tree large_branches_LOD2.h"
#include "vine tree small_trunk_LOD2.h"
#include "vine tree small_branches_LOD2.h"
#include "western pine_trunk_LOD2.h"
#include "western pine_branches_LOD2.h"
#include "white pine trunk_LOD2.h"
#include "white pine_branches_LOD2.h"

const GGTree g_GGTreesLOD2[ 38 ] = 
{
    { &g_TreeMeshBirchTrunkLOD2, &g_TreeMeshBirchBranchesLOD2, 312.85f, 0.800f, "birch_BB_SF_0.8_color.dds", "birch_BB_SF_0.8_normal.dds" }, 
    { &g_TreeMeshCactusVar1LOD2, 0, 164.06f, 0.300f, "cactus_var1_BB_SF_0.3_color.dds", "cactus_var1_BB_SF_0.3_normal.dds" }, 
    { &g_TreeMeshCactusVar2LOD2, 0, 137.50f, 0.300f, "cactus_var2_BB_SF_0.3_color.dds", "cactus_var2_BB_SF_0.3_normal.dds" }, 
    { &g_TreeMeshCactusVar3LOD2, 0, 188.94f, 0.360f, "cactus_var3_BB_SF_0.36_color.dds", "cactus_var3_BB_SF_0.36_normal.dds" }, 
    { &g_TreeMeshCactusVar4LOD2, 0, 188.94f, 0.260f, "cactus_var4_BB_SF_0.26_color.dds", "cactus_var4_BB_SF_0.26_normal.dds" }, 
    { &g_TreeMeshDeadPineTreeTrunkLOD2, 0, 451.63f, 0.400f, "dead pine tree_BB_SF_0.4_color.dds", "dead pine tree_BB_SF_0.4_normal.dds" }, 
    { &g_TreeMeshDrypineTrunkLOD2, &g_TreeMeshDrypineBranchesLOD2, 910.84f, 0.500f, "drypine_BB_SF_0.5_color.dds", "drypine_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshItalianPineTrunkLOD2, &g_TreeMeshItalianPineBranchesLOD2, 325.97f, 0.500f, "italian pine_BB_SF_0.5_color.dds", "italian pine_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshJungletree1TrunkLOD2, &g_TreeMeshJungletree1BranchesLOD2, 267.22f, 0.800f, "jungletree1_BB_SF_0.8_color.dds", "jungletree1_BB_SF_0.8_normal.dds" }, 
    { &g_TreeMeshJungletree2TrunkLOD2, &g_TreeMeshJungletree2BranchesLOD2, 307.05f, 0.750f, "jungletree2_BB_SF_0.75_color.dds", "jungletree2_BB_SF_0.75_normal.dds" }, 
    { &g_TreeMeshJungletree3aTrunkLOD2, &g_TreeMeshJungletree3aBranchesLOD2, 220.57f, 1.000f, "jungletree3a_BB_SF_1_color.dds", "jungletree3a_BB_SF_1_normal.dds" }, 
    { &g_TreeMeshJungletree3bTrunkLOD2, &g_TreeMeshJungletree3bBranchesLOD2, 373.90f, 0.560f, "jungletree3b_BB_SF_0.56_color.dds", "jungletree3b_BB_SF_0.56_normal.dds" }, 
    { &g_TreeMeshJungletree4aTrunkLOD2, &g_TreeMeshJungletree4aBranchesLOD2, 220.57f, 1.000f, "jungletree4a_BB_SF_1_color.dds", "jungletree4a_BB_SF_1_normal.dds" }, 
    { &g_TreeMeshJungletree4bTrunkLOD2, &g_TreeMeshJungletree4bBranchesLOD2, 373.90f, 0.560f, "jungletree4b_BB_SF_0.56_color.dds", "jungletree4b_BB_SF_0.56_normal.dds" }, 
    { &g_TreeMeshJungletree5aTrunkLOD2, &g_TreeMeshJungletree5aBranchesLOD2, 339.99f, 0.750f, "jungletree5a_BB_SF_0.75_color.dds", "jungletree5a_BB_SF_0.75_normal.dds" }, 
    { &g_TreeMeshJungletree5bTrunkLOD2, &g_TreeMeshJungletree5bBranchesLOD2, 435.37f, 0.600f, "jungletree5b_BB_SF_0.6_color.dds", "jungletree5b_BB_SF_0.6_normal.dds" }, 
    { &g_TreeMeshJungletree6aTrunkLOD2, &g_TreeMeshJungletree6aBranchesLOD2, 423.64f, 0.750f, "jungletree6a_BB_SF_0.75_color.dds", "jungletree6a_BB_SF_0.75_normal.dds" }, 
    { &g_TreeMeshJungletree6bTrunkLOD2, &g_TreeMeshJungletree6bBranchesLOD2, 367.90f, 0.750f, "jungletree6b_BB_SF_0.75_color.dds", "jungletree6b_BB_SF_0.75_normal.dds" }, 
    { &g_TreeMeshKentiaPalmTrunkLOD2, &g_TreeMeshKentiaPalmBranchesLOD2, 314.74f, 0.500f, "kentia palm_BB_SF_0.5_color.dds", "kentia palm_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshPalmTrunkLOD2, &g_TreeMeshPalmBranchesLOD2, 344.39f, 0.500f, "palm_BB_SF_0.5_color.dds", "palm_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshPineTrunkLOD2, &g_TreeMeshPineBranchesLOD2, 553.87f, 0.500f, "pine_BB_SF_0.5_color.dds", "pine_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshScotspine1TrunkLOD2, &g_TreeMeshScotspine1BranchesLOD2, 758.21f, 0.500f, "scotspine1_BB_SF_0.5_color.dds", "scotspine1_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshScotspine2TrunkLOD2, &g_TreeMeshScotspine2BranchesLOD2, 787.19f, 0.700f, "scotspine2_BB_SF_0.7_color.dds", "scotspine2_BB_SF_0.7_normal.dds" }, 
    { &g_TreeMeshScotspineDeadTrunkLOD2, 0, 715.94f, 0.700f, "scotspine dead_BB_SF_0.7_color.dds", "scotspine dead_BB_SF_0.7_normal.dds" }, 
    { &g_TreeMeshSnowFir2TrunkLOD2, &g_TreeMeshSnowFir2BranchesLOD2, 401.21f, 0.630f, "snow fir2_BB_SF_0.63_color.dds", "snow fir2_BB_SF_0.63_normal.dds" }, 
    { &g_TreeMeshSnowFir3TrunkLOD2, &g_TreeMeshSnowFir3BranchesLOD2, 262.68f, 0.500f, "snow fir3_BB_SF_0.5_color.dds", "snow fir3_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshSnowFirTrunkLOD2, &g_TreeMeshSnowFirBranchesLOD2, 553.31f, 0.500f, "snow fir1_BB_SF_0.5_color.dds", "snow fir1_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshSnowPineTrunkLOD2, &g_TreeMeshSnowPineBranchesLOD2, 527.09f, 0.800f, "snow pine_BB_SF_0.8_color.dds", "snow pine_BB_SF_0.8_normal.dds" }, 
    { &g_TreeMeshSnowPineTall2TrunkLOD2, &g_TreeMeshSnowPineTall2BranchesLOD2, 643.42f, 0.370f, "snow pine tall2_BB_SF_0.37_color.dds", "snow pine tall2_BB_SF_0.37_normal.dds" }, 
    { &g_TreeMeshSnowPineTallTrunkLOD2, &g_TreeMeshSnowPineTallBranchesLOD2, 590.61f, 0.370f, "snow pine tall_BB_SF_0.4_color.dds", "snow pine tall_BB_SF_0.4_normal.dds" }, 
    { &g_TreeMeshSparsePineTrunkLOD2, &g_TreeMeshSparsePineBranchesLOD2, 531.16f, 0.250f, "sparse pine_BB_SF_0.25_color.dds", "sparse pine_BB_SF_0.25_normal.dds" }, 
    { &g_TreeMeshVineTreeLargeTrunkLOD2, &g_TreeMeshVineTreeLargeBranchesLOD2, 573.26f, 0.670f, "vine tree large_BB_SF_0.67_color.dds", "vine tree large_BB_SF_0.67_normal.dds" }, 
    { &g_TreeMeshVineTreeSmallTrunkLOD2, &g_TreeMeshVineTreeSmallBranchesLOD2, 491.90f, 0.500f, "vine tree small_BB_SF_0.5_color.dds", "vine tree small_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshWesternPineTrunkLOD2, &g_TreeMeshWesternPineBranchesLOD2, 910.84f, 0.500f, "westernpine_BB_SF_0.5_color.dds", "westernpine_BB_SF_0.5_normal.dds" }, 
    { &g_TreeMeshWhitePineTrunkLOD2, &g_TreeMeshWhitePineBranchesLOD2, 285.31f, 0.450f, "white pine_BB_SF_0.45_color.dds", "white pine_BB_SF_0.45_normal.dds" }, 
	{ &g_TreeMeshBirchTrunkLOD2, &g_TreeMeshBirchAutumn1BranchesLOD2, 315.58f, 0.800f, "birch autumn1_BB_SF_0.8_color.dds", "birch autumn1_BB_SF_0.8_normal.dds" }, 
	{ &g_TreeMeshBirchTrunkLOD2, &g_TreeMeshBirchAutumn2BranchesLOD2, 315.58f, 0.800f, "birch autumn2_BB_SF_0.8_color.dds", "birch autumn2_BB_SF_0.8_normal.dds" }, 
	{ &g_TreeMeshBirchTrunkLOD2, &g_TreeMeshBirchAutumn3BranchesLOD2, 315.58f, 0.800f, "birch autumn3_BB_SF_0.8_color.dds", "birch autumn3_BB_SF_0.8_normal.dds" },
};
