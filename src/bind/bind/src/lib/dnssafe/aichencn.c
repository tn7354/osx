/* Copyright (C) RSA Data Security, Inc. created 1993, 1996.  This is an
   unpublished work protected as such under copyright law.  This work
   contains proprietary, confidential, and trade secret information of
   RSA Data Security, Inc.  Use, disclosure or reproduction without the
   express written authorization of RSA Data Security, Inc. is
   prohibited.
 */

#include "port_before.h"
#include "global.h"
#include "bsafe2.h"
#include "bkey.h"
#include "balg.h"
#include "aichencn.h"
#include "port_after.h"

B_AlgorithmInfoTypeVTable AITChooseEncryptNull_V_TABLE =
  {AITNullAddInfo, AITChooseEncryptNewHandler, B_AlgorithmInfoTypeMakeError};

