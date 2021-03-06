#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "i_video.h"

class IVideo
{
 public:
	virtual ~IVideo () {}

	virtual EDisplayType GetDisplayType () = 0;
	virtual bool FullscreenChanged (bool fs) = 0;
	virtual void SetWindowedScale (float scale) = 0;
	virtual bool CanBlit () = 0;

	virtual bool SetMode (int width, int height, int bits, bool fs) = 0;
	virtual void SetPalette (DWORD *palette) = 0;
	virtual void UpdateScreen (DCanvas *canvas) = 0;
	virtual void ReadScreen (byte *block) = 0;

	virtual int GetModeCount () = 0;
	virtual void StartModeIterator (int bits) = 0;
	virtual bool NextMode (int *width, int *height) = 0;

	virtual bool AllocateSurface (DCanvas *scrn, int width, int height, int bits, bool primary = false) = 0;
	virtual void ReleaseSurface (DCanvas *scrn) = 0;
	virtual void LockSurface (DCanvas *scrn) = 0;
	virtual void UnlockSurface (DCanvas *scrn) = 0;
	virtual bool Blit (DCanvas *src, int sx, int sy, int sw, int sh,
					   DCanvas *dst, int dx, int dy, int dw, int dh) = 0;
};

class IInputDevice
{
 public:
	virtual ~IInputDevice () {}
	virtual void ProcessInput (bool parm) = 0;
};

class IKeyboard : public IInputDevice
{
 public:
	virtual void ProcessInput (bool consoleOpen) = 0;
	virtual void SetKeypadRemapping (bool remap) = 0;
	virtual void GetKeyRepeats (int &delay, int &rate)
		{
			delay = (250*TICRATE)/1000;
			rate = TICRATE / 15;
		}
};

class IMouse : public IInputDevice
{
 public:
	virtual void SetGrabbed (bool grabbed) = 0;
	virtual void ProcessInput (bool active) = 0;
};

class IJoystick : public IInputDevice
{
 public:
	enum EJoyProp
	{
		JOYPROP_SpeedMultiplier,
		JOYPROP_XSensitivity,
		JOYPROP_YSensitivity,
		JOYPROP_XThreshold,
		JOYPROP_YThreshold
	};
	virtual void SetProperty (EJoyProp prop, float val) = 0;
};

void I_InitHardware ();
void STACK_ARGS I_ShutdownHardware ();

#endif	// __HARDWARE_H__
