/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/

#ifdef CONFIG_EVM_ECMA_MATH
#include "ecma.h"
#include <math.h>
#include <ctype.h>
#include <time.h>

evm_val_t * ecma_Math;

enum MathFuncs
{
    Abs = 0,
    Acos,
    Acosh,
    Asin,
    Asinh,
    Atan,
    Atanh,
    Atan2,
    Cbrt,
    Ceil,
    Clz32,
    Cos,
    Cosh,
    Exp,
    Expm1,
    Floor,
    Fround,
    Hypot,
    Imul,
    Log,
    Log1p,
    Log2,
    Log10,
    Max,
    Min,
    Pow,
    Random,
    Round,
    Sign,
    Sin,
    Sinh,
    Sqrt,
    Tan,
    Tanh,
    Trunc
};

evm_val_t ecma_math_args(evm_t * e, evm_val_t * p, int argc, evm_val_t * v, int type){
    (void)e;
    (void)p;
    (void)argc;
    (void)v;
    if(type!=Random && argc == 0) ARG_LENGTH_ERR;
    double args[argc];
    double ret;
    for(int j=0; j< argc; j++){
        double arg;
        if( argc >= 1) {
            if(evm_is_number(v)){
                arg = evm_2_double(v);
            }else if(evm_is_nan(v)){
                return evm_mk_nan();
            }else if(evm_is_string(v)){
                const char * s = evm_2_string(v);
                uint32_t l = strlen(s);
                if (strspn(s, "0123456789") == l){
                    arg = atoi(s);
                }else{
                    if (l > 2){
                        char perfix[2];
                        strncpy(perfix, s, 2);
                        if (!strcspn(perfix, "0X") || !strcspn(perfix, "0x")){
                            for(uint32_t i=2; i < l; i++){
                                if (!isxdigit(s[i])){
                                    return evm_mk_foreign_string((intptr_t)"NaN");
                                }
                            }
                            arg = strtol(s, NULL, 16);
                        }else{
                             return evm_mk_foreign_string((intptr_t)"NaN");
                        }
                    }else{
                        return evm_mk_foreign_string((intptr_t)"NaN");
                    }if( argc == 0) ARG_LENGTH_ERR;
                }
            }else{
                return evm_mk_undefined();
            }
        }
        args[j] = arg;
        v++;
    }

    switch (type) {
        case Abs:
            ret = fabs(args[0]);
            break;
        case Acos:
            ret = acos(args[0]);
            break;
        case Acosh:
            ret = acosh(args[0]);
            break;
        case Asin:
            ret = asin(args[0]);
            break;
        case Asinh:
            ret = asinh(args[0]);
            break;
        case Atan:
            ret = atan(args[0]);
            break;
        case Atanh:
            ret = atanh(args[0]);
            break;
        case Atan2:
            if(argc < 2) ARG_LENGTH_ERR;
            ret = atan2(args[0], args[1]);
            break;
        case Cbrt:
            ret = cbrt(args[0]);
            break;
        case Clz32:
            ret = atanh(args[0]);
            break;
        case Ceil:
            ret = ceil(args[0]);
            break;
        case Cos:
            ret = cos(args[0]);
            break;
        case Cosh:
            ret = cosh(args[0]);
            break;
        case Exp:
            ret = exp(args[0]);
            break;
        case Expm1:
            ret = expm1(args[0]);
            break;
        case Floor:
            ret = floor(args[0]);
            break;
        case Fround:
            ret = args[0]; //The Math.fround() function returns the nearest 32-bit single precision float representation of a Number.
            break;
        case Hypot:{
            double sum = 0;
            for(int i=0; i < argc; i++)
            {
                sum = sum + pow(args[i], 2);
            }
            ret = sqrt(sum);
            break;
        }
        case Imul:
            if(argc < 2) ARG_LENGTH_ERR;
            ret = args[0]*args[1]; // Math.imul(0xffffffff, 5) === -5  | Math.imul(0xfffffffe, 5) === -10
            break;
        case Log:
            ret = log(args[0]);
            break;
        case Log1p:
            ret = log1p(args[0]);
            break;
        case Log2:
            ret = log2(args[0]);
            break;
        case Log10:
            ret = log10(args[0]);
            break;
        case Max:{
            double max = args[0];
            int i;
            for(i=1; i < argc; i++)
            {
                if(args[i]> max){
                    max = args[i];
                }
            }
            ret = max;
            break;
        }
        case Min:{
            double min = args[0];
            int i;
            for(i=1; i < argc; i++)
            {
                if(args[i] < min){
                    min = args[i];
                }
            }
            ret = min;
            break;
        }
        case Pow:
            if(argc < 2) ARG_LENGTH_ERR;
            ret = pow(args[0], args[1]);
            break;
        case Random:{
            if(argc > 1) ARG_LENGTH_ERR;
            time_t t;
            /* 初始化随机数发生器 */
            srand((unsigned) time(&t));
            ret = rand()/(RAND_MAX+1.0);
            break;
        }
        case Round:
            ret = round(args[0]);
            break;
        case Sign:{
            if (args[0] > 0){
                ret = 1;
            }else if (args[0] < 0){
                ret = -1;
            }else{
                ret = 0;
            }
            break;
        }
        case Sin:
            ret = sin(args[0]);
            break;
        case Sinh:
            ret = sinh(args[0]);
            break;
        case Sqrt:
            ret = sqrt(args[0]);
            break;
        case Tan:
            ret = tan(args[0]);
            break;
        case Tanh:
            ret = tanh(args[0]);
            break;
        case Trunc:
            ret = trunc(args[0]);
            break;
        default:
            break;
    }
    if (!isnan(ret)){
        return evm_mk_number(ret);
    }
    return evm_mk_nan();
}

evm_val_t ecma_math_abs(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Abs);
}

evm_val_t ecma_math_acos(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Acos);
}

evm_val_t ecma_math_acosh(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Acosh);
}

evm_val_t ecma_math_asin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Asin);
}

evm_val_t ecma_math_asinh(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Asinh);
}

evm_val_t ecma_math_atan(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Atan);
}

//evm_val_t ecma_math_atan2(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
//    return ecma_math_args(e, p, argc, v, Atan2);
//}

evm_val_t ecma_math_atanh(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Atanh);
}

evm_val_t ecma_math_atan2(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Atan2);
}

evm_val_t ecma_math_cbrt(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Cbrt);
}

evm_val_t ecma_math_clz32(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Clz32);
}

evm_val_t ecma_math_ceil(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Ceil);
}

evm_val_t ecma_math_cos(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Cos);
}

evm_val_t ecma_math_cosh(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Cosh);
}

evm_val_t ecma_math_exp(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Exp);
}

evm_val_t ecma_math_expm1(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Expm1);
}


evm_val_t ecma_math_floor(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Floor);
}

evm_val_t ecma_math_fround(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Fround);
}

evm_val_t ecma_math_hypot(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Hypot);
}

evm_val_t ecma_math_imul(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Imul);
}

evm_val_t ecma_math_log(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Log);
}


evm_val_t ecma_math_log1p(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Log1p);
}

evm_val_t ecma_math_log2(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Log2);
}

evm_val_t ecma_math_log10(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Log10);
}

evm_val_t ecma_math_random(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Random);
}

evm_val_t ecma_math_max(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Max);
}

evm_val_t ecma_math_min(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Min);
}

evm_val_t ecma_math_pow(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Pow);
}


evm_val_t ecma_math_round(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Round);
}

evm_val_t ecma_math_sign(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Sign);
}

evm_val_t ecma_math_sin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Sin);
}


evm_val_t ecma_math_sinh(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Sinh);
}


evm_val_t ecma_math_sqrt(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Sqrt);
}

evm_val_t ecma_math_tan(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Tan);
}

evm_val_t ecma_math_tanh(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Tanh);
}

evm_val_t ecma_math_trunc(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    return ecma_math_args(e, p, argc, v, Trunc);
}

extern void ecma_object_attrs_apply(evm_t * e, evm_val_t * o, evm_val_t * prototype);
evm_val_t * ecma_math_init(evm_t * e){
    evm_val_t * ecma_math_object = evm_object_create(e, GC_NATIVE_OBJECT, 43, 0);
    if( ecma_math_object == NULL ) return NULL;
    evm_prop_set(e, ecma_math_object, 0, "E", evm_mk_number(2.718281828459045));
    evm_prop_set(e, ecma_math_object, 1, "LN2", evm_mk_number(0.6931471805599453));
    evm_prop_set(e, ecma_math_object, 2, "LN10", evm_mk_number(2.302585092994046));
    evm_prop_set(e, ecma_math_object, 3, "LOG2E", evm_mk_number(1.4426950408889634));
    evm_prop_set(e, ecma_math_object, 4, "LOG10E", evm_mk_number(0.4342944819032518));
    evm_prop_set(e, ecma_math_object, 5, "PI", evm_mk_number(3.141592653589793));
    evm_prop_set(e, ecma_math_object, 6, "SQRT1_2", evm_mk_number(0.7071067811865476));
    evm_prop_set(e, ecma_math_object, 7, "SQRT2", evm_mk_number(1.4142135623730951));

    evm_prop_set(e, ecma_math_object, 8, "abs", evm_mk_native((intptr_t)ecma_math_abs));
    evm_prop_set(e, ecma_math_object, 9, "acos", evm_mk_native((intptr_t)ecma_math_acos));
    evm_prop_set(e, ecma_math_object, 10, "acosh", evm_mk_native((intptr_t)ecma_math_acosh));
    evm_prop_set(e, ecma_math_object, 11, "asin", evm_mk_native((intptr_t)ecma_math_asin));
    evm_prop_set(e, ecma_math_object, 12, "asinh", evm_mk_native((intptr_t)ecma_math_asinh));
    evm_prop_set(e, ecma_math_object, 13, "atan", evm_mk_native((intptr_t)ecma_math_atan));
    evm_prop_set(e, ecma_math_object, 14, "atan2", evm_mk_native((intptr_t)ecma_math_atan2));
    evm_prop_set(e, ecma_math_object, 15, "atanh", evm_mk_native((intptr_t)ecma_math_atanh));
    evm_prop_set(e, ecma_math_object, 16, "cbrt", evm_mk_native((intptr_t)ecma_math_cbrt));
    evm_prop_set(e, ecma_math_object, 17, "ceil", evm_mk_native((intptr_t)ecma_math_ceil));
    evm_prop_set(e, ecma_math_object, 18, "clz32", evm_mk_native((intptr_t)ecma_math_clz32));
    evm_prop_set(e, ecma_math_object, 19, "cos", evm_mk_native((intptr_t)ecma_math_cos));
    evm_prop_set(e, ecma_math_object, 20, "cosh", evm_mk_native((intptr_t)ecma_math_cosh));
    evm_prop_set(e, ecma_math_object, 21, "exp", evm_mk_native((intptr_t)ecma_math_exp));
    evm_prop_set(e, ecma_math_object, 22, "expm1", evm_mk_native((intptr_t)ecma_math_expm1));
    evm_prop_set(e, ecma_math_object, 23, "floor", evm_mk_native((intptr_t)ecma_math_floor));
    evm_prop_set(e, ecma_math_object, 24, "fround", evm_mk_native((intptr_t)ecma_math_fround));
    evm_prop_set(e, ecma_math_object, 25, "hypot", evm_mk_native((intptr_t)ecma_math_hypot));
    evm_prop_set(e, ecma_math_object, 26, "imul", evm_mk_native((intptr_t)ecma_math_imul));
    evm_prop_set(e, ecma_math_object, 27, "log", evm_mk_native((intptr_t)ecma_math_log));
    evm_prop_set(e, ecma_math_object, 28, "log1p", evm_mk_native((intptr_t)ecma_math_log1p));
    evm_prop_set(e, ecma_math_object, 29, "log2", evm_mk_native((intptr_t)ecma_math_log2));
    evm_prop_set(e, ecma_math_object, 30, "log10", evm_mk_native((intptr_t)ecma_math_log10));
    evm_prop_set(e, ecma_math_object, 31, "max", evm_mk_native((intptr_t)ecma_math_max));
    evm_prop_set(e, ecma_math_object, 32, "min", evm_mk_native((intptr_t)ecma_math_min));
    evm_prop_set(e, ecma_math_object, 33, "pow", evm_mk_native((intptr_t)ecma_math_pow));
    evm_prop_set(e, ecma_math_object, 34, "random", evm_mk_native((intptr_t)ecma_math_random));
    evm_prop_set(e, ecma_math_object, 35, "round", evm_mk_native((intptr_t)ecma_math_round));
    evm_prop_set(e, ecma_math_object, 36, "sign", evm_mk_native((intptr_t)ecma_math_sign));
    evm_prop_set(e, ecma_math_object, 37, "sin", evm_mk_native((intptr_t)ecma_math_sin));
    evm_prop_set(e, ecma_math_object, 38, "sinh", evm_mk_native((intptr_t)ecma_math_sinh));
    evm_prop_set(e, ecma_math_object, 39, "sqrt", evm_mk_native((intptr_t)ecma_math_sqrt));
    evm_prop_set(e, ecma_math_object, 40, "tan", evm_mk_native((intptr_t)ecma_math_tan));
    evm_prop_set(e, ecma_math_object, 41, "tanh", evm_mk_native((intptr_t)ecma_math_tanh));
    evm_prop_set(e, ecma_math_object, 42, "trunc", evm_mk_native((intptr_t)ecma_math_trunc));

    ecma_object_attrs_apply(e, ecma_math_object, ecma_object_prototype);
    
    return ecma_math_object;
}
#endif
