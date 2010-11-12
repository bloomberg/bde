/************************************************************************
 *
 * braceexp.cpp - definitions of rw_brace_expand and rw_shell_expand
 *
 * $Id: braceexp.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <stdlib.h> // for malloc(), free()
#include <string.h> // for memcpy()
#include <ctype.h>  // for isspace()
#include <assert.h> // for assert()

#include <rw_braceexp.h>


// for convenience
typedef unsigned char UChar;


// search `beg' to `end' for a character that `fn'
// returns non-zero.
static const char*
_rw_find_match (const char* beg,
                const char* end,
                bool match_space)
{
    bool is_escaped = false;

    for (/**/; beg < end; ++beg) {

        const bool is_space = 0 != isspace (UChar (*beg));

        if (!is_escaped && match_space == is_space) {
            return beg;
        }

        is_escaped = !is_escaped && (*beg == '\\');
    }

    return 0;
}

// similar to sprintf (buffer, "%ld", value), but the output
// is not locale dependent. we choose not to use code from
// the test harness because we may need to move this around.
static int _rw_itoa_10 (char* buffer, long value)
{
    const bool neg = value < 0;

    if (neg)
        value = 0 - value;

    // write it out in reverse
    int n = 0;
    do {

        const long q = value / 10;
        const long r = value - (q * 10);
        value = q;

        switch (r)
        {
            case 0: buffer [n] = '0'; break;
            case 1: buffer [n] = '1'; break;
            case 2: buffer [n] = '2'; break;
            case 3: buffer [n] = '3'; break;
            case 4: buffer [n] = '4'; break;
            case 5: buffer [n] = '5'; break;
            case 6: buffer [n] = '6'; break;
            case 7: buffer [n] = '7'; break;
            case 8: buffer [n] = '8'; break;
            case 9: buffer [n] = '9'; break;
            default:                  break;
        }

        n += 1;

    } while (value != 0);

    // write the sign
    if (neg)
        buffer [n++] = '-';

    // then reverse it
    for (int i = 0, j = n - 1; i < j; ++i, --j)
    {
        const char c = buffer [i];
        buffer [i] = buffer [j];
        buffer [j] = c;
    }

    return n;
}


// search `beg' to `end' for the next unescaped open brace . if `end'
// is 0 then the search will terminate at the end of string. returns 0
// on failure.
//
// this function ignores escaped braces and those that look like they
// belong to a shell variable expansion. this is for consistency with
// bash. i.e. there is no open brace in a\{b\}c or a${b}c.
static const char* _rw_find_open_brace (const char* beg,
                                        const char* end)
{
    bool is_escaped = false;
    bool is_shelled = false;

    for (/**/; beg < end; ++beg) {

        const bool is_match = (*beg == '{');
        if (!is_escaped && !is_shelled && is_match) {
            return beg;
        }

        is_shelled = !is_shelled && (*beg == '$');
        is_escaped = !is_escaped && (*beg == '\\');
    }

    return 0;
}

// search `beg' to `end' for the next close brace at the current brace
// depth. if `end' is 0, the search will continue until a match or end
// of string. returns 0 on failure.
static const char* _rw_find_close_brace (const char* beg,
                                         const char* end)
{
    bool is_escaped = false;

    // nested brace depth
    for (int depth = 1; beg < end; ++beg) {

        const bool is_open_brace = (*beg == '{');
        if (!is_escaped && is_open_brace) {
            ++depth;
        }

        const bool is_close_brace = (*beg == '}');
        if (!is_escaped && is_close_brace) {
            --depth;
        }

        is_escaped = !is_escaped && (*beg == '\\');

        if (depth == 0)
            return beg;
    }

    return 0;
}

// search `beg' to `end' for the next unescaped comma at the current
// brace depth. if `end' is 0, the search will continue until a match
// or end of string. returns 0 on failure.
static const char* _rw_find_next_comma (const char* beg,
                                        const char* end)
{
    bool is_escaped = false;

    // nested brace depth
    for (int depth = 0; beg < end; ++beg) {

        const bool is_open_brace = (*beg == '{');
        if (!is_escaped && is_open_brace) {
            ++depth;
        }

        const bool is_close_brace = (*beg == '}');
        if (!is_escaped && is_close_brace) {
            --depth;
        }

        const bool is_comma = (*beg == ',');
        if (!is_escaped && is_comma) {
            if (depth == 0)
                return beg;
        }

        is_escaped = !is_escaped && (*beg == '\\');
    }

    return 0;
}

struct _rw_string_buffer
{
    _rw_string_buffer (char* s, size_t n)
        : capacity_ (n)
        , length_ (0)
        , buffer_ (s)
        , owned_ (false)
    {
    }

    // destructor does not deallocate memory
    // user expected to do that

    size_t capacity_;
    size_t length_;
    char* buffer_;
    bool  owned_;

    bool append (const char* s, size_t n);

private:
    // not implemented
    _rw_string_buffer (const _rw_string_buffer&);
    _rw_string_buffer& operator= (const _rw_string_buffer&);
};

// this is where most of the work is done.
struct _rw_brace_graph
{
    //
    _rw_brace_graph ();

    //
    ~_rw_brace_graph ();

    // expand brace expression from `beg' to `end' into `len' bytes of
    // `buf'. if it won't fit, we allocate a buffer with malloc() and
    // return that. so the caller needs to free() the return buffer if
    // it is not equal to `buf'.
    char* build_and_expand (const char* beg,
                            const char* end,
                            char* buf, size_t len, char sep);


private:

    // not implemented
    _rw_brace_graph (const _rw_brace_graph&);
    _rw_brace_graph& operator= (const _rw_brace_graph&);

    // node for a directed-acyclic-graph that we build from the original
    // brace expression
    struct _rw_brace_node
    {
        const char* str_;
        size_t      len_;

        _rw_brace_node* sibling_;
        _rw_brace_node* child_;
    };

    // retrieve a new node. nodes are allocated in large blocks. those
    // blocks are deallocated when this graph instance is destroyed.
    // and they are reused for every build_and_expand() call.
    _rw_brace_node* get_new_node ();

    // this function generates a dag from an arbitrary brace expression.
    // the format is `prefix{body}suffix'. prefix, and suffix can both
    // be the empty string. body may be a comma seperated list of tokens,
    // a sequence of tokens, or arbitrary literal text. escapes on commas
    // and braces are ignored, and left intact otherwise.
    _rw_brace_node* build_anything (const char* beg, const char* end);

    // generates a dag from an arbitrary sequence brace expression. the
    // format is `{?..?}suffix' where both ? are alphabetic characters
    // of the same character class [upper/lower].
    _rw_brace_node* build_character_sequence (const char* beg,
                                              const char* end);

    // generates a dag from an arbitrary sequence brace expression. the
    // format is `{?..?}suffix' where both ? are integer expressions.
    _rw_brace_node* build_integer_sequence (const char* beg,
                                            const char* end);

    // generates a dag from an arbitrary list brace expression. the
    // format is `{a,b[,c]}suffix', where `a', `b' and `c' are full
    // brace expansions that would be processed by build_anything.
    _rw_brace_node* build_list     (const char* beg, const char* end);

    // the number of nodes held by each brace buffer [see below]
    enum { size = 64 };

#ifdef _RWSTD_NO_NESTED_CLASS_ACCESS

    // allow _rw_brace_node_buffer access to _rw_brace_graph's private
    // type(s) if the resolution of cwg issue 45 is not yet implemented
    struct _rw_brace_node_buffer;
    friend struct _rw_brace_node_buffer;

    // allow _rw_recursion_context access to _rw_brace_graph's private
    // type(s) if the resolution of cwg issue 45 is not yet implemented
    struct _rw_recursion_context;
    friend struct _rw_recursion_context;

#endif    // _RWSTD_NO_NESTED_CLASS_ACCESS

    // this is essentially a rope with a fixed length payload of
    // brace nodes
    struct _rw_brace_node_buffer
    {
        _rw_brace_node nodes_ [size];
        size_t         used_; // number of nodes_ used in this buffer
        _rw_brace_node_buffer* next_;
    };

    // the initial set of nodes is preallocated as part of this graph
    // object instance. additional blocks of nodes will be allocated
    // as is necessary by the get_new_node() member.
    _rw_brace_node_buffer node_cache_;    
    _rw_brace_node_buffer* nodes_; // pointer to last allocated node buffer

    // code for handling integer ranges

    void reset_for_reuse (char* buf, size_t len);
    void free_range_buffers ();

    // this is essentially a rope with a variable length payload
    struct _rw_range_buffer
    {
        _rw_range_buffer* next_;
    };

    _rw_range_buffer* ranges_;

    // code for generating the string

    _rw_string_buffer string_;

    // we use this to walk the stack. we need to walk from the root
    // of the tree to each leaf. when we reach a leaf, we need a way
    // to see the path that we took to get where we are. we use this
    // path to write out each part of the full expansion.
    struct _rw_recursion_context
    {
        _rw_recursion_context (_rw_brace_node* pnode)
            : parent_ (0), node_ (pnode)
        {
        }

        _rw_recursion_context (_rw_recursion_context* parent)
            : parent_ (parent), node_ (parent->node_->child_)
        {
        }

        _rw_recursion_context* parent_;
        _rw_brace_node* node_;
    };

    // this function walks the dag, leaving a trail of context
    // objects so we can walk back to the root and write the data
    // at each level in the graph.
    bool brace_expand (_rw_recursion_context* self, char sep);

    // this function writes the data at each level of the dag
    // to our internal buffer.
    bool brace_expand_write (_rw_recursion_context* context);
};

_rw_brace_graph::_rw_brace_graph ()
    : nodes_ (&node_cache_)
    , ranges_ (0)
    , string_ (0, 0)
{
    node_cache_.next_ = 0;
}

_rw_brace_graph::~_rw_brace_graph ()
{
    _rw_brace_node_buffer* nodes = node_cache_.next_;
    while (nodes) {

        _rw_brace_node_buffer* next = nodes->next_;
        nodes->next_ = 0;

        // deallocate this buffer
        free (nodes);

        // setup to deallocate next one
        nodes = next;
    }

    reset_for_reuse (0, 0);
}



char*
_rw_brace_graph::build_and_expand (const char* beg,
                                   const char* end,
                                   char* buf, size_t len, char sep)
{
    assert (beg != 0);
    assert (end != 0);

    if (!beg || !end)
        return 0;

    // reset member data so we can reuse allocated buffers
    // if there are any
    reset_for_reuse (buf, len);

    _rw_brace_node* root
        = build_anything (beg, end);
    if (!root)
        return 0;

   // this helps us do the building of the output string
    _rw_recursion_context context (root);

    if (!brace_expand (&context, sep)) {
        if (string_.owned_)
            free (string_.buffer_);
        string_.buffer_ = 0;
    }

    // kill the last seperator with a null terminator
    else if (string_.buffer_) {
        const size_t pos = string_.length_ < 1 ? 0 : string_.length_ - 1;
        string_.buffer_ [pos] = '\0';
    }

    return string_.buffer_;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::get_new_node ()
{
    nodes_->used_ += 1;

    // if we run out of space, move to the next buffer
    if (! (nodes_->used_ < size)) {

        // if we have got a buffer, reuse it
        if (nodes_->next_) {

            nodes_ = nodes_->next_;
        }

        // otherwise we allocate one
        else {

            const size_t sz = sizeof (_rw_brace_node_buffer);

            nodes_->next_ = (_rw_brace_node_buffer*)malloc (sz);
            if (!nodes_->next_)
                return 0;

            nodes_ = nodes_->next_;
            nodes_->next_ = 0;
        }

        nodes_->used_ = 1;
    }

    _rw_brace_node* node = &nodes_->nodes_ [nodes_->used_ - 1];
    node->str_     = 0;
    node->len_     = 0;
    node->sibling_ = 0;
    node->child_   = 0;

    return node;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::build_anything (const char* beg, const char* end)
{
    // 
    const char* open_brace = _rw_find_open_brace (beg, end);
    if (open_brace) {

        // build a node for the prefix if there is one
        _rw_brace_node* prefix = get_new_node ();
        if (!prefix)
            return 0;
              
        prefix->str_ = beg;
        prefix->len_ = (open_brace - beg);

        _rw_brace_node* child = 0;
        
        // try to build a character sequence expansion like {a..g}
        child = build_character_sequence (open_brace, end);
        if (!child) {

            // try to do an integer sequence expansion like {-19..+72}
            child = build_integer_sequence (open_brace, end);
            if (!child) {

                // try to do a list expansion like {a,b,cd}
                child = build_list (open_brace, end);
                if (!child) {

                    // we can't figure out what to do, so we fail
                    return 0;
                }
            }
        }

        // we must have a valid child pointer at this point
        prefix->child_ = child;
   
        return prefix;
    }

    // there was no open brace, so the entire text from beg to end
    // is a literal
    _rw_brace_node* prefix = get_new_node ();
    if (!prefix)
        return 0;

    prefix->str_ = beg;
    prefix->len_ = beg < end ? (end - beg) : 0;

    return prefix;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::build_integer_sequence (const char* beg, const char* end)
{
    // check for {-10..100} type sequence expression. make sure not to
    // reference past the end of the string.
    if (*beg++ != '{')
        return 0;

    // this should point to first character after the integer value
    char* pend;

    // parse the first integer
    long ibeg = strtol (beg, &pend, 10);
    if (!ibeg && (pend == beg))
        return 0; // failed to parse an integer value

    // number of characters needed to represent ibeg
    const size_t ibeg_dig = (pend - beg);

    // make sure we have two dots
    beg = pend;
    if (! (beg [0] == '.' && beg [1] == '.'))
        return 0;

    // skip the two dots
    beg += 2;

    // parse the second integer
    long iend = strtol (beg, &pend, 10);
    if (!iend && (pend == beg))
        return 0; // failed to parse an integer value

    // number of characters needed to represent iend
    const size_t iend_dig = (pend - beg);

    // make sure we have an end brace
    beg = pend;
    if (beg [0] != '}')
        return 0;
    beg += 1;

    // build the suffix
    _rw_brace_node* suffix = build_anything (beg, end);
    if (!suffix)
        return 0; // failed to parse suffix

    // direction the range goes, +1 for increasing, -1 for decreasing
    const int dir = (ibeg < iend) ? 1 : -1;

    // maximum length of the string representation of a single
    // integer in the range
    const size_t len = (ibeg_dig < iend_dig ? iend_dig : ibeg_dig);

    // number of integers in the range [ibeg, iend]
    const size_t num = 1 + (iend - ibeg) * dir;

    // maximum number of bytes needed to represent all of the numbers
    // and a single null
    const size_t cnt = 1 + (num * len);

    // number of bytes we have to allocate, cnt of which is data
    // and the rest is to allow us to chain these buffers together
    const size_t bsz = cnt + sizeof (_rw_range_buffer);

    // allocate a rope segment big enough for all of the strings
    // we need to keep.
    _rw_range_buffer* buffer
        = (_rw_range_buffer*)malloc (bsz);
    if (!buffer)
        return 0;

    // add buffer to our list so we can free it later
    buffer->next_ = ranges_;
    ranges_ = buffer;

    // pointer to the current write position in the buffer
    char* ranges = (char*)&buffer [1];

    _rw_brace_node* first_child = 0;
    _rw_brace_node* final_child = 0;

    // build a list of children, associate each with suffix
    for (/**/; ibeg != iend; ibeg += dir) {

        // create a child from whatever is between beg and end. that childs
        // next pointer will be the suffix we created above.
        _rw_brace_node* child = get_new_node ();
        if (!child)
            return 0;

        // add a representation of cbeg to the ranges buffer
        child->len_ = _rw_itoa_10 (ranges, ibeg);
        child->str_ = ranges;

        // step past number of written characters
        ranges += child->len_;

        // track children we have created
        if (!first_child)
            first_child = child;

        if (final_child)
            final_child->sibling_ = child;
        final_child = child;

        // suffix is the suffix of the child expression
        while (child->child_)
            child = child->child_;
        child->child_ = suffix;
    }

    // create the last node in the sequence.
    _rw_brace_node* child = get_new_node ();
    if (!child)
        return 0;

    // add a representation of cbeg to the ranges buffer
    child->len_ = _rw_itoa_10 (ranges, ibeg);
    child->str_ = ranges;

    // trach child
    if (!first_child)
        first_child = child;

    if (final_child)
        final_child->sibling_ = child;
    final_child = child;

    // suffix is the suffix of the child expression
    while (child->child_)
        child = child->child_;
    child->child_ = suffix;

    return first_child;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::build_character_sequence (const char* beg, const char* end)
{
    // check for {a..z} type sequence expression. make sure not to
    // reference past the end of the string.
    if (   beg [0] != '{'
        || beg [1] == '\0'
        || beg [2] != '.'
        || beg [3] != '.'
        || beg [4] == '\0'
        || beg [5] != '}')
        return 0;

    // grab characters that represent the start and end of the sequence
    char cbeg = beg [1];
    char cend = beg [4];

    // only works if sequence characters are both lowercase or uppercase.
    const int both_are_lower =
        islower (UChar (cbeg)) && islower (UChar (cend));
    const int both_are_upper =
        isupper (UChar (cbeg)) && isupper (UChar (cend));

    if (! (both_are_lower || both_are_upper))
        return 0;

    // these must live for duration of program
    static const char* alpha_table [] =
    {
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    };

    const char* sequence = alpha_table [both_are_upper];

    const int dir = (cbeg < cend) ? 1 : -1;

    // build the suffix
    _rw_brace_node* suffix = build_anything (beg + 6, end);
    if (!suffix)
        return 0; // failed to parse suffix

    _rw_brace_node* first_child = 0;
    _rw_brace_node* final_child = 0;

    // build a list of children, associate each with suffix
    for (/**/; cbeg != cend; cbeg += dir) {

        // create a child from whatever is between beg and end. that childs
        // next pointer will be the suffix we created above.
        _rw_brace_node* child = get_new_node ();
        if (!child)
            return 0;

        // this finds beg in our array, we could have used strchr
        child->str_ = sequence + (cbeg - sequence [0]);
        child->len_ = 1;

        // track children we have created
        if (!first_child)
            first_child = child;

        if (final_child)
            final_child->sibling_ = child;
        final_child = child;

        // suffix is the suffix of the child expression
        while (child->child_)
            child = child->child_;
        child->child_ = suffix;
    }

    // create the last node in the sequence.
    _rw_brace_node* child = get_new_node ();
    if (!child)
        return 0;

    child->str_ = sequence + (cbeg - sequence [0]);
    child->len_ = 1;

    // trach child
    if (!first_child)
        first_child = child;

    if (final_child)
        final_child->sibling_ = child;
    final_child = child;

    // suffix is the suffix of the child expression
    while (child->child_)
        child = child->child_;
    child->child_ = suffix;

    return first_child;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::build_list (const char* beg, const char* end)
{
    // we really expect that the first token is an open paren the
    // caller should have consumed the prefix before calling this
    if (*beg++ != '{')
        return false;

    // now fill in the middle, each child we create directly will
    // have a child pointer to the suffix node

    // special case {}?

    // find the end of the brace list
    const char* end_of_list = _rw_find_close_brace (beg, end);
    if (!end_of_list)
        return false; // no list terminator

    // build a node for the suffix.
    _rw_brace_node* suffix = build_anything (end_of_list + 1, end);
    if (!suffix)
        return false; // failed to parse end

    // find the end of the first comma seperated token
    const char* mid = _rw_find_next_comma (beg, end_of_list);

    _rw_brace_node* first_child = 0;
    _rw_brace_node* final_child = 0;

    while (mid) {
    
        // create a child from whatever is between beg and end. that childs
        // next pointer will be the suffix we created above.
        _rw_brace_node* child = build_anything (beg, mid);
        if (!child)
            return false;

        if (!first_child)
            first_child = child;
        
        // append new child to end of chain
        if (final_child)
            final_child->sibling_ = child;
        final_child = child;

        // the nex pointer for this child is the suffix

        // suffix is the suffix of the child expression
        while (child->child_)
            child = child->child_;
        child->child_ = suffix;

        beg = mid + 1;
        mid = _rw_find_next_comma (beg, end_of_list);
    }

    // okay, we have a pointer to the last comma in the list. create an
    // item for the data between the comma and the list terminator

    // '{abc,d{1..3}e}a'
    //      ^        ^ ^
    //    beg      eol end

    // build nodes from the last entry in the list
    _rw_brace_node* child = build_anything (beg, end_of_list);
    if (!child)
        return false;

    if (!first_child)
        first_child = child;

    if (final_child)
        final_child->sibling_ = child;
    final_child = child;

    while (child->child_)
        child = child->child_;
    child->child_ = suffix;

    // success, return first child in this expansion
    return first_child;
}

void _rw_brace_graph::reset_for_reuse (char* buf, size_t len)
{
    node_cache_.used_ = 0;
    nodes_ = &node_cache_;

    string_.buffer_   = buf;
    string_.capacity_ = len;
    string_.length_   = 0;

    // we free these buffers because the size of the buffer
    // depends on the expression we are evaluating
    free_range_buffers ();
}

void _rw_brace_graph::free_range_buffers ()
{
    _rw_range_buffer* ranges = ranges_;
    while (ranges) {

        _rw_range_buffer* next = ranges->next_;
        ranges->next_ = 0;

        free (ranges);

        ranges = next;
    }

    ranges_ = 0;
}


bool _rw_brace_graph::brace_expand_write (_rw_recursion_context* context)
{
    if (context->parent_) {
        if (!brace_expand_write (context->parent_))
            return false;
    }

    bool is_escaped = false;

    const char* beg = context->node_->str_;
    const size_t len = context->node_->len_;

    for (size_t n = 0; n < len; ++n, ++beg) {

        is_escaped = !is_escaped && (*beg == '\\');
        if (!is_escaped) {
            if (!string_.append (beg, 1))
                return false;
        }
    }

    return true;
}

bool _rw_brace_graph::brace_expand (_rw_recursion_context* self, char sep)
{
    // if this node has no children or siblings, we have found a full
    // expansion.
    if (!self->node_ ||
        !self->node_->sibling_ && !self->node_->child_) {

        const size_t length_before = string_.length_;

        // use recursion again to walk back to the root the graph and
        // write each contexts data as we unwind back toward the leaf
        if (!brace_expand_write (self))
            return false;

        const size_t length_after = string_.length_;

        // don't write a seperator if we wrote no data
        if (length_before != length_after && !string_.append (&sep, 1))
            return false;

        return true;
    }
    
    // iterate through all of the children of the node, thus finding all
    // other combinations
    _rw_recursion_context context (self);
    while (context.node_) {

        if (!brace_expand (&context, sep))
            return false;

        context.node_ = context.node_->sibling_;
    }

    return true;
}

bool _rw_string_buffer::append (const char* s, size_t n)
{
    const size_t new_len = length_ + n;

    // not enough space, grow buf
    if (! (new_len < capacity_)) {

        // buf grows in 256 byte blocks
        size_t new_cap = capacity_;
        while (! (new_len < new_cap))
            new_cap += 256;

        char* new_buf = (char*)malloc (new_cap);
        if (!new_buf)
            return false;

        // copy the old data
        memcpy (new_buf, buffer_, length_);

        // copy the new data
        memcpy (new_buf + length_, s, n);
        new_buf [new_len] = '\0';

        // don't free until after append because `s' may
        // be in string_buf_
        if (owned_)
            free (buffer_);

        capacity_ = new_cap;
        length_   = new_len;
        buffer_   = new_buf;
        owned_    = true;
    }

    // just copy the data
    else {
        memcpy (buffer_ + length_, s, n);
        buffer_ [new_len] = '\0';
        length_ = new_len;
    }

    return true;
}

//
char* rw_brace_expand (const char* brace_expr,
                       size_t sz,
                       char* s, size_t n, char sep)
{
    if (!brace_expr)
        return 0;

    // if the length isn't provided, assume entire string
    if (!sz)
        sz = strlen (brace_expr);

    _rw_brace_graph graph;

    // build the graph, and then expand it into buf
    char* buf = graph.build_and_expand (brace_expr,
                                        brace_expr + sz, s, n, sep);
    if (!buf)
        return 0;

    return buf;
}


//
char* rw_shell_expand (const char* shell_expr, size_t sz,
                       char* s, size_t n, char sep)
{
    if (!shell_expr)
        return 0;

    // if the length isn't provided, assume entire string
    if (!sz)
        sz = strlen (shell_expr);

    // assume shell_expr is null terminated
    const char* beg = shell_expr;
    const char* end = shell_expr + sz;

    // helper for output
    _rw_string_buffer result (s, n);

    // helper for expanding braces
    _rw_brace_graph graph;

    // first non-whitespace character
    const char* tok_beg = _rw_find_match (beg, end, false);
    if (!tok_beg) {

        if (!result.append ("\0", 1))
            return 0;

        return result.buffer_;
    }

    bool is_first_expand = true;

    while (tok_beg)
    {
        // first whitespace character
        const char* tok_end = _rw_find_match (tok_beg, end, true);
        if (!tok_end)
            tok_end = end;

        // expand from tok_beg to tok_end into buf
        char buf [256];

        char* exp =
            graph.build_and_expand (tok_beg, tok_end, buf, sizeof (buf), sep);

        // apptok_end space, then expansion, expansion

        bool app = false;

        if (exp) {

            // in case user uses a null seperator
            const char* term = exp;
            for (/**/; *term; term += strlen (term) + 1)
                ;

            const size_t len = exp < term ? (term - exp) - 1 : 0;
        
            if (is_first_expand)
                app = result.append (exp, len);
            else if (result.append (&sep, 1))
                app = result.append (exp, len);
        }

        is_first_expand = false;

        if (exp != buf)
            free (exp);

        // if we didn't append, or we failed to append
        // then this function fails
        if (!app) {

            if (result.owned_)
                free (result.buffer_);

            return 0;
        }

        // bail if we're at the end of string
        if (!tok_end)
            break;

        // fid next nonwhitespace, which is the start of the next token
        tok_beg = _rw_find_match (tok_end, end, false);
    }

    return result.buffer_;
}
