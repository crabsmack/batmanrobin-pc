/* types/Obj.h - the object pool's blocks and the object lists' nodes (docs/NOTES.md "Objects",
 * "V-int object list"). */
#pragma once
#include "dk_mem.h"

/* the pool: 140 blocks of $5A bytes at g_ObjPool (ObjPool_Init) */
static constexpr int OBJ_BLOCK_SIZE = 0x5A;
static constexpr int OBJ_HANDLE_SLOTS = 8;     /* handle words +8..+$17 */

/* A free pool block: its first word links the next free block (g_ObjPoolHead is the first). */
typedef struct ObjFreeBlock {
    s16 next_free;                 /* +$00: handle of the next free block */
    u8  unk_02[OBJ_BLOCK_SIZE - 2];
} ObjFreeBlock;
static_assert(sizeof(ObjFreeBlock) == OBJ_BLOCK_SIZE, "ObjFreeBlock");

/* The head of an object on the main list (g_ObjListHead .. g_ObjListTail) or the V-int list
 * (g_VIntListHead .. g_VIntListTail): the same node layout, the head and tail sentinels too. */
typedef struct ObjNode {
    u32 handler;                   /* +$00: code run by Obj_RunList / VIntObj_RunList with a6 = the node */
    s16 prev;                      /* +$04: handle of the previous node (0 = on no list) */
    s16 next;                      /* +$06: handle of the next node */
    s16 blocks[OBJ_HANDLE_SLOTS];  /* +$08: handles of the extra pool blocks it owns (0 = none; Obj_NewMulti) */
} ObjNode;
static_assert(offsetof(ObjNode, prev) == 0x04, "ObjNode.prev");
static_assert(offsetof(ObjNode, next) == 0x06, "ObjNode.next");
static_assert(offsetof(ObjNode, blocks) == 0x08, "ObjNode.blocks");
static_assert(sizeof(ObjNode) == 0x18, "ObjNode");
