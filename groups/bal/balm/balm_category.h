// balm_category.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_CATEGORY
#define INCLUDED_BALM_CATEGORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: balm_category.h,v 1.4 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide a representation of a metric category.
//
//@CLASSES:
//    balm::Category: a representation of a metric category
//
//@SEE_ALSO: balm_metricregistry, balm_metricid, balm_metricdescription
//
//@DESCRIPTION: This component provides a class, 'balm::Category', whose values
// are used to categorize collected metrics.  A metric "category" is an
// identifier (chosen by the application) that groups together one or more
// metrics.  A 'balm::Category' object contains the address of a string holding
// the name of the category and a boolean value indicating whether the
// category is currently enabled.  The 'balm::Category' class suppresses copy
// construction and assignment, and does not provide equality operators;
// applications should use a *single* 'balm::Category' object instance per
// category (such as one provided by the *'balm::MetricRegistry'* component).
//
// IMPORTANT: The category name, whose type is 'const char *', must remain
// constant and valid throughout the lifetime of the 'balm::Category' object.
//
///Thread Safety
///-------------
// 'balm::Category' is generally *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'balm::Category' in one thread while another thread
// modifies the same object.  However, it is safe to *access* the 'enabled'
// property on one (or more) thread(s) while the object is being modified on
// another thread.
//
///Usage
///-----
// The following example demonstrates how to create, access, and modify a
// 'balm::Category' object.  We start by creating three category objects with
// different names:
//..
//  balm::Category categoryA("A", true);
//  balm::Category categoryB("B", false);
//  balm::Category categoryC("C");
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
//  categoryA: [ A ENABLED ]
//  categoryB: [ B DISABLED ]
//  categoryC: [ C DISABLED ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

namespace BloombergLP {


namespace balm {

class CategoryHolder;

                               // ==============
                               // class Category
                               // ==============

class Category {
    // This class provides a mechanism for representing a category.  A category
    // is an identifier used to group related metrics.  A 'Category' object
    // contains the address of a null-terminated string, 'name', holding the
    // name of the category and a boolean value, 'enabled', indicating whether
    // the category is currently enabled.

    // DATA
    const char     *d_name_p;     // name of the category (held, not owned)

    bsls::AtomicInt d_enabled;    // whether the category is enabled

    CategoryHolder *d_holders_p;  // linked list of holders of this category

    // NOT IMPLEMENTED
    Category(const Category& );
    Category& operator=(const Category& );

  public:
    // CREATORS
    explicit Category(const char *name, bool enabledFlag = true);
        // Create a category having the specified 'name' address.  Optionally
        // specify 'enabledFlag', the enabled status of the category; if
        // 'enabledFlag' is not specified, the 'enabled' status is 'true'.  The
        // behavior is undefined unless 'name' remains valid and unmodified for
        // the lifetime of this object.

    ~Category();
        // Destroy this category object.

    // MANIPULATORS
    void setName(const char *name);
        // Set the name of this metric to the specified 'name' address.  The
        // behavior is undefined unless 'name' remains valid and unmodified
        // for the lifetime of this object.

    void setEnabled(bool enabledFlag);
        // Set the 'enabled' state of this category to the value of the
        // specified 'enabledFlag' and update any 'CategoryHolder' objects
        // registered with this category.  Note that this operation is *not*
        // atomic, and other threads may simultaneously access the current
        // enabled value while this operation is performed.  Also note that
        // this operation has *linear* performance with respect to the number
        // of registered category holders for 'category'.

    void registerCategoryHolder(CategoryHolder *holder);
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

    bool enabled() const;
        // Report whether this category is enabled. This function is fully
        // thread-safe.

    const bsls::AtomicInt& isEnabledRaw() const;
        // Return a *reference* to a const value indicating the enabled status
        // of this category, allowing downstream uses to minimize latency by
        // avoiding indirection through abstracted interfaces, albeit at some
        // risk of object-lifetime violations.  The returned reference must not
        // be allowed to outlive this category object.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Print this category to the specified output 'stream' in some human
        // readable form, and return the modifiable 'stream'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Category& rhs);
    // Output a formatted description of the specified 'rhs' category to the
    // specified 'stream', and return the modifiable 'stream'.

                            // ====================
                            // class CategoryHolder
                            // ====================

class CategoryHolder {
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
    // implementation detail of the 'balm' metric collection system.

    // NOT IMPLEMENTED
    CategoryHolder& operator=(const CategoryHolder& rhs);

  public:

    // PUBLIC DATA MEMBERS
    bool            d_enabled;     // whether the category is enabled
    const Category *d_category_p;  // held category (not owned)
    CategoryHolder *d_next_p;      // next category holder in linked list

    // CREATORS

    // No constructors or destructors are declared in order to allow for static
    // initialization of instances of this class.

    // MANIPULATORS
    void reset();
        // Reset this object to its default value.  The default value is:
        //..
        //   { false, 0, 0 }
        //..

    void setCategory(const Category *category);
        // Set the address of the category held by this holder to the specified
        // 'category'.

    void setEnabled(bool enabledFlag);
        // Set the 'enabled' state of this category to the value of the
        // specified 'enabledFlag'.

    void setNext(CategoryHolder *holder);
        // Set this holder to point to the specified 'holder'.

    // ACCESSORS
    const Category *category() const;
        // Return the address of the non-modifiable category held by this
        // holder.

    bool enabled() const;
        // Return 'true' if 'category' is valid (i.e., non-null) and enabled,
        // and 'false' otherwise.

    CategoryHolder *next() const;
        // Return the address of the modifiable holder pointed to by this
        // holder.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // --------------
                               // class Category
                               // --------------

// CREATORS
inline
Category::Category(const char *name, bool enabledFlag)
: d_name_p(name)
, d_enabled(enabledFlag)
, d_holders_p(0)
{
}

// MANIPULATORS
inline
void Category::setName(const char *name)
{
    d_name_p = name;
}

// ACCESSORS
inline
const char *Category::name() const
{
    return d_name_p;
}

inline
bool Category::enabled() const
{
    return d_enabled;
}

inline
const bsls::AtomicInt& Category::isEnabledRaw() const
{
    return d_enabled;
}

                            // --------------------
                            // class CategoryHolder
                            // --------------------

// MANIPULATORS
inline
void CategoryHolder::setCategory(const Category *category)
{
    d_category_p = category;
}

inline
void CategoryHolder::setEnabled(bool enabledFlag)
{
    d_enabled = enabledFlag;
}

inline
void CategoryHolder::setNext(CategoryHolder *holder)
{
    d_next_p = holder;
}

// ACCESSORS
inline
const Category *CategoryHolder::category() const
{
    return d_category_p;
}

inline
bool CategoryHolder::enabled() const
{
    return d_enabled;
}

inline
CategoryHolder *CategoryHolder::next() const
{
    return d_next_p;
}
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& balm::operator<<(bsl::ostream& stream, const Category& rhs)
{
    return rhs.print(stream);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
