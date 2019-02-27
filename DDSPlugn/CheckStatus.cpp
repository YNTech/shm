
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
 * LOGICAL_NAME:    CheckStatus.cpp
 * FUNCTION:        OpenSplice Tutorial example code.
 * MODULE:          Tutorial for the C++ programming language.
 * DATE             june 2007.
 ************************************************************************
 * 
 * This file contains the implementation for the error handling operations.
 * 
 ***/

#include "CheckStatus.h"
#include <string>
using namespace std;

/* Array to hold the names for all ReturnCodes. */
string RetCodeName[13] = 
{
  "DDS_RETCODE_OK", "DDS_RETCODE_ERROR", "DDS_RETCODE_UNSUPPORTED", 
    "DDS_RETCODE_BAD_PARAMETER", "DDS_RETCODE_PRECONDITION_NOT_MET", 
    "DDS_RETCODE_OUT_OF_RESOURCES", "DDS_RETCODE_NOT_ENABLED", 
    "DDS_RETCODE_IMMUTABLE_POLICY", "DDS_RETCODE_INCONSISTENT_POLICY", 
    "DDS_RETCODE_ALREADY_DELETED", "DDS_RETCODE_TIMEOUT", "DDS_RETCODE_NO_DATA",
    "DDS_RETCODE_ILLEGAL_OPERATION"
};

/**
 * Returns the name of an error code.
 **/
string getErrorName(DDS::ReturnCode_t status)
{
  return RetCodeName[status];
}

/**
 * Check the return status for errors. If there is an error, then terminate.
 **/
bool checkStatus(DDS::ReturnCode_t status, const char *info,string* errString)
{


  if (status != DDS::RETCODE_OK && status != DDS::RETCODE_NO_DATA)
  {
	  if(errString)
		  *errString = string("Error in ")+info+":"+getErrorName(status);
    return false;
  }
  return true;
}

/**
 * Check whether a valid handle has been returned. If not, then terminate.
 **/
bool checkHandle(void *handle, const char* info,string* errString)
{

  if (!handle)
  {
	if(errString)
		*errString = string("Error in ")+info+": Creation failed: invalid handle";
    //exit(1);
	return false;
  }
  return true;
}
