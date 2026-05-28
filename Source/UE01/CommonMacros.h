#pragma once

#define AssignDefaultSubobject(ObjectPtr)\
(ObjectPtr = CreateDefaultSubobject<decltype(ObjectPtr)::ElementType>(TEXT(#ObjectPtr)))


