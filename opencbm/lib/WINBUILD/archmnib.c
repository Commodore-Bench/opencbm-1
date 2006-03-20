/*
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 *
 *  Copyright 2005      Tim Sch�rmann
 *  Copyright 2005      Spiro Trikaliotis
 *
*/

/*! ************************************************************** 
** \file lib/WINBUILD/archmnib.c \n
** \author Tim Sch�rmann, Spiro Trikaliotis \n
** \version $Id: archmnib.c,v 1.5 2006-03-20 11:45:53 strik Exp $ \n
** \n
** \brief Shared library / DLL for accessing the mnib driver functions, windows specific code
**
****************************************************************/

#include <windows.h>
#include <windowsx.h>

#include <mmsystem.h>

/*! Mark: We are in user-space (for debug.h) */
#define DBG_USERMODE

/*! The name of the executable */
#define DBG_PROGNAME "OPENCBM.DLL"

#include "debug.h"

#include <winioctl.h>
#include "cbmioctl.h"

#include <stdlib.h>

//! mark: We are building the DLL */
#define DLL
#include "i_opencbm.h"
#include "archlib.h"


/*! \brief PARBURST: Read from the parallel port

 This function is a helper function for parallel burst:
 It reads from the parallel port.

 \param HandleDevice
   A CBM_FILE which contains the file handle of the driver.

 \return
   The value read from the parallel port

 If cbm_driver_open() did not succeed, it is illegal to 
 call this function.
*/

__u_char
cbmarch_parallel_burst_read(CBM_FILE HandleDevice)
{
    CBMT_PARBURST_PREAD_OUT result;

    FUNC_ENTER();

    cbm_ioctl(HandleDevice, CBMCTRL(PARBURST_READ), NULL, 0, &result, sizeof(result));

    FUNC_LEAVE_UCHAR(result.Byte);
}

/*! \brief PARBURST: Write to the parallel port

 This function is a helper function for parallel burst:
 It writes to the parallel port.

 \param HandleDevice
   A CBM_FILE which contains the file handle of the driver.

 \param Value
   The value to be written to the parallel port

 If cbm_driver_open() did not succeed, it is illegal to 
 call this function.
*/

void
cbmarch_parallel_burst_write(CBM_FILE HandleDevice, __u_char Value)
{
    CBMT_PARBURST_PWRITE_IN parameter;

    FUNC_ENTER();

    parameter.Byte = Value;

    cbm_ioctl(HandleDevice, CBMCTRL(PARBURST_WRITE), &parameter, sizeof(parameter), NULL, 0);

    FUNC_LEAVE();
}

/*! \brief PARBURST: Read a complete track

 This function is a helper function for parallel burst:
 It reads a complete track from the disk

 \param HandleDevice
   A CBM_FILE which contains the file handle of the driver.

 \param Buffer
   Pointer to a buffer which will hold the bytes read.

 \param Length
   The length of the Buffer.

 \return
   != 0 on success.

 If cbm_driver_open() did not succeed, it is illegal to 
 call this function.
*/

int
cbmarch_parallel_burst_read_track(CBM_FILE HandleDevice, __u_char *Buffer, unsigned int Length)
{
    CBMT_PARBURST_READ_TRACK_OUT *result;
    int bufferlength = Length + sizeof(CBMT_PARBURST_READ_TRACK_OUT);
    int retval = 0;

    FUNC_ENTER();

    result = GlobalAlloc(GMEM_FIXED, bufferlength);

    if (result)
    {
        retval = cbm_ioctl(HandleDevice, CBMCTRL(PARBURST_READ_TRACK),
            NULL, 0,
            result, bufferlength);

        if (retval == 0)
        {
            DBG_WARN((DBG_PREFIX "opencbm: cbm.c: parallel_burst_read_track: ioctl returned with error %u", retval));
        }
        else
        {
            memcpy(Buffer, result->Buffer, Length);
        }

        GlobalFree(result);
    }

    FUNC_LEAVE_INT(retval);
}

/*! \brief PARBURST: Write a complete track

 This function is a helper function for parallel burst:
 It writes a complete track to the disk

 \param HandleDevice
   A CBM_FILE which contains the file handle of the driver.

 \param Buffer
   Pointer to a buffer which hold the bytes to be written.

 \param Length
   The length of the Buffer.

 \return
   != 0 on success.

 If cbm_driver_open() did not succeed, it is illegal to 
 call this function.
*/

int
cbmarch_parallel_burst_write_track(CBM_FILE HandleDevice, __u_char *Buffer, unsigned int Length)
{
    CBMT_PARBURST_WRITE_TRACK_IN *parameter;

    int bufferlength = Length + sizeof(CBMT_PARBURST_WRITE_TRACK_IN);

    int retval = 0;

    FUNC_ENTER();

    parameter = GlobalAlloc(GMEM_FIXED, bufferlength);

    if (parameter)
    {
        memcpy(parameter->Buffer, Buffer, Length);

        retval = cbm_ioctl(HandleDevice, CBMCTRL(PARBURST_WRITE_TRACK),
            parameter, bufferlength,
            NULL, 0);

        if (retval == 0)
        {
            DBG_WARN((DBG_PREFIX "opencbm: cbm.c: parallel_burst_write_track: ioctl returned with error %u", retval));
        }

        GlobalFree(parameter);
    }

    FUNC_LEAVE_INT(retval);
}
