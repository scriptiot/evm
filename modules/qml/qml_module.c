#include "qml_module.h"

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "Invalid argument length");return EVM_VAL_UNDEFINED; }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "Invalid argument type");return EVM_VAL_UNDEFINED; }
#define MEMORY_ERR   { evm_set_err(e, ec_memory, "Insufficient static memory");return NULL; }

typedef struct qml_value_t{
    int type;
    intptr_t name;
    void* api;
} qml_value_t;

typedef struct qml_binding_t{
    evm_val_t src_obj;
    uint32_t src_name;
    evm_val_t tar_obj;
    uint32_t tar_name;
    evm_val_t tar_value;

    struct qml_binding_t * next;
}qml_binding_t;

typedef struct qml_object_t{
    void *obj;
    intptr_t name;
    evm_native_fn api;
    int values_len;
    qml_value_t * values;
    struct qml_object_t * parent;
    qml_binding_t * bindings;
}qml_object_t;

typedef struct qml_func_binding_t{
    int type;
    evm_val_t obj;
    uint32_t name;
    evm_val_t func;
    evm_val_t last_executed_result;
    struct qml_func_binding_t * next;
}qml_func_binding_t;


void __triggerBinding(evm_t * e, evm_val_t * src_obj, uint32_t name);
int __qml_value_check_type(qml_value_t * value, evm_val_t * v);

evm_val_t * qml_root;
evm_val_t * qml_bindings_root;
int qml_object_index = 0;
int qml_binding_index = 0;
qml_object_t * qml_objects;
int qml_objects_len;
int qml_capture = 0;

qml_func_binding_t * func_bindings = NULL;

void __addBinding(evm_val_t * src_obj, uint32_t src, evm_val_t * tar_obj, uint32_t tar, evm_val_t * tar_value){
    qml_object_t * object = (qml_object_t *)evm_object_get_ext_data(src_obj);
    if( object->bindings == NULL ){
        object->bindings = (qml_binding_t*)evm_malloc(sizeof(qml_binding_t));
        object->bindings->src_obj = *src_obj;
        object->bindings->src_name = src;
        object->bindings->tar_obj = *tar_obj;
        object->bindings->tar_name = tar;
        object->bindings->tar_value = *tar_value;
        object->bindings->next = NULL;
    } else {
        qml_binding_t * next = object->bindings;
        if( next->src_obj == *src_obj &&
                next->src_name == src &&
                next->tar_name == tar &&
                next->tar_obj == *tar_obj)
        return;
        while( next->next ){
            if( next->src_obj == *src_obj &&
                    next->src_name == src &&
                    next->tar_name == tar &&
                    next->tar_obj == *tar_obj)
                return;
            next = next->next;
        }
        qml_binding_t * bindings = (qml_binding_t*)evm_malloc(sizeof(qml_binding_t));
        bindings->src_obj = *src_obj;
        bindings->src_name = src;
        bindings->tar_obj = *tar_obj;
        bindings->tar_name = tar;
        bindings->tar_value = *tar_value;
        bindings->next = NULL;
        next->next = bindings;
    }
}

int __evm_qml_search_and_write(evm_t * e,
                               evm_val_t * o,
                               qml_object_t * object,
                               uint32_t name,
                               evm_val_t * v){
    for(int i = 0; i < object->values_len; i++){
        qml_value_t * value = object->values + i;
        if( value->name == name ){
            if( __qml_value_check_type(value, v)){
                evm_native_fn api = value->api;
                api(e, o, 1, v);
                return 1;
            } else {
                evm_print("Error: Dismatched QML type\n");
                return 0;
            }
        }
    }
    return 0;
}

int __writeValue(evm_t * e, evm_val_t * o, intptr_t name, evm_val_t * v){
    qml_object_t * object = (qml_object_t *)evm_object_get_ext_data(o);
    if( __evm_qml_search_and_write(e, o, object, name, v) ) return 1;
    qml_object_t * parent = object->parent;
    while(parent){
        if( __evm_qml_search_and_write(e, o, parent, name, v) ) return 1;
        parent = parent->parent;
    }
    return 0;
}

void evm_qml_write_value(evm_t * e, evm_val_t * src_obj, char * name, evm_val_t v){
    if( qml_capture ) return;
    evm_val_t * local_v = evm_prop_get(e, src_obj, name, 0);
    if( !local_v ) return;
    *local_v = v;
    __triggerBinding(e, src_obj, evm_str_lookup(e, name));
}

void __qml_binding_capture(evm_t * e, evm_val_t * obj, uint32_t src_name){
    qml_func_binding_t * local_bindings = func_bindings;
    while(local_bindings){
        if( local_bindings->type & EVM_QML_CALLBACK ){
            local_bindings = local_bindings->next;
            continue;
        }
        if(local_bindings->type == EVM_QML_ANY){
           local_bindings = local_bindings->next;
           continue;
        }
        if( evm_2_intptr(&local_bindings->obj) == evm_2_intptr(obj) && local_bindings->name == src_name){
            local_bindings = local_bindings->next;
            continue;
        }
        evm_val_t * local_v = evm_prop_get_by_key(e, &local_bindings->obj, local_bindings->name, 0);
        if( local_v == NULL || evm_is_script(local_v) ){
            local_bindings = local_bindings->next;
            continue;
        }
        evm_val_t res = evm_run_callback(e, &local_bindings->func, &local_bindings->obj, NULL, 0);
        if( res != local_bindings->last_executed_result && !evm_is_undefined(local_v) ){
            local_bindings->last_executed_result = res;
            __addBinding(obj, src_name, &local_bindings->obj, local_bindings->name, &local_bindings->func);
        }
        local_bindings = local_bindings->next;
    }
}


void __triggerBinding(evm_t * e, evm_val_t * src_obj, uint32_t name){
    int local_qml_capture = 1;
    qml_object_t * object = (qml_object_t*)evm_object_get_ext_data(src_obj);
    qml_binding_t * next = object->bindings;

    while( next ){
        if( *src_obj == next->src_obj && (name == next->src_name ) ){
            evm_val_t * local_v = evm_prop_get_by_key(e, &next->tar_obj, next->tar_name, 0);
            evm_val_t res = evm_run_callback(e, &next->tar_value, &next->tar_obj, NULL, 0);
            if( __writeValue(e, &next->tar_obj, (intptr_t)next->tar_name, &res) ) *local_v = res;
            local_qml_capture = 0;
        }
        next = next->next;
    }

    if( local_qml_capture )
        __qml_binding_capture(e, src_obj, name);
}

//argc == 4: value, this, src_obj, hashname
evm_val_t evm_qml_write(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(p);
    if( argc == 3 ){
        evm_val_t * src_obj = v + 1;
        uint32_t name = evm_2_integer(v + 2);
        evm_val_t * local_v = evm_prop_get_by_key(e, src_obj, name, 0);
        if( local_v ) {
            if( *local_v != *v ){
                *local_v = *v;
//                __writeValue(e, src_obj, (intptr_t)name, v);
//                if( qml_capture ){
//                    __triggerBinding(e, src_obj, name);
//                }
            }
        }
    } else if(argc == 4){
        evm_val_t * src_obj = v + 2;
        uint32_t name = evm_2_integer(v + 3);
        evm_val_t * local_v = evm_prop_get_by_key(e, src_obj, name, 0);
        if( !local_v ) local_v = evm_attr_get_by_key(e, src_obj, name);
        if( local_v ) {
            if( *local_v != *v ){
                *local_v = *v;
                __writeValue(e, src_obj, (intptr_t)name, v);
                if( qml_capture ){
                    __triggerBinding(e, src_obj, name);
                }
            }

        }
    }
    return EVM_VAL_UNDEFINED;
}


int __qml_object_reg_len(evm_qml_object_reg_t * regs){
    if(regs == NULL) return 0;
    int i = 0;
    while(1){
        if( regs[i].name == NULL) return i;
        i++;
    }
    return i;
}

int __qml_value_reg_len(evm_qml_value_reg_t * regs){
    if(regs == NULL) return 0;
    int i = 0;
    while(1){
        if( regs[i].name == NULL ) return i;
        i++;
    }
    return i;
}

qml_object_t * __evm_qml_find_parent(evm_t *e, char * name, int len){
    for(int i = 0; i < len; i++){
        qml_object_t *obj = qml_objects + i;
        if( evm_str_lookup(e, name) == obj->name ){
            return obj;
        }
    }
    return NULL;
}

int evm_qml_register(evm_t * e, evm_qml_object_reg_t * regs){
    if( !regs ) return ec_err;
    int objects_len = __qml_object_reg_len(regs);
    qml_objects_len = objects_len;
    qml_objects = (qml_object_t*)evm_malloc(objects_len * sizeof(qml_object_t));
    memset(qml_objects, 0, objects_len * sizeof(qml_object_t));
    if(!qml_objects) return ec_memory;
    for(int i = 0; i < objects_len; i++){
        qml_objects[i].name = evm_str_insert(e, regs[i].name, 0);
        qml_objects[i].api = regs[i].api;
        if(regs[i].parent_name != NULL)
            qml_objects[i].parent = __evm_qml_find_parent(e, regs[i].parent_name, i);
        int values_len = __qml_value_reg_len(regs[i].values);
        if( values_len ){
            qml_objects[i].values_len = values_len;
            qml_objects[i].values = (qml_value_t*)evm_malloc(values_len * sizeof(qml_value_t));
            if(!qml_objects[i].values) return ec_memory;
            memset(qml_objects[i].values, 0, values_len * sizeof(qml_value_t));
            for(int j = 0; j < values_len; j++){
                qml_objects[i].values[j].api = regs[i].values[j].api;
                qml_objects[i].values[j].name = evm_str_insert(e, regs[i].values[j].name, 0);
                qml_objects[i].values[j].type = regs[i].values[j].type;
            }
        }
    }
    return ec_ok;
}

qml_object_t * __qml_object_get(intptr_t name){
    for(int i = 0; i < qml_objects_len; i++){
        qml_object_t * o = qml_objects + i;
        if( o->name == name )
            return o;
    }
    return NULL;
}

qml_value_t * __qml_value_get(qml_object_t * object, intptr_t name){
    qml_object_t * parent = object;
    while(parent){
        for(int i = 0; i < parent->values_len; i++){
            qml_value_t * o = parent->values + i;
            if( o->name == name )
                return o;
        }
        parent = parent->parent;
    }
    return NULL;
}

void evm_qml_object_set_pointer(evm_val_t * o, void * pointer){
    if( !evm_is_object(o) && !evm_is_class(o)) return;
    qml_object_t * qml_obj = (qml_object_t*)evm_object_get_ext_data(o);
    if( !qml_obj ) return;
    qml_obj->obj = pointer;
}

void * evm_qml_object_get_pointer(evm_val_t * o){
    if( !evm_is_object(o) && !evm_is_class(o)) return NULL;
    qml_object_t * qml_obj = (qml_object_t*)evm_object_get_ext_data(o);
    if( !qml_obj ) return NULL;
    return qml_obj->obj;
}

char * evm_qml_object_get_name(evm_t* e, evm_val_t * o){
    if( !evm_is_object(o) && !evm_is_class(o)) return NULL;
    qml_object_t * qml_obj = (qml_object_t*)evm_object_get_ext_data(o);
    if( !qml_obj ) return NULL;
    return evm_string_get(e, qml_obj->name);
}

void __qml_add_func_binding(evm_t * e, evm_val_t * obj, int type, uint32_t name, evm_val_t * func){
    EVM_UNUSED(e);
    if( func_bindings == NULL ){
        func_bindings = (qml_func_binding_t*)evm_malloc(sizeof(qml_func_binding_t));
        func_bindings->obj = *obj;
        func_bindings->name = name;
        func_bindings->func = *func;
        func_bindings->next = NULL;
        func_bindings->type = type;
    } else {
        qml_func_binding_t * next = func_bindings;
        if( next->obj == *obj && next->name == name) return;
        while( next->next ){
            if( next->next->obj == *obj && next->next->name == name) return;
            next = next->next;
        }
        qml_func_binding_t * bindings = (qml_func_binding_t*)evm_malloc(sizeof(qml_func_binding_t));
        bindings->obj = *obj;
        bindings->name = name;
        bindings->func = * func;
        bindings->next = NULL;
        bindings->type = type;
        next->next = bindings;
    }
}

void qml_object_gc_init(evm_t *e, evm_val_t *old_self, evm_val_t * new_self){
    EVM_UNUSED(new_self);
    qml_object_t * qml_obj = (qml_object_t*)evm_object_get_ext_data(old_self);
    qml_binding_t * next = qml_obj->bindings;
    while( next ){
        evm_gc_protect(e, &next->src_obj);
        evm_gc_protect(e, &next->tar_obj);
        evm_gc_protect(e, &next->tar_value);
        next = next->next;
    }
}

int __qml_value_check_type(qml_value_t * value, evm_val_t * v){
    if( !value ) return 0;
    if( value->type & EVM_QML_ANY ) return 1;
    if( (value->type & EVM_QML_INT) && evm_is_number(v) ) return 1;
    if( (value->type & EVM_QML_DOUBLE) && evm_is_number(v) ) return 1;
    if( (value->type & EVM_QML_STRING) && evm_is_string(v) ) return 1;
    if( (value->type & EVM_QML_BOOLEAN) && evm_is_boolean(v) ) return 1;
    return 0;
}

int __qml_root_add_object(evm_t * e, int index, const char * name, evm_val_t v){
    evm_prop_set(e, qml_root, index, name, v);
    return ec_ok;
}

int __qml_setup_root_object(evm_t * e, uint32_t name, evm_val_t * v){
    int err = ec_ok;
    evm_val_t * id = evm_prop_get(e, v, "id", 0);
    if( id && evm_is_string(id)){
        if( __qml_root_add_object(e, qml_object_index++, evm_2_string(id), *v) ){
            return evm_set_err(e, ec_name, "Multiple definition of id");
        }
    }
    else evm_prop_set_key_value(e, qml_root, qml_object_index++, name, *v);

    for(int i = 0; i < evm_prop_len(v); i++ ){
        evm_val_t * local_v = evm_prop_get_by_index(e, v, i);
        uint32_t key = evm_prop_get_key_by_index(e, v, i);
        if( evm_is_class(local_v) ){
            err = __qml_setup_root_object(e, key, local_v);
            if(err) return err;
        }
    }
    return err;
}

int __qml_object_get_group_len(qml_object_t * qml_obj){
    int count = 0;
    for(int i = 0; i < qml_obj->values_len; i++){
        if( qml_obj->values[i].type & EVM_QML_GROUP ) count++;
    }
    return count;
}

void __qml_object_build_group_and_parent(evm_t * e, qml_object_t * qml_obj,
                                         evm_val_t * o,
                                         evm_val_t * parent){

    int len = __qml_object_get_group_len(qml_obj) + 1;
    evm_attr_create(e, o, len);
    if( !parent ) evm_attr_set(e, o, 0, "parent", EVM_VAL_UNDEFINED);
    else {
        evm_set_parent(o, parent);
        evm_attr_set(e, o, 0, "parent", *parent);
    }
    int group_index = 1;
    for(int i = 0; i < qml_obj->values_len; i++){
        if( qml_obj->values[i].type & EVM_QML_GROUP ){
            len = __qml_object_reg_len(qml_obj->values[i].api);
            evm_val_t * local_obj = evm_object_create(e, GC_CLASS, len, 0);
            qml_object_t * local_qml_obj = (qml_object_t*)evm_malloc(sizeof(qml_object_t));
            memset(local_qml_obj, 0, sizeof(qml_object_t));
            local_qml_obj->name = qml_obj->values[i].name;
            local_qml_obj->values_len = len;
            if( len ) local_qml_obj->values = evm_malloc(sizeof(qml_value_t) * len);
            evm_object_set_ext_data(local_obj, (intptr_t)local_qml_obj);
            for(int j = 0; j < len; j++){
                evm_qml_value_reg_t * reg = (evm_qml_value_reg_t*)qml_obj->values[i].api;
                local_qml_obj->values[j].api = (reg + j)->api;
                local_qml_obj->values[j].type = (reg + j)->type;
                local_qml_obj->values[j].name = evm_str_insert(e, (reg + j)->name, 0);
                evm_prop_set_key_value(e, local_obj, j, local_qml_obj->values[j].name, EVM_VAL_UNDEFINED);
            }
            evm_attr_set_key_value(e, o, group_index++, qml_obj->values[i].name, *local_obj);
            evm_set_parent(local_obj, o);
            evm_object_deref(e, local_obj);
        }
    }
}

void * __qml_object_create(evm_t * e, evm_val_t * parent, uint32_t name, evm_val_t * v){
    evm_val_t * obj = v;
//    evm_object_set_init(obj, (evm_init_fn)qml_object_gc_init);
    qml_object_t * qml_obj = (qml_object_t*)evm_malloc(sizeof(qml_object_t));
    if(!qml_obj) MEMORY_ERR;
    evm_object_set_ext_data(obj, (intptr_t)qml_obj);

    int class_hash_name = evm_object_get_hash(v);
    qml_object_t * object_template = __qml_object_get(class_hash_name);
    if( object_template ) {
        memcpy(qml_obj, object_template, sizeof(qml_object_t));
        if( parent == NULL && qml_obj->api)
            qml_obj->api(e, obj, 0, NULL);
        else if( qml_obj->api )
            qml_obj->api(e, obj, 1, parent);
    } else {
        memset(qml_obj, 0, sizeof(qml_object_t));
    }
    __qml_object_build_group_and_parent(e, qml_obj, obj, parent);

    if( parent ){
        qml_object_t * parent_qml_obj = (qml_object_t*)evm_object_get_ext_data(parent);
        qml_value_t * value = __qml_value_get(parent_qml_obj, name);
        if(value && value->api){
            evm_native_fn api = value->api;
            api(e, obj, 1, obj);
        }
    }

    evm_run_callback(e, v, obj, NULL, 0);

    for(int i = 0; i < evm_prop_len(obj); i++ ){
        evm_val_t * local_v = evm_prop_get_by_index(e, obj, i);
        uint32_t key = evm_prop_get_key_by_index(e, obj, i);
        if( evm_is_class(local_v) ){
            __qml_object_create(e, obj, key, local_v);
        } else if( !evm_is_script(local_v) ){
            qml_value_t * value = __qml_value_get(qml_obj, key);
            if( value ){
                if( value->type & EVM_QML_CALLBACK ){
                    if(value->api) {
                        evm_native_fn api = value->api;
                        api(e, obj, 1, local_v);
                    }
                } if( __qml_value_check_type(value, local_v) ){
                    if(value->api) {
                        evm_native_fn api = value->api;
                        api(e, obj, 1, local_v);
                    }
                } else if(value){
                    evm_print("Dismatched qml value type\n");
                }
            }
        }
    }
    return qml_obj->obj;
}

evm_val_t evm_qml_bind(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(p);
    if( argc >= 3){
        evm_val_t *obj = v;
        if( evm_is_null(obj) || evm_is_undefined(obj) )
            return EVM_VAL_UNDEFINED;
        uint32_t key = evm_2_integer(v + 1);
        evm_val_t * local_v = v + 2;
        qml_object_t * qml_obj = (qml_object_t*)evm_object_get_ext_data(obj);
        if( evm_is_script(local_v)){
            qml_value_t * value = __qml_value_get(qml_obj, key);
            if( value) {
                if( (value->type & EVM_QML_ANY) == 0 )
                    __qml_add_func_binding(e, obj, value->type, key, local_v);
                evm_prop_set_key_value(e, qml_bindings_root, qml_binding_index++, key, *local_v);
                if( value->type & EVM_QML_CALLBACK){
                    if(value->api) {
                        evm_native_fn api = value->api;
                        api(e, obj, 1, local_v);
                    }
                } else {
                    evm_val_t res = evm_run_callback(e, local_v, obj, NULL, 0);
                    if( __qml_value_check_type(value, &res) ){
                        evm_val_t * fn = evm_prop_get_by_key(e, obj, key, 0);
                        * fn = res;
                    } else {
                        evm_print("Dismatched qml value type\n");
                    }
                }
            } else {
                __qml_add_func_binding(e, obj, EVM_QML_VALUE, key, local_v);
                evm_prop_set_key_value(e, qml_bindings_root, qml_binding_index++, key, *local_v);
                evm_val_t res = evm_run_callback(e, local_v, obj, NULL, 0);
                evm_val_t * target = evm_prop_get_by_key(e, obj, key, 0);
                if( target ){
                    *target = res;
                }
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}


evm_native_fn qml_init_api;
evm_val_t evm_qml_run(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(p);
    if( argc != 4 ) ARG_LENGTH_ERR;
    qml_object_index = 0;
    int qml_object_count = evm_2_integer(v + 2);
    int qml_binding_count = evm_2_integer(v + 3);
    evm_prop_create(e, qml_root, qml_object_count);
    evm_prop_create(e, qml_bindings_root, qml_binding_count);
    evm_object_set_scope(v + 1, qml_root);
    if( evm_is_class(v + 1) ){
        uint32_t obj_name = evm_2_integer(v);

        if( __qml_setup_root_object(e, obj_name, v + 1) ) return EVM_VAL_UNDEFINED;

        qml_capture = 0;
        __qml_object_create(e, NULL, obj_name, v + 1);

        qml_func_binding_t * binding = func_bindings;
        while(binding){
            if(binding->type == EVM_QML_ANY || binding->type == EVM_QML_CALLBACK){
               binding = binding->next;
               continue;
            }
            evm_val_t res = evm_run_callback(e, &binding->func, &binding->obj, NULL, 0);
            binding->last_executed_result = res;
            binding = binding->next;
        }
        qml_capture = 1;
        qml_init_api(e, qml_root, 0, NULL);
    }
    return EVM_VAL_UNDEFINED;
}

void qml_root_gc_init(evm_t *e, evm_val_t *old_self, evm_val_t * new_self){
    EVM_UNUSED(old_self);EVM_UNUSED(new_self);
    qml_func_binding_t * next = func_bindings;
    while( next ){
        evm_gc_protect(e, &next->obj);
        evm_gc_protect(e, &next->func);
        next = next->next;
    }
}

int qml_module(evm_t * e, evm_native_fn init_api){
    evm_builtin_t natives[] = {
        {".qml_write", evm_mk_native( (intptr_t)evm_qml_write )},
        {".qml_bind", evm_mk_native( (intptr_t)evm_qml_bind )},
        {".qml_run", evm_mk_native( (intptr_t)evm_qml_run )},
        {NULL, NULL}
    };
    qml_init_api = init_api;
    qml_root = evm_object_create(e, GC_OBJECT, 0, 0);
    qml_bindings_root = evm_object_create(e, GC_OBJECT, 0, 0);
//    evm_object_set_init(qml_root, (evm_init_fn)qml_root_gc_init);
    return evm_native_add(e, natives);
}
