#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Example of multi-threading in Python for handling counting."""

import threading
import sys

class CommonCtx:
    """Common context to be used in threads"""

    def __init__(self, total_count, do_debug = False):
        self.total_count = total_count
        self.do_debug = do_debug
        self.printer_tid = 0
        self.cur_count = 0
        self.cur_count_handled = False
        self.lock = threading.Lock()
        self.cv = threading.Condition(self.lock)

    def __repr__(self):
        return "(total_count={}, ".format(self.total_count) + \
            "do_debug={}, printer_tid={} ".format(self.do_debug,
                                                  self.printer_tid) + \
            "cur_count={}, cur_count_handled={})".format(self.cur_count,
                                                         self.cur_count_handled)

class ThreadCtx:
    """Thread-specific context to be used in threads"""

    def __init__(self, id, common_ctx):
        self.id = id
        self.common_ctx = common_ctx

    def __repr__(self):
        return "(id={}, common_ctx={})".format(self.id, self.common_ctx)

def tprint(str):
    """Utility for thread-safe printing"""

    sys.stdout.write(str + "\n")

def counting_fn(thread_ctx):
    """Thread counting function"""

    ctx = thread_ctx.common_ctx
    id = thread_ctx.id

    if (ctx.do_debug):
        tprint("starting thread {}".format(id))

    with ctx.cv:
        if (ctx.do_debug):
            tprint("(thread {}) got lock".format(id))
        while (True):
            if (id is ctx.printer_tid):
                tprint("(thread {}) cur_count={}".format(id, ctx.cur_count))
                ctx.cur_count_handled = True
                ctx.cv.notify()
            if (ctx.cur_count >= ctx.total_count):
                break
            ctx.cv.wait()

    if (ctx.do_debug):
        tprint("finishing thread {}".format(id))

THREAD_COUNT = 5
ctx = CommonCtx(50)

thread_list = []
for i in range(THREAD_COUNT):
    thread_ctx = ThreadCtx((i + 1), ctx)
    t = threading.Thread(target=counting_fn, args=(thread_ctx,))
    t.daemon = True
    t.start()
    thread_list.append((t, thread_ctx))

with ctx.cv:
    ctx.printer_tid = 0
    for ctx.cur_count in range(1, (ctx.total_count + 1)):
        ctx.printer_tid = \
            1 if (ctx.printer_tid >= THREAD_COUNT) else (ctx.printer_tid + 1)
        ctx.cur_count_handled = False
        tprint("cur_count={}".format(ctx.cur_count))
        ctx.cv.notify_all()
        while (not ctx.cur_count_handled):
            ctx.cv.wait()

try:
    for (t, t_ctx) in thread_list:
        t.join(1.0)
        if (t.is_alive()):
            tprint("thread {} did not finish".format(t_ctx.id))
except KeyboardInterrupt:
    tprint("Caught keyboard interrupt")

tprint("{} threads finished".format(THREAD_COUNT))
