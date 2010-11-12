// bdecs_regex.cpp             -*-C++-*-

#include <bdecs_regex.h>

#include <bdecs_finiteautomatonutil.h>
#include <bdeimp_duffsdevice.h>

#include <cassert>
#include <iostream>

// STATIC METHODS

namespace BloombergLP {

static void parseList(char       *set,
                      int        *size,
                      const char *data,
                      int         length,
                      int         isExclusion)
{
    const int isInclusion = 1 - isExclusion;

    bdeimp_DuffsDevice<char>::initializeRaw(set, isExclusion, 256);

    int i = 0;
    while (i < length) {
        char start = data[i];
        char end = start;
        ++i;
        if (i + 1 < length && data[i] == '-') {
            ++i;
            end = data[i];
            ++i;
        }
        for (int j = start; j <= end; ++j) {
            set[j] = isInclusion;
        }
    }
    int j = 0;
    for (i = 0; i < 256; ++i) {
        if (set[i]) {
            set[j++] = (char)i;
        }
    }
    *size = j;
}

static void parseRepeat(int *min, int *max, const char *data, int length)
{
    int i;

    *min = 0;
    for (i = 0; i < length && data[i] != ','; ++i) {
        if (data[i] != ' ') {
            *min = *min * 10 + data[i] - '0';
        }
    }

    if (i == length) {
        *max = *min;
    }
    else {
        *max = -1;
        for (++i; i < length; ++i) {
            if (data[i] != ' ') {
                if (*max == -1) {
                    *max = 0;
                }
                *max = *max * 10 + data[i] - '0';
            }
        }
    }
    if (*max > 255) {
        *max = 255;
    }
    if (*max != -1 && *min > *max) {
        *min = *max;
    }
}

struct bdecs_Regex_Token;

struct bdecs_Regex_Node {
    enum KIND { CHAR = 1, CHARSET, ANY, CARAT, DOLLAR, // leaf nodes
                REPEAT, STAR, PLUS, HUH, OR, CONCAT    // interior
    };

    enum { FLAG_LEFTMOST = 0x1, FLAG_RIGHTMOST = 0x2 };

    int d_kind;
    int d_flags;
    union {
        struct {    // CHAR: plain-jane and \-escaped characters
            char value;
        } n_char;

        struct {    // CHARSET: e.g., [_a-zA-Z0-9]; ANY: '.'
            char set[256];
            int  size;
        } n_charset;

        struct {    // REPEAT: {m}, {m,}, {m,n}; STAR: '*'; PLUS: '+'; HUH: '?'
            bdecs_Regex_Node *operand;
            int               min;
            int               max;
        } n_postfix;

        struct {    // OR: '|'; CONCAT: implied by juxtaposition
            bdecs_Regex_Node *left;
            bdecs_Regex_Node *right;
        } n_binary;
    } d_u;

    bdecs_Regex_Node(KIND k);
    bdecs_Regex_Node(const bdecs_Regex_Token& token);
};

struct bdecs_Regex_Token {
    enum TOKEN { NONE = 0, OR, OPEN_PAREN, CLOSE_PAREN,
                 CARAT, DOLLAR, AXIOM, POSTFIX, EOS, ERROR
    };

    TOKEN                  d_tokenKind;
    bdecs_Regex_Node::KIND d_treeKind;
    union {
        struct {
            char value;
        } n_char;

        struct {
            char set[256];
            int  size;
        } n_charset;

        struct {
            int min;
            int max;
        } n_postfix;
    } d_u;

    bdecs_Regex_Token() : d_tokenKind(NONE) {}
};

bdecs_Regex_Node::bdecs_Regex_Node(KIND k)
: d_kind(k)
, d_flags(0)
{
    assert(d_kind == OR || d_kind == CONCAT);

    d_u.n_binary.left = 0;
    d_u.n_binary.right = 0;
}

bdecs_Regex_Node::bdecs_Regex_Node(const bdecs_Regex_Token& token)
: d_kind(token.d_treeKind)
, d_flags(0)
{
    switch (token.d_treeKind) {
      case CHAR: {
        d_u.n_char.value = token.d_u.n_char.value;
      } break;
      case CHARSET:
      case ANY: {
        d_u.n_charset.size = token.d_u.n_charset.size;
        std::memcpy(d_u.n_charset.set, token.d_u.n_charset.set,
                    d_u.n_charset.size);
      } break;
      case CARAT:
      case DOLLAR: {
      } break;
      case REPEAT: {
        d_u.n_postfix.min = token.d_u.n_postfix.min;
        d_u.n_postfix.max = token.d_u.n_postfix.max;
        d_u.n_postfix.operand = 0;
      } break;
      case STAR:
      case PLUS:
      case HUH: {
        d_u.n_postfix.operand = 0;
      } break;
      default: {
        assert(1 == 0);
      } break;
    }
}

struct bdecs_Regex_Lexer {
    const char        *d_data;
    int                d_length;
    bdecs_Regex_Token  d_lookahead;
    int                d_parenLevel;

    bdecs_Regex_Lexer(const char *data, int length);

    bdecs_Regex_Token::TOKEN laType();
    const bdecs_Regex_Token& laValue();
    void advance();
};

bdecs_Regex_Lexer::bdecs_Regex_Lexer(const char *data, int length)
: d_data(data)
, d_length(length)
, d_parenLevel(0)
{
    assert(d_lookahead.d_tokenKind == bdecs_Regex_Token::NONE);

    advance();
}

inline
bdecs_Regex_Token::TOKEN bdecs_Regex_Lexer::laType()
{
    return d_lookahead.d_tokenKind;
}

inline
const bdecs_Regex_Token& bdecs_Regex_Lexer::laValue()
{
    return d_lookahead;
}

void bdecs_Regex_Lexer::advance()
{
    if (d_length == 0) {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::EOS;
        return;
    }

    switch (*d_data) {
      case '(': {
        ++d_parenLevel;
        d_lookahead.d_tokenKind = bdecs_Regex_Token::OPEN_PAREN;
        ++d_data;
        --d_length;
      } break;
      case ')': {
        if (--d_parenLevel < 0) {
            d_lookahead.d_tokenKind = bdecs_Regex_Token::ERROR;
        }
        else {
            d_lookahead.d_tokenKind = bdecs_Regex_Token::CLOSE_PAREN;
        }
        ++d_data;
        --d_length;
      } break;
      case '|': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::OR;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::OR;
        ++d_data;
        --d_length;
      } break;
      case '*': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::POSTFIX;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::STAR;
        ++d_data;
        --d_length;
      } break;
      case '+': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::POSTFIX;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::PLUS;
        ++d_data;
        --d_length;
      } break;
      case '?': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::POSTFIX;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::HUH;
        ++d_data;
        --d_length;
      } break;
      case '{': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::POSTFIX;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::REPEAT;
        ++d_data;
        --d_length;
        int len = 0;
        while (len < d_length && d_data[len] != '}') {
            ++len;
        }
        parseRepeat(&d_lookahead.d_u.n_postfix.min,
                    &d_lookahead.d_u.n_postfix.max, d_data, len);
        if (len < d_length) {  // in case '}' is missing
            ++len;
            if (len == 1) {  // '{}'
                d_lookahead.d_tokenKind = bdecs_Regex_Token::ERROR;
            }
        }
        else {
            d_lookahead.d_tokenKind = bdecs_Regex_Token::ERROR;
        }
        d_data += len;
        d_length -= len;
      } break;
      case '^': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::CARAT;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::CARAT;
        ++d_data;
        --d_length;
      } break;
      case '$': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::DOLLAR;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::DOLLAR;
        ++d_data;
        --d_length;
      } break;
      case '[': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::AXIOM;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::CHARSET;
        ++d_data;
        --d_length;
        int isExclusion = 0;
        if (d_length && *d_data == '^') {
            isExclusion = 1;
            ++d_data;
            --d_length;
        }
        int len = 0;
        if (d_length && *d_data == ']') {
            len = 1;
        }
        while (len < d_length && d_data[len] != ']') {
            ++len;
        }
        parseList(d_lookahead.d_u.n_charset.set,
                  &d_lookahead.d_u.n_charset.size,
                  d_data, len, isExclusion);
        if (len < d_length) {  // in case ']' is missing
            ++len;
        }
        else {
            d_lookahead.d_tokenKind = bdecs_Regex_Token::ERROR;
        }
        d_data += len;
        d_length -= len;
      } break;
      case '.': {
        d_lookahead.d_tokenKind = bdecs_Regex_Token::AXIOM;
        d_lookahead.d_treeKind  = bdecs_Regex_Node::ANY;
        char *set = d_lookahead.d_u.n_charset.set;
        for (int i = 0; i < 256; ++i) {
            set[i - (i > '\n')] = i;   // exclude '\n' from set
        }
        d_lookahead.d_u.n_charset.size = 255;
        ++d_data;
        --d_length;
      } break;
      case '}':
      case ']': {
        d_lookahead.d_tokenKind      = bdecs_Regex_Token::ERROR;
        d_lookahead.d_treeKind       = bdecs_Regex_Node::CHAR;
        d_lookahead.d_u.n_char.value = *d_data;
        ++d_data;
        --d_length;
      } break;
      case '\\':
        ++d_data;
        --d_length;
      default: {
        d_lookahead.d_tokenKind      = bdecs_Regex_Token::AXIOM;
        d_lookahead.d_treeKind       = bdecs_Regex_Node::CHAR;
        d_lookahead.d_u.n_char.value = *d_data;
        ++d_data;
        --d_length;
      } break;
    }
}

struct bdecs_Regex_Parser {
    bdecs_Regex_Lexer  d_lexer;
    bdema_Pool         d_pool;
    int                d_valid;

    bdecs_Regex_Parser(const char      *data,
                       int              length,
                       bdema_Allocator *basicAllocator);

    bdecs_Regex_Node *E();
    bdecs_Regex_Node *T();
    bdecs_Regex_Node *F();
    void invalidate() { d_valid = 0; }
    int isValid() const { return d_valid; }
};

bdecs_Regex_Parser::bdecs_Regex_Parser(const char      *data,
                                       int              length,
                                       bdema_Allocator *basicAllocator)
: d_lexer(data, length)
, d_pool(sizeof(bdecs_Regex_Node), basicAllocator)
, d_valid(1)
{}

bdecs_Regex_Node *bdecs_Regex_Parser::E()
{
    bdecs_Regex_Node *left  = 0;
    bdecs_Regex_Node *right = 0;

    switch (d_lexer.laType()) {
      case bdecs_Regex_Token::OPEN_PAREN:
      case bdecs_Regex_Token::CARAT:
      case bdecs_Regex_Token::DOLLAR:
      case bdecs_Regex_Token::AXIOM:
      case bdecs_Regex_Token::POSTFIX: {
        left = T();
      } break;
      case bdecs_Regex_Token::EOS: {
        return 0;
      }
      case bdecs_Regex_Token::NONE:
      case bdecs_Regex_Token::OR:
      case bdecs_Regex_Token::CLOSE_PAREN: {
      } break;
      case bdecs_Regex_Token::ERROR: {
        invalidate();
      } break;
    }

    if (d_lexer.laType() == bdecs_Regex_Token::OR) {
        d_lexer.advance();
        right = E();
    }

    if (right) {
        bdecs_Regex_Node *orNode =
                new (d_pool.allocate()) bdecs_Regex_Node(bdecs_Regex_Node::OR);
        orNode->d_u.n_binary.left  = left;
        orNode->d_u.n_binary.right = right;
        return orNode;
    }
    else {
        return left;
    }
}

bdecs_Regex_Node *bdecs_Regex_Parser::T()
{
    bdecs_Regex_Node *left  = 0;
    bdecs_Regex_Node *right = 0;

    switch (d_lexer.laType()) {
      case bdecs_Regex_Token::OPEN_PAREN:
      case bdecs_Regex_Token::CARAT:
      case bdecs_Regex_Token::DOLLAR:
      case bdecs_Regex_Token::AXIOM:
      case bdecs_Regex_Token::POSTFIX: {
        left = F();
      } break;
      case bdecs_Regex_Token::EOS: {
        return 0;
      }
      case bdecs_Regex_Token::ERROR: {
        invalidate();
      } break;
      case bdecs_Regex_Token::NONE:
      case bdecs_Regex_Token::OR:
      case bdecs_Regex_Token::CLOSE_PAREN: {
      } break;
    }

    switch (d_lexer.laType()) {
      case bdecs_Regex_Token::OPEN_PAREN:
      case bdecs_Regex_Token::CARAT:
      case bdecs_Regex_Token::DOLLAR:
      case bdecs_Regex_Token::AXIOM:
      case bdecs_Regex_Token::POSTFIX: {
        right = T();
      } break;
      case bdecs_Regex_Token::ERROR: {
        invalidate();
      } break;
      case bdecs_Regex_Token::NONE:
      case bdecs_Regex_Token::OR:
      case bdecs_Regex_Token::CLOSE_PAREN:
      case bdecs_Regex_Token::EOS: {
      } break;
    }

    if (right) {
        bdecs_Regex_Node *concat =
            new (d_pool.allocate()) bdecs_Regex_Node(bdecs_Regex_Node::CONCAT);
        concat->d_u.n_binary.left  = left;
        concat->d_u.n_binary.right = right;
        return concat;
    }
    else {
        return left;
    }
}

bdecs_Regex_Node *bdecs_Regex_Parser::F()
{
    bdecs_Regex_Node *carat  = 0;
    bdecs_Regex_Node *node   = 0;
    bdecs_Regex_Node *dollar = 0;

    if (d_lexer.laType() == bdecs_Regex_Token::CARAT) {
        carat = new (d_pool.allocate()) bdecs_Regex_Node(d_lexer.laValue());
        d_lexer.advance();
    }

    switch (d_lexer.laType()) {
      case bdecs_Regex_Token::AXIOM: {
        node = new (d_pool.allocate()) bdecs_Regex_Node(d_lexer.laValue());
        d_lexer.advance();
      } break;
      case bdecs_Regex_Token::OPEN_PAREN: {
        d_lexer.advance();
        node = E();
        if (d_lexer.laType() == bdecs_Regex_Token::CLOSE_PAREN) {
            d_lexer.advance();
        }
        else {
            invalidate();
        }
      } break;
      case bdecs_Regex_Token::CLOSE_PAREN:
      case bdecs_Regex_Token::ERROR: {
        invalidate();
      } break;
      case bdecs_Regex_Token::NONE:
      case bdecs_Regex_Token::OR:
      case bdecs_Regex_Token::CARAT:
      case bdecs_Regex_Token::DOLLAR:
      case bdecs_Regex_Token::POSTFIX:
      case bdecs_Regex_Token::EOS: {
      } break;
    }

    if (d_lexer.laType() == bdecs_Regex_Token::POSTFIX) {
        bdecs_Regex_Node *postfix =
                   new (d_pool.allocate()) bdecs_Regex_Node(d_lexer.laValue());
        d_lexer.advance();
        postfix->d_u.n_postfix.operand = node;
        node = postfix;
    }

    if (d_lexer.laType() == bdecs_Regex_Token::DOLLAR) {
        dollar = new (d_pool.allocate()) bdecs_Regex_Node(d_lexer.laValue());
        d_lexer.advance();
        if (node) {
            bdecs_Regex_Node *concat =
            new (d_pool.allocate()) bdecs_Regex_Node(bdecs_Regex_Node::CONCAT);
            concat->d_u.n_binary.left  = node;
            concat->d_u.n_binary.right = dollar;
            node = concat;
        }
        else {
            node = dollar;
        }
    }

    if (carat) {
        if (node) {
            bdecs_Regex_Node *concat =
            new (d_pool.allocate()) bdecs_Regex_Node(bdecs_Regex_Node::CONCAT);
            concat->d_u.n_binary.left  = carat;
            concat->d_u.n_binary.right = node;
            node = concat;
        }
        else {
            node = carat;
        }
    }

    return node;
}

#ifndef NDEBUG
static void printRE(bdecs_Regex_Node *node, int level)
{
    if (!node) {
        return;
    }

    switch (node->d_kind) {
      case bdecs_Regex_Node::CHAR: {
        std::cout << " " << node->d_u.n_char.value;
      } break;
      case bdecs_Regex_Node::CHARSET: {
        std::cout << " [";
        char *set = node->d_u.n_charset.set;
        int size = node->d_u.n_charset.size;
        for (int i = 0; i < size; ++i) {
            std::cout << set[i];
        }
        std::cout << "]";
      } break;
      case bdecs_Regex_Node::ANY: {
        std::cout << " .";
      } break;
      case bdecs_Regex_Node::STAR: {
        printRE(node->d_u.n_postfix.operand, ++level);
        std::cout << " *";
      } break;
      case bdecs_Regex_Node::PLUS: {
        printRE(node->d_u.n_postfix.operand, ++level);
        std::cout << " +";
      } break;
      case bdecs_Regex_Node::HUH: {
        printRE(node->d_u.n_postfix.operand, ++level);
        std::cout << " ?";
      } break;
      case bdecs_Regex_Node::REPEAT: {
        printRE(node->d_u.n_postfix.operand, ++level);
        std::cout << " {" << node->d_u.n_postfix.min << ", "
             << node->d_u.n_postfix.max << "}";
      } break;
      case bdecs_Regex_Node::OR: {
        if (level != 0) {
            std::cout << '\n';
        }
        std::cout << "|\nLEFT >> ";
        printRE(node->d_u.n_binary.left, ++level);
        std::cout << "\nRIGHT >> ";
        printRE(node->d_u.n_binary.right, ++level);
      } break;
      case bdecs_Regex_Node::CONCAT: {
        printRE(node->d_u.n_binary.left, ++level);
        printRE(node->d_u.n_binary.right, ++level);
      } break;
      case bdecs_Regex_Node::CARAT: {
        std::cout << " ^";
      } break;
      case bdecs_Regex_Node::DOLLAR: {
        std::cout << " $";
      } break;
    }

    if (level == 0) {
        std::cout << '\n';
    }
}
#endif

static void anchorsAweigh(bdecs_Regex_Node *node,
                          int               leftFlags,
                          int               rightFlags)
{
    if (!node) {
        return;
    }

    switch (node->d_kind) {
      case bdecs_Regex_Node::CHAR:
      case bdecs_Regex_Node::CHARSET:
      case bdecs_Regex_Node::ANY:
      case bdecs_Regex_Node::CARAT:
      case bdecs_Regex_Node::DOLLAR:
      case bdecs_Regex_Node::REPEAT:
      case bdecs_Regex_Node::STAR:
      case bdecs_Regex_Node::PLUS:
      case bdecs_Regex_Node::HUH: {
        node->d_flags |= leftFlags | rightFlags;
      } break;
      case bdecs_Regex_Node::OR: {
        anchorsAweigh(node->d_u.n_binary.left, leftFlags, rightFlags);
        anchorsAweigh(node->d_u.n_binary.right, leftFlags, rightFlags);
      } break;
      case bdecs_Regex_Node::CONCAT: {
        anchorsAweigh(node->d_u.n_binary.left, leftFlags, 0);
        anchorsAweigh(node->d_u.n_binary.right, 0, rightFlags);
      } break;
    }
}

static void gobble(bdecs_FiniteAutomaton& gobbler)
{
        char set[256];
        for (int i = 0; i < 256; ++i) {
            set[i] = i;
        }
        gobbler.appendSet(set, 256);
        gobbler.assignKleeneStar();
}

static void buildFA(bdecs_Regex_Node        *node,
                    bdecs_FiniteAutomaton&   fa,
                    bdema_Allocator         *basicAllocator,
                    int                      level)
{
    if (!node) {
        if (level == 0) {
            gobble(fa);  // "", "()", "((()()))", etc. pathological cases
        }
        return;
    }

    int preGobble = 0;

    if ((node->d_flags & bdecs_Regex_Node::FLAG_LEFTMOST)
        && (node->d_kind != bdecs_Regex_Node::CARAT)) {
        preGobble = 1;
    }

    switch (node->d_kind) {
      case bdecs_Regex_Node::CHAR: {
        fa.append(node->d_u.n_char.value);
      } break;
      case bdecs_Regex_Node::CHARSET:
      case bdecs_Regex_Node::ANY: {
        fa.appendSet(node->d_u.n_charset.set, node->d_u.n_charset.size);
      } break;
      case bdecs_Regex_Node::CARAT:
      case bdecs_Regex_Node::DOLLAR: {
        ;
      } break;
      case bdecs_Regex_Node::STAR: {
        bdecs_FiniteAutomaton fa2;
        buildFA(node->d_u.n_postfix.operand, fa2, basicAllocator, ++level);
        fa2.assignKleeneStar();
        fa += fa2;
      } break;
      case bdecs_Regex_Node::PLUS: {
        bdecs_FiniteAutomaton fa2;
        buildFA(node->d_u.n_postfix.operand, fa2, basicAllocator, ++level);
        bdecs_FiniteAutomatonUtil::repeatMin(&fa2, 1);
        fa += fa2;
      } break;
      case bdecs_Regex_Node::HUH: {
        bdecs_FiniteAutomaton fa2;
        buildFA(node->d_u.n_postfix.operand, fa2, basicAllocator, ++level);
        bdecs_FiniteAutomatonUtil::repeat(&fa2, 0, 1);
        fa += fa2;
      } break;
      case bdecs_Regex_Node::REPEAT: {
        bdecs_FiniteAutomaton fa2;
        buildFA(node->d_u.n_postfix.operand, fa2, basicAllocator, ++level);
        int min = node->d_u.n_postfix.min;
        int max = node->d_u.n_postfix.max;
        if (-1 != max) {
            if (min == max) {
                bdecs_FiniteAutomatonUtil::repeat(&fa2, min);
            }
            else {
                bdecs_FiniteAutomatonUtil::repeat(&fa2, min, max);
            }
        }
        else {
            bdecs_FiniteAutomatonUtil::repeatMin(&fa2, min);
        }
        fa += fa2;
      } break;
      case bdecs_Regex_Node::OR: {
        bdecs_FiniteAutomaton left;
        buildFA(node->d_u.n_binary.left, left, basicAllocator, ++level);
        bdecs_FiniteAutomaton right;
        buildFA(node->d_u.n_binary.right, right, basicAllocator, ++level);
        left |= right;
        fa += left;
      } break;
      case bdecs_Regex_Node::CONCAT: {
        bdecs_FiniteAutomaton left;
        buildFA(node->d_u.n_binary.left, left, basicAllocator, ++level);
        bdecs_FiniteAutomaton right;
        buildFA(node->d_u.n_binary.right, right, basicAllocator, ++level);
        left += right;
        fa += left;
      } break;
    }

    if (preGobble) {
        bdecs_FiniteAutomaton gobbler(basicAllocator);
        gobble(gobbler);
        gobbler += fa;
        fa = gobbler;
    }

    if ((node->d_flags & bdecs_Regex_Node::FLAG_RIGHTMOST)
        && (node->d_kind != bdecs_Regex_Node::DOLLAR)) {
        bdecs_FiniteAutomaton gobbler(basicAllocator);
        gobble(gobbler);
        fa += gobbler;
    }
}

static void re2dfa(const char             *expression,
                   int                     length,
                   bdecs_FiniteAutomaton&  fa,
                   bdema_Allocator        *basicAllocator)
{
    bdecs_Regex_Parser parser(expression, length, basicAllocator);
    bdecs_Regex_Node *re = parser.E();

    if (parser.isValid()) {
        anchorsAweigh(re,
                      bdecs_Regex_Node::FLAG_LEFTMOST,
                      bdecs_Regex_Node::FLAG_RIGHTMOST);
        buildFA(re, fa, basicAllocator, 0);
    }
    else {
        fa.invalidate();
    }
}

// CREATORS

bdecs_Regex::bdecs_Regex(const char      *strExpression,
                         bdema_Allocator *basicAllocator)
: d_finiteAutomaton(basicAllocator)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    re2dfa(strExpression,
           std::strlen(strExpression),
           d_finiteAutomaton,
           d_allocator_p);
}

bdecs_Regex::bdecs_Regex(const char      *expression,
                         int              length,
                         bdema_Allocator *basicAllocator)
: d_finiteAutomaton(basicAllocator)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    re2dfa(expression, length, d_finiteAutomaton, d_allocator_p);
}

// ACCESSORS

int bdecs_Regex::isMatch(const char *data, int length) const
{
    bdecs_FiniteAutomatonAccumulator acc(d_finiteAutomaton);
    for (int i = 0; i < length; ++i) { // TBD duff's device like thing
        acc.transition(data[i]);
    }
    return acc.isAccepting();
}

int bdecs_Regex::isMatch(const char *string) const
{
    bdecs_FiniteAutomatonAccumulator acc(d_finiteAutomaton);
    while (*string) {
        acc.transition(*string);
        ++string;
    }
    return acc.isAccepting();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
