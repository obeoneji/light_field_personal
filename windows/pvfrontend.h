/****************************************************************************
 *                pvfrontend.h
 *
 * from Persistence of Vision(tm) Ray Tracer version 3.6.
 * Copyright 1991-2003 Persistence of Vision Team
 * Copyright 2003-2004 Persistence of Vision Raytracer Pty. Ltd.
 *---------------------------------------------------------------------------
 * NOTICE: This source code file is provided so that users may experiment
 * with enhancements to POV-Ray and to port the software to platforms other
 * than those supported by the POV-Ray developers. There are strict rules
 * regarding how you are permitted to use this file. These rules are contained
 * in the distribution and derivative versions licenses which should have been
 * provided with this file.
 *
 * These licences may be found online, linked from the end-user license
 * agreement that is located at http://www.povray.org/povlegal.html
 *---------------------------------------------------------------------------
 * This program is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 *---------------------------------------------------------------------------
 * $File: //depot/povray/3.5/windows/pvfrontend.h $
 * $Revision: #9 $
 * $Change: 2933 $
 * $DateTime: 2004/07/03 13:06:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef PVFRONTEND_H
#define PVFRONTEND_H

#include "renderfrontend.h"
#include "processrenderoptions.h"

BEGIN_POV_FRONTEND_NAMESPACE

USING_POV_BASE_NAMESPACE

class WinRenderFrontend : public RenderFrontend
{
	public:
		WinRenderFrontend (POVMSContext, POVMSAddress) ;
		~WinRenderFrontend () ;
    void PrintHelpScreens () ;
    void PrintUsage (int n) ;
    void PrintToStatisticsStream (char *format, ...) ;

	protected:
		void RenderStarted (POVMS_Message&, POVMS_Message&, int) ;
		void RenderFinished (POVMS_Message&, POVMS_Message&, int) ;
    void RenderProgress (POVMSObjectPtr msg, POVMSObjectPtr, int) ;
    void RenderOptions (POVMSObjectPtr msg, POVMSObjectPtr, int) ;
    void RenderStatistics (POVMSObjectPtr msg, POVMSObjectPtr, int) ;
    void FatalError (POVMSObjectPtr msg, POVMSObjectPtr, int) ;
    void InitInfo (POVMSObjectPtr msg, POVMSObjectPtr, int) ;
    void FlushStreams (void) ;
    static bool CollectStats ;
    static int StatLine ;

    class WinStreamBuffer : public TextStreamBuffer
    {
      public:
        WinStreamBuffer (OTextStream *h, msgtype mtype, bool l, bool i) ;
        ~WinStreamBuffer () ;
        virtual void lineoutput (const char *str, unsigned int chars) ;
        virtual void directoutput (const char *str, unsigned int chars) ;
        virtual void rawoutput (const char *str, unsigned int chars) ;
      private:
        OTextStream *handle;
        bool linebuffermode;
        bool inhibitmode;
        msgtype type ;
        char raw_buffer [1024] ;
    } ;
    virtual void OpenStreams (bool) ;
    virtual void CloseStreams (void) ;
} ;

class WinProcessRenderOptions : public ProcessRenderOptions
{
	public:
		WinProcessRenderOptions();
		~WinProcessRenderOptions();
	protected:
		virtual void ParseError(const char *, ...);
		virtual void ParseErrorAt(ITextStream *, const char *, ...);
		virtual void WriteError(const char *, ...);
};

END_POV_FRONTEND_NAMESPACE

#endif
