#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Example of multi-threading in Python for handling counting."""

import threading

class ThreadCtx:
    """Context to be used in threads"""

    def __init__(self, id):
        self.id = id

    def __repr__(self):
        print "ThreadCtx {}".format(self.id)

def counting_fn(ctx):
    print "ID: {}".format(ctx.id)

lock = threading.Lock()
cv = threading.Condition(lock)

ctx = ThreadCtx(1)
t = threading.Thread(target=counting_fn, args=(ctx,))
t.start()
