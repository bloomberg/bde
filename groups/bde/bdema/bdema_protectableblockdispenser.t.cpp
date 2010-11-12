// bdema_protectableblockdispenser.t.cpp  -*-C++-*-

#include <bdema_protectableblockdispenser.h>

#include <bslma_allocator.h>                // for testing only
#include <bslma_defaultallocatorguard.h>    // for testing only
#include <bslma_testallocator.h>            // for testing only

#include <bsls_alignmentutil.h>

#include <bsl_iostream.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The protectable memory block dispenser contains a protocol class.
//-----------------------------------------------------------------------------
// [ 1] Virtual method test
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdema_MemoryBlockDescriptor Block;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void printBytes( void *address, int numBytes)
// Print on a single line the specified address (in hex) follow by the
// (hex) value of the block of 16 bytes it addresses.
{
    unsigned char *addr = (unsigned char *) address;
    unsigned char *end  = addr + numBytes;
    while (addr < end) {
        std::printf("%p:\t", addr);
        for (int i = 0; i < 16 && addr < end; ++i) {
            if (i % 4 == 0) {
                std::printf("  ");
            }
            std::printf("%02x", *addr++);
        }
        std::printf("\n");
    }
}

class NoopProtectableBlockDispenser : public bdema_ProtectableBlockDispenser {
        // The NoopProtectedAllocator is an empty implementation of the
        // 'bdema_ProtectableBlockDispenser' interface that tracks the
        // methods called and verifies the 'bdema_ProtectedAllocator'
        // protocol.

  private:
    char const **d_lastMethod;  // last method called on this instance

  public:
    // CREATORS
    NoopProtectableBlockDispenser(char const **lastMethod)
        // Create this 'NoopProtectableBlockDispenser' with the specified
        // 'lastMethod' to store the last method called.
    : d_lastMethod(lastMethod)
    {
        *d_lastMethod = "NoopProtectableBlockDispenser";
    }

    ~NoopProtectableBlockDispenser();
        // Destroy this 'NoopProtectableBlockDispenser'

    // MANIPULATORS
    virtual bdema_MemoryBlockDescriptor allocate(size_type size);
        // This method has no effect for this allocator except that a
        // subsequent call to 'lastMethod()' will return 'allocate'.

    virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
        // This method has no effect for this allocator except that a
        // subsequent call to 'lastMethod()' will return 'deallocate'.

    virtual int protect(const bdema_MemoryBlockDescriptor& block);
        // Return '0'.  This method has no effect for this allocator except
        // that a subsequent call to 'lastMethod()' will return 'protect'.

    virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
        // Return '0'.  This method has no effect for this allocator except
        // that a subsequent call to 'lastMethod()' will return 'unprotect'.

    // ACCESSORS
    int minimumBlockSize() const;
        // Return '0'.  This method has no effect for this allocator except
        // that a subsequent call to 'lastMethod()' will return 'blockSize'.

    const char *lastMethod() { return *d_lastMethod; }
        // Return the last method called on this 'NoopProtectedAllocator'
        // instance.
};

NoopProtectableBlockDispenser::~NoopProtectableBlockDispenser()
{
    *d_lastMethod = "~NoopProtectableBlockDispenser";
}

bdema_MemoryBlockDescriptor
NoopProtectableBlockDispenser::allocate(size_type size)
{
    *d_lastMethod = "allocate";
    return bdema_MemoryBlockDescriptor();
}

void NoopProtectableBlockDispenser::deallocate(
                           const bdema_MemoryBlockDescriptor& block)
{
    *d_lastMethod = "deallocate";
}

int NoopProtectableBlockDispenser::protect(
                           const bdema_MemoryBlockDescriptor& block)
{
    *d_lastMethod = "protect";
    return 0;
}

int NoopProtectableBlockDispenser::unprotect(
                           const bdema_MemoryBlockDescriptor& block)
{
    *d_lastMethod = "unprotect";
    return 0;
}

int NoopProtectableBlockDispenser::minimumBlockSize() const
{
    *d_lastMethod = "minimumBlockSize";
    return 0;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following examples demonstrate how the
// 'bdema_ProtectableBlockDispenser' protocol can be implemented and
// used.

///Usage 1 -- Trivial example
///- - - - - - - - - - - - -
// The following is a simple and somewhat unrealistic example of using a
// 'bdema_ProtectableBlockDispenser' to allocate, protect, unprotect,
// and deallocate memory blocks:
//..
    void doImportantStuff(bdema_ProtectableBlockDispenser *dispenser)
    {
        ASSERT(0 != dispenser);
//..
// We now allocate memory for our data using the specified 'dispenser' and
// then protect the data from accidental writes (by another thread, for
// example):
//..
        const int size = dispenser->minimumBlockSize();
        bdema_MemoryBlockDescriptor block = dispenser->allocate(size);
//..
// Memory is unprotected when initially allocated from a protectable block
// dispenser, so we can freely write to it:
//..
        std::strcpy(static_cast<char *>(block.address()), "data");

        dispenser->protect(block);
//..
// But now that the block is protected, it cannot be written to without causing
// a memory fault:
//..
          // std::strcpy(static_cast<char *>(block.address()), "BAD");
          // WARNING: THIS WILL CAUSE A SEGMENTATION VIOLATION!
//..
// Note that the memory block must be *unprotected* before being deallocated:
//..
         dispenser->unprotect(block);
         dispenser->deallocate(block);
    }
//..
///Usage 2 - Creating a Class that Uses a 'bdema_ProtectableBlockDispenser'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates a somewhat more realistic use of a protectable
// memory block dispenser.  The class 'IntegerStack' uses a
// 'bdema_ProtectableBlockDispenser', supplied at construction, to
// protect the data inside the stack from being unintentionally overwritten:
//..
    class IntegerStack {
        // This is a trivial implementation of a stack of ints whose data
        // has READ-ONLY access protection.  This class does not perform
        // bounds checking.

        bdema_MemoryBlockDescriptor     d_data;        // block of memory
                                                       // holding the stack

        int                            *d_cursor_p;    // top of the stack
        bdema_ProtectableBlockDispenser
                                       *d_dispenser_p; // memory dispenser
                                                       // (held not owned)
  // ...

      private:
        // PRIVATE MANIPULATORS
        void resize()
            // Increase the size of the stack by a factor of 2.
        {
            bdema_MemoryBlockDescriptor oldData = d_data;
            d_data = d_dispenser_p->allocate(d_data.size() * 2);
            d_cursor_p = (int *)((char *)d_data.address() +
                         ((char *)d_cursor_p - (char *)oldData.address()));
            std::memcpy(d_data.address(), oldData.address(), oldData.size());
            d_dispenser_p->deallocate(oldData);
        }

      public:
        // CREATORS
        IntegerStack(int                              initialSize,
                     bdema_ProtectableBlockDispenser *protectedDispenser)
              // Create an 'IntegerStack' having the specified 'initialSize'.
              // Use the specified 'protectedDispenser' to supply memory.  The
              // behavior is undefined unless '0 <= maxStackSize' and
              // 'protectedDispenser' is non-null.
        : d_data()
        , d_dispenser_p(protectedDispenser)
        {
            d_data   = d_dispenser_p->allocate(initialSize * sizeof(int));
            ASSERT(!d_data.isNull());
            d_cursor_p = (int *)d_data.address();
            d_dispenser_p->protect(d_data);
        }
//..
// We must unprotect the dispenser before modifying or deallocating
// memory:
//..
        ~IntegerStack()
            // Destroy this object and release its memory.
        {
            d_dispenser_p->unprotect(d_data);
            d_dispenser_p->deallocate(d_data);
        }
  // ...
        // MANIPULATORS
        void push(int value)
            // Push the specified 'value' onto the stack.
        {
            d_dispenser_p->unprotect(d_data);
            if (sizeof(int) >
               (char *)d_data.address() + d_data.size() - (char *)d_cursor_p){
                resize();
            }
            *d_cursor_p++ = value;
            d_dispenser_p->protect(d_data);
        }

        int pop()
            // Remove the top value from the stack and return it.
        {
            // The buffer isn't modified so there is no need to unprotect it.
            const int value = *--d_cursor_p;
            return value;
        }

  // ...
    };
//..
///Usage 3 -- Implementing the 'bdema_ProtectableBlockDispenser' Protocol
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we create a dummy implementation of the
// 'bdema_ProtectableBlockDispenser' protocol.  In order to avoid
// platform-specific dependencies, the 'protect' and 'unprotect' methods do not
// provide access protection.  Along with each block of memory returned by this
// dispenser, we maintain a header that tracks the memory's size, allocation
// state, and protection state:
//..
//    // dummyprotectablebufferblockdispenser.h

      class DummyProtectableBufferBlockDispenser :
                                       public bdema_ProtectableBlockDispenser {
          // This class returns portions of memory from a single buffer
          // supplied at construction.

          // DATA
          char      *d_buffer;    // start of the memory block
          char      *d_cursor;    // next free byte
          const int  d_size;      // size of the buffer
          const int  d_pageSize;  // size of the blocks

        public:
          // CREATORS
          DummyProtectableBufferBlockDispenser(char *buffer,
                                              int   bufferSize,
                                              int   pageSize);
              // Create a 'DummyProtectableBufferBlockdispenser' on the
              // specified 'buffer' of the specified 'bufferSize' using the
              // specified 'pageSize.

          ~DummyProtectableBufferBlockDispenser();
              // Destroy this 'DummyProtectableBufferBlockdispenser'.

          // MANIPULATORS
          bdema_MemoryBlockDescriptor allocate(size_type size);
              // Return a descriptor for a newly-allocated block of memory of
              // at least the specified 'size'.  If 'size' is 0, a null block
              // descriptor is returned with no other effect.  If this
              // dispenser cannot return the requested number of bytes, then it
              // will throw a 'std::bad_alloc' exception in an
              // exception-enabled build, or else will abort the program in a
              // non-exception build.  The behavior is undefined unless
              // 'size >= 0'.

          virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
              // This operations has no effect for this dispenser.  If it can
              // be determined that the specified 'block' was not returned by
              // this dispenser then write an error to the console and abort.

          virtual int protect(const bdema_MemoryBlockDescriptor& block);
              // Protect the memory described by the specified 'block'.  To
              // avoid platform dependencies, no access protection is applied
              // to the internal buffer, only the header for the 'block' is
              // updated to reflect its new state.  If it can be determined
              // that 'block' was not returned by this dispenser, then write an
              // error to the console and abort.

          virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
              // Unprotect the memory described by the specified 'block'.  To
              // avoid platform dependencies, access protection is not applied
              // to the internal buffer, only the header for the 'block' is
              // updated to reflect its new state.  If it can be determined
              // that 'block' was not returned by this dispenser, then write an
              // error to the console and abort.

          // ACCESSORS
          virtual int minimumBlockSize() const;
              // Return the minimum size of a block returned by this allocator.
              // This value indicates the size of the block that would be
              // returned if 'allocate(1)' were called.
              //
              // This implementation returns the 'pageSize' supplied at
              // construction.

          bool isProtected(const bdema_MemoryBlockDescriptor& block) const;
              // Return the current protection status of this dispenser.

          // ...

          void print(const bdema_MemoryBlockDescriptor& block) const;
              // Prints the header information plus the data from the specified
              // block.

          void print() const;
              // Prints the buffer supplied on construction.
      };

      // dummyprotectablebufferblockdispenser.cpp
      // #include <dummyprotectablebufferblockdispenser.h>

      void die(bool condition, const char *stmnt, const char *file, int line)
          // If the specified 'condition' is 'false', write an error message
          // with the specified 'stmnt', 'file', and 'line' to the console
          // and abort the program.  This operation has no effect if
          // 'condition' is 'true'.
      {
          if (condition) {
              bsl::cout << "Abort:" << stmnt << "  " << file << ":" << line
                        << bsl::endl;
              std::exit(-1);
          }
      }

      // The 'VERIFY' macro is similar to 'ASSERT', but does not depend on the
      // build target being an DEBUG build.
      #define VERIFY(X) { die( !(X), #X, __FILE__, __LINE__); }

      namespace {

          struct Header {
              // This structure is used to keep track of the state of an
              // allocated block of memory.

              char d_allocated;
              char d_protected;
              int  d_size;
          };

          union Align {
              // This structure is used to ensure the alignment of the header
              // information.

              Header                             d_header;
              bsls_AlignmentUtil::MaxAlignedType d_align;
          };

          // The following constants are used for the header bytes.
          const char READONLY_FLAG      = 0xDD; // marked as read only
          const char READWRITE_FLAG     = 0XCC; // marked as read write
          const char ALLOCATED_FLAG     = 0xAA; // marked as allocated
          const char DEALLOCATED_FLAG   = 0xBB; // marked as freed

          // The following constants are used to tag memory in the buffer.
          const char UNUSED_BUFFER_BYTE = 0XEE; // an unused buffer byte
          const char HEADPADDING_BYTE   = 0x11; // padding for a 'Header'
          const char UNINITIALIZED_BYTE = 0xFF; // newly allocated memory
          const char FREED_BYTE         = 0xA5; // freed memory
      }

      // CREATORS
      DummyProtectableBufferBlockDispenser::
      DummyProtectableBufferBlockDispenser(char *buffer,
                                           int   bufferSize,
                                           int   pageSize)
      : d_buffer(buffer)
      , d_cursor(buffer)
      , d_size(bufferSize)
      , d_pageSize(pageSize)
      {
          std::memset(buffer, UNUSED_BUFFER_BYTE, bufferSize);
          d_cursor += bsls_AlignmentUtil::calculateAlignmentOffset(
                                       d_cursor,
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
      }

      DummyProtectableBufferBlockDispenser::
      ~DummyProtectableBufferBlockDispenser()
      {
      }

      // MANIPULATORS
      bdema_MemoryBlockDescriptor
      DummyProtectableBufferBlockDispenser::allocate(size_type size)
      {
          const int numPages       = (size + d_pageSize - 1) / d_pageSize;
          const int actualSize     = numPages * d_pageSize;
          const int sizeWithHeader = actualSize + sizeof(Align);

          if (0 == size) {
              return bdema_MemoryBlockDescriptor();
          }
          if (d_cursor + sizeWithHeader > d_buffer + d_size) {
              // Not enough free space in the buffer
              bslma_Allocator::throwBadAlloc();
          }

          char *cursor = d_cursor;

          // Mark the bits to help with debugging
          std::memset(cursor, HEADPADDING_BYTE, sizeof(Align));
          std::memset(cursor + sizeof(Align), UNINITIALIZED_BYTE, actualSize);

          // Initialize the header for the block we will return
          ((Align *)cursor)->d_header.d_allocated = ALLOCATED_FLAG;
          ((Align *)cursor)->d_header.d_protected = READWRITE_FLAG;
          ((Align *)cursor)->d_header.d_size      = actualSize;

          // update the cursor to the next empty buffer location and then
          // ensure the proper alignment of the next position.
          d_cursor += sizeWithHeader;
          d_cursor += bsls_AlignmentUtil::calculateAlignmentOffset(
                                       d_cursor,
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);

          // Return the block of memory starting after the header.
          return bdema_MemoryBlockDescriptor(
                                           cursor + sizeof(Align), actualSize);
      }

      void DummyProtectableBufferBlockDispenser::deallocate(
                                      const bdema_MemoryBlockDescriptor& block)
      {
          Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
          VERIFY(align->d_header.d_allocated == ALLOCATED_FLAG);
          VERIFY(align->d_header.d_size      == block.size());
          VERIFY(align->d_header.d_protected == READWRITE_FLAG);

          align->d_header.d_allocated = DEALLOCATED_FLAG;
          std::memset(block.address(), FREED_BYTE, block.size());
      }
//..
// In order to avoid any system dependencies, implement 'protect' and
// 'unprotect' without providing system level access protection:
//..
      int DummyProtectableBufferBlockDispenser::protect(
                                      const bdema_MemoryBlockDescriptor& block)
      {
          Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
          VERIFY(align->d_header.d_allocated == ALLOCATED_FLAG);
          VERIFY(align->d_header.d_size      == block.size());
          VERIFY(align->d_header.d_protected == READWRITE_FLAG ||
                 align->d_header.d_protected == READONLY_FLAG);

          align->d_header.d_protected = READONLY_FLAG;
          return 0;
      }

      int DummyProtectableBufferBlockDispenser::unprotect(
                                      const bdema_MemoryBlockDescriptor& block)
      {
          Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
          VERIFY(align->d_header.d_allocated == ALLOCATED_FLAG);
          VERIFY(align->d_header.d_size      == block.size());
          VERIFY(align->d_header.d_protected == READWRITE_FLAG ||
                 align->d_header.d_protected == READONLY_FLAG);
          align->d_header.d_protected = READWRITE_FLAG;
          return 0;
      }

      // ACCESSORS
      int DummyProtectableBufferBlockDispenser::minimumBlockSize() const
      {
          return d_pageSize;
      }

      bool DummyProtectableBufferBlockDispenser::isProtected(
                                const bdema_MemoryBlockDescriptor& block) const
      {
          // We use the by address before the returned block to track the
          // protection status.
          Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
          VERIFY(align->d_header.d_allocated == ALLOCATED_FLAG);
          VERIFY(align->d_header.d_size      == block.size());
          VERIFY(align->d_header.d_protected == READWRITE_FLAG ||
                 align->d_header.d_protected == READONLY_FLAG);
          return align->d_header.d_protected == READONLY_FLAG;
      }

      // ...
//..
      void DummyProtectableBufferBlockDispenser::print(
                  const bdema_MemoryBlockDescriptor& block) const
      {
          void *align = ((char *)block.address()) - sizeof(Align);
          std::printf("Header --------\n");
          printBytes(align, sizeof(Align));
          std::printf("Data ----------\n");
          printBytes(block.address(), block.size());
      }

      void DummyProtectableBufferBlockDispenser::print() const
      {
          std::printf("Full Buffer -----\n");
          printBytes(d_buffer, d_size);
      }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator         talloc;
    bslma_DefaultAllocatorGuard guard(&talloc);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE"
                          << endl << "============="
                          << endl;

        {
            char                         buffer[200];
            DummyProtectableBufferBlockDispenser dispenser(buffer, 200, 10);
            bdema_MemoryBlockDescriptor desc;

            desc = dispenser.allocate(1);
            ASSERT(!dispenser.isProtected(desc));
            ASSERT(0 == dispenser.protect(desc));
            ASSERT(dispenser.isProtected(desc));
            ASSERT(0 == dispenser.unprotect(desc));
            ASSERT(!dispenser.isProtected(desc));
            dispenser.deallocate(desc);

            doImportantStuff(&dispenser);
            IntegerStack stack(1, &dispenser);
            stack.push(10);
            stack.push(5);
            stack.push(9);

            ASSERT(9  == stack.pop());
            ASSERT(5  == stack.pop());
            ASSERT(10 == stack.pop());
            if (veryVerbose) {
                dispenser.print();
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VIRTUAL METHODS
        //
        // Concerns:
        //   That the virtual methods of the abstract base false
        //   bdem_ProtectableBlockDispenser will call the implemented
        //   methods of concrete implementations.
        //
        // Plan:
        //  Construct a NoopProtectedBlockAllcoator.  Obtain a
        //  'bdema_ProtectableBlockDispenser' reference to it and call
        //  the various methods on the 'bdema_ProtectableBlockDispenser'
        //  reference.  Check the 'lastMethod()' to verify the method was
        //  called.  Ensure that the destructor is called explicitly on the
        //  base class.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Verify: Virtual Methods"
                          << endl << "======================="
                          << endl;

        const char *lastMethod;
        NoopProtectableBlockDispenser  *dummyPtr =
            new (talloc) NoopProtectableBlockDispenser(&lastMethod);
        bdema_ProtectableBlockDispenser *paPtr = dummyPtr;
        ASSERT( 0 == std::strcmp("NoopProtectableBlockDispenser",
                                 lastMethod));

        paPtr->minimumBlockSize();
        ASSERT(0 == std::strcmp("minimumBlockSize", lastMethod));

        bdema_MemoryBlockDescriptor desc;
        desc = paPtr->allocate(1);
        ASSERT(0 == std::strcmp("allocate", lastMethod));

        paPtr->protect(desc);
        ASSERT(0 == std::strcmp("protect", lastMethod));

        paPtr->unprotect(desc);
        ASSERT(0 == std::strcmp("unprotect", lastMethod));

        paPtr->deallocate(desc);
        ASSERT(0 == std::strcmp("deallocate", lastMethod));

        talloc.deleteObjectRaw(paPtr);
        ASSERT(0 == std::strcmp("~NoopProtectableBlockDispenser",
                                lastMethod));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
