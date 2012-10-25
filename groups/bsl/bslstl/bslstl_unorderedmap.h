// bslstl_unorderedmap.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMAP
#define INCLUDED_BSLSTL_UNORDEREDMAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE:Provide an STL-compliant unordered_map class.
//
//@CLASSES:
//   bsl::unordered_map : STL-compliant unordered-map class
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component defines a single class template 'unordered_map',
// implementing the standard container holding a collection of unique keys,
// each mapped to an associated value (with no guarantees on ordering).
//
// An instantiation of 'unordered_map' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of keys) and the set of
// key-value pairs the 'unordered_map' contains, without regard to their order.
// If 'unordered_map' is instantiated with a key type or mapped value-type
// that is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.  In particular, if the key or value type cannot be
// tested for equality, then an 'unordered_map' containing that type cannot be
// tested for equality.  It is even possible to instantiate 'unordered_map'
// with type that do not have an accessible copy-constructor, in which case the
// 'unordered_map' will not be copyable.  Note that the equality comparison
// operator for each key-value pair is used to determine when two
// 'unordered_map' objects have the same value, and not the instance of the
// 'EQUAL' template parameter supplied at construction.
//
// An 'unordered_map' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_map' implemented here adheres to the C++11 standard, except that
// it does not have interfaces that take rvalue references, 'initializer_list',
// 'emplace', or operations taking a variadic number of template parameters.
// Note that excluded C++11 features are those that require (or are greatly
// simplified by) C++11 compiler support.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// An 'unordered_map' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' and 'VALUE' template
// parameters are fully value-semantic.  It is possible to instantiate an
// 'unoredered_map' with 'KEY' and 'VALUE' parameter arguments that do not
// provide a full set of value-semantic operations, but then some methods of
// the container may not be instantiable.  The following terminology, adopted
// from the C++11 standard, is used in the function documentation of
// 'unordered_map' to describe a function's requirements for the 'KEY' and
// 'VALUE' template parameters.  These terms are also defined in section
// [utility.arg.requirements] of the C++11 standard.  Note that, in the context
// of an 'unordered_map' instantiation, the requirements apply specifically to
// the 'unordered_map's element type, 'value_type', which is an alias for
// std::pair<const KEY, VALUE_TYPE>'.
//
//: "default-constructible": The type provides an accessible default
//:                          constructor.
//:
//: "copy-constructible": The type provides an accessible copy constructor.
//:
//: "equality-comparable": The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
//
///Memory Allocation
///-----------------
// The type supplied as the 'ALLOCATOR' template parameter determines how
// memory will be allocated.  The 'unordered_map' template supports allocators
// meeting the requirements of the C++11 standard [allocator.requirements], and
// in addition it supports scoped-allocators derived from the
// 'bslma::Allocator' memory allocation protocol.  Clients intending to use
// 'bslma' style allocators should use the template's default 'ALLOCATOR' type:
// The default type for the 'ALLOCATOR' template parameter, 'bsl::allocator',
// provides a C++11 standard-compatible adapter for a 'bslma::Allocator'
// object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of an 'unordered_map'
// instantiation is 'bsl::allocator', then objects of that unordered map type
// will conform to the standard behavior of a 'bslma'-allocator-enabled type.
// Such an unordered map accepts an optional 'bslma::Allocator' argument at
// construction.  If the address of a 'bslma::Allocator' object is explicitly
// supplied at construction, it will be used to supply memory for the
// 'unordered_map' throughout its lifetime; otherwise, the 'unordered_map' will
// use the default allocator installed at the time of the 'unordered_map's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', an 'unordered_map' supplies
// that allocator's address to the constructors of contained objects of the
// parameterized 'KEY' types with the 'bslalg::TypeTraitUsesBslmaAllocator'
// trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_map':
//..
//  Legend
//  ------
//  'K'             - template parameter type 'KEY' of the unordered map
//  'V'             - template parameter type 'VALUE' of the unordered map
//  'a', 'b'        - two distinct objects of type 'unordered_map<K, V>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - unordered_map<K, V>::value_type
//  'c'             - equality functor comparing objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'v'             - an object of type 'value_type'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//  distance(p1,p2) - the number of elements in the range [p1, p2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_map<K, V> a;    (default construction)   | O[1]               |
//  | unordered_map<K, V> a(al);                         |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_map<K, V> a(b); (copy construction)      | Average: O[n]      |
//  | unordered_map<K, V> a(b, al);                      | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_map<K, V> a(w);                          | O[n]               |
//  | unordered_map<K, V> a(w, hf);                      |                    |
//  | unordered_map<K, V> a(w, hf, eq);                  |                    |
//  | unordered_map<K, V> a(w, hf, eq, al);              |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_map<K, V> a(i1, i2);                     | Average: O[N]      |
//  | unordered_map<K, V> a(i1, i2, w)                   | Worst:   O[N^2]    |
//  | unordered_map<K, V> a(i1, i2, w, hf);              | where N =          |
//  | unordered_map<K, V> a(i1, i2, w, hf, eq);          |  distance(i1, i2)] |
//  | unordered_map<K, V> a(i1, i2, w, hf, eq, al);      |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_map<K, V>(); (destruction)            | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b                               | O[1] if 'a' and    |
//  |                                                    | 'b' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.key_eq()                                         | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.hash_function()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.allocator()                                      | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a[k]                                               | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.at(k)                                            | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(v)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v)                                    | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | Average: O[        |
//  |                                                    |   distance(i1, i2)]|
//  |                                                    | Worst:   O[n *     |
//  |                                                    |   distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | Average:           |
//  |                                                    |       O[a.count(k)]|
//  |                                                    | Worst:             |
//  |                                                    |       O[n]         |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | Average: O[        |
//  |                                                    |   distance(p1, p2)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.find(k)                                          | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.count(k)                                         | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.equal_range(k)                                   | Average: O[        |
//  |                                                    |         a.count(k)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_count()                                   | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_bucket_count()                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket(k)                                        | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_size(k)                                   | O[a.bucket_size(k)]|
//  +----------------------------------------------------+--------------------+
//  | a.load_factor()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_load_factor()                                | O[1]               |
//  | a.max_load_factor(z)                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.rehash(k)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.resize(k)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Gathering Document Statistics
/// - - - - - - - - - - - - - - - - - - - -
// Unordered maps are useful in situations when there is no meaningful way to
// order the key values, when the order of the keys is irrelevant to the
// problem domain (see {Example 3}), and (even if there is a meaningful
// ordering) the value of ordering the results is outweighed by the higher
// performance provided by unordered maps (compared to ordered maps).
//
// Suppose one wished to gather statistics on the words appearing in a large
// set of documents on disk or in a data base.  Gathering those statics is
// intrusive (as one is competing for access to the documents with the regular
// users) and must be done as quickly as possible.  Moreover, the set of unique
// words appearing in those documents may be high.  The English language has in
// excess of a million words (albeit many appear infrequently), and, if the
// documents contain serial numbers, or Social Security numbers, or chemical
// formulas, etc. then the O[log(n)] insertion time of ordered maps may well be
// inadequate.  The unordered map, having an O[1] average insersion cost, is a
// viable alternative.  In many problem domains, sorting, if needed, can be
// done after the data is gathered.
//
// This example illustrates the use of 'bsl::unordered_map' to gather one
// simple statistic (counts of unique words) on a single document.  To avoid
// irrelevant details of acquiring the data, several modestly sized documents
// are stored in static arrays:
//..
//  static char document0[] =
//  " IN CONGRESS, July 4, 1776.\n"
//  "\n"
//  " The unanimous Declaration of the thirteen united States of America,\n"
//  "\n"
//  " When in the Course of human events, it becomes necessary for one\n"
//  " people to dissolve the political bands which have connected them with\n"
//  " another, and to assume among the powers of the earth, the separate\n"
//  " and equal station to which the Laws of Nature and of Nature's God\n"
//  " entitle them, a decent respect to the opinions of mankind requires\n"
//  " that they should declare the causes which impel them to the\n"
//  " separation.  We hold these truths to be self-evident, that all men\n"
//  " are created equal, that they are endowed by their Creator with\n"
//  " certain unalienable Rights, that among these are Life, Liberty and\n"
//  " the pursuit of Happiness.--That to secure these rights, Governments\n"
//  " are instituted among Men, deriving their just powers from the consent\n"
//  " of the governed, --That whenever any Form of Government becomes\n"
//  " destructive of these ends, it is the Right of the People to alter or\n"
//  " to abolish it, and to institute new Government, laying its foundation\n"
//  " on such principles and organizing its powers in such form, as to them\n"
//  " shall seem most likely to effect their Safety and Happiness.\n"
//  " Prudence, indeed, will dictate that Governments long established\n"
//  " should not be changed for light and transient causes; and accordingly\n"
//  " all experience hath shewn, that mankind are more disposed to suffer,\n"
//  " while evils are sufferable, than to right themselves by abolishing\n"
//  " the forms to which they are accustomed.  But when a long train of\n"
//  " abuses and usurpations, pursuing invariably the same Object evinces a\n"
//  " design to reduce them under absolute Despotism, it is their right, it\n"
//  " is their duty, to throw off such Government, and to provide new\n"
//  " Guards for their future security.--Such has been the patient\n"
//  " sufferance of these Colonies; and such is now the necessity which\n"
//  " constrains them to alter their former Systems of Government.  The\n"
//  " history of the present King of Great Britain is a history of repeated\n"
//  " injuries and usurpations, all having in direct object the\n"
//  " establishment of an absolute Tyranny over these States.  To prove\n"
//  " this, let Facts be submitted to a candid world.\n"
//  "\n"
//  ": o He has refused his Assent to Laws, the most wholesome and\n"
//  ":   necessary for the public good.\n"
//  ":\n"
//  // ...
//  " We, therefore, the Representatives of the united States of America,\n"
//  " in General Congress, Assembled, appealing to the Supreme Judge of the\n"
//  " world for the rectitude of our intentions, do, in the Name, and by\n"
//  " Authority of the good People of these Colonies, solemnly publish and\n"
//  " declare, That these United Colonies are, and of Right ought to be\n"
//  " Free and Independent States; that they are Absolved from all\n"
//  " Allegiance to the British Crown, and that all political connection\n"
//  " between them and the State of Great Britain, is and ought to be\n"
//  " totally dissolved; and that as Free and Independent States, they have\n"
//  " full Power to levy War, conclude Peace, contract Alliances, establish\n"
//  " Commerce, and to do all other Acts and Things which Independent\n"
//  " States may of right do.  And for the support of this Declaration,\n"
//  " with a firm reliance on the protection of divine Providence, we\n"
//  " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
//  " Honor.\n";
//
//  static char document1[] =
//  "/The Universal Declaration of Human Rights\n"
//  "/-----------------------------------------\n"
//  "/Preamble\n"
//  "/ - - - -\n"
//  " Whereas recognition of the inherent dignity and of the equal and\n"
//  " inalienable rights of all members of the human family is the\n"
//  " foundation of freedom, justice and peace in the world,\n"
//  "\n"
//  " Whereas disregard and contempt for human rights have resulted in\n"
//  " barbarous acts which have outraged the conscience of mankind, and the\n"
//  " advent of a world in which human beings shall enjoy freedom of speech\n"
//  " and belief and freedom from fear and want has been proclaimed as the\n"
//  " highest aspiration of the common people,\n"
//  "\n"
//  " Whereas it is essential, if man is not to be compelled to have\n"
//  " recourse, as a last resort, to rebellion against tyranny and\n"
//  " oppression, that human rights should be protected by the rule of law,\n"
//  "\n"
//  " Whereas it is essential to promote the development of friendly\n"
//  " relations between nations,\n"
//  "\n"
//  " Whereas the peoples of the United Nations have in the Charter\n"
//  " reaffirmed their faith in fundamental human rights, in the dignity\n"
//  " and worth of the human person and in the equal rights of men and\n"
//  " women and have determined to promote social progress and better\n"
//  " standards of life in larger freedom,\n"
//  "\n"
//  " Whereas Member States have pledged themselves to achieve, in\n"
//  " co-operation with the United Nations, the promotion of universal\n"
//  " respect for and observance of human rights and fundamental freedoms,\n"
//  "\n"
//  " Whereas a common understanding of these rights and freedoms is of the\n"
//  " greatest importance for the full realization of this pledge, Now,\n"
//  " Therefore THE GENERAL ASSEMBLY proclaims THIS UNIVERSAL DECLARATION\n"
//  " OF HUMAN RIGHTS as a common standard of achievement for all peoples\n"
//  " and all nations, to the end that every individual and every organ of\n"
//  " society, keeping this Declaration constantly in mind, shall strive by\n"
//  " teaching and education to promote respect for these rights and\n"
//  " freedoms and by progressive measures, national and international, to\n"
//  " secure their universal and effective recognition and observance, both\n"
//  " among the peoples of Member States themselves and among the peoples\n"
//  " of territories under their jurisdiction.\n"
//  "\n"
//  "/Article 1\n"
//  "/- - - - -\n"
//  " All human beings are born free and equal in dignity and rights.  They\n"
//  " are endowed with reason and conscience and should act towards one\n"
//  " another in a spirit of brotherhood.\n"
//  "\n"
//  // ...
//  "\n"
//  "/Article 30\n"
//  "/ - - - - -\n"
//  " Nothing in this Declaration may be interpreted as implying for any\n"
//  " State, group or person any right to engage in any activity or to\n"
//  " perform any act aimed at the destruction of any of the rights and\n"
//  " freedoms set forth herein.\n";
//
//  static char document2[] =
//  "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
//  "/---------------------------------------------------\n"
//  " PREAMBLE\n"
//  "\n"
//  " The peoples of Europe, in creating an ever closer union among them,\n"
//  " are resolved to share a peaceful future based on common values.\n"
//  "\n"
//  " Conscious of its spiritual and moral heritage, the Union is founded\n"
//  " on the indivisible, universal values of human dignity, freedom,\n"
//  " equality and solidarity; it is based on the principles of democracy\n"
//  " and the rule of law.  It places the individual at the heart of its\n"
//  " activities, by establishing the citizenship of the Union and by\n"
//  " creating an area of freedom, security and justice.\n"
//  "\n"
//  " The Union contributes to the preservation and to the development of\n"
//  " these common values while respecting the diversity of the cultures\n"
//  " and traditions of the peoples of Europe as well as the national\n"
//  " identities of the Member States and the organisation of their public\n"
//  " authorities at national, regional and local levels; it seeks to\n"
//  " promote balanced and sustainable development and ensures free\n"
//  " movement of persons, goods, services and capital, and the freedom of\n"
//  " establishment.\n"
//  "\n"
//  " To this end, it is necessary to strengthen the protection of\n"
//  " fundamental rights in the light of changes in society, social\n"
//  " progress and scientific and technological developments by making\n"
//  " those rights more visible in a Charter.\n"
//  "\n"
//  " This Charter reaffirms, with due regard for the powers and tasks of\n"
//  " the Community and the Union and the principle of subsidiarity, the\n"
//  " rights as they result, in particular, from the constitutional\n"
//  " traditions and international obligations common to the Member States,\n"
//  " the Treaty on European Union, the Community Treaties, the European\n"
//  " Convention for the Protection of Human Rights and Fundamental\n"
//  " Freedoms, the Social Charters adopted by the Community and by the\n"
//  " Council of Europe and the case-law of the Court of Justice of the\n"
//  " European Communities and of the European Court of Human Rights.\n"
//  "\n"
//  " Enjoyment of these rights entails responsibilities and duties with\n"
//  " regard to other persons, to the human community and to future\n"
//  " generations.\n"
//  "\n"
//  " The Union therefore recognises the rights, freedoms and principles\n"
//  " set out hereafter.\n"
//  "\n"
//  "/CHAPTER I\n"
//  "/- - - - -\n"
//  " DIGNITY\n"
//  "\n"
//  "/Article 1\n"
//  "/  -  -  -\n"
//  " Human dignity\n"
//  "\n"
//  " Human dignity is inviolable.  It must be respected and protected.\n"
//  "\n"
//  "/Article 2\n"
//  "/  -  -  -\n"
//  " Right to life\n"
//  "\n"
//  ": 1 Everyone has the right to life.\n"
//  ": 2 No one shall be condemned to the death penalty, or executed.\n"
//  "\n"
//  // ...
//  "/Article 54\n"
//  "/-  -  -  -\n"
//  " Prohibition of abuse of rights\n"
//  "\n"
//  " Nothing in this Charter shall be interpreted as implying any right to\n"
//  " engage in any activity or to perform any act aimed at the destruction\n"
//  " of any of the rights and freedoms recognised in this Charter or at\n"
//  " their limitation to a greater extent than is provided for herein.\n";
//
//  static char * const documents[] = { &document0[0],
//                                      &document1[0],
//                                      &document2[0]
//                                    };
//  const int           numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define several aliases to make our code more comprehensible.
//..
//  typedef bsl::unordered_map<bsl::string, int> WordTally;
//  typedef bsl::pair         <bsl::string, int> WordTallyEntry;
//  typedef bsl::pair<WordTally::iterator, bool> WordTallyInsertStatus;
//..
// Next, we (default) create an unordered map to hold our word tallies.  The
// output from the 'printf' statements will be discussed in {Example 2}.
//..
//  WordTally wordTally;
//
//  printf("size             %4d initial\n", wordTally.size());
//  printf("bucket_count     %4d initial\n", wordTally.bucket_count());
//  printf("load_factor      %f  initial\n", wordTally.load_factor());
//  printf("max_load_factor  %f  initial\n", wordTally.max_load_factor());
//..
// Then, we define the set of characters that define word boundaries:
//..
//  const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// We tentatively assume that we are seeing each word for the first time and
// attempt to insert an initial record for that word.  If that succeeds (a
// 'true' value in the 'second' member of the 'bsl::pair' returned by the
// 'insert' method), we have correctly recorded that word; otherwise, we are
// returned an iterator (the 'first' member of the 'bsl::pair' returned) to the
// entry that had previously been added to the map.  In that case, we increment
// the data portion (the 'second' member) of that entry.
//..
//  for (int idx = 0; idx < numDocuments; ++idx) {
//      for (char *cur = strtok(documents[idx], delimiters);
//                 cur;
//                 cur = strtok(NULL,     delimiters)) {
//          WordTallyEntry        initialEntry(bsl::string(cur), 1);
//          WordTallyInsertStatus status = wordTally.insert(initialEntry);
//          if (!status.second) {
//              ++status.first->second;
//          }
//      }
//  }
//..
// Then, now that the data has been (quickly) gathered, we can indulge in
// analysis that is more time consuming.  For example, we can define a
// comparison function, sort the entries, and determine the 20 most commonly
// used words in the given document:
//..
//  struct WordTallyEntryCompare {
//      static bool lessValue(const WordTallyEntry& a,
//                            const WordTallyEntry& b) {
//          return a.second < b.second;
//      }
//      static bool moreValue(const WordTallyEntry& a,
//                            const WordTallyEntry& b) {
//          return !lessValue(a, b);
//      }
//  };
//
//  bsl::vector<WordTallyEntry> array(wordTally.begin(), wordTally.end());
//
//  std::partial_sort(array.begin(),
//                    array.begin() + 20,
//                    array.end(),
//                    WordTallyEntryCompare::moreValue);
//..
// Notice that 'partial_sort' suffices since we seek only the 20 most used
// words, and not a complete distribution of word counts.
//
// Finally, we print the sorted portion of 'array':
//..
//  for (bsl::vector<WordTallyEntry>::const_iterator cur  = array.begin(),
//                                                   end  = cur + 20;
//                                                   end != cur; ++cur) {
//      printf("%-10s %4d\n", cur->first.c_str(), cur->second);
//  }
//..
// and standard output shows:
//..
//  the         463
//  -           398
//  of          361
//  and         349
//  to          306
//  in          141
//  or          106
//  right        93
//  be           90
//  Article      86
//  has          79
//  a            76
//  shall        69
//  for          69
//  by           62
//  with         50
//  Everyone     49
//  rights       44
//  their        44
//  is           43
//..
// Notice that "-" (used as an header underscore in our markup) appears in the
// word count.  That could be elimiated by adding '-' to the set of delimiters;
// however, that would partition hyphenated words into separate words.  In
// practice, one defines a "stop list" of common words (e.g., "the", "of",
// "and", "is") that one does not wish to tally.  We could easily add "-" to
// the stop list.
//
///Example 2: Examining and Setting Unordered Map Configuration
///------------------------------------------------------------
// The unordered map interfaces provide some insight into and control of its
// inner workings.  The unordered map is implemented using a hash table (see
// {'bslstl_hashtable'}), a dynamically sized array of "buckets".  If two
// elements hash to the same position the the table (the same bucket), then
// that bucket will house multiple elements.  As elements are added to the
// unordered map, the number of buckets is increased (and the existing elements
// redistributed) to keep the average number of elements per bucket (the
// "loading factor") below the specified maximum (the "maximum load factor", 1
// by default).
//
// First, when 'wordTally' was created in {Example 1}, its metrics were:
//..
//  size                0 initial
//  bucket_count        1 initial
//  load_factor      0.000000  initial
//  max_load_factor  1.000000  initial
//..
// Notice that even when there are no elements ('size' is 0) there is one
// bucket.  Since there are no elements, the average number of elements per
// bucket must be 0 (the 'load_factor').
//
// Next, after 'wordTally' has been loaded, we examine its metrics:
//..
//  printf("size             %4d\n", wordTally.size());
//  printf("bucket_count     %4d\n", wordTally.bucket_count());
//  printf("load_factor      %f\n",  wordTally.load_factor());
//  printf("max_load_factor  %f\n",  wordTally.max_load_factor());
//..
// and find at standard output:
//..
//  size             1504
//  bucket_count     2099
//  load_factor      0.716532
//  max_load_factor  1.000000
//..
// Notice how the number of buckets has increased.  (Sampling this metric as
// the map was loaded would show that the increase was done in several stages.)
//
// Then, we see that the load factor is indeed below the specified maximum;
// however we obtain further details of how the buckets are used.
//
// The unordered map provides and interface giving the element count of of
// elements in each bucket, we can easily determine the bucket with the
// greatest number of elements (i.e., (greatest number of has collisions):
//..
//  bsl::vector<int> bucketSizes;
//  for (int idx = 0; idx < wordTally.bucket_count(); ++idx) {
//     bucketSizes.push_back(wordTally.bucket_size(idx));
//  }
//  int maxBucketSize = *std::max_element(bucketSizes.begin(),
//                                        bucketSizes.end());
//  printf("maxBucketSize    %4d\n", maxBucketSize);
//..
// and find on standard output:
//..
//  maxBucketSize       5
//..
// We can also count the number of empty buckets, and the number of buckets at
// 'maxBucketSize'.
//..
//  int numEmptyBuckets = std::count_if(bucketSizes.begin(),
//                                      bucketSizes.end(),
//                                      std::bind1st(std::equal_to<int>(), 0));
//
//  printf("numEmptyBuckets  %4d\n", numEmptyBuckets);
//
//  int numMaxBuckets = std::count_if(
//                         bucketSizes.begin(),
//                         bucketSizes.end(),
//                         std::bind1st(std::equal_to<int>(), maxBucketSize));
//
//  printf("numMaxBuckets    %4d\n", numMaxBuckets);
//..
// which shows on standard output:
//..
//  numEmptyBuckets  1031
//  numMaxBuckets       3
//..
// Suppose we are not satisfied with this distribution.  (Perhaps the load
// factor is too high.)  We can create a second, differently configured table.
//
// Next, create a new table 'wordTally2' with twice the bucket count shown by
// the first table ('wordTally'), and examine its initial metrics.
//..
//  WordTally wordTally2(wordTally.bucket_count() * 2);
//
//  printf("size2            %4d initial\n", wordTally2.size());
//  printf("bucket_count2    %4d initial\n", wordTally2.bucket_count());
//  printf("load_factor2     %f  initial\n", wordTally2.load_factor());
//  printf("max_load_factor2 %f  initial\n", wordTally2.max_load_factor());
//..
// Standard output shows:
//..
//  size2               0 initial
//  bucket_count2    4201 initial
//  load_factor2     0.000000  initial
//  max_load_factor2 1.000000  initial
//..
// Notice that although we requested 4198 buckets (2 * 2099), we created a
// table with 4201 buckets.  (4201 is the smallest prime number greater than
// 4198).
//
// Then, we load our new table and examine its metrics.  For simplicity, we
// load data from the first table rather than re-tokenize our documents.
//..
//  wordTally2.insert(wordTally.begin(), wordTally.end());
//
//  printf("size2            %4d\n", wordTally2.size());
//  printf("bucket_count2    %4d\n", wordTally2.bucket_count());
//  printf("load_factor2     %f\n",  wordTally2.load_factor());
//  printf("max_load_factor2 %f\n",  wordTally2.max_load_factor());
//
//  bsl::vector<int> bucketSizes2;
//  for (int idx = 0; idx < wordTally2.bucket_count(); ++idx) {
//     bucketSizes2.push_back(wordTally2.bucket_size(idx));
//  }
//  int maxBucketSize2 = *std::max_element(bucketSizes2.begin(),
//                                         bucketSizes2.end());
//
//  printf("maxBucketSize2   %4d\n", maxBucketSize2);
//
//  int numEmptyBuckets2 = std::count_if(
//                                      bucketSizes2.begin(),
//                                      bucketSizes2.end(),
//                                      std::bind1st(std::equal_to<int>(), 0));
//
//  printf("numEmptyBuckets2 %4d\n", numEmptyBuckets2);
//
//  int numMaxBuckets2 = std::count_if(
//                         bucketSizes2.begin(),
//                         bucketSizes2.end(),
//                         std::bind1st(std::equal_to<int>(), maxBucketSize2));
//
//  printf("numMaxBuckets2   %4d\n", numMaxBuckets2);
//..
// Finally, we see on standard output:
//..
//  size2            1504
//  bucket_count2    4201
//  load_factor2     0.358010
//  max_load_factor2 1.000000
//  maxBucketSize2      4
//  numEmptyBuckets2 2971
//  numMaxBuckets2      5
//..
// Notice that the loading factor has been (roughly) cut in half; we have
// achieved our goal.  Also notice that the bucket count is unchanged since
// construction; thus, there were no rehashs during the loading this unordered
// map.  Finally, notice that the number of empty (unused) buckets is
// significantly higher, and there's been a modest decrease in the largest
// bucket size, but more instances of them.
//
// Thus, the unordered map provides facilities by which we can make trade-offs
// in performance characteristics of the containers we create.
//
///Example 3: Inverse Concordance
///------------------------------
// If one has a concordance for a set of documents (an index of the position of
// every unique word in those documents), then words of interest can be
// efficiently located.  Suppose after locating a word of interest one also
// needs the surrounding words (for context).  Searching in the original
// document requires re-tokenization (time consuming).  Alternatively, one can
// use the concordance to create an inverse concordance to provide a fast
// lookup of the words at given locations in a document and then examine words
// near the word of interest.
//
// First, define the types required (and convenient aliases) to create an
// unordered map from a word location to the corresponding word.  The "key"
// value will be 'WordLocation', a pair 'int' values: the first being the
// document code number (arbitrarily assigned), and second the word offset in
// that document (the first word of the document is at offset 0).  The "value"
// of each entry is a 'bsl::string' containing the word at that location.
//..
//  typedef bsl::pair<int, int> WordLocation;
//      // Document code number and word offset in that document specify
//      // a word location.
//
//  typedef bsl::pair<WordLocation, bsl::string> InverseConcordanceEntry;
//      // Entry that maps a 'WordLocation' value to a 'bsl::string' value.
//..
// Notice that that 'WordLocation', the type of the key value, has no natural
// ordering.  The assignment of document codes is arbitrary so there is no
// reason to consider the words on one document to sort below those in any
// another.
//
// Then, since there is no default hash function for the 'WordLocation' type,
// we define one.  The document code and the word offset are combined to form a
// single 'int' value which is then hashed using the default mechanism.
//..
//  class WordLocationHash
//  {
//    public:
//      // CREATORS
//      //! WordLocationHash() = default;
//          // Create a 'WordLocationHash' object.
//
//      //! WordLocationHash(const WordLocationHash& original) = default;
//          // Create a 'WordLocationHash' object.  Note that as
//          // 'WordLocationHash' is an empty (stateless) type, this operation
//          // will have no observable effect.
//
//      //! ~WordLocationHash() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      //! WordLocationHash& operator=(const WordLocationHash& rhs) = default;
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.  Note that as 'WordLocationHash' is an empty (stateless)
//          // type, this operation will have no observable effect.
//
//      // ACCESSORS
//      std::size_t operator()(WordLocation x) const
//          // Return a hash value computed using the specified 'x'.
//      {
//          return bsl::hash<int>()(x.first * 1000000 + x.second);
//      }
//  };
//..
// Notice that many of the required methods of the hash type are compiler
// generated.  (The declaration of those methods are commented out and suffixed
// by an '= default' comment.)
//
// Next, we define the type of the unordered map and associated convenience
// aliases:
//..
//  typedef bsl::unordered_map<WordLocation, bsl::string, WordLocationHash>
//                                               InverseConcordance;
//
//  typedef InverseConcordance::iterator         InverseConcordanceItr;
//
//  typedef bsl::pair<InverseConcordanceItr, bool>
//                                              InverseConcordanceInsertStatus;
//..
// Next, obtain a concordance for the document set (see
// {'bslstl_unorderedmultimap'|Example 1}).  Here, the concordance is provided
// as a statically initialized array:
//..
//  static struct {
//      const char *d_word;
//      int         d_documentCode;
//      int         d_wordOffset;
//  } concordance[] = {
//      { "extent",             2,  3597 }, { "to",                 2,  1225 },
//      { "greater",            2,  3596 }, { "to",                 2,  1221 },
//      { "abuse",              2,  3551 }, { "to",                 2,  1182 },
//      { "constitutions",      2,  3546 }, { "to",                 2,  1141 },
//      { "affecting",          2,  3491 }, { "to",                 2,  1134 },
//      { "Level",              2,  3477 }, { "to",                 2,  1115 },
//      { "provision",          2,  3465 }, { "to",                 2,  1109 },
//      { "to",                 2,  1292 }, { "Every",              2,  1559 },
//      // ...
//      { "to",                 2,  1252 }, { "Every",              2,  1049 }
//  };
//  const int numConcordance = sizeof concordance/sizeof *concordance;
//..
// Then, we create 'inverseConcordance', an unordered map, and initialize it
// with values obtained from 'concordance'.
//..
//  InverseConcordance inverseConcordance;
//
//  for (int idx = 0; idx < numConcordance; ++idx) {
//      const char *word         = concordance[idx].d_word;
//      int         documentCode = concordance[idx].d_documentCode;
//      int         wordOffset   = concordance[idx].d_wordOffset;
//
//      WordLocation                   location(documentCode, wordOffset);
//      InverseConcordanceEntry        entry(location, bsl::string(word));
//      InverseConcordanceInsertStatus status = inverseConcordance.insert(
//                                                                    entry);
//      assert(status.second);
//  }
//..
// Notice that we expect every 'insert' to be successful, as the concordance
// should not show more than one word at any location.
//
// Next, suppose we knew the location of the word "unalienable" in the document
// set (see {'bslstl_unorderedmultimap'|Example 1}) and want to know its
// context?
//..
//  "unalienable",  0,  109
//..
// We use the 'find' method of 'inverseConcordance' to determine the words
// within offset 'delta' of "unalienable".  Note that we must check the
// validity of the returned interator, in case we probe beyond the boundaries
// of the document.
//..
//  const int docCode =   0;
//  const int origin  = 109;
//  const int delta   =  16;
//
//  for (int offset = origin - delta; offset < origin + delta; ++offset) {
//      WordLocation          location(docCode, offset);
//      InverseConcordanceItr itr = inverseConcordance.find(location);
//
//      if (inverseConcordance.end() != itr) {
//          printf("%d %4d: %s\n",
//                 itr->first.first,
//                 itr->first.second,
//                 itr->second.c_str());
//          assert((origin == offset ? bsl::string("unalienable")
//                                  : itr->second) == itr->second);
//      }
//  }
//..
// Finally, we find on standard output:
//..
//  0   93: evident
//  0   94: that
//  0   95: all
//  0   96: men
//  0   97: are
//  0   98: created
//  0   99: equal
//  0  100: that
//  0  101: they
//  0  102: are
//  0  103: endowed
//  0  104: by
//  0  105: their
//  0  106: Creator
//  0  107: with
//  0  108: certain
//  0  109: unalienable
//  0  110: Rights
//  0  111: that
//  0  112: among
//  0  113: these
//  0  114: are
//  0  115: Life
//  0  116: Liberty
//  0  117: and
//  0  118: the
//  0  119: pursuit
//  0  120: of
//  0  121: Happiness
//  0  122: That
//  0  123: to
//  0  124: secure
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_unorderedmap.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>  // Can probably escape with a fwd-decl, but not
#endif                         // very user friendly

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_EQUALTO
#include <bslstl_equalto.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLE
#include <bslstl_hashtable.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLEBUCKETITERATOR
#include <bslstl_hashtablebucketiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLEITERATOR
#include <bslstl_hashtableiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATORUTIL
#include <bslstl_iteratorutil.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h> // required to implement 'at'
#endif

#ifndef INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION
#include <bslstl_unorderedmapkeyconfiguration.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {
                        // =======================
                        // class bsl::unorderedmap
                        // =======================

template <
        class KEY,
        class VALUE,
        class HASH  = bsl::hash<KEY>,
        class EQUAL = bsl::equal_to<KEY>,
        class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class unordered_map {
    // This class template implements a value-semantic container type holding
    // an unordered set of key-value pairs having unique keys that provide a
    // mapping from keys (of template parameter type 'KEY') to their associated
    // values (of template parameter type 'VALUE').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:

    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR> AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef bsl::pair<const KEY, VALUE>  ValueType;
        // This typedef is an alias for the type of key-value pair objects
        // maintained by this map.

    typedef ::BloombergLP::bslstl::UnorderedMapKeyConfiguration<ValueType>
                                                             ListConfiguration;
        // This typedef is an alias for the policy used internally by this
        // container to extract the 'KEY' value from the key-value pair
        // objects maintained by this map.

    typedef ::BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR> HashTable;
        // This typedef is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this container.

    typedef ::BloombergLP::bslalg::BidirectionalLink HashTableLink;
        // This typedef is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    typedef typename HashTable::NodeType HashTableNode;
        // This typedef is an alias for the type of nodes that hold the values
        // in this container.

    // FRIEND
    template <class KEY2,
              class VALUE2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
                const unordered_map<KEY2, VALUE2, HASH2, EQUAL2, ALLOCATOR2>&,
                const unordered_map<KEY2, VALUE2, HASH2, EQUAL2, ALLOCATOR2>&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    unordered_map,
                    ::BloombergLP::bslmf::IsBitwiseMoveable,
                    ::BloombergLP::bslmf::IsBitwiseMoveable<HashTable>::value);

    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef VALUE                                      mapped_type;
    typedef bsl::pair<const KEY, VALUE>                value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOCATOR                                  allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

    typedef ::BloombergLP::bslstl::HashTableIterator<
                                         value_type, difference_type> iterator;
    typedef ::BloombergLP::bslstl::HashTableIterator<
                             const value_type, difference_type> const_iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                                   value_type, difference_type> local_iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                       const value_type, difference_type> const_local_iterator;

  private:
    // DATA
    HashTable d_impl;  // underlying hash table used by this container

  public:
    // CREATORS
    explicit unordered_map(size_type             initialNumBuckets = 0,
                           const hasher&         hash = hasher(),
                           const key_equal&      keyEqual = key_equal(),
                           const allocator_type& allocator = allocator_type());
        // Create an empty unordered map having a 'max_load_factor' of 1.0.
        // Optionally specify an 'initialNumBuckets' indicating the minimum
        // initial size of the array of buckets of this container.  If
        // 'initialNumBuckets' is not supplied, one empty bucket shall be used
        // and no memory allocated.  Optionally specify a 'hasher' used to
        // generate the hash values associated to the key-value pairs contained
        // in this object.  If 'hash' is not supplied, a default-constructed
        // object of type 'hasher' is used.  Optionally specify a key-equality
        // functor 'keyEqual' used to verify that two key values are the same.
        // If 'keyEqual' is not supplied, a default-constructed object of type
        // 'key_equal' is used.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is not supplied, a default-
        // constructed object of the (template parameter) type 'allocator_type'
        // is used.  If the 'allocator_type' is 'bsl::allocator' (the default),
        // then 'allocator' shall be convertible to 'bslma::Allocator *'.  If
        // the 'allocator_type' is 'bsl::allocator' and 'allocator' is not
        // supplied, the currently installed default allocator will be used to
        // supply memory.  Note that more than 'initialNumBuckets' buckets may
        // be created in order to preserve the bucket allocation strategy of
        // the hash-table (but never fewer).

    explicit unordered_map(const allocator_type& allocator);
        // Create an empty unordered map, having a 'max_load_factor' of 1.0,
        // that uses the specified 'allocator' to supply memory.  Use a
        // default-constructed object of type 'hasher' to generate hash values
        // for the key-value pairs contained in this object.  Also, use a
        // default-constructed object of type 'key_equal' to verify that two
        // key values are the same.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.

    unordered_map(const unordered_map& original);
        // Create an unordered map having the same value, hasher, key-equality
        // comparator and 'max_load_factor' as the specified 'original'.  Use
        // the allocator returned by 'bsl::allocator_traits<allocator_type>::
        // select_on_container_copy_construction(original.get_allocator())'
        // to supply memory.  If the 'allocator_type' is 'bsl::allocator'
        // (the default), the currently installed default allocator will be
        // used to supply memory.

    unordered_map(const unordered_map&  original,
                  const allocator_type& allocator);
        // Create an unordered map having the same value, hasher, key-equality
        // comparator and 'max_load_factor' as the specified 'original', and
        // using the specified 'allocator' to supply memory.  If the
        // 'allocator_type' is 'bsl::allocator' (the default), then 'allocator'
        // shall be convertible to 'bslma::Allocator *'.

    template <class INPUT_ITERATOR>
    unordered_map(INPUT_ITERATOR        first,
                  INPUT_ITERATOR        last,
                  size_type             initialNumBuckets = 0,
                  const hasher&         hash = hasher(),
                  const key_equal&      keyEqual = key_equal(),
                  const allocator_type& allocator = allocator_type());
        // Create an empty unordered map, having a 'max_load_factor' of 1.0,
        // and then create a 'value_type' object for each iterator in the range
        // starting at the specified 'first' iterator and ending immediately
        // before the specified 'last' iterator, by converting from the object
        // referred to by each iterator.  Insert into this unordered map each
        // such object whose key is not already contained from earlier in the
        // sequence.  Optionally specify a minimum 'initialNumBuckets'
        // suggesting the initial size of the array of buckets of this
        // container.  If 'initialNumBuckets' is not supplied, and 'first' and
        // 'last' denote an empty range, a single empty bucket shall be
        // supplied.  Optionally specify a 'hasher' used to generate hash
        // values for the key-value pairs contained in this object.  If 'hash'
        // is not supplied, a default-constructed object of type 'hasher' is
        // used.  Optionally specify a key-equality functor 'keyEqual' used to
        // verify that two key values are the same.  If 'keyEqual' is not
        // supplied, a default-constructed object of type 'key_equal' is used.
        // Optionally specify an 'allocator' used to supply memory.  If
        // 'allocator' is not supplied, a default-constructed object of the
        // (template parameter) type 'allocator_type' is used.  If the
        // 'allocator_type' is 'bsl::allocator' (the default), then 'allocator'
        // shall be convertible to 'bslma::Allocator *'.  If the
        // 'allocator_type' is 'bsl::allocator' and 'allocator' is not
        // supplied, the currently installed default allocator will be used to
        // supply memory.  The (template parameter) type 'INPUT_ITERATOR' shall
        // meet the requirements of an input iterator defined in the C++11
        // standard [24.2.3] providing access to values of a type convertible
        // to 'value_type'.  The behavior is undefined unless 'first' and
        // 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that more than
        // 'initialNumBuckets' buckets may be created in order to preserve the
        // bucket allocation strategy of the hash-table (but never fewer).

    ~unordered_map();
        // Destroy this object and each of its elements.

    // MANIPULATORS
    unordered_map& operator=(const unordered_map& rhs);
        // Assign to this object the value, hasher, key-equality functor and
        // 'max_load_factor' of the specified 'rhs' object, propagate to this
        // object the allocator of 'rhs' if 'allocator_type' has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter types) 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    mapped_type& operator[](const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key'; if this container does not
        // already contain a 'value_type' object with 'key', first insert a new
        // 'value_type' object having 'key' and a default-constructed
        // 'VALUE' object.  This method requires that the (template parameter)
        // type 'KEY' is "copy-constructible" and the (template parameter)
        // 'VALUE' is "default-constructible" (see {Requirements on 'KEY' and
        // 'VALUE'}).

    mapped_type& at(const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key', if such an entry exists;
        // otherwise throw a 'std::out_of_range' exception.  Note that this
        // method is not exception agnostic.

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this unordered map, or the 'end' iterator if this
        // unordered map is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // unordered map.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // of the bucket having the specified 'index', in the array of buckets
        // maintained by this unordered map, or the 'end(index)' otherwise.
        // The behavior is undefined unless 'index < bucket_count()'.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index's , in the array of buckets
        // maintained by this unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    template <class SOURCE_TYPE>
    pair<iterator, bool> insert(const SOURCE_TYPE& value);
        // Insert the specified 'value' into this unordered map if the key (the
        // 'first' element) of a 'value_type' object constructed from 'value'
        // does not already exist in this unordered map; otherwise this method
        // has no effect (a 'value_type' object having the same key as the
        // converted 'value' already exists in this map) .  Return a 'pair'
        // whose 'first' member is an iterator referring to the (possibly newly
        // inserted) 'value_type' object in this map whose key is the same as
        // that of 'value', and whose 'second' member is 'true' if a new value
        // was inserted, and 'false' if the value was already present.  This
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' types both be "copy-constructible" (see {Requirements on
        // 'KEY' and 'VALUE'}).  Note that this one template stands in for two
        // 'insert' functions in the C++11 standard.

    template <class SOURCE_TYPE>
    iterator insert(const_iterator hint, const SOURCE_TYPE& value);
        // Insert the specified 'value' into this unordered map if the key (the
        // 'first' element) of a 'value_type' object constructed from 'value'
        // does not already exist in this unordered map.  Return an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this map whose key is the same as that of the converted 'value'.
        // This method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be "copy-constructible" (see {Requirements on 'KEY' and
        // 'VALUE'}).  The behavior is undefined unless 'hint' is a valid
        // iterator into this unordered map, although its value is not used
        // otherwise.  Note that this one template stands in for two 'insert'
        // functions in the C++11 standard.

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Create a 'value_type' object for each iterator in the range starting
        // at the specified 'first' iterator and ending immediately before the
        // specified 'last' iterator, by converting from the object referred
        // to by each iterator.  Insert into this unordered map each such
        // object whose key is not already contained.  The (template parameter)
        // type 'INPUT_ITERATOR' shall meet the requirements of an input
        // iterator defined in the C++11 standard [24.2.3] providing access to
        // values of a type convertible to 'value_type'.  This method requires
        // that the (template parameter) types 'KEY' and 'VALUE_TYPE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    iterator erase(const_iterator position);
        // Remove from this unordered map the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this map.  The behavior is
        // undefined unless 'position' refers to a 'value_type' object in this
        // unordered map.

    size_type erase(const key_type& key);
        // Remove from this map the 'value_type' object having the specified
        // 'key', if it exists, and return 1; otherwise (there is no
        // 'value_type' object having 'key') return 0 with no other effect.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this map the 'value_type' objects starting at the
        // specified 'first' position up to, but not including, the specified
        // 'last' position, and return 'last'.  The behavior is undefined
        // unless 'first' and 'last' either refer to elements in this map or
        // are the 'end' iterator, and the 'first' position is at or before the
        // 'last' position in the iteration sequence provided by this
        // container.

    void swap(unordered_map& other);
        // Exchange the value of this object as well as its hasher,
        // key-equality functor and 'max_load_factor' with those of the
        // specified 'other' object.  If 'allocator_type' has the trait
        // 'propagate_on_container_swap', exchange the allocator of this object
        // with that of the 'other' object, and do not modify either allocator
        // otherwise.  This method provides the no-throw exception-safety
        // guarantee and guarantees O[1] complexity, as long as the (template
        // parameter) 'hasher' and (template parameter) 'key_equal' do not
        // throw when 'swap'ped.  The behavior is undefined unless either this
        // object was created with the same allocator as 'other' or
        // 'allocator_type' has the 'propagate_on_container_swap' trait.

    void clear();
        // Remove all entries from this map.  Note that the container is
        // empty after this call, but allocated memory may be retained for
        // future use.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this map having the specified 'key', if such an entry
        // exists, and the past-the-end ('end') iterator otherwise.

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered map having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the
        // end of the sequence.  If this unordered map contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value, 'end()'.  Note that since a map maintains unique keys,
        // the range will contain at most one element.

    void  max_load_factor(float newMaxLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newMaxLoadFactor'.  This may require an immediate rehash if
        // 'newMaxLoadFactor < loadFactor()', otherwise is has a constant-time
        // cost.  The behavior is undefined unless '0 < newMaxLoadFactor'.
        // Note that the C++11 standard does not allow this operation to
        // rehash, as it requires a constant cost for all (positive) values of
        // 'newMaxLoadFactor'.

    void  rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to at least the specified 'numBuckets', and redistribute all the
        // contained elements into the new sequence of buckets, according to
        // their hash values.  After this call, 'load_factor' will be less than
        // or equal to 'max_load_factor'.  This operation has no effect if
        // rehashing the elements into 'numBuckets' would cause this map to
        // exceed its 'max_load_factor'.

    void  reserve(size_type numElements);
        // Change the size of the array of buckets maintained by this container
        // so that 'load_factor() < max_load_factor()', even if the container
        // grows to a 'size' of the specificed 'numElements', and redistribute
        // all the contained elements into the new sequence of buckets,
        // according to their hash values.  This operation has no effect if
        // 'size() <= numElements'.

    // ACCESSORS
    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // unordered map.

    const_iterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this unordered map, or the 'end' iterator if this
        // unordered map is empty.

    const_iterator end() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects)
        // maintained by this unordered map.

    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this unordered map, or the 'end' iterator if this
        // unordered map is empty.

    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects)
        // maintained by this unordered map.

    const_local_iterator begin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map, or the 'end(index)' otherwise.
        // The behavior is undefined unless 'index < bucket_count()'.

    const_local_iterator end(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map, or the 'end(index)' otherwise.
        // The behavior is undefined unless 'index < bucket_count()'.

    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    bool empty() const;
        // Return 'true' if this unordered map contains no elements, and
        // 'false' otherwise.

    size_type size() const;
        // Return the number of elements in this unordered map.

    size_type max_size() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this unordered map could possibly hold.  Note that there is no
        // guarantee that the unordered map can successfully grow to the
        // returned size, or even close to that size, without running out of
        // resources.

    const mapped_type& at(const key_type& key) const;
        // Return a reference providing non-modifiable access to the
        // mapped-value associated with the specified 'key', if such an entry
        // exists; otherwise throw a 'std::out_of_range' exception.  Note that
        // this method is not exception agnostic.

    hasher hash_function() const;
        // Return (a copy of) the hash unary functor used by this unordered map
        // to generate a hash value (of type 'size_t') for a 'key_type' object.

    key_equal key_eq() const;
        // Return (a copy of) the key-equality binary functor used by this
        // unordered map that returns 'true' if two 'key_type' objects have the
        // same value, and 'false' otherwise.

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this unordered map having the specified
        // 'key', if such an entry exists, and the past-the-end ('end')
        // iterator otherwise.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this container
        // having the specified 'key'.  Note that since an unordered map
        // maintains unique keys, the returned value will be either 0 or 1.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this container having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence and the second iterator is positioned one past
        // the end of the sequence.  If this map contains no 'value_type'
        // objects having 'key' then the two returned iterators will have the
        // same value, 'end()'.  Note that since an unordered map maintains
        // unique keys, the range will contain at most one element.

    size_type bucket_count() const;
        // Return the number of buckets in the array of buckets maintained by
        // this map.

    size_type max_bucket_count() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the unordered map can successfully grow to the
        // returned size, or even close to that size, without running out of
        // resources.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.  The behavior is undefined unless
        // 'index < bucket_count()'.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where values having the specified 'key' would be
        // inserted.

    float load_factor() const;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The 'load_factor' is a measure of how full
        // the container is, and a higher load factor typically leads to an
        // increased number of collisions, thus resulting in a loss
        // performance.

    float max_load_factor() const;
        // Return the maximum load factor allowed for this container.  If
        // an insert operation would cause 'load_factor' to exceed
        // the 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets (see rehash).
};

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_map' objects have the
    // same value if they have the same number of key-value pairs, and for each
    // key-value pair that is contained in 'lhs' there is a key-value pair
    // contained in 'rhs' having the same value, and vice-versa.  This method
    // requires that the (template parameter) types 'KEY' and 'VALUE' both be
    // "equality-comparable" (see {Requirements on 'KEY' and 'VALUE'}).

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(
                 const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
                 const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_map' objects do not
    // have the same value if they do not have the same number of key-value
    // pairs, or that for some key-value pair that is contained in 'lhs' there
    // is not a key-value pair in 'rhs' having the same value, and vice-versa.
    // This method requires that the (template parameter) types 'KEY' and
    // 'VALUE' both be "equality-comparable" (see {Requirements on 'KEY' and
    // 'VALUE'}).

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& a,
          unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& b);
    // Exchange the value, the hasher, the key-equality functor and the
    // 'max_load_factor' of the specificed 'a' object with those of the
    // specified 'b' object.  If the (template parameter) type 'ALLOCATOR' has
    // the trait 'propagate_on_container_swap', exchange the allocator of 'a'
    // with that of 'b', and do not modify either allocator otherwise.  This
    // function provides the no-throw exception-safety guarantee and guarantees
    // O[1] complexity, as long as the (template parameter) type 'HASH' and the
    // (template parameter) type 'EQUAL' do not throw when 'swap'ped.  The
    // behavior is undefined unless the objects 'a' and 'b' were created with
    // the same allocator, or 'ALLOCATOR' has the 'propagate_on_container_swap'
    // trait.

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *unordered* *associative* containers:
//: o An unordered associative container defines STL iterators.
//: o An unordered associative container is bitwise moveable if the both
//:      functors and the allocator are bitwise moveable.
//: o An unordered associative container uses 'bslma' allocators if the
//:   parameterized 'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {
namespace bslalg {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR> >
     : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
struct UsesBslmaAllocator<bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC> >
     : bsl::is_convertible<Allocator*, ALLOC>::type
{};

}  // close namespace bslma

}  // close namespace BloombergLP

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

namespace bsl
{
                        //--------------------
                        // class unordered_map
                        //--------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
unordered_map(size_type             initialNumBuckets,
              const hasher&         hash,
              const key_equal&      keyEqual,
              const allocator_type& allocator)
: d_impl(hash, keyEqual, initialNumBuckets, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                       INPUT_ITERATOR        first,
                                       INPUT_ITERATOR        last,
                                       size_type             initialNumBuckets,
                                       const hasher&         hash,
                                       const key_equal&      keyEqual,
                                       const allocator_type& allocator)
: d_impl(hash, keyEqual, initialNumBuckets, allocator)
{
    this->insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                                 const unordered_map& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                               const allocator_type& allocator)
: d_impl(HASH(), EQUAL(), 0, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                               const unordered_map&  original,
                                               const allocator_type& allocator)
: d_impl(original.d_impl, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::~unordered_map()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator=(
                                                    const unordered_map& rhs)
{
    unordered_map(rhs, this->get_allocator()).swap(*this);
    return *this;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::mapped_type&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator[](
                                                           const key_type& key)
{
    HashTableLink *node = d_impl.insertIfMissing(key);
    return static_cast<HashTableNode *>(node)->value().second;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::mapped_type&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::at(const key_type& key)
{
    HashTableLink *node = d_impl.find(key);

    if (!node) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                        "unordered_map<...>::at(key_type): invalid key value");
    }

    return static_cast<HashTableNode *>(node)->value().second;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin()
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end()
{
    return iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::clear()
{
    d_impl.removeAll();
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                       const_iterator position)
{
    BSLS_ASSERT(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(const key_type& key)
{
    if (HashTableLink *target = d_impl.find(key)) {
        d_impl.remove(target);
        return 1;                                                     // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(const_iterator first,
                                                         const_iterator last)
{

#if defined BDE_BUILD_TARGET_SAFE_2
    if (first != last) {
        iterator it        = this->begin();
        const iterator end = this->end();
        for (; it != first; ++it) {
            BSLS_ASSERT(last != it);
            BSLS_ASSERT(end  != it);
        }
        for (; it != last; ++it) {
            BSLS_ASSERT(end  != it);
        }
    }
#endif

    while (first != last) {
        first = this->erase(first);
    }

    return iterator(first.node()); // convert from const_iterator
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
inline
bsl::pair<typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator,
          bool>
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                      const SOURCE_TYPE& value)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, value);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                      const_iterator,
                                                      const SOURCE_TYPE& value)
{
    // There is no realistic use-case for the 'hint' in an unordered_map of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key-check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.

    bool isInsertedFlag;  // not used

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, value);

    return iterator(result);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                          INPUT_ITERATOR first,
                                                          INPUT_ITERATOR last)
{
    size_t maxInsertions =
            ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);
    if (maxInsertions) {
        this->reserve(this->size() + maxInsertions);
    }

    bool isInsertedFlag;  // not used
    while (first != last) {
        d_impl.insertIfMissing(&isInsertedFlag, *first++);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator,
          typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                           const key_type& key)
{
    typedef bsl::pair<iterator, iterator> ResultType;

    HashTableLink *first = d_impl.find(key);
    return first
         ? ResultType(iterator(first), iterator(first->nextLink()))
         : ResultType(iterator(0),     iterator(0));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                        float newMaxLoadFactor)

{
    d_impl.maxLoadFactor(newMaxLoadFactor);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::rehash(size_type numBuckets)
{
    return d_impl.rehash(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::reserve(
                                                         size_type numElements)
{
    return d_impl.rehashForNumElements(numElements);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::swap(unordered_map& other)
{
    BSLS_ASSERT_SAFE(this->get_allocator() == other.get_allocator());

    d_impl.swap(other.d_impl);
}


// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
const typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::mapped_type&
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::at(
                                                     const key_type& key) const
{
    HashTableLink *target = d_impl.find(key);
    if (!target ){
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                        "unordered_map<...>::at(key_type): invalid key value");
    }
    return static_cast<HashTableNode *>(target)->value().second;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end() const
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend() const
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
        unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket(
                                                     const key_type& key) const
{
    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_count() const
{
    return d_impl.numBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_size(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::count(
                                                     const key_type& key) const
{
    return d_impl.find(key) != 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::empty() const
{
    return 0 == d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<typename unordered_map<KEY,
                                 VALUE,
                                 HASH,
                                 EQUAL,
                                 ALLOCATOR>::const_iterator,
          typename unordered_map<KEY,
                                 VALUE,
                                 HASH,
                                 EQUAL,
                                 ALLOCATOR>::const_iterator>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    typedef bsl::pair<const_iterator, const_iterator> ResultType;

    HashTableLink *first = d_impl.find(key);
    return first
         ? ResultType(const_iterator(first), const_iterator(first->nextLink()))
         : ResultType(const_iterator(0),     const_iterator(0));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
       unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(
                                                     const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::get_allocator() const
{
    return d_impl.allocator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hasher
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_size() const
{
    return d_impl.maxSize();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::load_factor() const
{
    return d_impl.loadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor() const
{
    return d_impl.maxLoadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_bucket_count() const
{
    return d_impl.maxNumOfBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_impl.size();
}

}  // close namespace bsl

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
      const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
      const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
             const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
             const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
bsl::swap(bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& a,
          bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& b)
{
    a.swap(b);
}


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
