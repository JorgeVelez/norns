#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "events.h"
#include "device.h"
#include "device_list.h"

static int id = 0;

struct dev_node {
    struct dev_node *next;
    struct dev_node *prev;
    union dev *d;
};

struct dev_q {
    struct dev_node *head;
    struct dev_node *tail;
    size_t size;
};

struct dev_q dq;

static struct dev_node *dev_lookup_path(const char *path) {
    struct dev_node *n = dq.head;
    const char *npath;
    while(n != NULL) {
        npath = n->d->base.path;
        if(strcmp(path, npath) == 0) {
            return n;
        }
        n = n->next;
    }
    return NULL;
}

void dev_list_init(void) {
    dq.size = 0;
    dq.head = NULL;
    dq.tail = NULL;
}

void dev_list_add(device_t type, const char *path) {
    if(type < 0) {
        return;
    }
    struct dev_node *dn = calloc( 1, sizeof(struct dev_node) );
    fflush(stdout);
    if (dn == NULL) {
        printf("dev_list_add: error allocating device queue node\n");
        fflush(stdout);
        return;
    }
    union dev *d = dev_new(type, path);
    if (d == NULL) {
        printf("dev_list_add: error allocating device data\n");
        fflush(stdout);
        return;
    }

    d->base.id = id++;

    dn->d = d;

    insque(dn, dq.tail);
    dq.tail = dn;
    if(dq.size == 0) {
        dq.head = dn;
    }
    dq.size++;

    union event_data *ev;
    switch(type) {
    case DEV_TYPE_MONOME:
        ev = event_data_new(EVENT_MONOME_ADD);
        ev->monome_add.dev = d;
        break;
    case DEV_TYPE_INPUT:
        ev = event_data_new(EVENT_INPUT_ADD);
        ev->input_add.dev = d;
        break;
    default:
        printf("dev_list_add(): error posting event (unknown type)\n");
        fflush(stdout);
        return;
    }
    event_post(ev);
}

void dev_list_remove(device_t type, const char *node) {
    struct dev_node *dn = dev_lookup_path(node);
    if(dn == NULL) { return; }
    union event_data *ev;
    switch(type) {
    case DEV_TYPE_MONOME:
        ev = event_data_new(EVENT_MONOME_REMOVE);
        ev->monome_remove.id = dn->d->base.id;
        break;
    case DEV_TYPE_INPUT:
        ev = event_data_new(EVENT_INPUT_REMOVE);
        ev->input_remove.id = dn->d->base.id;
        break;
    default:
        printf("dev_list_remove(): error posting event (unknown type)\n");
        fflush(stdout);
        return;
    }
    event_post(ev);

    if(dq.head == dn) { dq.head = NULL; }
    if(dq.tail == dn) { dq.tail = dn->prev; }
    remque(dn);
    dq.size--;

    dev_delete(dn->d);
    free(dn);
}
