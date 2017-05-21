#ifndef WINDOWLIST_H
#define WINDOWLIST_H TRUE

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <intuition/intuition.h>
#include <clib/utility_protos.h>
#include <exec/memory.h>
#include <exec/lists.h>

#include "rkrmsupport.h"

extern struct List *winList; 
#define WINDOWNODE_ID		101

struct WinNode {
	struct Node wn_Node;
	struct Window *wn_Win;
	char *wn_Name;
	struct TagItem *wn_Tags;
};

VOID removeWindowFromList ( struct Window *win );
VOID freeWinNode( struct WinNode *winNode );
BOOL setupWinList ( void );
BOOL freeWinList ( void );
BOOL addWinNodeToList( struct WinNode *winNode );
BOOL addWindowToList ( struct Window *win, struct List *list );

#endif
