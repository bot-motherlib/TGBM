#ifndef FIELD
#define FIELD(Type, Name)
#endif

#ifndef COMPOUND
#define COMPOUND(Type, Name) FIELD(Type, Name)
#endif

#ifndef INLINED
#define INLINED(Type, Name) FIELD(Type, Name)
#endif

#ifndef INTEGER
#define INTEGER(Name) FIELD(Integer, Name)
#endif

#ifndef FLOAT
#define FLOAT(Name) FIELD(Integer, Name)
#endif

#ifndef BOOLEAN
#define BOOLEAN(Name) FIELD(Boolean, Name)
#endif

#ifndef TRUE_FIELD
#define TRUE_FIELD(Name) FIELD(True, Name)
#endif

#ifndef STRING
#define STRING(Name) FIELD(String, Name)
#endif

#ifndef INT_OR_STR
#define INT_OR_STR(Name) FIELD(InrOrStr, Name)
#endif

#ifndef FILE_OR_STR
#define FILE_OR_STR(Name) FIELD(FileOrStr, Name)
#endif

#ifndef ARRAYOF
#define ARRAYOF(Type, Name) FIELD(arrayof<Type>, Name)
#endif

#ifndef ARRAYOF_ARRAYOF
#define ARRAYOF_ARRAYOF(Type, Name) FIELD(arrayof<arrayof<Type>>, Name)
#endif

// #define OPTIONAL(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
// #define REQUIRED(TYPEMACRO, ...) TYPEMACRO(__VA_ARGS__)
#ifndef OPTIONAL
#define OPTIONAL(...)
#endif

#ifndef REQUIRED
#define REQUIRED(...)
#endif

#ifndef VARIANT_FIELD
#define VARIANT_FIELD(...)
#endif

#ifndef DISCRIMINATOR_FIELD
#define DISCRIMINATOR_FIELD(...)
#endif

#ifndef DISCRIMINATOR_VALUE
#define DISCRIMINATOR_VALUE(Type, Value)
#endif
