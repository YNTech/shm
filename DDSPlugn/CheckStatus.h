
/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2012 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

/************************************************************************
 * LOGICAL_NAME:    CheckStatus.h
 * FUNCTION:        OpenSplice Tutorial example code.
 * MODULE:          Tutorial for the C++ programming language.
 * DATE             june 2007.
 ************************************************************************
 * 
 * This file contains the headers for the error handling operations.
 * 
 ***/

#ifndef __CHECKSTATUS_H__
#define __CHECKSTATUS_H__

#include "ccpp_dds_dcps.h"
#include <iostream>
#include "ddalib_export.h"

  using namespace std;

  /**
   * Returns the name of an error code.
   **/
   string getErrorName(DDS::ReturnCode_t status);

  /**
   * Check the return status for errors. If there is an error, then terminate.
   **/
   bool checkStatus(DDS::ReturnCode_t status, const char *info,string* errString=0);

  /**
   * Check whether a valid handle has been returned. If not, then terminate.
   **/
   bool checkHandle(void *handle, const char* info,string* errString=0);

#endif
