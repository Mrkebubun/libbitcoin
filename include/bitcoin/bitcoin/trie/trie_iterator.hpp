/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef LIBBITCOIN_TRIE_ITERATOR_HPP
#define LIBBITCOIN_TRIE_ITERATOR_HPP

#include <bitcoin/bitcoin/trie/trie_structure_node.hpp>
#include <bitcoin/bitcoin/trie/trie_value_node.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
class binary_trie;

template<typename Value, typename Reference, typename Pointer>
class trie_iterator
{
    template<class V, class StructureNodeAllocator, class ValueNodeAllocator,
        class Comparer> friend class binary_trie;

public:
    // iterator_traits required typedefs
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Value value_type;

    typedef trie_iterator<Value, Reference, Pointer> iter_type;
    typedef trie_iterator<Value, Value&, Value*> iterator;
    typedef trie_iterator<Value, const Value&, const Value*> const_iterator;
    typedef trie_structure_node<Value> structure_node_type;
    typedef trie_value_node<Value> value_node_type;

public:
    // constructors
    explicit trie_iterator();
    trie_iterator(structure_node_type* node);
    trie_iterator(value_node_type* value);
    trie_iterator(structure_node_type* node, value_node_type* value);
    trie_iterator(const iterator& it);

    // accessors
    binary_type get_key();

    // iterator methods
    reference operator*() const;
    pointer operator->() const;
    bool operator==(const trie_iterator& other) const;
    bool operator!=(const trie_iterator& other) const;
    iter_type& operator++();
    iter_type operator++(int);
    iter_type& operator--();
    iter_type operator--(int);

protected:
    // increment/decrement implementation
    void trie_node_increment();
    void trie_node_decrement();
    void increment();
    void decrement();

private:
    structure_node_type* trie_node_;
    value_node_type* value_node_;
    binary_type cached_key_;
};

}

#include <bitcoin/bitcoin/impl/trie/trie_iterator.ipp>

#endif
