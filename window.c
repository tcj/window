
#define INTUI_V36_NAMES_ONLY

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

#include <clib/alib_stdio_protos.h>
#include <exec/memory.h>
#include <prefs/prefhdr.h>

#include <strings.h>

#include "rkrmsupport.h"
#include "windowlist.h"
#include "inspector.h"

#ifdef LATTICE
int CXBRK(void)		{ return(0); }
int chkabort(void)	{ return(0); }
#endif

#define ID_WDST	MAKE_ID('W','D','S','T')
#define ID_WIND	MAKE_ID('W','I','N','D')

#define MAX_TITLE_LENGTH	64

#define MYGAD_BUTTON 		(4)
#define MYGAD_CHECKBOX_1_1	(5)
#define MYGAD_CHECKBOX_1_2 (6)
#define MYGAD_CHECKBOX_1_3 (7)
#define MYGAD_CHECKBOX_1_4 (8)
#define MYGAD_CHECKBOX_1_5 (9)

BPTR stdout;

UBYTE vers[] = "$VER: window 10.01";
UBYTE win_title[] = "New Window";

struct Menu *menuStrip;
struct Window *paletteWin;
struct MsgPort *msgPort;
struct Screen *myScreen;

struct NewGadget myButtonGad;
struct Gadget *glist = NULL;
struct Gadget *pgad;

struct TextAttr Topaz80 = { "topaz.font", 8, 0, 0, };
APTR *my_VisualInfo;
	
struct TagItem win_tags[] = 
{
	{	WA_Left, 			20		},
	{	WA_Top,				10		},
	{	WA_Width,			300	},
	{	WA_Height,			110	},
	{	WA_MinWidth,		0		},
	{  WA_MinHeight,		0		},
	{	WA_MaxWidth,		640	},
	{	WA_MaxHeight,		480	},
	{	WA_CloseGadget,	TRUE	},
	{	WA_SizeGadget,		TRUE	},
//	{	WA_Title,			&win_title },	
//	{	WA_IDCMP,			IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | 
//			IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | IDCMP_MENUPICK },
	{	WA_IDCMP,			NULL	},
	{	WA_DragBar,			TRUE	},
	{	WA_Activate,		FALSE },
	{	TAG_DONE,			NULL	},
};

struct TagItem paletteWin_tags[] = 
{
	{	WA_Left, 			18		},
	{	WA_Top,				268	},
	{	WA_Width,			567	},
	{	WA_Height,			110	},
	{	WA_MinWidth,		0		},
	{  WA_MinHeight,		0		},
	{	WA_MaxWidth,		0		},
	{	WA_MaxHeight,		0		},
	{	WA_CloseGadget,	TRUE	},
	{	WA_SizeGadget,		FALSE	},
//	{	WA_Title,			&win_title },	
//	{	WA_IDCMP,			IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | 
//			IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | IDCMP_MENUPICK },
	{	WA_IDCMP,			NULL	},
	{	WA_DragBar,			TRUE	},
	{	WA_Activate,		TRUE	},
	{	TAG_DONE,			NULL	},
};

struct NewMenu paletteMenu[] =
{
	{ NM_TITLE,		"Project",		0 , 0, 0, 0,},
	{  NM_ITEM,        "New",	  "N", 0, 0, 0,},
	{  NM_ITEM,			"Open",    "O", 0, 0, 0,},
	{  NM_ITEM,			"Save",	  "S", 0, 0, 0,},
	{  NM_ITEM,       "Quit",	  "Q", 0, 0, 0,},
		
	{ NM_TITLE,		"Special",	   0 , 0, 0, 0,},
	{  NM_ITEM, "Save Palette Window",0,0,0,0,},	
	{   NM_END,			 NULL ,		0 , 0, 0, 0,},
};

#define FILEREQ_HEIGHT	220
#define FILEREQ_WIDTH	120
#define FILEREQ_LEFT		20
#define FILEREQ_TOP		20

struct TagItem fileOpenTags[] =
{
	ASL_Hail, 		(ULONG)"Open",
	ASL_Height,		FILEREQ_HEIGHT,
	ASL_Width,		FILEREQ_WIDTH,
	ASL_LeftEdge,	FILEREQ_LEFT,
	ASL_TopEdge,	FILEREQ_TOP,
	ASL_Dir,			(ULONG)"PROGDIR:",
	TAG_DONE
};

enum { PROJECT_NEW, PROJECT_OPEN, PROJECT_SAVE, PROJECT_QUIT };

struct WindowStore {
	WORD leftEdge;
	WORD topEdge;
	
	WORD width;
	WORD height;
	
	ULONG IDCMPFlags;
	ULONG flags;
};


struct Window *newWindowFromStoreFile( char *fn )
{
	struct Window *newWin;
	
	struct IFFHandle *iff;
	struct StoredProperty *sp;
	struct WindowStore *wndStore;	

	LONG error = 0;
	

	iff = AllocIFF();
	if ( iff != NULL )
	{
		if ( iff->iff_Stream = Open( fn, MODE_OLDFILE ) )
		{
			InitIFFasDOS( iff );
			if ( error = OpenIFF( iff, IFFF_READ ) )
			{
				Printf("OpenIFF for read failed.\n");
			}
			else
			{
				Printf("Opening IFF for read.\n");
				
				PropChunk( iff, ID_WDST, ID_WIND);
			for (;;)
				{
					error = ParseIFF( iff, IFFPARSE_STEP );
					
					if( error == IFFERR_EOC )
						continue;
					else if( error )
						break;
						
					if ( sp = FindProp( iff, ID_WDST, ID_WIND ) )
					{
						Printf("ID_WIND found!\n");
						wndStore = (struct WindowStore *)sp->sp_Data;
						newWin = OpenWindowTags( NULL,
							WA_Left, 		wndStore->leftEdge,
							WA_Top,			wndStore->topEdge,
							WA_Width,		wndStore->width,
							WA_Height,		wndStore->height,
							WA_IDCMP,		wndStore->IDCMPFlags,
							WA_Flags,		wndStore->flags,
							WA_PubScreenName, NULL,
							TAG_END );
					}
					else
					{
						Printf("ID_WDST not found.\n");
					}
				}
				CloseIFF( iff );
			}
			Close( iff->iff_Stream );
		}
		FreeIFF( iff );
	}

	return newWin;
}

BOOL writeWindowPrefs( struct Window *win, char *fn )
{
	struct IFFHandle *iff;
	LONG error = 0;

	struct ContextNode *cn;
	struct WindowStore *wndStore;
	struct PrefHeader prefHead;
	UBYTE idbuf[5];
	
	prefHead.ph_Version = NULL;
	prefHead.ph_Type = NULL;
	prefHead.ph_Flags = NULL;
	
	wndStore = AllocVec( sizeof( struct WindowStore ), MEMF_PUBLIC | MEMF_CLEAR );
	if (wndStore)
	{
		if (win)
		{
			wndStore->topEdge 		= win->TopEdge;
			wndStore->leftEdge 		= win->LeftEdge;
			wndStore->width 			= win->Width;
			wndStore->height 			= win->Height;
		
			wndStore->IDCMPFlags 	= win->IDCMPFlags;
			
			wndStore->flags		 	= win->Flags;
		}
	}

	iff = AllocIFF();
	if ( iff != NULL )
	{
		if ( iff->iff_Stream = Open( fn, MODE_NEWFILE ) )
		{
			InitIFFasDOS( iff );
			if ( error = OpenIFF( iff, IFFF_WRITE ) )
			{
				Printf("OpenIFF for write failed.\n");
			}
			else
			{
				if ( !( error = PushChunk( iff, ID_WDST, ID_FORM, IFFSIZE_UNKNOWN ) ) )
				{
					cn = CurrentChunk( iff );
					IDtoStr(cn->cn_ID, (STRPTR)&idbuf);
					Printf( "ID of current chunk: %s\n", idbuf);
					
					error = PushChunk( iff, 0, ID_WIND, sizeof( struct WindowStore ) );
					if (error)
						Printf( "Error %ld: PushChunk(ID_WIND)\n", error );
					else
					{
						cn = CurrentChunk( iff );
						IDtoStr(cn->cn_ID, (STRPTR)&idbuf);
						Printf( "ID of current chunk: %s\n", idbuf);
						
						WriteChunkBytes( iff, 
									wndStore, sizeof( struct WindowStore ) );
						PopChunk( iff );
					}
					PopChunk( iff );
				}
				else
					Printf( "Error %ld: PushChunk(ID_WDST)\n", error );
				CloseIFF( iff );
				Close( iff->iff_Stream );
			}
			FreeIFF( iff );
		}
	}

	if (wndStore)
		FreeVec(wndStore);
		
	return TRUE;
}		
	

BOOL openFileFromASL( struct Window *win, struct FileRequester **myFReq )
{
	BOOL result = FALSE;
	struct FileRequester *tmpFReq;
	
	if( AslBase = OpenLibrary( "asl.library", 37L ) )
	{
		if ( tmpFReq = (struct FileRequester *)AllocAslRequestTags(
							ASL_FileRequest,
							ASL_Hail,		(ULONG)"Open",
							ASL_Dir,			(ULONG)"PROGDIR:",
							ASL_Window,		win,
							TAG_DONE ) )
		{
			if( AslRequest( tmpFReq, NULL ) )
			{
				*myFReq = AllocVec( sizeof( struct FileRequester ), MEMF_PUBLIC | MEMF_CLEAR );
				CopyMem( tmpFReq, *myFReq, sizeof( struct FileRequester ) );
				Printf("Got file: %s\n", (*myFReq)->rf_File);
				result = TRUE;
			}
			FreeAslRequest( tmpFReq );
		}
		else 
			Printf( "User Cancelled\n" );
			
		CloseLibrary( AslBase );
	}
	return result;
}



BOOL addNewWindow( void )
{
	struct WinNode *winNode;
	BOOL success = FALSE;
	
	winNode = AllocVec( sizeof(struct WinNode), MEMF_PUBLIC | MEMF_CLEAR );
	if ( winNode != NULL )
	{
		winNode->wn_Name = AllocVec( MAX_TITLE_LENGTH, MEMF_PUBLIC | MEMF_CLEAR );
		if ( winNode->wn_Name != NULL )
		{
			sprintf( winNode->wn_Name, "Untitled %ld", ( numItemsInList( winList ) + 1 ) );
			winNode->wn_Tags = MakeNewTagList( WA_Title, winNode->wn_Name,
															TAG_MORE, &win_tags );
			if ( winNode->wn_Tags != NULL )
			{
				winNode->wn_Win = OpenWindowTagList( NULL, winNode->wn_Tags );
				if ( winNode->wn_Win )
				{
					winNode->wn_Win->UserPort = msgPort;
					ModifyIDCMP( winNode->wn_Win, 
						IDCMP_CLOSEWINDOW | 
						IDCMP_MENUPICK | 
						IDCMP_ACTIVEWINDOW );
					if ( NULL != SetMenuStrip( winNode->wn_Win, menuStrip ) )
					{						
						addWinNodeToList( winNode );			
						/* do we need to check result here?  probably */
						success = TRUE;
					}
					else 
						CloseWindow( winNode->wn_Win );
				}
				else FreeTagItems( winNode->wn_Tags );
			}
			else FreeVec( winNode->wn_Name );
		}
		else FreeVec( winNode );
	}
	
	if( !success )
	{
		if( winNode->wn_Win )
			CloseWindowSafely( winNode->wn_Win, FALSE );
		if( winNode->wn_Tags )
			FreeTagItems( winNode->wn_Tags );
		if( winNode->wn_Name )
			FreeVec( winNode->wn_Name );
		if( winNode )
			FreeVec( winNode );
	}
	
	return success;
}			
							

BOOL handle_window_events ( BOOL done )
{
//	struct IntuiMessage *msg;
	struct IntuiMessage *gtimsg;
	USHORT code;
	ULONG msgClass;
	struct Window *win;	
	struct Gadget *gad;
	
	BOOL deferCloseWindow = FALSE;
		
	UWORD menuNumber;
	UWORD menuNum;
	UWORD itemNum;
	UWORD subNum;
	struct MenuItem *item;

	struct FileRequester *fReq = NULL;

	WaitPort( msgPort );

	while ( NULL != ( gtimsg = (struct IntuiMessage *)GT_GetIMsg( msgPort ) ) )
	{
		deferCloseWindow = FALSE;
		
		win = gtimsg->IDCMPWindow;
		
//		Printf("window: %s\n", ( win == paletteWin ) ? "palette" : "document" );
		
//		if( gtimsg = GT_FilterIMsg( msg ) )
		{
			gad = (struct Gadget *)gtimsg->IAddress;
			code = gtimsg->Code;
			msgClass = gtimsg->Class;
			win = gtimsg->IDCMPWindow;
				
			switch( msgClass )
			{
				case IDCMP_CLOSEWINDOW:
					if ( win != paletteWin )
//						CloseWindowSafely( win, TRUE );
						/* We can't dispose of window until
						** we have replied to this intuimessage
						*/
						deferCloseWindow = TRUE;
					else
						done = TRUE;
						
					break;
				case IDCMP_NEWSIZE:
//					doNewSize();
//					doDrawStuff();
					break;
				case IDCMP_INACTIVEWINDOW:
					break;
				case IDCMP_ACTIVEWINDOW:
					inspectWindow( win );
					break;
				case IDCMP_GADGETUP:
					switch( gad->GadgetID )
					{
						case MYGAD_BUTTON:
							Printf("Button was pressed.\n");
							break;
						case MYGAD_CHECKBOX_1_1:
							Printf("checkbox: %s\n", ( gad->Flags & GFLG_SELECTED ) ?
										"ON" : "OFF" );
							break;
						default:
							Printf("unknown gadget up\n");
							break;
					}
					break;
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( win );
					GT_EndRefresh( win, TRUE );
					break;
				case IDCMP_MENUPICK:
					menuNumber = code;
					while( ( menuNumber != MENUNULL ) && ( !done ) )
					{
						item = ItemAddress( menuStrip, menuNumber );
					
						/* process the item here! */
						menuNum = MENUNUM( menuNumber );
						itemNum = ITEMNUM( menuNumber );
						subNum = SUBNUM( menuNumber ); 

						if( menuNum == 0 )
						{
							/* "Project" menu */
							
							switch( itemNum )
							{
								case PROJECT_NEW:
									/* Project -> New */
									addNewWindow();
										break;
									
								case PROJECT_OPEN:
									/* Project -> Open */
									/* TODO: add ASL file requester */
									/* TODO: add "file changed - save before closing?" */
									if ( openFileFromASL( win, &fReq ) )
									{
										Printf("freq name: %s, freq dir: %s\n", fReq->rf_File, fReq->rf_Dir );
										/* fReq is AllocVec()ed in openFileFromASL() */
										FreeVec( fReq );
									}
									break;
									
								case PROJECT_SAVE:
									/* Project -> Save */
									/* TODO: Add ASL file requester */
									break;

								case PROJECT_QUIT:
									/* stop if quit is selected. */
									done = TRUE;
									break;
								default:
									Printf("item selected: %ld, menu: %ld\n", itemNum, menuNum);
									break;
							}
						}
						else		
						if( menuNum == 1 )
						{
							/* "Special" menu */
			
							switch( itemNum )
							{
								case 0:
									break;
								default:
									break;
							}
						}
						
						menuNumber = item->NextSelect;
					}
					break;	
				default:
					Printf("Unknown IDCMP message.\n");
					break;
			}
//			GT_PostFilterIMsg(gtimsg);	
		}
		GT_ReplyIMsg( gtimsg );
		if ( deferCloseWindow )
			CloseWindowSafely( win, TRUE );		
	}
	
	return done;
}

void setupGadgets( void )
{
	ULONG topEdge = 0;

	pgad = CreateContext( &glist );

	topEdge = 4 + myScreen->WBorTop + ( myScreen->Font->ta_YSize + 1 );
							
	myButtonGad.ng_TextAttr = &Topaz80;
	myButtonGad.ng_VisualInfo = my_VisualInfo;
	myButtonGad.ng_LeftEdge = 165;
	myButtonGad.ng_TopEdge = topEdge;
	myButtonGad.ng_Width = 100;
	myButtonGad.ng_Height = 12;
	myButtonGad.ng_GadgetText = "Click Here";
	myButtonGad.ng_GadgetID = MYGAD_BUTTON;
	myButtonGad.ng_Flags = 0;

	pgad = CreateGadget( BUTTON_KIND, pgad, &myButtonGad, TAG_END );
	myButtonGad.ng_TopEdge += 14;
	myButtonGad.ng_GadgetText = (STRPTR)idcmpNames[0];
	myButtonGad.ng_GadgetID = MYGAD_CHECKBOX_1_1;

	pgad = CreateGadget( CHECKBOX_KIND, pgad, &myButtonGad, TAG_END );
	myButtonGad.ng_TopEdge += 14;
	myButtonGad.ng_GadgetText = (STRPTR)idcmpNames[1];
	myButtonGad.ng_GadgetID = MYGAD_CHECKBOX_1_2;

	pgad = CreateGadget( CHECKBOX_KIND, pgad, &myButtonGad, TAG_END );
	myButtonGad.ng_TopEdge += 14;
	myButtonGad.ng_GadgetText = (STRPTR)idcmpNames[2];
	myButtonGad.ng_GadgetID = MYGAD_CHECKBOX_1_3;

	pgad = CreateGadget( CHECKBOX_KIND, pgad, &myButtonGad, TAG_END );
	myButtonGad.ng_TopEdge += 14;
	myButtonGad.ng_GadgetText = (STRPTR)idcmpNames[3];
	myButtonGad.ng_GadgetID = MYGAD_CHECKBOX_1_4;
	
	pgad = CreateGadget( CHECKBOX_KIND, pgad, &myButtonGad, TAG_END );							
	myButtonGad.ng_TopEdge += 14;
	myButtonGad.ng_GadgetText = (STRPTR)idcmpNames[4];
	myButtonGad.ng_GadgetID = MYGAD_CHECKBOX_1_5;
	pgad = CreateGadget( CHECKBOX_KIND, pgad, &myButtonGad, TAG_END );	
}

void closeLibraries( void )
{
	if( IFFParseBase != NULL )
		CloseLibrary( IFFParseBase );
	if( GadToolsBase != NULL )
		CloseLibrary( GadToolsBase );
	if( UtilityBase != NULL )
		CloseLibrary( UtilityBase );
	if( IntuitionBase != NULL );
		CloseLibrary( (struct Library *)IntuitionBase );

}

BOOL openLibraries( void )
{
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37);
	if ( IntuitionBase != NULL )
	{
		UtilityBase = OpenLibrary( "utility.library", 37 );
		if( UtilityBase != NULL )
		{		
			GadToolsBase = OpenLibrary("gadtools.library", 37);
			if (GadToolsBase != NULL)
			{
				IFFParseBase = OpenLibrary( "iffparse.library", 37 );
				if( IFFParseBase != NULL )
				{
					return TRUE;
				}
			}
		}
	}
	closeLibraries( );
	return FALSE;
}


					
int main(int argc, char **argv)
{
	struct TagItem *newWinTags;
	BOOL done = FALSE;
	
	stdout = Output();

	if( openLibraries( ) )
	{
		if ( setupWinList( ) )
		{
			if ( msgPort = CreateMsgPort() )
			{
				msgPort->mp_Node.ln_Name = "WindowShop";
				msgPort->mp_Node.ln_Pri = 0;
				AddPort( msgPort );
			
				if( myScreen = LockPubScreen( NULL ) )
				{
					if ( NULL != ( my_VisualInfo = GetVisualInfo( myScreen, TAG_END ) ) )
					{
						setupGadgets( );
										
						if ( pgad )
						{
							if ( newWinTags = MakeNewTagList( WA_Gadgets, glist,
														TAG_MORE, &paletteWin_tags ) )
							{
								paletteWin = OpenWindowTagList( NULL, newWinTags );
								UnlockPubScreen( NULL, myScreen );
								if ( paletteWin )
								{			
									SetWindowTitles( paletteWin, "Tool Palette", "WindowShop" );
									paletteWin->UserPort = msgPort;
									ModifyIDCMP( paletteWin, 
										IDCMP_CLOSEWINDOW | 
										IDCMP_MENUPICK | 
										IDCMP_REFRESHWINDOW |
										IDCMP_ACTIVEWINDOW |
										CHECKBOXIDCMP | 
										BUTTONIDCMP );
	
									if (NULL != ( menuStrip = CreateMenus( paletteMenu, TAG_END ) ) )
									{
										if ( LayoutMenus( menuStrip, my_VisualInfo, TAG_END ) )
										{
											if ( SetMenuStrip( paletteWin, menuStrip ) )
											{
												inspector = openInspector();
												while ( !done )
												{
													GT_RefreshWindow( paletteWin, NULL );
													done = handle_window_events( done );
												}
												closeInspector();
												ClearMenuStrip( paletteWin );
											}
											FreeMenus( menuStrip );
										}
									}
									FreeVisualInfo( my_VisualInfo );
									CloseWindowSafely( paletteWin, FALSE );
								}
								FreeTagItems( newWinTags );
							}
							FreeGadgets( glist );
						}
					}
				}
				RemPort( msgPort );
				DeleteMsgPort( msgPort );
			}	
			freeWinList( );
		}
		closeLibraries( );	
	}
	return 0;
}
