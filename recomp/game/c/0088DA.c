#include "game.h"

/* Obj_LinkAtEnd ($0088DA-$0088F5); the raw unlink after it ($0088F6-$008909) never runs. */

/* Obj_LinkAtEnd - appends an already-built object to the end of the main object list (just
 * before the tail sentinel), so Obj_RunList runs it from the next walk.
 * In:  obj (a0, its handler set); a2 is kept (the original parks it in usp)
 * Out: obj prev = the old last object, next = the tail; returns the old last object (a1,
 *      sign-extended handle)
 * Evidence: lea (g_ObjListTail).w,a2 / movea.w $4(a2),a1 / move.w a0,$4(a2) / move.w a0,$6(a1) /
 *           movem.w a1-a2,$4(a0); only caller $29464 (links an Obj_SpawnScriptedUnlinked object)
 */
uint32_t Obj_LinkAtEnd(uint32_t obj, uint32_t a2)
{
    (void)a2;
    PTR(ObjNode) p;
    p.v = obj;
    ObjNode *node = p;
    ObjNode *tail = (ObjNode *)g_ObjListTail;
    ObjNode *last = dk::handle<ObjNode>(tail->prev);
    tail->prev = (int16_t)(uint16_t)obj;
    last->next = (int16_t)(uint16_t)obj;
    node->prev = dk::handle_of(last);
    node->next = dk::handle_of(tail);
    return ADDR68(last);
}
