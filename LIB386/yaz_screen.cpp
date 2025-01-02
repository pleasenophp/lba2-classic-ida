#include <svga.h>
#include <system.h>
#include <SDL.h>

constexpr auto FullScreen = 1;

SDL_Surface *sdlScreen;
SDL_Window *sdlWindow;
SDL_Renderer* sdlRenderer = nullptr;
SDL_Texture* sdlTexture = nullptr;
U32 g_pal[256];

S32 InitVESA()
{
	SDL_VideoInit(NULL);
	return 1;
}

S32	DetectInitVESAMode(U32 ResX, U32 ResY, U32 Depth, U32 Memory)
{
	if (ResX != 640 || ResY != 480) {
		fprintf(stderr, "Only 640x480 resolution is supported at the moment. Tried to initialize: %dx%d", ResX, ResY);
		exit(1);
	}

	SDL_ShowCursor(false);

	if (FullScreen) 
	{
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		sdlWindow = SDL_CreateWindow("LBA2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else 
	{
		sdlWindow = SDL_CreateWindow("LBA2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ResX, ResY, SDL_WINDOW_SHOWN);
		sdlScreen = SDL_GetWindowSurface(sdlWindow);
	}

    if (sdlWindow == NULL) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        exit(1);
    }

	if (FullScreen) {
		sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
		if (!sdlRenderer) {
			fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
			exit(1);
		}

		SDL_RenderSetLogicalSize(sdlRenderer, ResX, ResY);

		sdlTexture = SDL_CreateTexture(
			sdlRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			ResX,
			ResY
		);

		if (!sdlTexture) {
			fprintf(stderr, "Unable to create texture: %s\n", SDL_GetError());
			exit(1);
		}
	}

	ModeResX = ResX;
	ModeResY = ResY;
	BytesScanLine = ResX;

	Phys = malloc(ResX*ResY);

	SetPaletteVESA= &SetPaletteVESA1;

	ProcessorSignature.FPU = 1;
	ProcessorSignature.Family = 5;
	ProcessorSignature.Manufacturer = 1;

	return 1;
}

void CopyBoxF(void *dst, void *src, U32 *TabOffDst, T_BOX *box) 
{
	// If destination is a physical buffer. We don't write anything into Phys anymore here, just using it to check the desired destination
	if (dst == Phys)
	{
		if (FullScreen) 
		{
			void* pixels;
			int pitch;
			SDL_LockTexture(sdlTexture, NULL, &pixels, &pitch);

			Uint32* dstPixels = (Uint32*)pixels;
			for (int x = box->x0; x < box->x1; x++)
			{
				for (int y = box->y0; y < box->y1; y++)
				{
					U8 colorIndex = *((U8*)src + TabOffDst[y] + x);
					Uint32 color = g_pal[colorIndex];
					dstPixels[y * (pitch / 4) + x] = color;
				}
			}

			SDL_UnlockTexture(sdlTexture);
			SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
			SDL_RenderPresent(sdlRenderer);
		}
		else 
		{
			SDL_LockSurface(sdlScreen);
			for (int x = box->x0; x < box->x1; x++)
			{
				for (int y = box->y0; y < box->y1; y++)
				{
					U32* pOut = (U32*)((U8*)sdlScreen->pixels + y * sdlScreen->pitch + x * 4);
					U8 colorIndex = *((U8*)src + TabOffDst[y] + x);
					*pOut = g_pal[colorIndex];
				}
			}
			SDL_UnlockSurface(sdlScreen);
			SDL_Rect updateRect = {
				box->x0,
				box->y0,
				box->x1 - box->x0,
				box->y1 - box->y0
			};
			SDL_UpdateWindowSurfaceRects(sdlWindow, &updateRect, 1);
		}
	}
	else
	{
		for(int x=box->x0; x<box->x1; x++)
		{
			for(int y=box->y0; y<box->y1; y++)
			{
				*((U8*)dst + TabOffDst[y] + x) = *((U8*)src + TabOffDst[y] + x);
			}
		}
	}
}

extern "C" {
	
	void VblVESA(void)
	{

	}

	void SetPaletteVESA2(void *pal, S32 start, S32 n)
	{

	}

	void SetPaletteVESA1(void *pal, S32 start, S32 n)
	{
		U8 *palette = (U8*)pal;

		for (U32 i=start;i<start+n; i++)
		{
			PalOne(i, palette[i*3+0], palette[i*3+1], palette[i*3+2]);
		}
	}

	void (*SetPaletteVESA)(void *pal, S32 start, S32 n) = 0;

	void SetPaletteOneVESA(S32 col, S32 red, S32 green, S32 blue)
	{
		U32 color = ((U32)0xFF << 24) | ((U32)red << 16) | ((U32)green << 8) | ((U32)blue << 0);

		g_pal[col] = color;
	}

	void SetPaletteDirect(void *pal, S32 start, S32 n)
	{

	}

	void PaletteSync(U8 *palette, bool videoMode = false) 
	{
		U32 yStart = videoMode ? 40 : 0;
		U32 yEnd = videoMode ? 440 : 480;

		for (U32 i=0; i<256; i++)
		{
			PalOne(i, palette[i*3+0], palette[i*3+1], palette[i*3+2]);
		}

		if (FullScreen)
		{

			void* pixels;
			int pitch;
			SDL_LockTexture(sdlTexture, NULL, &pixels, &pitch);


			Uint32* dstPixels = (Uint32*)pixels;
			for (U32 x = 0; x < 640; x++)
			{
				for (U32 y = yStart; y < yEnd; y++)
				{
					U8 colorIndex = ((U8*)Log)[y * 640 + x];
					Uint32 color = g_pal[colorIndex];
					dstPixels[y * (pitch / 4) + x] = color;

					// U32* pOut = (U32*)((U8*)sdlScreen->pixels + y*sdlScreen->pitch + x * 4);
					// *pOut = g_pal[color];
				}
			}

			SDL_UnlockTexture(sdlTexture);
			SDL_RenderClear(sdlRenderer);
			SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
			SDL_RenderPresent(sdlRenderer);
		}
		else 
		{
			SDL_LockSurface(sdlScreen);

			for (U32 x = 0; x < 640; x++)
			{
				for (U32 y = yStart; y < yEnd; y++)
				{
					U8 colorIndex = ((U8*)Log)[y * 640 + x];
					U32* pOut = (U32*)((U8*)sdlScreen->pixels + y*sdlScreen->pitch + x * 4);
					*pOut = g_pal[colorIndex];
				}
			}

			SDL_UnlockSurface(sdlScreen);
			SDL_UpdateWindowSurface(sdlWindow);
		}
	}

	void SavePCX(char *,unsigned char *,unsigned long,unsigned long,unsigned char *)
	{

	}
}

