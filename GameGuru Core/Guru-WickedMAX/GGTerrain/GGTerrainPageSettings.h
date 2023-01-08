#ifndef _H_GGTERRAIN_PAGE_SETTINGS
#define _H_GGTERRAIN_PAGE_SETTINGS

#define GGTERRAIN_TEXTURE_FILTERING_TRILINEAR  0  // requires 2 pixels of padding per page
#define GGTERRAIN_TEXTURE_FILTERING_ANISO_X4   1  // requires 4 pixels of padding per page
#define GGTERRAIN_TEXTURE_FILTERING_ANISO_X8   2  // requires 6 pixels of padding per page

// choose the filtering mode here, ansiotropic filtering uses up much more memory
// in the page cache and may cause more noticeable page swapping
#define GGTERRAIN_TEXTURE_FILTERING   GGTERRAIN_TEXTURE_FILTERING_ANISO_X4

#ifdef __cplusplus
	#define VARTYPE const uint32_t
	#define VARTYPEF const float
#else
	#define VARTYPE static const float
	#define VARTYPEF static const float
#endif

#if (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_TRILINEAR)
	VARTYPE pagePaddingLeft = 2;
	VARTYPE pagePaddingRight = 2;
#elif (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_ANISO_X4)
	VARTYPE pagePaddingLeft = 4;
	VARTYPE pagePaddingRight = 4;
#elif (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_ANISO_X8)
	VARTYPE pagePaddingLeft = 6;
	VARTYPE pagePaddingRight = 6;
#else
	#error Unknown GGTERRAIN_TEXTURE_FILTERING mode
#endif

VARTYPE pagesX = 256; // size of the page table texture in pixels
VARTYPE pagesY = 256;
VARTYPE pageSize = 128; // size of individual pages in pixels before padding

VARTYPE paddedPageSize = pageSize + pagePaddingLeft + pagePaddingRight;
VARTYPEF paddingScale = pagePaddingLeft / (float) pageSize;

VARTYPE physPagesX = 80; // size of the physical texture in pages
VARTYPE physPagesY = 80;

VARTYPE physTexSizeX = paddedPageSize * physPagesX;
VARTYPE physTexSizeY = paddedPageSize * physPagesY;

#define GGTERRAIN_MAX_PAGE_TABLE_MIP  7   // not a full mip stack, stops at 4x4 since the lowest levels aren't very useful

#endif

