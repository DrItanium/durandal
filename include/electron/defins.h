   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*               CLIPS Version 6.30  08/22/14          */
   /*                                                     */
   /*                                                     */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Brian L. Dantes                                      */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.23: Corrected compilation errors for files         */
/*            generated by constructs-to-c. DR0861           */
/*                                                           */
/*      6.24: Renamed BOOLEAN macro type to intBool.         */
/*                                                           */
/*      6.30: Removed conditional code for unsupported       */
/*            compilers/operating systems (IBM_MCW,          */
/*            MAC_MCW, and IBM_TBC).                         */
/*                                                           */
/*            GetConstructNameAndComment API change.         */
/*                                                           */
/*            Added const qualifiers to remove C++           */
/*            deprecation warnings.                          */
/*                                                           */
/*            Converted API macros to function calls.        */
/*                                                           */
/*************************************************************/

#ifndef _H_defins
#define _H_defins

#if DEFINSTANCES_CONSTRUCT

struct definstances;

#ifndef _H_conscomp
#include "conscomp.h"
#endif
#ifndef _H_constrct
#include "constrct.h"
#endif
#ifndef _H_cstrccom
#include "cstrccom.h"
#endif
#ifndef _H_moduldef
#include "moduldef.h"
#endif
#ifndef _H_object
#include "object.h"
#endif

typedef struct definstancesModule
  {
   struct defmoduleItemHeader header;
  } DEFINSTANCES_MODULE;

typedef struct definstances
  {
   struct constructHeader header;
   unsigned busy;
   EXPRESSION *mkinstance;
  } DEFINSTANCES;

#define DEFINSTANCES_DATA 22

struct definstancesData
  { 
   struct construct *DefinstancesConstruct;
   int DefinstancesModuleIndex;
#if CONSTRUCT_COMPILER && (! RUN_TIME)
   struct CodeGeneratorItem *DefinstancesCodeItem;
#endif
  };

#define DefinstancesData(theEnv) ((struct definstancesData *) GetEnvironmentData(theEnv,DEFINSTANCES_DATA))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _DEFINS_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE const char                    *EnvDefinstancesModule(void *,void *);
   LOCALE const char                    *EnvDefinstancesModuleName(void *,void *);
   LOCALE void                          *EnvFindDefinstances(void *,const char *);
   LOCALE void                           EnvGetDefinstancesList(void *,DATA_OBJECT *,struct defmodule *);
   LOCALE const char                    *EnvGetDefinstancesName(void *,void *);
   LOCALE SYMBOL_HN                     *EnvGetDefinstancesNamePointer(void *,void *);
   LOCALE const char                    *EnvGetDefinstancesPPForm(void *,void *);
   LOCALE void                          *EnvGetNextDefinstances(void *,void *);
   LOCALE int                            EnvIsDefinstancesDeletable(void *,void *);
   LOCALE void                           EnvSetDefinstancesPPForm(void *,void *,const char *);
   LOCALE intBool                        EnvUndefinstances(void *,void *);
   LOCALE void                           GetDefinstancesListFunction(void *,DATA_OBJECT *);
   LOCALE void                          *GetDefinstancesModuleCommand(void *);
   LOCALE void                           SetupDefinstances(void *);
   LOCALE void                           UndefinstancesCommand(void *);
#if DEBUGGING_FUNCTIONS
   LOCALE void                           PPDefinstancesCommand(void *);
   LOCALE void                           ListDefinstancesCommand(void *);
   LOCALE void                           EnvListDefinstances(void *,const char *,struct defmodule *);
#endif

#if ALLOW_ENVIRONMENT_GLOBALS

   LOCALE const char                    *DefinstancesModule(void *);
   LOCALE void                          *FindDefinstances(const char *);
   LOCALE void                           GetDefinstancesList(DATA_OBJECT *,struct defmodule *);
   LOCALE const char                    *GetDefinstancesName(void *);
   LOCALE const char                    *GetDefinstancesPPForm(void *,void *);
   LOCALE void                          *GetNextDefinstances(void *);
   LOCALE int                            IsDefinstancesDeletable(void *);
#if DEBUGGING_FUNCTIONS
   LOCALE void                           ListDefinstances(const char *,struct defmodule *);
#endif
   LOCALE intBool                        Undefinstances(void *);

#endif /* ALLOW_ENVIRONMENT_GLOBALS */

#endif /* DEFINSTANCES_CONSTRUCT */

#endif /* _H_defins */



