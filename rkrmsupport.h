
#ifndef RKRMSUPPORT_H
#define RKRMSUPPORT_H TRUE

#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/intuition.h>

/*
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
*/

struct TagItem *MakeNewTagList( ULONG data, ...);
BOOL listIsEmpty ( struct List *list );
VOID CloseWindowSafely( struct Window *win, BOOL removeFromList );
VOID StripIntuiMessages( struct MsgPort *mp, struct Window *win );
ULONG numItemsInList( struct List *list );

#endif
