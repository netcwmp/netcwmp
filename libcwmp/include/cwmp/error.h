/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2008-2010  netcwmp.kongdai.com                         *
 *                                                                      *
 * Copyright 2008-2010           Mr.x() <azhenglive@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMPERROR_H__
#define __CWMPERROR_H__




#define CWMP_OK  0
#define CWMP_ERROR -1
#define CWMP_SOCKCLOSEERR  (CWMP_ERROR -1)
#define CWMP_BADFUNCARG	(CWMP_ERROR -2)
#define CWMP_ENOMEM		(CWMP_ERROR -3)
#define CWMP_COULDNOT_CONNECT   (CWMP_ERROR -4)
#define CWMP_TIMEOUT		(CWMP_ERROR -5)



#define CWMP_TRUE			1
#define CWMP_FALSE			0

#define CWMP_YES			1
#define CWMP_NO				0

#endif

