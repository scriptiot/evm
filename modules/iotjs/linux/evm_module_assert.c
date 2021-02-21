#ifdef CONFIG_EVM_MODULE_ASSERT
#include "evm_module.h"

enum {
    NOT_EQUAL, // !=
    EQUAL,     // ==
    GREATER,   // >
    GREATER_THAN, // >=
    LESS_THAN,    // <=
    LESS          // <
};

//assert(value[, message])
static evm_val_t evm_module_assert_assert(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1)
        return EVM_VAL_UNDEFINED;

    char *message = NULL;

    if (argc > 1 && evm_is_string(v + 1))
        message = evm_2_string(v + 1);

    if (evm_is_boolean(v))
    {
        if (evm_2_boolean(v))
            return EVM_VAL_TRUE;
        evm_set_err(e, ec_type, message);
        return NULL;
    }
    else if (evm_is_number(v))
    {
        if (evm_2_integer(v))
            return EVM_VAL_TRUE;
        evm_set_err(e, ec_type, message);
        return NULL;
    }
}

//doesNotThrow(block[, message])
static evm_val_t evm_module_assert_doesNotThrow(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_script(v))
        return EVM_VAL_UNDEFINED;

    char *message = NULL;
    if (argc > 1 && evm_is_string(v + 1))
        message = evm_2_string(v + 1);

    if (evm_run_callback(e, v, NULL, NULL, 0) == EVM_VAL_TRUE) {
        return EVM_VAL_TRUE;
    } else {
        evm_set_err(e, ec_type, message);
        return NULL;
    }
}

static evm_val_t compare(evm_val_t *l, evm_val_t *r) {
    if (evm_is_integer(l)) {
        if (evm_is_integer(r)) {
            if (evm_2_integer(l) == evm_2_integer(r)) {
                return EVM_VAL_TRUE;
            } else {
                return EVM_VAL_FALSE;
            }
        } else {
            return EVM_VAL_FALSE;
        }
    } else if (evm_is_boolean(l)) {
        if (evm_2_boolean(r)) {
            if (evm_2_boolean(l) == evm_2_boolean(r)) {
                return EVM_VAL_TRUE;
            } else {
                return EVM_VAL_FALSE;
            }
        } else {
            return EVM_VAL_FALSE;
        }
    } else if (evm_is_string(l)) {
        if (evm_is_string(r)) {
            if (strcmp(evm_2_string(l), evm_2_string(r)) == 0) {
                return EVM_VAL_TRUE;
            } else {
                return EVM_VAL_FALSE;
            }
        } else {
            return EVM_VAL_FALSE;
        }
    } else if (evm_is_object(l)) {
        if (evm_is_object(r)) {
            if (evm_object_get_hash(l) != evm_object_get_hash(r)) {
                return EVM_VAL_FALSE;
            } else {
                return EVM_VAL_TRUE;
            }
        }
        else {
            return EVM_VAL_FALSE;
        }
    } else {
        return EVM_VAL_FALSE;
    }
}

//equal(actual, expected[, message])
static evm_val_t evm_module_assert_equal(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc <= 1)
        return EVM_VAL_UNDEFINED;

    char *message = NULL;
    if (argc > 1 && evm_is_string(v + 1))
        message = evm_2_string(v + 1);

    evm_val_t result = compare(v, v + 1);
    if (result == EVM_VAL_TRUE) return result;
    else {
        evm_set_err(e, ec_type, message);
        return NULL;
    }
}

static evm_val_t compare_by_value(evm_val_t *l, evm_val_t *r, int operator) {
    evm_val_t *result;
    switch (operator) {
    case NOT_EQUAL: {
        if (evm_2_integer(l) != evm_2_integer(r)) {
            result = EVM_VAL_FALSE;
        } else {
            result = EVM_VAL_TRUE;
        }
        break;
    }
    case EQUAL: {
        if (evm_2_integer(l) == evm_2_integer(r)) {
            result = EVM_VAL_FALSE;
        } else {
            result = EVM_VAL_TRUE;
        }
        break;
    }
    case GREATER: {
        if (evm_2_integer(l) > evm_2_integer(r)) {
            result = EVM_VAL_FALSE;
        } else {
            result = EVM_VAL_TRUE;
        }
        break;
    }
    case GREATER_THAN: {
        if (evm_2_integer(l) >= evm_2_integer(r)) {
            result = EVM_VAL_FALSE;
        } else {
            result = EVM_VAL_TRUE;
        }
        break;
    }
    case LESS_THAN: {
        if (evm_2_integer(l) <= evm_2_integer(r)) {
            result = EVM_VAL_FALSE;
        } else {
            result = EVM_VAL_TRUE;
        }
        break;
    }
    case LESS: {
        if (evm_2_integer(l) < evm_2_integer(r)) {
            result = EVM_VAL_FALSE;
        } else {
            result = EVM_VAL_TRUE;
        }
        break;
    }
    }
    return *result;
}

//fail(actual, expected, message, operator)
//operator: != | == | > | >= | < | <=
static evm_val_t evm_module_assert_fail(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc <= 3 || !evm_is_string(v + 3))
        return EVM_VAL_UNDEFINED;

    char *message = evm_2_string(v + 2);

    evm_val_t result = evm_module_assert_equal(e, p, argc, v);
    if (result != EVM_VAL_TRUE) {
        evm_set_err(e, ec_type, message);
        return NULL;
    }

    char *operator = evm_2_string(v + 3);
    if (strcmp(operator, "!=") == 0) {
        result = compare_by_value(v, v + 1, NOT_EQUAL);
    } else if (strcmp(operator, "==") == 0) {
        result = compare_by_value(v, v + 1, EQUAL);
    } else if (strcmp(operator, ">") == 0) {
        result = compare_by_value(v, v + 1, GREATER);
    } else if (strcmp(operator, ">=") == 0) {
        result = compare_by_value(v, v + 1, GREATER_THAN);
    } else if (strcmp(operator, "<=") == 0) {
        result = compare_by_value(v, v + 1, LESS_THAN);
    } else if (strcmp(operator, "<") == 0) {
        result = compare_by_value(v, v + 1, LESS);
    } else {
        result = EVM_VAL_UNDEFINED;
    }
    return result;
}

//notEqual(actual, expected[, message])
static evm_val_t evm_module_assert_notEqual(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2)
        return EVM_VAL_UNDEFINED;

    char *message = NULL;
    if (argc > 1 && evm_is_string(v + 2))
        message = evm_2_string(v + 2);

    evm_val_t result = compare(v, v + 1);
    if (result == EVM_VAL_TRUE) {
        evm_set_err(e, ec_type, message);
        return NULL;
    }
    return EVM_VAL_TRUE;
}

//notStrictEqual(actual, expected[, message])
static evm_val_t evm_module_assert_notStrictEqual(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_assert_notEqual(e, p, argc, v);
}

//strictEqual(actual, expected[, message])
static evm_val_t evm_module_assert_strictEqual(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_assert_equal(e, p, argc, v);
}

//throws(block[, expected, message])
static evm_val_t evm_module_assert_throws(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_script(v))
        return EVM_VAL_UNDEFINED;

    char *message = NULL;
    if (argc > 1 && evm_is_string(v + 1))
        message = evm_2_string(v + 1);

    if (evm_run_callback(e, v, NULL, NULL, 0) == EVM_VAL_TRUE) {
        evm_set_err(e, ec_type, message);
        return NULL;
    } else {
        return EVM_VAL_TRUE;
    }
}

evm_err_t evm_module_assert(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"assert", evm_mk_native((intptr_t)evm_module_assert_assert)},
        {"doesNotThrow", evm_mk_native((intptr_t)evm_module_assert_doesNotThrow)},
        {"equal", evm_mk_native((intptr_t)evm_module_assert_equal)},
        {"fail", evm_mk_native((intptr_t)evm_module_assert_fail)},
        {"notEqual", evm_mk_native((intptr_t)evm_module_assert_notEqual)},
        {"notStrictEqual", evm_mk_native((intptr_t)evm_module_assert_notStrictEqual)},
        {"strictEqual", evm_mk_native((intptr_t)evm_module_assert_strictEqual)},
        {"throws", evm_mk_native((intptr_t)evm_module_assert_throws)},
        {NULL, NULL}};
    evm_module_create(e, "assert", builtin);
    return e->err;
}
#endif
