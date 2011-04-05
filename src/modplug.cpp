/*
 *      Copyright (C) 2009 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C"
{
#include "xbmc_ac_types.h"
#include "xbmc_ac_dll.h"

#ifdef __linux__
#include <libmodplug/modplug.h>
#else
#include "src/modplug.h"
#endif


//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
ADDON_STATUS Create(void* hdl, void* props)
{
  return STATUS_OK;
}

//-- Stop ---------------------------------------------------------------------
// This dll must cease all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void Stop()
{
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void Destroy()
{
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
bool HasSettings()
{
  return false;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS GetStatus()
{
  return STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
unsigned int GetSettings(StructSetting ***sSet)
{
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

void FreeSettings()
{
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS SetSetting(const char *strSetting, const void* value)
{
  return STATUS_OK;
}

AC_INFO* Init(const char* strFile, int track)
{
  // Read our file to memory so it can be passed to ModPlug_Load()
  FILE* file;
  if (!(file = fopen(strFile,"rb")))
    return NULL;
  fseek(file,0,SEEK_END);
  int64_t pos = ftell(file);
  fseek(file,0,SEEK_SET);

  char *data = new char[pos];
  fread(data,pos,1,file);
  fclose(file);

  // Now load the module
  AC_INFO* info = new AC_INFO;
  info->mod = ModPlug_Load(data,pos);
  delete[] data;
  if (!info->mod)
  {
    delete info;
    return NULL;
  }
  info->channels = 2;
  info->samplerate = 44100;
  info->bitpersample = 16;
  info->totaltime = ModPlug_GetLength((ModPlugFile*)info->mod);
  strcpy(info->name,strrchr(strFile,'.')+1);

  return info;
}

void DeInit(AC_INFO* info)
{
  if (info && info->mod)
    ModPlug_Unload((ModPlugFile*)info->mod);
  delete info;
}

int64_t Seek(AC_INFO* info, int64_t iSeekTime)
{
  ModPlug_Seek((ModPlugFile*)info->mod, (int)iSeekTime);
  return iSeekTime;
}

int ReadPCM(AC_INFO* info, void* pBuffer, unsigned int size, unsigned int *actualsize)
{
  if (!info || !info->mod)
    return READ_ERROR;

  if ((*actualsize = ModPlug_Read((ModPlugFile*)info->mod, pBuffer, size)) == size)
    return READ_SUCCESS;
  
  return READ_ERROR;
}

int GetNumberOfTracks(const char* strFile)
{
  return 1;
}

}
