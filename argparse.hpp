#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#pragma once

// bool $(char* inLocation, void* valueStore)
typedef bool(*ValueAssertTypeCb)(char*, void*);

typedef enum : unsigned char {
    AIT_STRING = 0x0, // raw input
    AIT_INTEGER = 0x1, // signed int
    AIT_LONGINTEGER = 0x2, // signed long long
    AIT_DECIMAL = 0x3, // float
    AIT_LONGDECIMAL = 0x4, // double
    AIT_BOOLEAN = 0x5, // bool
    AIT_MAPPED = 0x6, // custom user defined argument type
    AIT_HELP_FLAG = 0x7 // no argument, signals to call a function
} ArgInputType;

typedef enum : unsigned char {
    AT_FLAG = 0x0,
    AT_INPUT = 0x1
} ArgType;

typedef struct {
    ValueAssertTypeCb assertCb;
    const char* typeName;
    unsigned char mapid;
} ArgMappedType, *pArgMappedType;

typedef struct {
    struct _in_info {
        unsigned char map : 4; // max 15
        unsigned char intype : 3;
        unsigned char argtype : 1;
    } info;
    void* storage;
    const char* desc;
    char** argstrs;
    bool required;
} Arg, *pArg;

typedef void(*ArgListHelpCb)(std::vector<Arg>&, std::vector<ArgMappedType>&);

class ArgParser {
public:
    static bool stringTypeAssert(char*, void*);
    static bool integerTypeAssert(char*, void*);
    static bool longTypeAssert(char*, void*);
    static bool floatTypeAssert(char*, void*);
    static bool doubleTypeAssert(char*, void*);
    static bool boolTypeAssert(char*, void*);
    static void defaultHelpCb(std::vector<Arg>&,std::vector<ArgMappedType>&);
private:
    static const ValueAssertTypeCb VATCBs[6];
    static const char* VARTYPE_STRS[6]; 
    
    std::vector<Arg> args;
    std::vector<ArgMappedType> argTypeMap;
    ArgListHelpCb helpcb = ArgParser::defaultHelpCb;
public:
    ArgParser();
    ArgParser(const ArgParser& ref);

    ~ArgParser();

    ArgParser& addHelpCommand(
        std::vector<const char*> helpcmd,
        ArgListHelpCb helpcb = NULL
    );

    ArgParser& addHelpCommand(
        const char* helpcmd,
        ArgListHelpCb helpcb = NULL
    );

    ArgParser& addArg(Arg& a);

    ArgParser& addMapType(
        ArgMappedType type
    );
    ArgParser& addMapType(
        ValueAssertTypeCb assertCb,
        const char* typeName
    );

    ArgParser& addFlagArg(
        bool* storage,
        const char* desc,
        std::vector<const char*> argstrs 
    );
    ArgParser& addFlagArg(
        bool* storage,
        const char* desc,
        const char* argstr
    );

    ArgParser& addInputArg(
        void* storage,
        const char* desc,
        std::vector<const char*> argstrs,
        ArgInputType type,
        bool required = 0
    );
    ArgParser& addInputArg(
        void* storage,
        const char* desc,
        const char* argstr,
        ArgInputType type,
        bool required = 0
    );
    ArgParser& addInputArg(
        void* storage,
        const char* desc,
        const char* argstr,
        const char* mappedType,
        bool required = 0
    );
    ArgParser& addInputArg(
        void* storage,
        const char* desc,
        std::vector<const char*> argstr,
        const char* mappedType,
        bool required = 0
    );

    // ArgParser& addCallableArg(
    //     ArgCallableCb cb,
    //     const char* desc,
    //     std::vector<const char*> argstrs
    // );
    // ArgParser& addCallableArg(
    //     ArgCallableCb cb,
    //     const char* desc,
    //     const char* argstr
    // );

    std::vector<char*> parse(
        char** args,
        size_t i = 1
    );

};