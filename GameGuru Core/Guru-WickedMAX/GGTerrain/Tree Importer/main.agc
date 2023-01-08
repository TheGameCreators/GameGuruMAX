
SetErrorMode(2)

SetWindowSize ( 1024, 768, 0 )
SetWindowAllowResize ( 1 )
SetVirtualResolution( 1024, 768 )
UseNewDefaultFonts ( 1 )
SetSyncRate( 60, 0 )
SetClearColor( 101,120,154 )
SetAmbientColor( 128,128,128 )
SetGenerateMipmaps( 1 )

SetPrintSize( 20 )

global srcPath as string = "raw:E:/Temp/GGMax/Jan 2022 trees and grass/trees"
global dstPath as string = "raw:E:/Data/Github/GameGuruV3Repo/GameGuru Core/Guru-WickedMAX/GGTerrain/TreeMeshes"

global numTrees as integer = 0

global maxHeight as float = 0
type tree_t
	commonName as string
	headername as string
	branchesheadername as string
	trunkName as string
	branchesName as string
	billboardName as string
	billboardNormalName as string
	height as float
	lodLevel as integer
endtype

global allTrees as tree_t[]
global lodCount as integer[2]
global missingTextures as string[]
global availableTextures as string[]
global availableBillboards as string[]
global availableBillboardsNorm as string[]

global lastSyncTime as float = 0

// parse all trees
ParseFolder( srcPath )

// write final files
for lod = 0 to 2
	OpenToWrite( 2, dstPath + "/" + "TreeMeshesLOD" + str(lod) + ".h" )
	
	for tree = 0 to allTrees.length
		if ( allTrees[ tree ].lodLevel <> lod ) then continue
		if ( len(allTrees[tree].headername) > 0 ) then WriteLine( 2, '#include "' + allTrees[tree].headername + '"' )
		if ( len(allTrees[tree].branchesheadername) > 0 ) then WriteLine( 2, '#include "' + allTrees[tree].branchesheadername + '"' )
	next tree
	
	WriteLine( 2, "" )
	WriteLine( 2, "const GGTree g_GGTreesLOD" + str(lod) + "[ " + str(lodCount[lod]) + " ] = " ) 
	WriteLine( 2, "{" )

	for tree = 0 to allTrees.length
		if ( allTrees[ tree ].lodLevel <> lod ) then continue
		line$ = "    { "
		
		trunk$ = allTrees[ tree ].trunkName
		if ( len(trunk$) = 0 ) then trunk$ = "0" else trunk$ = "&g_TreeMesh" + trunk$
		line$ = line$ + trunk$ + ", "
		
		branches$ = allTrees[ tree ].branchesName
		if ( len(branches$) = 0 ) then branches$ = "0" else branches$ = "&g_TreeMesh" + branches$
		line$ = line$ + branches$ + ", "
		
		scaleFactor# = 0.0
		index = FindString( allTrees[ tree ].billboardName, "_SF_" )
		if ( index > 0 )
			SF$ = Mid( allTrees[ tree ].billboardName, index+4, -1 )
			scaleFactor# = ValFloat( SF$ )
		endif
		
		line$ = line$ + str(allTrees[ tree ].height,2) + "f, "
		line$ = line$ + str(scaleFactor#,3) + "f, "
		line$ = line$ + '"' + allTrees[ tree ].billboardName + '", '
		line$ = line$ + '"' + allTrees[ tree ].billboardNormalName + '" }, '
		WriteLine( 2, line$ )
	next tree

	WriteLine( 2, "};" )
	CloseFile( 2 )
next lod

// done
do
	Print( "Done Processing " + str(numTrees) + " Files, " + str(missingTextures.length+1) + " are missing texxtures" )
	
	for i = 0 to missingTextures.length
		Print( missingTextures[i] )
	next i
	
	Sync()
loop

function ParseFolder( path as string )
	folderID = OpenRawFolder( path )
	
	availableTextures.length = -1
	for file = 0 to GetRawFolderNumFiles(folderID)-1
		filename$ = GetRawFolderFileName(folderID, file)
		if ( Lower(Right(filename$,4)) = ".dds" and FindString(filename$,"normal.") < 1 )
			availableTextures.insert( filename$ )
		endif
	next file
	
	for folder = 0 to GetRawFolderNumFolders(folderID)-1
		folderName$ = GetRawFolderFolderName(folderID, folder)
		if ( Lower(folderName$) = "billboards" )
			availableBillboards.length = -1
			folderID2 = OpenRawFolder( path + "/" + folderName$ )
			for file = 0 to GetRawFolderNumFiles(folderID2)-1
				filename$ = GetRawFolderFileName(folderID2, file)
				if ( Lower(Right(filename$,4)) = ".dds" )
					if ( FindString(filename$,"normal.") > 0 ) 
						availableBillboardsNorm.insert( filename$ )
					else
						availableBillboards.insert( filename$ )
					endif
				endif
			next file
			CloseRawFolder( folderID2 )
		endif
	next folder
	
	for file = 0 to GetRawFolderNumFiles(folderID)-1
		filename$ = GetRawFolderFileName(folderID, file)
		ParseFile( path, filename$ )
	next file
	
	for folder = 0 to GetRawFolderNumFolders(folderID)-1
		ParseFolder( path + "/" + GetRawFolderFolderName(folderID, folder) )
	next folder
endfunction

function ParseFile( folder as string, filename as string )
	if ( lower(right(filename,2)) <> ".x" ) then exitfunction
	
	if ( Timer() - lastSyncTime > 0.016 )
		lastSyncTime = Timer()
		Print( "Processing File " + str(numTrees) )
		Sync()
	endif
	
	headerName$ = TruncateString( filename, "." )
	headerName$ = headerName$ + ".h"
	headerPath$ = dstPath + "/" + headerName$
	
	lodLevel = 255
	if ( FindString( filename, "LOD0" ) ) then lodLevel = 0
	if ( FindString( filename, "LOD1" ) ) then lodLevel = 1
	if ( FindString( filename, "LOD2" ) ) then lodLevel = 2
	
	if ( lodLevel > 2 ) 
		Message( "Unknown LOD level in " + filename )
		exitfunction
	endif
	
	isBranches = 0
	if ( FindString( lower(filename), "branches" ) ) then isBranches = 1
	
	varName$ = TruncateString( filename, "." )
	varName2$ = ""
	commonName$ = ""
	chars = Len(varName$)
	captialise = 1
	for i = 1 to chars
		char$ = mid(varName$, i, 1)
		if ( char$ <> " " and char$ <> "_" )
			if ( captialise )
				char$ = Upper( char$ )
				captialise = 0
			endif
			varName2$ = varName2$ + char$
		else
			captialise = 1
		endif
	next i
	
	commonName$ = ReplaceString( varName2$, "Branches", "", -1 )
	commonName$ = ReplaceString( commonName$, "Trunk", "", -1 )
	
	path$ = folder + "/" + filename
	LoadObject( 1, path$ )
	//SetObjectScalePermanent( 1, 0.177746178, 0.177746178, 0.177746178 )
	CreateMemblockFromObjectMesh( 1, 1, 1 )
	DeleteObject( 1 )

	numVertices = GetMemblockInt(1,0)
	numIndices = GetMemblockInt(1,4)
	numAttributes = GetMemblockInt(1,8)
	vertexSize = GetMemblockInt(1,12)
	vertexOffset = GetMemblockInt(1,16)
	IndexOffset = GetMemblockInt(1,20)
	maxHeight# = 0

	OpenToWrite( 1, headerPath$ )

	WriteLine( 1, "" )
	WriteLine( 1, "const VertexTreeHigh g_TreeVertices" + varName2$ + "[ " + str(numVertices) + " ] = " )
	WriteLine( 1, "{" )
	for v = 0 to numVertices-1
		index = vertexOffset + vertexSize * v
		x# = GetMemblockFloat( 1, index + 0 )
		y# = GetMemblockFloat( 1, index + 4 )
		z# = GetMemblockFloat( 1, index + 8 )
		nx# = GetMemblockFloat( 1, index + 12 )
		ny# = GetMemblockFloat( 1, index + 16 )
		nz# = GetMemblockFloat( 1, index + 20 )
		uvx# = GetMemblockFloat( 1, index + 24 )
		uvy# = GetMemblockFloat( 1, index + 28 )
		
		iNx = nx# * 127.5 + 127.5
		iNy = ny# * 127.5 + 127.5
		iNz = nz# * 127.5 + 127.5

		if ( iNx > 255 ) then iNx = 255
		if ( iNy > 255 ) then iNy = 255
		if ( iNz > 255 ) then iNz = 255

		if ( iNx < 0 ) then iNx = 0
		if ( iNy < 0 ) then iNy = 0
		if ( iNz < 0 ) then iNz = 0
		
		iNormal = (iNz << 16) || (iNy << 8) || (iNx)
		
		if ( y# > maxHeight# ) then maxHeight# = y#
		
		line$ = "    { " 
		line$ = line$ + str( x#, 4 ) + "f, " + str( y#, 4 ) + "f, " + str( z#, 4 ) + "f, "
		//line$ = line$ + str( nx#, 4 ) + "f, " + str( ny#, 4 ) + "f, " + str( nz#, 4 ) + "f, "
		line$ = line$ + "0x" + hex(iNormal) + ", "
		line$ = line$ + str( uvx#, 4 ) + "f, " + str( uvy#, 4 ) + "f"
		line$ = line$ + " },"
		
		WriteLine( 1, line$ )
	next v
	WriteLine( 1, "};" )

	WriteLine( 1, "" )
	//WriteLine( 1, "uint16_t g_pIndicesBushVar" + filename$ + "_LOD" + lodname$ + "[ " + str(numIndices) + " ] = " )
	WriteLine( 1, "const uint16_t g_TreeIndices" + varName2$ + "[ " + str(numIndices) + " ] = " )

	WriteLine( 1, "{" )
	for i = 0 to (numIndices/3)-1
		index = indexOffset + 12 * i
		v1 = GetMemblockInt( 1, index + 0 )
		v2 = GetMemblockInt( 1, index + 4 )
		v3 = GetMemblockInt( 1, index + 8 )
		
		line$ = "    " + str(v1) + ", " + str(v2) + ", " + str(v3) + ","
			
		WriteLine( 1, line$ )
	next i
	WriteLine( 1, "};" )
	
	// find texture
	textureName$ = ""
	index = FindString( filename, "_LOD" )
	if ( index > 0 ) 
		textureName$ = Left( filename, index-1 )
		textureName$ = textureName$ + ".dds"
		
		texturePath$ = folder + "/" + textureName$
		if ( GetFileExists( texturePath$ ) = 0 ) 
			index = FindClosestTexture( availableTextures, textureName$ )
			if ( index < 0 )
				textureName$ = ""
			else
				textureName$ = availableTextures[index]
			endif
		endif
	endif
	
	if ( len(textureName$) = 0 ) 
		missingTextures.insert( path$ )
	endif
	
	WriteLine( 1, "" )
	WriteLine( 1, "const TreeMeshHigh g_TreeMesh" + varName2$ + " = " )
	WriteLine( 1, "{" )
	WriteLine( 1, "    g_TreeVertices" + varName2$ + ", " )
	WriteLine( 1, "    g_TreeIndices" + varName2$ + ", " )
	WriteLine( 1, "    " + str(numVertices) + ", " )
	WriteLine( 1, "    " + str(numIndices) + ", " )
	WriteLine( 1, '    "' + textureName$ + '", ' )
	WriteLine( 1, "    " + str(maxHeight#,2) + "f, " )
	WriteLine( 1, "};" )
	
/*
struct TreeMeshHigh
{
	VertexTreeHigh* pVertices;
	uint32_t numVertices;
	uint16_t* pIndices;
	uint32_t numIndices;
	float height;
	const char* textureName;
};
*/

	index = allTrees.find( commonName$ )
	if ( index >= 0 )
		if ( isBranches ) 
			allTrees[index].branchesheaderName = headerName$
			allTrees[index].branchesName = varName2$
		else
			allTrees[index].headerName = headerName$
			allTrees[index].trunkName = varName2$
			index2 = FindClosestTexture( availableBillboards, filename )
			if ( index2 >= 0 ) then allTrees[index].billboardName = availableBillboards[index2]
			index2 = FindClosestTexture( availableBillboardsNorm, filename )
			if ( index2 >= 0 ) then allTrees[index].billboardNormalName = availableBillboardsNorm[index2]
		endif
		if ( maxHeight# > allTrees[index].height ) then allTrees[index].height = maxHeight#
	else
		newTree as tree_t
		newTree.commonName = commonName$
		if ( isBranches ) 
			newTree.branchesheaderName = headerName$
			newTree.branchesName = varName2$
		else
			newTree.headerName = headerName$
			newTree.trunkName = varName2$
			index2 = FindClosestTexture( availableBillboards, filename )
			if ( index2 >= 0 ) then newTree.billboardName = availableBillboards[index2]
			index2 = FindClosestTexture( availableBillboardsNorm, filename )
			if ( index2 >= 0 ) then newTree.billboardNormalName = availableBillboardsNorm[index2]
		endif
		newTree.height = maxHeight#
		newTree.lodLevel = lodLevel
		allTrees.insertSorted( newTree )
		inc lodCount[ lodLevel ]
	endif
	
	CloseFile( 1 )
	DeleteMemblock( 1 )
	
	inc numTrees
endfunction

function FindClosestTexture( textures ref as string[], textureName$ )
	nameLength = len(textureName$)
	
	index = -1
	longest = 0
	for i = 0 to textures.length
		testLength = len(textures[i])
		if ( testLength > nameLength ) then testLength = nameLength
		
		matchCount = 0
		for j = 1 to testLength
			if ( mid(textures[i],j,1) <> mid(textureName$,j,1) ) then exit
			inc matchCount
		next j
		
		if ( matchCount = longest and index >= 0 )
			if ( FindString(textureName$,"branches") > 0 )
				if ( FindString(textures[index],"branches") < 1 and FindString(textures[i],"branches") > 0 )
					inc matchCount
				endif
			else
				if ( FindString(textures[index],"branches") > 0 and FindString(textures[i],"branches") < 1 )
					inc matchCount
				endif
			endif
		endif
		
		if ( matchCount > longest ) 
			longest = matchCount
			index = i
		endif
	next i
	if ( longest < 4 ) then index = -1
endfunction index
