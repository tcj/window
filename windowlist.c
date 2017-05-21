#include "windowlist.h"

struct List *winList; 

VOID removeWindowFromList ( struct Window *win )
{
	struct Node *node;
	
	if( NULL != ( node = FindName( winList, win->Title ) ) )
	{	
		Remove( node );
	}
}

VOID freeWinNode( struct WinNode *winNode )
{
//	printf("freeWinNode()\n");
	FreeVec( winNode->wn_Name );
	FreeTagItems( winNode->wn_Tags );
	FreeVec( winNode );
}



BOOL setupWinList ( void )
{
	winList = AllocVec( sizeof( struct List ), MEMF_PUBLIC | MEMF_CLEAR );
	if ( winList != NULL )
	{
		NewList( winList );
		return TRUE;
	}
	else
		return FALSE;
}

BOOL freeWinList ( void )
{
	struct WinNode *worknode; 
	struct WinNode *nextnode;
	
	worknode = (struct WinNode *)(winList->lh_Head);
	while ( nextnode = (struct WinNode *)(worknode->wn_Node.ln_Succ) )
	{
		if( NULL != ((struct WinNode *)worknode)->wn_Win )
			CloseWindowSafely( ((struct WinNode *)worknode)->wn_Win, FALSE );
		freeWinNode( (struct WinNode *)worknode );
		worknode = nextnode;
	}
	FreeVec( winList );
	return TRUE;
}


BOOL addWinNodeToList( struct WinNode *winNode )
{
	BOOL success = FALSE;
	
//	Printf("addWinNodeToList\n");
	
	if( winNode != NULL )
	{
		winNode->wn_Node.ln_Name = winNode->wn_Name;
		winNode->wn_Node.ln_Type = WINDOWNODE_ID;
		winNode->wn_Node.ln_Pri  = 0;
		
		AddTail( winList, (struct Node *)winNode );
		
		success = TRUE;
	}
	
	return success;
}

/*

BOOL addWindowToList ( struct Window *win, struct List *list )
{
	BOOL success = FALSE;
	struct WinNode *winNode;
	ULONG titleLength;
	
	Printf("addWindowToList()\n");
	
	if ( win != NULL )
	{
		Printf("win != NULL\n");
		winNode = AllocVec( sizeof( struct WinNode ), MEMF_PUBLIC | MEMF_CLEAR );
		if ( winNode != NULL )
		{
			Printf("winNode != NULL\n");
			titleLength = strlen( win->Title );
			if ( NULL != ( winNode->wn_Name = AllocVec( titleLength + 1 , MEMF_PUBLIC | MEMF_CLEAR ) ) );
			{
				strncpy( winNode->wn_Name, title, titleLength );
				winNode->wn_Win = win;
				
				addWinNodeToList( winNode, list );
				
				Printf( "Added window to list, with name %s.\n", winNode->wn_Name );
				success = TRUE;
			}
		}
	}
	return success;
}

*/
