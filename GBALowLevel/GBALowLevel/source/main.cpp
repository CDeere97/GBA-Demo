#include "Intellisense.h"
#include "gba_types.h"
#include "gba_reg.h"
#include "gba_input.h"
#include "gba_mathUtil.h"
#include "gba_drawing.h"

#include <string.h>
#include "bg_field.h"
#include "Sprite.h"
#include "Charizard_Sprite.h"
#include "Charizard_Sprite4bpp.h"


//-----------------------------------------------------------------------------
//Define Externs
//-----------------------------------------------------------------------------
//set the extern values for key values
u16 __currKeys = 0, __prevKeys = 0;
//random seed extern
s32 __qran_seed = 42;


#define REG_DISPLAYCONTROL *((vu32*)(REG_BASE))
#define VIDEOMODE_0 0x0000  // Sprite Mode 0 
#define VIDEOMODE_3 0x0003	// < Mode 3; 240x160 @16bpp
#define VIDEOMODE_4	0x0004	// < Mode 4; 240x160 @8 bpp
#define VIDEOMODE_5 0x0005  // < Mode 5; 

#define BGMODE_0	0x0100
#define BGMODE_1	0x0200
#define BGMODE_2	0x0400

#define REG_VCOUNT (*(vu16*)(REG_BASE + 0x06))

#define VIDEO_PAGE			0x0010	//!< Page indicator

#define VRAM_PAGE_SIZE		0x0A000
#define VRAM_BACK	(VRAM+ VRAM_PAGE_SIZE)

// --- memmap ---
#define MEM_PALETTE		0x05000000
#define pal_bg_mem		((u16*)MEM_PALETTE)
#define pal_sp_mem		((u16*)(MEM_PALETTE + 0x200))

// vid_mem[a] = COLOR
#define vid_mem			((u16*)VRAM)
#define vid_page_front	((u16*)VRAM)
#define vid_page_back	((u16*)VRAM_BACK)


void vsync()
{
	while (REG_VCOUNT >= 160);
	while (REG_VCOUNT < 160);
}


u16* vid_page = vid_page_back;

typedef struct TILE { u32 data[8]; }TILE, TILE4;
typedef struct TILE8 { u32 data[16]; }TILE8;

typedef TILE TILEBLOCK[512];
typedef TILE8 TILEBLOCK8[256];

#define tile_mem ((TILEBLOCK*)0X06000000)
#define tile8_mem ((TILEBLOCK8*)0x06000000)

#define ENABLE_OBJECTS 0x1000
#define MAPPINGMODE_1D 0x0040

typedef struct ObjectAttributes {
	u16 attr0;
	u16 attr1;
	u16 attr2;
	s16 pad;
}__attribute__((packed, aligned(4)))ObjectAttributes;

#define MEM_OAM ((volatile ObjectAttributes*) 0x07000000)

int main()
{
	//set GBA rendering to MODE 3 Bitmap rendering
	//REG_DISPLAYCONTROL = VIDEOMODE_4 | BGMODE_2 | MAPPINGMODE_1D;
	REG_DISPLAYCONTROL = VIDEOMODE_0 | BGMODE_1 | BGMODE_0 |ENABLE_OBJECTS | MAPPINGMODE_1D;

	memcpy(vid_page_front, bg_fieldPal, bg_fieldPalLen);
	memcpy(pal_bg_mem, bg_fieldTiles, bg_fieldTilesLen);


	memcpy(pal_sp_mem, SpritePal, SpritePalLen);
	memcpy(&tile8_mem[4][0], SpriteTiles, SpriteTilesLen);

	memcpy(pal_sp_mem, Charizard_SpritePal, Charizard_SpritePalLen);
	memcpy(&tile8_mem[4][0], Charizard_SpriteTiles, Charizard_SpriteTilesLen);

	volatile ObjectAttributes* spriteAttribs = &MEM_OAM[0];

	spriteAttribs->attr0 = 0xA060;
	spriteAttribs->attr1 = 0xC064;
	spriteAttribs->attr2 = 0;
	
	u8 yPos = 96;

	while (1) { //loop forever
		vsync();
		--yPos;
		spriteAttribs->attr0 = (spriteAttribs->attr0 & ~0x00FF) | (yPos & 0x00FF);
	}
	return 0;
}