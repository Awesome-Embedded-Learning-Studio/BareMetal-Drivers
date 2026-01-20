#include "widget.h"

#include <stddef.h>

#include "cfbd_define.h"


/*
 Helper: detach child from its current parent (if any).
 This will search the parent's children array and set the matching slot to NULL,
 and decrement parent's children_cnt accordingly.
 After this call, child's parent will be set to NULL.
*/
static void detach_from_parent(CFBD_Widget* child)
{
    if (!child)
        return;
    CFBD_Widget* old_parent = child->parent;
    if (!old_parent) {
        child->parent = NULL;
        return;
    }

    CFBD_Widget** arr = old_parent->children;
    size_t cap = old_parent->children_container_sz;
    if (!arr || cap == 0) {
        // parent's container not available: just clear parent reference
        child->parent = NULL;
        return;
    }

    CFBD_Bool removed = CFBD_FALSE;
    for (size_t i = 0; i < cap; ++i) {
        if (arr[i] == child) {
            arr[i] = NULL;
            removed = CFBD_TRUE;
            break;
        }
    }
    if (removed) {
        // Recompute children_cnt conservatively (faster: decrement if >0, but ensure correctness)
        size_t cnt = 0;
        for (size_t i = 0; i < cap; ++i) {
            if (arr[i] != NULL)
                ++cnt;
        }
        old_parent->children_cnt = cnt;
    }
    child->parent = NULL;
}

/*
 Public API implementations
*/

static void CFBD_WidgetSetParent_Default(CFBD_Widget* self, CFBD_Widget* new_parent)
{
    if (self->parent) {
        CFBD_Widget* old = self->parent;
        if (old->children && old->children_container_sz) {
            for (size_t i = 0; i < old->children_container_sz; ++i) {
                if (old->children[i] == self) {
                    old->children[i] = NULL;
                    break;
                }
            }

            /* 重新统计 */
            size_t cnt = 0;
            for (size_t i = 0; i < old->children_container_sz; ++i) {
                if (old->children[i])
                    cnt++;
            }
            old->children_cnt = cnt;
        }
    }

    self->parent = NULL;

    if (!new_parent)
        return;

    if (!new_parent->children || new_parent->children_container_sz == 0)
        return;

    for (size_t i = 0; i < new_parent->children_container_sz; ++i) {
        if (new_parent->children[i] == NULL) {
            new_parent->children[i] = self;
            new_parent->children_cnt++;
            self->parent = new_parent;
            return;
        }
    }
}

static CFBD_WidgetOperations g_cfbd_widget_default_ops = {
        .set_parent = CFBD_WidgetSetParent_Default,
};

void CFBD_WidgetCreateBase(CFBD_Widget* self, CFBD_WidgetCreationPack* pack)
{
    if (!self || !pack)
        return;

    self->ops = &g_cfbd_widget_default_ops;
    self->override_ops = NULL;

    self->parent = NULL;
    self->children = pack->children;
    self->children_container_sz = pack->children_container_sz;
    self->children_cnt = 0;

    self->tl_point = pack->tl_point;
    self->size = pack->size;

    /* 统计已有 children */
    if (self->children) {
        for (size_t i = 0; i < self->children_container_sz; ++i) {
            if (self->children[i]) {
                self->children[i]->parent = self;
                self->children_cnt++;
            }
        }
    }

    /* 使用 ops->set_parent */
    if (pack->parent) {
        self->ops->set_parent(self, pack->parent);
    }
}

void CFBD_WidgetAssignedChildPlace(CFBD_Widget* self,
                                   CFBD_Widget** children_array,
                                   size_t children_container_sz)
{
    if (!self)
        return;

    // Detach existing children: set their parent to NULL and clear existing container slots
    if (self->children && self->children_container_sz > 0) {
        for (size_t i = 0; i < self->children_container_sz; ++i) {
            CFBD_Widget* c = self->children[i];
            if (c != NULL) {
                // only clear child's parent if it points to us
                if (c->parent == self) {
                    c->parent = NULL;
                }
                self->children[i] = NULL;
            }
        }
    }

    // assign new container pointer and capacity
    self->children = children_array;
    self->children_container_sz = children_container_sz;
    self->children_cnt = 0;

    // populate new container: ensure each non-NULL child points to self
    if (self->children && self->children_container_sz > 0) {
        size_t cnt = 0;
        for (size_t i = 0; i < self->children_container_sz; ++i) {
            CFBD_Widget* c = self->children[i];
            if (c != NULL) {
                // detach from child's old parent first (so parent's arrays remain consistent)
                detach_from_parent(c);
                c->parent = self;
                ++cnt;
            }
        }
        self->children_cnt = cnt;
    }
}
