#define INTUI_V36_NAMES_ONLY

#include <proto/utility.h>
#include <clib/exec_protos.h>

#include "rkrmsupport.h"
#include "windowlist.h"

struct TagItem *MakeNewTagList( ULONG data, ...)
{
	struct TagItem *tags = (struct TagItem *) &data;
	
	return ( CloneTagItems( tags ) );
}


BOOL listIsEmpty ( struct List *list )
{
	if ( list->lh_TailPred == (struct Node *)list ) 
		return TRUE;
	else
		return FALSE;
} 

VOID StripIntuiMessages( struct MsgPort *mp, struct Window *win )
{
	struct IntuiMessage *msg;
	struct Node *succ;
	
	msg = (struct IntuiMessage *)mp->mp_MsgList.lh_Head;
	
	while ( succ = msg->ExecMessage.mn_Node.ln_Succ )
	{
		if ( msg->IDCMPWindow == win )
		{
			/* Intuition is about to free this message.
			** Make sure that we politely send it back.
			*/
			Remove( (struct Node *)msg );
			
			ReplyMsg( (struct Message *)msg );
		}
		msg = (struct IntuiMessage *)succ;
	}
}

VOID CloseWindowSafely( struct Window *win, BOOL removeFromList )
{
//	Printf( "CloseWindowSafely: %s\n", win->Title );

	ClearMenuStrip( win );
	
	if ( removeFromList )
		removeWindowFromList( win );

	/* we forbid here to keep out of race conditions with Intuition */
	Forbid();
	
	/* send back any messages for this window that have not been processed */
	StripIntuiMessages( win->UserPort, win );
	
	/* Clear UserPort so Intuition will not free it */
	win->UserPort = NULL;
	
	/* tell Intuition to stop sending more messages */
	ModifyIDCMP( win, 0L );
	
	/* turn multitasking back on */
	Permit();
	
	/* now it's really safe to close the window */
	CloseWindow( win );
}

ULONG numItemsInList( struct List *list )
{
	ULONG i = 0;
	struct Node *node;
	
	if( list->lh_TailPred == (struct Node *)list )
		/* list is empty */
		;
	else 
	for( node = list->lh_Head ; node->ln_Succ ; node = node->ln_Succ )
	{
		i++;
	}

	return i;
}		