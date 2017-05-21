#include "inspector.h"

BPTR inspector;
struct TagItem *idcmpNameTags;

const char *idcmpNames[] = 
{
	"IDCMP_SIZEVERIFY",
	"IDCMP_NEWSIZE",
	"IDCMP_REFRESHWINDOW",
	"IDCMP_MOUSEBUTTONS",
	"IDCMP_MOUSEMOVE",
	"IDCMP_GADGETDOWN",
	"IDCMP_GADGETUP",
	"IDCMP_REQSET",
	"IDCMP_MENUPICK",
	"IDCMP_CLOSEWINDOW",
	"IDCMP_RAWKEY",
	"IDCMP_REQVERIFY",
	"IDCMP_REQCLEAR",
	"IDCMP_MENUVERIFY",
	"IDCMP_NEWPREFS",
	"IDCMP_DISKINSERTED",
	"IDCMP_DISKREMOVED",
	"IDCMP_WBENCHMESSAGE",
	"IDCMP_ACTIVEWINDOW",
	"IDCMP_INACTIVEWINDOW",
	"IDCMP_DELTAMOVE",
	"IDCMP_VANILLAKEY",
	"IDCMP_INTUITICKS",
	"IDCMP_IDCMPUPDATE",
	"IDCMP_MENUHELP",
	"IDCMP_CHANGEWINDOW",
	"IDCMP_GADGETHELP",
	"IDCMP_LONELYMESSAGE"
};

#define NUM_TAG_ITEMS 29

BPTR openInspector( void )
{
//	ULONG i;
//	UWORD j;
	
	inspector = Open("CON:385/18/200/200/Inspector/CLOSE", MODE_OLDFILE);
/*
	idcmpNameTags = MakeNewTagList(
	 IDCMP_SIZEVERIFY, 		(ULONG)"IDCMP_SIZEVERIFY" ,
	 IDCMP_NEWSIZE, 			(ULONG)"IDCMP_NEWSIZE" ,
	 IDCMP_REFRESHWINDOW, 	(ULONG)"IDCMP_REFRESHWINDOW" ,
	 IDCMP_MOUSEBUTTONS, 	(ULONG)"IDCMP_MOUSEBUTTONS"	,
	 IDCMP_MOUSEMOVE, 		(ULONG)"IDCMP_MOUSEMOVE"	,
	 IDCMP_GADGETDOWN,		(ULONG)"IDCMP_GADGETDOWN" ,
	 IDCMP_GADGETUP, 		(ULONG)"IDCMP_GADGETUP" ,
	 IDCMP_REQSET, 			(ULONG)"IDCMP_REQSET" ,
	 IDCMP_MENUPICK, 		(ULONG)"IDCMP_MENUPICK" ,
	 IDCMP_CLOSEWINDOW, 	(ULONG)"IDCMP_CLOSEWINDOW" ,
	 IDCMP_RAWKEY, 			(ULONG)"IDCMP_RAWKEY" ,
	 IDCMP_REQVERIFY,		(ULONG)"IDCMP_REQVERIFY" ,
	 IDCMP_REQCLEAR, 		(ULONG)"IDCMP_REQCLEAR" ,
	 IDCMP_MENUVERIFY, 		(ULONG)"IDCMP_MENUVERIFY" ,
	 IDCMP_NEWPREFS,			(ULONG)"IDCMP_NEWPREFS" ,
	 IDCMP_DISKINSERTED, 	(ULONG)"IDCMP_DISKINSERTED" ,
	 IDCMP_DISKREMOVED, 	(ULONG)"IDCMP_DISKREMOVED" ,
	 IDCMP_WBENCHMESSAGE,	(ULONG)"IDCMP_WBENCHMESSAGE" ,
	 IDCMP_ACTIVEWINDOW, 	(ULONG)"IDCMP_ACTIVEWINDOW" ,
	 IDCMP_INACTIVEWINDOW, (ULONG)"IDCMP_INACTIVEWINDOW" ,
	 IDCMP_DELTAMOVE,		(ULONG)"IDCMP_DELTAMOVE" ,
	 IDCMP_VANILLAKEY, 		(ULONG)"IDCMP_VANILLAKEY" ,
	 IDCMP_INTUITICKS, 		(ULONG)"IDCMP_INTUITICKS" ,
	 IDCMP_IDCMPUPDATE,		(ULONG)"IDCMP_IDCMPUPDATE" ,
	 IDCMP_MENUHELP, 		(ULONG)"IDCMP_MENUHELP" ,
	 IDCMP_CHANGEWINDOW, 	(ULONG)"IDCMP_CHANGEWINDOW" ,
	 IDCMP_GADGETHELP,		(ULONG)"IDCMP_GADGETHELP" ,
	 IDCMP_LONELYMESSAGE, 	(ULONG)"IDCMP_LONELYMESSAGE" ,
	 TAG_DONE, NULL );
*/

/*
	if( idcmpNameTags = AllocateTagItems( NUM_TAG_ITEMS ) )
	{
		Printf( "Allocation success.\n" );
		for ( i = 0; i < (NUM_TAG_ITEMS - 1) ; i++ )
		{
			idcmpNameTags[i].ti_Tag = i;
			idcmpNameTags[i].ti_Data = (ULONG) idcmpNames[i];
			Printf("%ld ", i);
		};
		Printf("\nDONE, i = %ld\n", i);
		idcmpNameTags[i].ti_Tag = TAG_DONE;
		
		Printf("tag #8: %ld, %s\n", idcmpNameTags[8].ti_Tag, 
						idcmpNameTags[8].ti_Data );
	};
*/

	return inspector;
}

void closeInspector( void )
{
	if (inspector != NULL)
		Close(inspector);
		
//	FreeTagItems( idcmpNameTags );
}

void inspectWindow ( struct Window *win )
{
	ULONG i;
	UWORD j;
	
	FPrintf( inspector, "c" );
	FPrintf( inspector, "width: %ld\n", win->Width );
	FPrintf( inspector, "height: %ld\n", win->Height );
	FPrintf( inspector, "leftEdge: %ld\n", win->LeftEdge );
	FPrintf( inspector, "topEdge: %ld\n", win->TopEdge );
	FPrintf( inspector, "IDCMP Flags:\n" );
/*	
	if( win->IDCMPFlags & IDCMP_SIZEVERIFY )
		FPrintf( inspector, " IDCMP_SIZEVERIFY\n" );
	if( win->IDCMPFlags & IDCMP_NEWSIZE )
		FPrintf( inspector, " IDCMP_NEWSIZE\n" );
	if( win->IDCMPFlags & IDCMP_REFRESHWINDOW )
		FPrintf( inspector, " IDCMP_REFRESHWINDOW\n");
	if( win->IDCMPFlags & IDCMP_MOUSEBUTTONS )
		FPrintf( inspector, " IDCMP_MOUSEBUTTONS\n" );
	if( win->IDCMPFlags & IDCMP_MOUSEMOVE )
		FPrintf( inspector, " IDCMP_MOUSEMOVE\n" );		
	if( win->IDCMPFlags & IDCMP_GADGETDOWN )
		FPrintf( inspector, " IDCMP_GADGETDOWN\n");
	if( win->IDCMPFlags & IDCMP_MOUSEBUTTONS )
		FPrintf( inspector, " IDCMP_GADGETUP\n" );
	if( win->IDCMPFlags & IDCMP_GADGETUP )
		FPrintf( inspector, " IDCMP_GADGETUP\n" );
*/

	for( i = IDCMP_SIZEVERIFY, j = 0 ; i < IDCMP_LONELYMESSAGE ; i <<= 1, j++ )
	{
		if( win->IDCMPFlags & i )
			FPrintf( inspector, " %s\n", idcmpNames[j] );
	}
	
}
