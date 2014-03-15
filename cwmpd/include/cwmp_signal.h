/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014 netcwmp group                         *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMP_SIGNAL_H__
#define __CWMP_SIGNAL_H__

#define cwmp_signal_helper(n)     SIG##n
#define cwmp_signal_value(n)      cwmp_signal_helper(n)

#ifdef WIN32
#define CWMP_SIG_SHUTDOWN    		TERM
#else
#define CWMP_SIG_SHUTDOWN    		QUIT
#endif
#define CWMP_SIG_TERMINATE    		TERM
#define CWMP_SIG_NOACCEPT      		WINCH
#define CWMP_SIG_RECONFIGURE   	HUP


#if (CWMP_LINUXTHREADS)
#define CWMP_SIG_REOPEN						INFO
#define CWMP_SIG_CHANGEBIN				XCPU
#else
#define CWMP_SIG_REOPEN        				USR1
#define CWMP_SIG_CHANGEBIN				USR2
#endif

#endif
