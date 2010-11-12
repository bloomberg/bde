// baem_category.h                -*-C++-*-
#ifndef INCLUDED_BAEM_CATEGORY
#define INCLUDED_BAEM_CATEGORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: baem_category.h,v 1.4 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide a representation of a metric category.
//
//@CLASSES:
//    baem_Category: a representation of a metric category
//
//@SEE_ALSO: baem_metricregistry, baem_metricid, baem_metricdescription
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a class, 'baem_Category', whose values
// are used to categorize collected metrics.  A metric "category" is an
// identifier (chosen by the application) that groups together one or more
// metrics.  A 'baem_Category' object contains the address of a string holding
// the name of the category and a boolean value indicating whether the
// category is currently enabled.  The 'baem_Category' class suppresses
// copy construction and assignment, and does not provide equality operators;
// applications should use a *single* 'baem_Category' object instance per
// category (such as one provided by the *'baem_MetricRegistry'* component).
//
// IMPORTANT: The category name, whose type is 'const char *', must remain
// constant and valid throughout the lifetime of the 'baem_Category' object.
//
///Thread Safety
///-------------
// 'baem_Category' is generally *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'baem_Category' in one thread while another thread
// modifies the same object.  However, it is safe to *access* the 'enabled'
// property on one (or more) thread(s) while the object is being modified on
// another thread.
//
///Usage
///-----
// The following example demonstrates how to create, access, and modify a
// 'baem_Category' object.  We start by creating three category objects with
// different names:
//..
//  baem_Category categoryA("A", true);
//  baem_Category categoryB("B", false);
//  baem_Category categoryC("C");
//..
// Once the category objects have been created, we can use the 'name' and
// 'enabled' methods to access their values:
//..
//  assert(0 == bsl::strcmp("A", categoryA.name()));
//  assert(0 == bsl::strcmp("B", categoryB.name()));
//  assert(0 == bsl::strcmp("C", categoryC.name()));
//
//  assert( categoryA.enabled());
//  assert(!categoryB.enabled());
//  assert( categoryC.enabled());
//..
// Finally, we modify the enablement status of one of the categories, and then
// write all three categories to the console:
//..
//  categoryC.setEnabled(false);
//
//  bsl::cout << "categoryA: " << categoryA << bsl::endl
//            << "categoryB: " << categoryB << bsl::endl
//            << "categoryC: " << categoryC << bsl::endl;
//..
// With the resulting console output:
//..
// categoryA: [ A ENABLED ]
// categoryB: [ B DISABLED ]
// categoryC: [ C DISABLED ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class baem_CategoryHolder;

                          // ===================
                          // class baem_Category
                          // ===================

class baem_Category {
    // This class provides a mechanism for representing a category.  A category
    // is an identifier used to group related metrics.  A 'baem_Category'
    // object contains the address of a null-terminated string, 'name', holding
    // the name of the category and a boolean value, 'enabled', indicating
    // whether the category is currently enabled.

    // DATA
    const char          *d_name_p;     // name of the category (held, not
                                       // owned)

    volatile bool        d_enabled;    // whether the category is enabled

    baem_CategoryHolder *d_holders_p;  // linked list of holders of this
                                       // category

    // NOT IMPLEMENTED
    baem_Category(const baem_Category& );
    baem_Category& operator=(const baem_Category& );

  public:
    // CREATORS
    explicit baem_Category(const char *name, bool enabledFlag = true);
        // Create a category having the specified 'name' address.  Optionally
        // set the 'enabled' status of the category to the specified
        // 'enabledFlag'; if 'enabledFlag' is not specified, the 'enabled'
        // status is set to 'true'.  The behavior is undefined unless 'name'
        // remains valid and unmodified for the lifetime of this object.

    ~baem_Category();
        // Destroy this category object.

    // MANIPULATORS
    void setName(const char *name);
        // Set the name of this metric to the specified 'name' address.  The
        // behavior is undefined unless 'name' remains valid and unmodified
        // for the lifetime of this object.

    void setEnabled(bool enabledFlag);
        // Set the 'enabled' state of this category to the value of the
        // specified 'enabledFlag' and update any 'baem_CategoryHolder'
        // objects registered with this category.  Note that this operation is
        // *not* atomic, and other threads may simultaneously access the
        // current enabled value while this operation is performed.  Also note
        // that this operation has *linear* performance with respect to the
        // number of registered category holders for 'category'.

    void registerCategoryHolder(baem_CategoryHolder *holder);
        // Load into the specified 'holder' the address of this category, its
        // 'enabled()' status, and the address of the next holder in the
        // linked list of category holders maintained by this object
        // (prepending 'holder' to this category's linked list of category
        // holders).  This category will update 'holder->enabled()' when its
        // enabled state changes, and will reset 'holder' (i.e.,
        // 'holder->reset()') when this category is destroyed.  The behavior
        // is undefined unless 'holder' remains valid and *unmodified* (by the
        // client) for the lifetime of this object and is *not* registered
        // again with any category (including this one).

    // ACCESSORS
    const char *name() const;
        // Return the address of the non-modifiable null-terminated string
        // containing the name of this category.

    const volatile bool& enabled() const;
        // Return a reference to the non-modifiable boolean value indicating
        // whether this category is enabled.  Note that this value is
        // explicitly returned by reference to allow clients to refer to the
        // value directly (rather than indirectly, through the owning
        // category).

    bsl::ostream& print(bsl::ostream& stream) const;
        // Print this category to the specified output 'stream' in some human
        // readable form, and return the modifiable 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_Category& rhs);
    // Output a formatted description of the specified 'rhs' category to the
    // specified 'stream', and return the modifiable 'stream'.

                        // =========================
                        // class baem_CategoryHolder
                        // =========================

class baem_CategoryHolder {
    // This class, informally referred to as a "category holder" (or simply
    // "holder"), holds a category, an enabled value, and a pointer to a
    // "next" holder.  Both the category and next pointer may be null.  The
    // intended use is as follows: (1) instances of this class are (only)
    // declared in contexts where collecting a metric occurs; (2) if the
    // enabled value is true, the category contains the address of a valid,
    // enabled, category; (3) if the next pointer is non-null, then the holder
    // pointed to holds the same category and threshold.  Instances of this
    // class must be *statically* initializable.  Hence, the data members are
    // 'public', and no constructors or destructor are defined.
    //
    // This class should *not* be used directly by client code.  It is an
    // implementation detail of the 'baem' metric collection system.

    // NOT IMPLEMENTED
    baem_CategoryHolder& operator=(const baem_CategoryHolder& rhs);

  public:

    // PUBLIC DATA MEMBERS
    bool                 d_enabled;     // whether the category is enabled
    const baem_Category *d_category_p;  // held category (not owned)
    baem_CategoryHolder *d_next_p;      // next category holder in linked list

    // CREATORS

    // No constructors or destructors are declared in order to allow for static
    // initialization of instances of this class.

    // MANIPULATORS
    void reset();
        // Reset this object to its default value.  The default value is:
        //..
        //   { false, 0, 0 }
        //..

    void setCategory(const baem_Category *category);
        // Set the address of the category held by this holder to the specified
        // 'category'.

    void setEnabled(bool enabledFlag);
        // Set the 'enabled' state of this category to the value of the
        // specified 'enabledFlag'.

    void setNext(baem_CategoryHolder *holder);
        // Set this holder to point to the specified 'holder'.

    // ACCESSORS
    const baem_Category *category() const;
        // Return the address of the non-modifiable category held by this
        // holder.

    bool enabled() const;
        // Return 'true' if 'category' is valid (i.e., non-null) and enabled,
        // and 'false' otherwise.

    baem_CategoryHolder *next() const;
        // Return the address of the modifiable holder pointed to by this
        // holder.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // -------------------
                          // class baem_Category
                          // -------------------

// CREATORS
inline
baem_Category::baem_Category(const char *name, bool enabled)
: d_name_p(name)
, d_enabled(enabled)
, d_holders_p(0)
{
}

// MANIPULATORS
inline
void baem_Category::setName(const char *name)
{
    d_name_p = name;
}

// ACCESSORS
inline
const char *baem_Category::name() const
{
    return d_name_p;
}

inline
const volatile bool& baem_Category::enabled() const
{
    return d_enabled;
}

                        // -------------------------
                        // class baem_CategoryHolder
                        // -------------------------

// MANIPULATORS
inline
void baem_CategoryHolder::setCategory(const baem_Category *category)
{
    d_category_p = category;
}

inline
void baem_CategoryHolder::setEnabled(bool enabledFlag)
{
    d_enabled = enabledFlag;
}

inline
void baem_CategoryHolder::setNext(baem_CategoryHolder *holder)
{
    d_next_p = holder;
}

// ACCESSORS
inline
const baem_Category *baem_CategoryHolder::category() const
{
    return d_category_p;
}

inline
bool baem_CategoryHolder::enabled() const
{
    return d_enabled;
}

inline
baem_CategoryHolder *baem_CategoryHolder::next() const
{
    return d_next_p;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_Category& rhs)
{
    return rhs.print(stream);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
