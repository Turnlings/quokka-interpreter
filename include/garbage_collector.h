#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

void gc_reference(Value *value);
void gc_dereference(Value *value);
Value *gc_malloc();

#endif