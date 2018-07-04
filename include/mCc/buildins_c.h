#ifndef BUILDINS_C_H
#define BUILDINS_C_H

#if defined(__cplusplus) && __cplusplus >= 201103L
constexpr
#else
const
#endif
char buildins_c_data[] =
    "#include <stdio.h>\n"
    "void   __attribute__((cdecl)) print(const char *msg);\n"
    "void   __attribute__((cdecl)) print_nl(void);\n"
    "void   __attribute__((cdecl)) print_int(long x);\n"
    "void   __attribute__((cdecl)) print_float(float x);\n"
    "long   __attribute__((cdecl)) read_int(void);\n"
    "float __attribute__((cdecl)) read_float(void);\n"
    "void print(const char *msg)\n"
    "{\n"
    "	printf(\"%s\", msg);\n"
    "}\n"
    "void print_nl(void)\n"
    "{\n"
    "	printf(\"\\n\");\n"
    "}\n"
    "void print_int(long x)\n"
    "{\n"
    "	printf(\"%ld\", x);\n"
    "}\n"
    "void print_float(float x)\n"
    "{\n"
    "	printf(\"%f\", x);\n"
    "}\n"
    "long read_int(void)\n"
    "{\n"
    "	long ret = 0;\n"
    "	scanf(\"%ld\", &ret);\n"
    "	return ret;\n"
    "}\n"
    "float read_float(void)\n"
    "{\n"
    "	float ret = 0.0f;\n"
    "	scanf(\"%f\", &ret);\n"
    "	return ret;\n"
    "}\n";

#endif // header guard
