#ifdef CONFIG_EVM_ECMA_NUMBER
#include "ecma.h"

evm_val_t * ecma_Number;
evm_val_t ecma_number_toString(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

void ecma_number_attrs_apply(evm_t * e, evm_val_t * o, evm_val_t * v){
    if( evm_attr_create(e, o, ECMA_NUMBER_ATTR_SIZE) == ec_ok ){
        evm_attr_set(e, o, 0, "__proto__", *ecma_object_prototype);
        evm_attr_set(e, o, 1, "toString", evm_mk_native((intptr_t)ecma_number_toString));
        evm_attr_set(e, o, 2, "PrimitiveValue", *v);
    }
}

evm_val_t ecma_number_toString(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)v;
    if( argc == 0 ){
        evm_val_t * num;
        char buf[64];
        if( evm_is_object(p) ){
            evm_val_t * s = p;
            if( evm_get_gc_type(s) != GC_NUMBER ) return EVM_VAL_UNDEFINED;
            num = evm_attr_get(e, s, "PrimitiveValue");
        } else if( evm_is_number(p) ){
            num = p;
        } else {
            return EVM_VAL_UNDEFINED;
        }
        if( !num || !evm_is_number(num) ) return EVM_VAL_UNDEFINED;
        if( evm_is_integer(num) ){
            sprintf(buf, "%ld", evm_2_integer(num));
        } else {
            sprintf(buf, "%e", evm_2_double(num));
        }
        int len = strlen(buf);
        evm_val_t * o = evm_heap_string_create(e, buf, len + 1);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_number(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(p);
    evm_val_t *o = NULL;
    if(argc == 1 && evm_is_number(v) ) {
        o = evm_object_create(e, GC_NUMBER, 1, 0);
    }
    if( o != NULL){
        ecma_number_attrs_apply(e, o, v);
    }
    return *o;
}

evm_val_t *  ecma_number_init(evm_t * e){
    e->number_object = *evm_native_function_create(e, (evm_native_fn)ecma_number, ECMA_NUMBER_PROP_SIZE);
    evm_attr_set(e, &e->number_object, 0, "prototype", *ecma_object_prototype);
    evm_attr_set(e, &e->number_object, 1, "toString", evm_mk_native((intptr_t)ecma_number_toString));
    return &e->number_object;
}
#endif
