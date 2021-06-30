#include "argparse.hpp"

const ValueAssertTypeCb ArgParser::VATCBs[6] = {
    ArgParser::stringTypeAssert,
    ArgParser::integerTypeAssert,
    ArgParser::longTypeAssert,
    ArgParser::floatTypeAssert,
    ArgParser::doubleTypeAssert,
    ArgParser::boolTypeAssert
};

const char* ArgParser::VARTYPE_STRS[6] = {
    "String", "Integer", "Large Integer",
    "Decimal", "Large Decimal", "Boolean"
};

// Type Asserts
//

bool ArgParser::stringTypeAssert(char* in, void* out) {
    *(std::string*)out = std::string(in);
    return 1;
}

bool ArgParser::integerTypeAssert(char* in, void* out) {
    char* end;
    long l = strtol(in, &end, 10);
    if (*end) *(long*)out = l;
    else return 0;
    return 1;
}

bool ArgParser::longTypeAssert(char* in, void* out) {
    char* end;
    long long ll = strtoll(in, &end, 10);
    if (*end) *(long long*)out = ll;
    else return 0;
    return 1;
}

bool ArgParser::floatTypeAssert(char* in, void* out) {
    char* end;
    float f = strtof(in, &end);
    if (*end) *(float*)out = f;
    else return 0;
    return 1;
}

bool ArgParser::doubleTypeAssert(char* in, void* out) {
    char* end;
    double d = strtod(in, &end);
    if (*end) *(double*)out = strtod(in, &end); 
    else return 0;
    return 1;
}

bool ArgParser::boolTypeAssert(char* in, void* out) {
    for (size_t i = 0; in[i]; i++)
        in[i] = tolower(in[i]);

    if (*in == '1' || !strcmp(in, "true"))
        *(bool*)out = 1;
    else if (*in == '0' || !strcmp(in, "false"))
        *(bool*)out = 0;
    else 
        return 0;

    return 1;
}

void ArgParser::defaultHelpCb(std::vector<Arg>& args, std::vector<ArgMappedType>& mapargs) {
    for (Arg& a : args) {
        
        for (size_t i = 0; a.argstrs[i]; i++) {
            if (i)
                std::cout << ' ' << '|' << ' ';
            std::cout << a.argstrs[i];
        }

        if (a.info.argtype == AT_INPUT) {
            std::cout << "\n\tInput Type: ";
            if (a.info.intype == AIT_MAPPED) {
                std::cout << mapargs[a.info.map].typeName;
            } else {
                std::cout << ArgParser::VARTYPE_STRS[a.info.intype];
            }
        }

        std::cout << "\n\tRequired: " << (a.required ? "Yes" : "No");
        std::cout << "\n\tDescription: " << a.desc;
        std::cout << '\n';
    }
}

//
//

ArgParser::ArgParser() { }

ArgParser::ArgParser(const ArgParser& ref) : args(ref.args) { }

ArgParser::~ArgParser() {
    for (Arg& a : this->args) {
        if (a.argstrs) {
            delete[] a.argstrs;
        }
    }
}

ArgParser& ArgParser::addHelpCommand(
    std::vector<const char*> helpcmds,
    ArgListHelpCb callback
) {
    if (callback)
        this->helpcb = callback;

    Arg a = { 0 };
    
    a.info.argtype = AT_FLAG;
    a.info.intype = AIT_HELP_FLAG;
    a.storage = NULL;
    a.desc = "Displays Help";
    a.argstrs = new char*[helpcmds.size() + 1]();

    for (size_t i = 0; i != helpcmds.size(); i++)
        a.argstrs[i] = (char*)helpcmds[i];

    return this->addArg(a);
}

ArgParser& ArgParser::addHelpCommand(
    const char* helpcmd,
    ArgListHelpCb callback
) {
    return this->addHelpCommand(std::vector<const char*>(1, helpcmd), callback);
}

ArgParser& ArgParser::addMapType(
    ArgMappedType type
) {
    if (this->argTypeMap.size() == 16) {
        std::cout << "Unable To Add Mapped Input Type \'" << type.typeName << "\', Max Types\n";
    } else {
        this->argTypeMap.push_back(type);
    }

    return *this;
}

ArgParser& ArgParser::addMapType(
    ValueAssertTypeCb assertCb,
    const char* typeName
) {
    ArgMappedType type = { assertCb, typeName, (unsigned char)this->argTypeMap.size() };
    return this->addMapType(type);
}

ArgParser& ArgParser::addArg(Arg& a) {
    this->args.push_back(a);
    return *this;
}

ArgParser& ArgParser::addFlagArg(
    bool* storage,
    const char* desc,
    std::vector<const char*> argstrs
) {
    Arg a = { 0 };
    
    a.storage = storage;
    a.desc = desc;
    a.argstrs = new char*[argstrs.size() + 1]();

    for (size_t i = 0; i != argstrs.size(); i++)
        a.argstrs[i] = (char*)argstrs[i];

    return this->addArg(a);
}

ArgParser& ArgParser::addFlagArg(
    bool* storage,
    const char* desc,
    const char* argstr
) {
    return this->addFlagArg(storage, desc, std::vector<const char*>(1, argstr));
}

 ArgParser& ArgParser::addInputArg(
    void* storage,
    const char* desc,
    std::vector<const char*> argstrs,
    ArgInputType type,
    bool required
) {
    Arg a = { 0 };
    a.info.argtype = ArgType::AT_INPUT;
    a.info.intype = type;
    a.storage = storage;
    a.desc = desc;
    a.argstrs = new char*[argstrs.size() + 1]();

    for (size_t i = 0; i != argstrs.size(); i++)
        a.argstrs[i] = (char*)argstrs[i];

    a.required = required;

    return this->addArg(a);
}

ArgParser& ArgParser::addInputArg(
    void* storage,
    const char* desc,
    const char* argstr,
    ArgInputType type,
    bool required 
) {
    return this->addInputArg(storage, desc, std::vector<const char*>(1, argstr), type, required);
}

ArgParser& ArgParser::addInputArg(
    void* storage,
    const char* desc,
    std::vector<const char*> argstrs,
    const char* mappedType,
    bool required
) {
    Arg a = { 0 };
    
    for (ArgMappedType& amt : this->argTypeMap) {
        if (!strcmp(amt.typeName, mappedType)) {
            a.info.map = amt.mapid;
        }
    }

    a.info.argtype = ArgType::AT_INPUT;
    a.info.intype = ArgInputType::AIT_MAPPED;
    a.storage = storage;
    a.desc = desc;
    a.argstrs = new char*[argstrs.size() + 1]();

    for (size_t i = 0; i != argstrs.size(); i++)
        a.argstrs[i] = (char*)argstrs[i];

    a.required = required;

    return this->addArg(a);
}

ArgParser& ArgParser::addInputArg(
    void* storage,
    const char* desc,
    const char* argstr,
    const char* mappedType,
    bool required
) {
    return this->addInputArg(storage, desc, std::vector<const char*>(1, argstr), mappedType, required);
}

// ArgParser& ArgParser::addCallableArg(
//     ArgCallableCb cb,
//     const char* desc,
//     std::vector<const char*> argstrs
// ) {
//     Arg a = { 0 };
    
//     a.storage = (void*)cb;
//     a.desc = desc;
//     a.info.intype = AIT_CALLABLE;
//     a.argstrs = new char*[argstrs.size() + 1]();

//     for (size_t i = 0; i != argstrs.size(); i++)
//         a.argstrs[i] = (char*)argstrs[i];

//     return this->addArg(a);
// }

// ArgParser& ArgParser::addCallableArg(
//     ArgCallableCb cb,
//     const char* desc,
//     const char* argstr
// ) {
//     return this->addCallableArg(cb, desc, std::vector<const char*>(1, argstr));
// }

std::vector<char*> ArgParser::parse(char** argv, size_t i) {
    std::vector<char*> nonargs;
    std::vector<Arg> argscpy = this->args;
    char* arg;

    while ((arg = argv[i++])) {

        for (size_t ai = 0; ai != argscpy.size(); ai++) {
            Arg& a = argscpy[ai];

            for (size_t si = 0; a.argstrs[si]; si++) {
                if (strcmp(a.argstrs[si], arg)) {
                    continue;   
                }

                if (a.info.argtype) { // input
                    if (argv[i] == NULL) {
                        std::cout << "Trailing \'" << arg << "\' Missing Argument\n";
                        goto for_loop_out;
                    }

                    ValueAssertTypeCb vatcb = NULL;

                    if (a.info.intype != AIT_MAPPED) {
                        vatcb = ArgParser::VATCBs[a.info.intype];
                    } else {
                        vatcb = this->argTypeMap[a.info.map].assertCb;
                    }

                    if (!vatcb(argv[i], a.storage)) {
                        std::cout << "Argument \'" << argv[i] << "\' Is Invalid For Type " 
                        << ((a.info.intype == ArgInputType::AIT_MAPPED) ? this->argTypeMap[a.info.map].typeName : ArgParser::VARTYPE_STRS[a.info.argtype])
                        << std::endl;

                        if (a.required) {
                            std::cout << "Exiting...\n";
                            exit(-1);
                        }

                        i++;
                        goto for_loop_out;
                    }
                } else if (a.storage) { // flag
                    *(bool*)a.storage = 1;
                } else if (a.info.intype == AIT_HELP_FLAG) {
                    this->defaultHelpCb(this->args, this->argTypeMap);
                }

                argscpy.erase(argscpy.begin() + ai--);
                goto for_loop_out;
            }
        }
    //for_loop_nonarg_out:
        nonargs.push_back(arg);
    for_loop_out:
        continue;
    }

    for (Arg& aref : argscpy) {
        if (aref.required) {
            std::cout << "Required Argument \'" << aref.argstrs[0] << "\' Not Entered\nExiting...\n";
            exit(-1);
        }
    }

    return nonargs;
}
