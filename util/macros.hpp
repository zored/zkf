#pragma once

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>

#define _x_apply_array_elem(_, n, data) BOOST_PP_COMMA_IF(n) (data)[n]
#define _x_apply_array(func, ...) func(__VA_ARGS__)

#define _x_map_list_elem(func, ...) func(__VA_ARGS__)
#define _x_map_list(_, func, i, elem) \
    BOOST_PP_COMMA_IF(i) _x_map_list_elem(func, BOOST_PP_REMOVE_PARENS(elem))

/**
 * Apply function/macro `func` to a sequence of array elements:
 *      func(arr[0], ..., arr[size - 1])
 *
 * @param  arr  amything indexable that accepts numbers
 * @param size  number of elements to expand
 * @param func  macro (or function) that accepts `size` arguments
 */
#define apply_array(func, size, arr) \
    _x_apply_array(func, BOOST_PP_REPEAT(size, _x_apply_array_elem, arr))

/**
 * Apply function/macro `func` to each of of macro arguments:
 *      func(0, arg0), func(1, arg1), ..., func(n, argn)
 * @param func  macro (or function) that accepts multiple arguments
 * @param  ...  arguments (could be tuples if func accepts multiple arguments)
 */
#define map_list(func, ...) \
    BOOST_PP_SEQ_FOR_EACH_I(_x_map_list, func, BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))
