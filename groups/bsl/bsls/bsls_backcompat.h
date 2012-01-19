// bsls_backcompat.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLS_BACKCOMPAT
#define INCLUDED_BSLS_BACKCOMPAT

#if defined(__cplusplus)

#if defined(__xlC__) || defined(__IBMC__) || defined(__IBMCPP__)
#define BSLS_BACKCOMPAT_IBM
#endif

// protect ourselves from evil code
#if defined(bool)
#define BSLS_BACKCOMPAT_BOOL_MACRO bool
#undef bool
#endif

#if defined(class)
#define BSLS_BACKCOMPAT_CLASS_MACRO class
#undef class
#endif

extern "C++" {

namespace BloombergLP {

namespace bslalg {
struct ArrayDestructionPrimitives;
}
typedef bslalg::ArrayDestructionPrimitives bslalg_ArrayDestructionPrimitives;
#define bslalg_AutoArrayMoveDestructor bslalg::AutoArrayMoveDestructor
namespace bslalg {
template <class OBJECT_TYPE>
class AutoArrayMoveDestructor;
}
#define bslalg_AutoArrayDestructor bslalg::AutoArrayDestructor
namespace bslalg {
template <class OBJECT_TYPE>
class AutoArrayDestructor;
}
#define bslalg_AutoScalarDestructor bslalg::AutoScalarDestructor
namespace bslalg {
template <class OBJECT_TYPE>
class AutoScalarDestructor;
}
#define bslalg_BitwiseEqPassthroughTrait bslalg::BitwiseEqPassthroughTrait
namespace bslalg {
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct BitwiseEqPassthroughTrait;
}
#define bslalg_DequeImpUtil bslalg::DequeImpUtil
namespace bslalg {
template <class VALUE_TYPE, int BLOCK_LENGTH>
struct DequeImpUtil;
}
#define bslalg_ConstructorProxy bslalg::ConstructorProxy
namespace bslalg {
template <typename OBJECT_TYPE>
class ConstructorProxy;
}
#define bslalg_DequeIterator bslalg::DequeIterator
namespace bslalg {
template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequeIterator;
}
#define bslalg_HasTrait bslalg::HasTrait
namespace bslalg {
template <typename TYPE, typename TRAIT>
struct HasTrait;
}
#define bslalg_PassthroughTraitBslmaAllocator bslalg::PassthroughTraitBslmaAllocator
namespace bslalg {
template <typename ALLOCATOR>
struct PassthroughTraitBslmaAllocator;
}
#define bslalg_PassthroughTrait bslalg::PassthroughTrait
namespace bslalg {
template <typename T, typename TRAIT>
struct PassthroughTrait;
}
namespace bslalg {
struct ScalarDestructionPrimitives;
}
typedef bslalg::ScalarDestructionPrimitives bslalg_ScalarDestructionPrimitives;
namespace bslalg {
struct RangeCompare;
}
typedef bslalg::RangeCompare bslalg_RangeCompare;
namespace bslalg {
struct ArrayPrimitives;
}
typedef bslalg::ArrayPrimitives bslalg_ArrayPrimitives;
#define bslalg_DequePrimitives bslalg::DequePrimitives
namespace bslalg {
template <class VALUE_TYPE, int BLOCK_LENGTH>
struct DequePrimitives;
}
#define bslalg_SelectTrait bslalg::SelectTrait
namespace bslalg {
template <typename TYPE, typename TRAIT1, typename TRAIT2, typename TRAIT3, typename TRAIT4, typename TRAIT5>
struct SelectTrait;
}
namespace bslalg {
class SwapUtil;
}
typedef bslalg::SwapUtil bslalg_SwapUtil;
namespace bslalg {
struct TypeTraitBitwiseCopyable;
}
typedef bslalg::TypeTraitBitwiseCopyable bslalg_TypeTraitBitwiseCopyable;
namespace bslalg {
struct TypeTraitBitwiseEqualityComparable;
}
typedef bslalg::TypeTraitBitwiseEqualityComparable bslalg_TypeTraitBitwiseEqualityComparable;
namespace bslalg {
struct TypeTraitBitwiseMoveable;
}
typedef bslalg::TypeTraitBitwiseMoveable bslalg_TypeTraitBitwiseMoveable;
namespace bslalg {
struct TypeTraitHasPointerSemantics;
}
typedef bslalg::TypeTraitHasPointerSemantics bslalg_TypeTraitHasPointerSemantics;
namespace bslalg {
struct TypeTraitHasStlIterators;
}
typedef bslalg::TypeTraitHasStlIterators bslalg_TypeTraitHasStlIterators;
namespace bslalg {
struct TypeTraitHasTrivialDefaultConstructor;
}
typedef bslalg::TypeTraitHasTrivialDefaultConstructor bslalg_TypeTraitHasTrivialDefaultConstructor;
#define bslalg_TypeTraitNil bslalg::TypeTraitNil
namespace bslalg {
struct TypeTraitPair;
}
typedef bslalg::TypeTraitPair bslalg_TypeTraitPair;
namespace bslalg {
struct TypeTraitsGroupPod;
}
typedef bslalg::TypeTraitsGroupPod bslalg_TypeTraitsGroupPod;
namespace bslalg {
struct ScalarPrimitives;
}
typedef bslalg::ScalarPrimitives bslalg_ScalarPrimitives;
#define bslalg_TypeTraits_NotTrait bslalg::TypeTraits_NotTrait
namespace bslalg {
template <typename TRAIT> struct TypeTraits_NotTrait;
}
#define bslalg_TypeTraitsGroupStlOrdered bslalg::TypeTraitsGroupStlOrdered
namespace bslalg {
template <typename T, typename COMP, typename ALLOCATOR>
struct TypeTraitsGroupStlOrdered;
}
#define bslalg_TypeTraitsGroupStlSequence bslalg::TypeTraitsGroupStlSequence
namespace bslalg {
template <typename T, typename ALLOCATOR>
struct TypeTraitsGroupStlSequence;
}
#define bslalg_TypeTraitsGroupStlUnordered bslalg::TypeTraitsGroupStlUnordered
namespace bslalg {
template <typename T, typename HASH, typename EQ, typename ALLOCATOR>
struct TypeTraitsGroupStlUnordered;
}

namespace bslalg {
struct TypeTraitUsesBslmaAllocator;
}
#if defined(BSLS_BACKCOMPAT_IBM)
    typedef bslalg::TypeTraitUsesBslmaAllocator bslalg_TypeTraitUsesBslmaAllocator;
#else
#   define bslalg_TypeTraitUsesBslmaAllocator bslalg::TypeTraitUsesBslmaAllocator
#endif

#define bslma_AutoDeallocator bslma::AutoDeallocator
namespace bslma {
template <class ALLOCATOR>
class AutoDeallocator;
}
#define bslma_AutoDestructor bslma::AutoDestructor
namespace bslma {
template <class TYPE>
class AutoDestructor;
}
namespace bslma {
class BufferAllocator;
}
typedef bslma::BufferAllocator bslma_BufferAllocator;
#define bslma_AutoRawDeleter bslma::AutoRawDeleter
namespace bslma {
template <class TYPE, class ALLOCATOR>
class AutoRawDeleter;
}
#define bslma_DeallocatorGuard bslma::DeallocatorGuard
namespace bslma {
template <class ALLOCATOR>
class DeallocatorGuard;
}
#define bslma_DeallocatorProctor bslma::DeallocatorProctor
namespace bslma {
template <class ALLOCATOR>
class DeallocatorProctor;
}
namespace bslma {
class DefaultAllocatorGuard;
}
typedef bslma::DefaultAllocatorGuard bslma_DefaultAllocatorGuard;
namespace bslma {
struct DeleterHelper;
}
typedef bslma::DeleterHelper bslma_DeleterHelper;
#define bslma_DestructorGuard bslma::DestructorGuard
namespace bslma {
template <class TYPE>
class DestructorGuard;
}
namespace bslma {
class InfrequentDeleteBlockList;
}
typedef bslma::InfrequentDeleteBlockList bslma_InfrequentDeleteBlockList;
#define bslma_DestructorProctor bslma::DestructorProctor
namespace bslma {
template <class TYPE>
class DestructorProctor;
}
namespace bslma {
class ManagedAllocator;
}
typedef bslma::ManagedAllocator bslma_ManagedAllocator;
namespace bslma {
class MallocFreeAllocator;
}
typedef bslma::MallocFreeAllocator bslma_MallocFreeAllocator;

namespace bslma {
class Allocator;
}
#if defined(BSLS_BACKCOMPAT_IBM)
    typedef bslma::Allocator bslma_Allocator;
    typedef bslma::Allocator bdema_Allocator;
#else
#   define bslma_Allocator bslma::Allocator
#   define bdema_Allocator bslma::Allocator
#endif

#define bslma_RawDeleterGuard bslma::RawDeleterGuard
namespace bslma {
template <class TYPE, class ALLOCATOR>
class RawDeleterGuard;
}
#define bslma_RawDeleterProctor bslma::RawDeleterProctor
namespace bslma {
template <class TYPE, class ALLOCATOR>
class RawDeleterProctor;
}
namespace bslma {
class NewDeleteAllocator;
}
typedef bslma::NewDeleteAllocator bslma_NewDeleteAllocator;
namespace bslma {
struct Default;
}
typedef bslma::Default bslma_Default;

namespace bslma {
class SequentialAllocator;
}
#if defined(BSLS_BACKCOMPAT_IBM)
    typedef bslma::SequentialAllocator bslma_SequentialAllocator;
#else
#   define bslma_SequentialAllocator bslma::SequentialAllocator
#endif

namespace bslma {
class TestAllocatorException;
}
typedef bslma::TestAllocatorException bslma_TestAllocatorException;
namespace bslma {
class TestAllocator;
}
typedef bslma::TestAllocator bslma_TestAllocator;
#define bslmf_AddReference bslmf::AddReference
namespace bslmf {
template <class BSLMF_TYPE>
struct AddReference;
}
#define bslmf_TypeRep bslmf::TypeRep
namespace bslmf {
template <typename TYPE>
struct TypeRep;
}
namespace bslmf {
struct AnyType;
}
typedef bslmf::AnyType bslmf_AnyType;
#define bslmf_ArrayToConstPointer bslmf::ArrayToConstPointer
namespace bslmf {
template <typename TYPE>
struct ArrayToConstPointer;
}
#define bslmf_ArrayToPointer bslmf::ArrayToPointer
namespace bslmf {
template <typename TYPE>
struct ArrayToPointer;
}
#define bslmf_AssertTest bslmf::AssertTest
namespace bslmf {
template <int INTEGER>
struct AssertTest;
}
#define bslmf_ConstForwardingType bslmf::ConstForwardingType
namespace bslmf {
template <typename TYPE>
struct ConstForwardingType;
}
#define bslmf_ForwardingType bslmf::ForwardingType
namespace bslmf {
template <typename TYPE>
struct ForwardingType;
}
namespace bslma {
class TestAllocatorMonitor;
}
typedef bslma::TestAllocatorMonitor bslma_TestAllocatorMonitor;
namespace bslmf {
struct FunctionPointerCPlusPlusLinkage;
}
typedef bslmf::FunctionPointerCPlusPlusLinkage bslmf_FunctionPointerCPlusPlusLinkage;
namespace bslmf {
struct FunctionPointerCLinkage;
}
typedef bslmf::FunctionPointerCLinkage bslmf_FunctionPointerCLinkage;
#define bslmf_FunctionPointerTraits bslmf::FunctionPointerTraits
namespace bslmf {
template <class PROTOTYPE>
struct FunctionPointerTraits;
}
#define bslmf_IsFunctionPointer bslmf::IsFunctionPointer
namespace bslmf {
template <class PROTOTYPE>
struct IsFunctionPointer;
}
#define bslmf_If bslmf::If
namespace bslmf {
template <int   CONDITION, class IF_TRUE_TYPE, class IF_FALSE_TYPE>
struct If;
}
#define bslmf_IsArray bslmf::IsArray
namespace bslmf {
template <typename TYPE>
struct IsArray;
}
#define bslmf_IsClass bslmf::IsClass
namespace bslmf {
template <typename TYPE>
struct IsClass;
}
namespace bslma {
class SequentialPool;
}
typedef bslma::SequentialPool bslma_SequentialPool;
#define bslmf_IsEnum bslmf::IsEnum
namespace bslmf {
template <class TYPE>
struct IsEnum;
}
#define bslmf_IsFundamental bslmf::IsFundamental
namespace bslmf {
template <typename TYPE>
struct IsFundamental;
}
#define bslmf_IsPointer bslmf::IsPointer
namespace bslmf {
template <typename T>
struct IsPointer;
}
#define bslmf_EnableIf bslmf::EnableIf
namespace bslmf {
template <bool BSLMA_CONDITION, class BSLMA_TYPE>
struct EnableIf;
}
#define bslmf_IsPolymorphic bslmf::IsPolymorphic
namespace bslmf {
template <typename TYPE>
struct IsPolymorphic;
}
#define bslmf_IsPointerToMember bslmf::IsPointerToMember
namespace bslmf {
template <typename TYPE>
struct IsPointerToMember;
}
#define bslmf_IsPointerToMemberFunction bslmf::IsPointerToMemberFunction
namespace bslmf {
template <typename TYPE>
struct IsPointerToMemberFunction;
}
#define bslmf_IsPointerToMemberData bslmf::IsPointerToMemberData
namespace bslmf {
template <typename TYPE>
struct IsPointerToMemberData;
}
#define bslmf_IsSame bslmf::IsSame
namespace bslmf {
template <typename U, typename V>
struct IsSame;
}
#define bslmf_MetaInt bslmf::MetaInt
namespace bslmf {
template <unsigned INT_VALUE>
struct MetaInt;
}
#define bslmf_IsNil bslmf::IsNil
namespace bslmf {
template <class T>
struct IsNil;
}

namespace bslmf {
struct Nil;
}
#if defined(BSLS_BACKCOMPAT_IBM)
    typedef bslmf::Nil bslmf_Nil;
#else
#   define bslmf_Nil bslmf::Nil
#endif

#define bslmf_RemoveCvq bslmf::RemoveCvq
namespace bslmf {
template <typename T>
struct RemoveCvq;
}
#define bslmf_RemovePtrCvq bslmf::RemovePtrCvq
namespace bslmf {
template <typename T>
struct RemovePtrCvq;
}
#define bslmf_MemberFunctionPointerTraits bslmf::MemberFunctionPointerTraits
namespace bslmf {
template <class PROTOTYPE>
struct MemberFunctionPointerTraits;
}
#define bslmf_IsMemberFunctionPointer bslmf::IsMemberFunctionPointer
namespace bslmf {
template <class PROTOTYPE>
struct IsMemberFunctionPointer;
}
#define bslmf_MemberFunctionPointerTraitsImp bslmf::MemberFunctionPointerTraitsImp
namespace bslmf {
template <class PROTOTYPE, class TEST_PROTOTYPE>
struct MemberFunctionPointerTraitsImp;
}
#define bslmf_RemoveReference bslmf::RemoveReference
namespace bslmf {
template <typename TYPE>
struct RemoveReference;
}
#define bslmf_Tag bslmf::Tag
namespace bslmf {
template <unsigned N>
struct Tag;
}
#define bsls_AlignedBuffer bsls::AlignedBuffer
namespace bsls {
template <int SIZE, int ALIGNMENT>
union AlignedBuffer;
}
#define bslmf_TypeList14 bslmf::TypeList14
namespace bslmf {
template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
struct TypeList14;
}
#define bslmf_TypeListTypeOf bslmf::TypeListTypeOf
namespace bslmf {
template <int INDEX, class LIST, class DEFAULTTYPE, int INRANGE>
struct TypeListTypeOf;
}
#define bslmf_TypeList13 bslmf::TypeList13
namespace bslmf {
template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
struct TypeList13;
}
#define bslmf_TypeList4 bslmf::TypeList4
namespace bslmf {
template <class A1, class A2, class A3, class A4>
struct TypeList4;
}
#define bslmf_TypeList12 bslmf::TypeList12
namespace bslmf {
template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
struct TypeList12;
}
#define bslmf_TypeList18 bslmf::TypeList18
namespace bslmf {
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
struct TypeList18;
}
#define bslmf_TypeList3 bslmf::TypeList3
namespace bslmf {
template <class A1, class A2, class A3>
struct TypeList3;
}
#define bslmf_TypeList15 bslmf::TypeList15
namespace bslmf {
template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
struct TypeList15;
}
#define bslmf_TypeList6 bslmf::TypeList6
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5, class A6>
struct TypeList6;
}
#define bslmf_TypeList7 bslmf::TypeList7
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
struct TypeList7;
}
#define bslmf_TypeList10 bslmf::TypeList10
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
struct TypeList10;
}
#define bslmf_TypeList20 bslmf::TypeList20
namespace bslmf {

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
struct TypeList20;
}
#define bslmf_TypeList17 bslmf::TypeList17
namespace bslmf {
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
struct TypeList17;
}
#define bslmf_TypeList1 bslmf::TypeList1
namespace bslmf {
template <class A1>
struct TypeList1;
}
#define bslmf_TypeList11 bslmf::TypeList11
namespace bslmf {
template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
struct TypeList11;
}
namespace bslmf {
struct TypeList0;
}
typedef bslmf::TypeList0 bslmf_TypeList0;
#define bslmf_TypeList9 bslmf::TypeList9
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
struct TypeList9;
}
#define bslmf_TypeList8 bslmf::TypeList8
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
struct TypeList8;
}
#define bslmf_TypeList5 bslmf::TypeList5
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5>
struct TypeList5;
}
#define bslmf_TypeList bslmf::TypeList
namespace bslmf {
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16, class A17, class A18, class A19, class A20>
struct TypeList;
}
#define bslmf_TypeList19 bslmf::TypeList19
namespace bslmf {

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
struct TypeList19;
}
#define bslmf_TypeList16 bslmf::TypeList16
namespace bslmf {
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16 >
struct TypeList16;
}
#define bslmf_TypeList2 bslmf::TypeList2
namespace bslmf {
template <class A1, class A2>
struct TypeList2;
}
#define bslmf_TypeListNil bslmf::TypeListNil
#define bslmf_Switch3 bslmf::Switch3
namespace bslmf {
template <int SELECTOR, typename T0, typename T1, typename T2>
struct Switch3;
}
#define bslmf_Switch7 bslmf::Switch7
namespace bslmf {

template <int      SELECTOR,
          typename T0,
          typename T1,
          typename T2,
          typename T3,
          typename T4,
          typename T5,
          typename T6>
struct Switch7;
}
#define bslmf_Switch2 bslmf::Switch2
namespace bslmf {
template <int SELECTOR, typename T0, typename T1>
struct Switch2;
}
#define bslmf_Switch bslmf::Switch
namespace bslmf {
template <int      SELECTOR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct Switch;
}
#define bslmf_Switch9 bslmf::Switch9
namespace bslmf {

template <int      SELECTOR,
          typename T0,
          typename T1,
          typename T2,
          typename T3,
          typename T4,
          typename T5,
          typename T6,
          typename T7,
          typename T8>
struct Switch9;
}
#define bslmf_Switch8 bslmf::Switch8
namespace bslmf {

template <int      SELECTOR,
          typename T0,
          typename T1,
          typename T2,
          typename T3,
          typename T4,
          typename T5,
          typename T6,
          typename T7>
struct Switch8;
}
#define bslmf_Switch5 bslmf::Switch5
namespace bslmf {

template <int      SELECTOR,
          typename T0,
          typename T1,
          typename T2,
          typename T3,
          typename T4>
struct Switch5;
}
#define bslmf_Switch6 bslmf::Switch6
namespace bslmf {

template <int      SELECTOR,
          typename T0,
          typename T1,
          typename T2,
          typename T3,
          typename T4,
          typename T5>
struct Switch6;
}
#define bslmf_Switch4 bslmf::Switch4
namespace bslmf {
template <int SELECTOR, typename T0, typename T1, typename T2, typename T3>
struct Switch4;
}
namespace bsls {
struct Alignment;
}
typedef bsls::Alignment bsls_Alignment;
namespace bsls {
struct AlignmentImp8ByteAlignedType;
}
typedef bsls::AlignmentImp8ByteAlignedType bsls_AlignmentImp8ByteAlignedType;
#define bsls_AlignmentImpTag bsls::AlignmentImpTag
namespace bsls {
template <int SIZE>
struct AlignmentImpTag;
}
#define bsls_AlignmentImpPriorityToType bsls::AlignmentImpPriorityToType
namespace bsls {
template <int PRIORITY>
struct AlignmentImpPriorityToType;
}
namespace bsls {
struct AlignmentImpMatch;
}
typedef bsls::AlignmentImpMatch bsls_AlignmentImpMatch;
#define bsls_AlignmentImpCalc bsls::AlignmentImpCalc
namespace bsls {
template <typename TYPE>
struct AlignmentImpCalc;
}
#define bsls_AlignmentFromType bsls::AlignmentFromType
namespace bsls {
template <typename TYPE>
struct AlignmentFromType;
}
#define bsls_AlignmentToType bsls::AlignmentToType
namespace bsls {
template <int ALIGNMENT>
struct AlignmentToType;
}
namespace bsls {
struct AlignmentUtil;
}
typedef bsls::AlignmentUtil bsls_AlignmentUtil;
namespace bsls {
class AssertTestException;
}
typedef bsls::AssertTestException bsls_AssertTestException;
namespace bsls {
struct BlockGrowth;
}
typedef bsls::BlockGrowth bsls_BlockGrowth;
namespace bsls {
struct NoMtBuildTarget;
}
typedef bsls::NoMtBuildTarget bsls_NoMtBuildTarget;
namespace bsls {
struct Yes64BitBuildTarget;
}
typedef bsls::Yes64BitBuildTarget bsls_Yes64BitBuildTarget;
namespace bsls {
struct NoExcBuildTarget;
}
typedef bsls::NoExcBuildTarget bsls_NoExcBuildTarget;
namespace bsls {
struct YesMtBuildTarget;
}
typedef bsls::YesMtBuildTarget bsls_YesMtBuildTarget;
namespace bsls {
struct YesExcBuildTarget;
}
typedef bsls::YesExcBuildTarget bsls_YesExcBuildTarget;
namespace bsls {
struct No64BitBuildTarget;
}
typedef bsls::No64BitBuildTarget bsls_No64BitBuildTarget;
#define bsls_ExcBuildTarget bsls::ExcBuildTarget
#define bsls_MtBuildTarget bsls::MtBuildTarget
namespace bsls {
class AssertTestHandlerGuard;
}
typedef bsls::AssertTestHandlerGuard bsls_AssertTestHandlerGuard;
namespace bsls {
struct AssertTest;
}
typedef bsls::AssertTest bsls_AssertTest;
#define bsls_ObjectBuffer bsls::ObjectBuffer
namespace bsls {
template <typename TYPE>
union ObjectBuffer;
}
#define bslmf_IsConvertible bslmf::IsConvertible
namespace bslmf {
template <typename FROM_TYPE, typename TO_TYPE>
struct IsConvertible;
}

#if defined(BSLS_BACKCOMPAT_IBM)
#define bslmf_IsConvertible_Imp bslmf::IsConvertible_Imp
#endif

namespace bsls {
struct PerformanceHint;
}
typedef bsls::PerformanceHint bsls_PerformanceHint;
namespace bsls {
struct Platform;
}
typedef bsls::Platform bsls_Platform;
#define bdes_Platform bdes::Platform
namespace bsls {
struct PlatformUtil;
}
typedef bsls::PlatformUtil bsls_PlatformUtil;
namespace bsls {
class Stopwatch;
}
typedef bsls::Stopwatch bsls_Stopwatch;
#define bsls_ProtocolTest bsls::ProtocolTest
namespace bsls {
template <class BSLS_TESTIMP>
class ProtocolTest;
}
#define bsls_ProtocolTestImp bsls::ProtocolTestImp
namespace bsls {
template <class BSLS_PROTOCOL>
class ProtocolTestImp;
}
namespace bsls {
struct TimeUtil;
}
typedef bsls::TimeUtil bsls_TimeUtil;
namespace bsls {
struct Types;
}
typedef bsls::Types bsls_Types;
#define bsls_UnspecifiedBool bsls::UnspecifiedBool
namespace bsls {
template<class BSLS_HOST_TYPE>
class UnspecifiedBool;
}
namespace bslscm {
struct Version;
}
typedef bslscm::Version bslscm_Version;
namespace bsls {
struct Util;
}
typedef bsls::Util bsls_Util;
#define bslstl_AllocatorProxy bslstl::AllocatorProxy
namespace bslstl {
template <class ALLOCATOR>
class AllocatorProxy;
}
#define bslstl_AllocatorProxyBslmaBase bslstl::AllocatorProxyBslmaBase
namespace bslstl {
template <class ALLOCATOR>
class AllocatorProxyBslmaBase;
}
#define bslstl_AllocatorProxyNonBslmaBase bslstl::AllocatorProxyNonBslmaBase
namespace bslstl {
template <class ALLOCATOR>
class AllocatorProxyNonBslmaBase;
}
#define bslstl_BidirectionalIterator bslstl::BidirectionalIterator
namespace bslstl {
template <typename T, typename ITER_IMP, typename TAG_TYPE>
class BidirectionalIterator;
}
#define bslstl_ForwardIterator bslstl::ForwardIterator
namespace bslstl {
template <typename T, typename ITER_IMP, typename TAG_TYPE>
class ForwardIterator;
}
#define bslstl_RandomAccessIterator bslstl::RandomAccessIterator
namespace bslstl {
template <typename T, typename ITER_IMP, typename TAG_TYPE>
class RandomAccessIterator;
}
namespace bslstl {
struct StdExceptUtil;
}
typedef bslstl::StdExceptUtil bslstl_StdExceptUtil;
#define bslstl_TypeTraitsGroupPair bslstl::TypeTraitsGroupPair
namespace bslstl {
template <class T1, class T2>
struct TypeTraitsGroupPair;
}
namespace bsls {
class Assert;
}
typedef bsls::Assert bsls_Assert;
namespace bsls {
class AssertFailureHandlerGuard;
}
typedef bsls::AssertFailureHandlerGuard bsls_AssertFailureHandlerGuard;
#define bdes_AssertFailureHandlerGuard bdes::AssertFailureHandlerGuard
#define bdes_Assert bdes::Assert
#define bslstl_StringRefData bslstl::StringRefData
namespace bslstl {
template <typename CHAR_TYPE>
class StringRefData;
}
namespace bslstl {
struct UtilIterator;
}
typedef bslstl::UtilIterator bslstl_UtilIterator;
namespace bslstl {
class Util;
}
typedef bslstl::Util bslstl_Util;
#define bslstl_StringRefImp bslstl::StringRefImp
namespace bslstl {
template <typename CHAR_TYPE>
class StringRefImp;
}
#define bslstl_StringRefWide bslstl::StringRefWide
#define bslstl_StringRef bslstl::StringRef

}  // close enterprise namespace

}

#if defined(BSLS_BACKCOMPAT_CLASS_MACRO)
#define class BSLS_BACKCOMPAT_CLASS_MACRO
#undef BSLS_BACKCOMPAT_CLASS_MACRO
#endif

#if defined(BSLS_BACKCOMPAT_BOOL_MACRO)
#define bool BSLS_BACKCOMPAT_BOOL_MACRO
#undef BSLS_BACKCOMPAT_BOOL_MACRO
#endif

#if defined(BSLS_BACKCOMPAT_IBM)
#undef BSLS_BACKCOMPAT_IBM
#endif

#endif  // __cplusplus
#endif  // INCLUDED_BSLS_BACKCOMPAT

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
