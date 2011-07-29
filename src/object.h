// Copyright 2011 Google Inc. All Rights Reserved.

#ifndef ART_SRC_OBJECT_H_
#define ART_SRC_OBJECT_H_

#include "constants.h"
#include "casts.h"
#include "globals.h"
#include "heap.h"
#include "logging.h"
#include "macros.h"
#include "offsets.h"
#include "stringpiece.h"
#include "monitor.h"

namespace art {

class Array;
class Class;
class DexCache;
class InstanceField;
class InterfaceEntry;
class Monitor;
class Method;
class Object;
class String;
template<class T> class ObjectArray;
class StaticField;

union JValue {
  uint8_t z;
  int8_t b;
  uint16_t c;
  int16_t s;
  int32_t i;
  int64_t j;
  float f;
  double d;
  Object* l;
};

static const uint32_t kAccPublic = 0x0001;  // class, field, method, ic
static const uint32_t kAccPrivate = 0x0002;  // field, method, ic
static const uint32_t kAccProtected = 0x0004;  // field, method, ic
static const uint32_t kAccStatic = 0x0008;  // field, method, ic
static const uint32_t kAccFinal = 0x0010;  // class, field, method, ic
static const uint32_t kAccSynchronized = 0x0020;  // method (only allowed on natives)
static const uint32_t kAccSuper = 0x0020;  // class (not used in Dalvik)
static const uint32_t kAccVolatile = 0x0040;  // field
static const uint32_t kAccBridge = 0x0040;  // method (1.5)
static const uint32_t kAccTransient = 0x0080;  // field
static const uint32_t kAccVarargs = 0x0080;  // method (1.5)
static const uint32_t kAccNative = 0x0100;  // method
static const uint32_t kAccInterface = 0x0200;  // class, ic
static const uint32_t kAccAbstract = 0x0400;  // class, method, ic
static const uint32_t kAccStrict = 0x0800;  // method
static const uint32_t kAccSynthetic = 0x1000;  // field, method, ic
static const uint32_t kAccAnnotation = 0x2000;  // class, ic (1.5)
static const uint32_t kAccEnum = 0x4000;  // class, field, ic (1.5)

static const uint32_t kAccMiranda = 0x8000;  // method

static const uint32_t kAccJavaFlagsMask = 0xffff;  // bits set from Java sources (low 16)

static const uint32_t kAccConstructor = 0x00010000;  // method (Dalvik only)
static const uint32_t kAccDeclaredSynchronized = 0x00020000;  // method (Dalvik only)

/*
 * Definitions for packing refOffsets in Class.
 */
/*
 * A magic value for refOffsets. Ignore the bits and walk the super
 * chain when this is the value.
 * [This is an unlikely "natural" value, since it would be 30 non-ref instance
 * fields followed by 2 ref instance fields.]
 */
#define CLASS_WALK_SUPER ((unsigned int)(3))
#define CLASS_SMALLEST_OFFSET (sizeof(struct Object))
#define CLASS_BITS_PER_WORD (sizeof(unsigned long int) * 8)
#define CLASS_OFFSET_ALIGNMENT 4
#define CLASS_HIGH_BIT ((unsigned int)1 << (CLASS_BITS_PER_WORD - 1))
/*
 * Given an offset, return the bit number which would encode that offset.
 * Local use only.
 */
#define _CLASS_BIT_NUMBER_FROM_OFFSET(byteOffset) \
    (((unsigned int)(byteOffset) - CLASS_SMALLEST_OFFSET) / \
     CLASS_OFFSET_ALIGNMENT)
/*
 * Is the given offset too large to be encoded?
 */
#define CLASS_CAN_ENCODE_OFFSET(byteOffset) \
    (_CLASS_BIT_NUMBER_FROM_OFFSET(byteOffset) < CLASS_BITS_PER_WORD)
/*
 * Return a single bit, encoding the offset.
 * Undefined if the offset is too large, as defined above.
 */
#define CLASS_BIT_FROM_OFFSET(byteOffset) \
    (CLASS_HIGH_BIT >> _CLASS_BIT_NUMBER_FROM_OFFSET(byteOffset))
/*
 * Return an offset, given a bit number as returned from CLZ.
 */
#define CLASS_OFFSET_FROM_CLZ(rshift) \
   ((static_cast<int>(rshift) * CLASS_OFFSET_ALIGNMENT) + CLASS_SMALLEST_OFFSET)


class Object {
 public:
  static Object* Alloc(Class* klass);

  Class* GetClass() const {
    return klass_;
  }

  void MonitorEnter() {
    monitor_->Enter();
  }

  void MonitorExit() {
    monitor_->Exit();
  }

  void Notify() {
    monitor_->Notify();
  }

  void NotifyAll() {
    monitor_->NotifyAll();
  }

  void Wait() {
    monitor_->Wait();
  }

  void Wait(int64_t timeout) {
    monitor_->Wait(timeout);
  }

  void Wait(int64_t timeout, int32_t nanos) {
    monitor_->Wait(timeout, nanos);
  }

  const Object* GetFieldObject(size_t field_offset) const {
    const byte* raw_addr = reinterpret_cast<const byte*>(this) + field_offset;
    return *reinterpret_cast<Object* const*>(raw_addr);
  }

  Object* GetFieldObject(size_t field_offset) {
    return const_cast<Object*>(GetFieldObject(field_offset));
  }

  void SetFieldObject(size_t offset, Object* new_value) {
    byte* raw_addr = reinterpret_cast<byte*>(this) + offset;
    *reinterpret_cast<Object**>(raw_addr) = new_value;
    // TODO: write barrier
  }

  bool IsClass() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  Class* AsClass() {
    return down_cast<Class*>(this);
  }

  const Class* AsClass() const {
    return down_cast<const Class*>(this);
  }

  bool IsObjectArray() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  const ObjectArray<Object>* AsObjectArray() const {
    return down_cast<const ObjectArray<Object>*>(this);
  }

  bool IsReference() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  bool IsWeakReference() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  bool IsSoftReference() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  bool IsFinalizerReference() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  bool IsPhantomReference() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

  bool IsArray() const {
    LOG(FATAL) << "Unimplemented";
    return true;
  }

 public:
  Class* klass_;

  Monitor* monitor_;

 private:
  Object();
  DISALLOW_COPY_AND_ASSIGN(Object);
};

class ObjectLock {
 public:
  explicit ObjectLock(Object* object) : obj_(object) {
    CHECK(object != NULL);
    obj_->MonitorEnter();
  }

  ~ObjectLock() {
    obj_->MonitorExit();
  }

  void Wait(int64_t millis = 0) {
    return obj_->Wait(millis);
  }

  void Notify() {
    obj_->Notify();
  }

  void NotifyAll() {
    obj_->NotifyAll();
  }

 private:
  Object* obj_;
  DISALLOW_COPY_AND_ASSIGN(ObjectLock);
};

class AccessibleObject : public Object {
 private:
  // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
  uint32_t java_flag_;
};

class Field : public AccessibleObject {
 public:
  Class* GetDeclaringClass() const {
    return declaring_class_;
  }

  const StringPiece& GetName() const {
    return name_;
  }

  char GetType() const {  // TODO: return type
    return descriptor_[0];
  }

  const StringPiece& GetDescriptor() const {
    return descriptor_;
  }

 public:  // TODO: private
  // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
  Class* java_declaring_class_;
  Object* java_generic_type_;
  uint32_t java_generic_types_are_initialized_;
  String* java_name_;
  uint32_t java_slot_;
  Class* java_type_;

  // The class in which this field is declared.
  Class* declaring_class_;

  StringPiece name_;

  // e.g. "I", "[C", "Landroid/os/Debug;"
  StringPiece descriptor_;

  uint32_t access_flags_;

 private:
  Field();
};

// Instance fields.
class InstanceField : public Field {
 public:
  uint32_t GetOffset() const {
    return offset_;
  }

  void SetOffset(size_t num_bytes) {
    offset_ = num_bytes;
  }

 private:
  size_t offset_;
  InstanceField();
};

// Static fields.
class StaticField : public Field {
 public:
  void SetBoolean(bool z) {
    CHECK_EQ(GetType(), 'Z');
    value_.z = z;
  }

  void SetByte(int8_t b) {
    CHECK_EQ(GetType(), 'B');
    value_.b = b;
  }

  void SetChar(uint16_t c) {
    CHECK_EQ(GetType(), 'C');
    value_.c = c;
  }

  void SetShort(uint16_t s) {
    CHECK_EQ(GetType(), 'S');
    value_.s = s;
  }

  void SetInt(int32_t i) {
    CHECK_EQ(GetType(), 'I');
    value_.i = i;
  }

  int64_t GetLong() {
    CHECK_EQ(GetType(), 'J');
    return value_.j;
  }

  void SetLong(int64_t j) {
    CHECK_EQ(GetType(), 'J');
    value_.j = j;
  }

  void SetFloat(float f) {
    CHECK_EQ(GetType(), 'F');
    value_.f = f;
  }

  void SetDouble(double d) {
    CHECK_EQ(GetType(), 'D');
    value_.d = d;
  }

  Object* GetObject() {
    return value_.l;
  }

  const Object* GetObject() const {
    return value_.l;
  }

  void SetObject(Object* l) {
    CHECK(GetType() == 'L' || GetType() == '[');
    value_.l = l;
    // TODO: write barrier
  }

 private:
  JValue value_;
  StaticField();
};

class Method : public AccessibleObject {
 public:
  // Returns the method name, e.g. "<init>" or "eatLunch"
  const StringPiece& GetName() const {
    return name_;
  }

  Class* GetDeclaringClass() const {
    return declaring_class_;
  }

  static MemberOffset ClassOffset() {
    return MemberOffset(OFFSETOF_MEMBER(Method, klass_));
  }

  // const char* GetReturnTypeDescriptor() const {
  //   return FindDexFile(declaring_class_->GetDexCache()
  //          ->dexStringByTypeIdx(proto_id_.return_type_id_);
  // }

  // Returns true if the method is declared public.
  bool IsPublic() const {
    return (access_flags_ & kAccPublic) != 0;
  }

  // Returns true if the method is declared private.
  bool IsPrivate() const {
    return (access_flags_ & kAccPrivate) != 0;
  }

  // Returns true if the method is declared static.
  bool IsStatic() const {
    return (access_flags_ & kAccStatic) != 0;
  }

  // Returns true if the method is declared synchronized.
  bool IsSynchronized() const {
    uint32_t synchonized = kAccSynchronized | kAccDeclaredSynchronized;
    return (access_flags_ & synchonized) != 0;
  }

  // Returns true if the method is declared final.
  bool IsFinal() const {
    return (access_flags_ & kAccFinal) != 0;
  }

  // Returns true if the method is declared native.
  bool IsNative() const {
    return (access_flags_ & kAccNative) != 0;
  }

  // Returns true if the method is declared abstract.
  bool IsAbstract() const {
    return (access_flags_ & kAccAbstract) != 0;
  }

  bool IsSynthetic() const {
    return (access_flags_ & kAccSynthetic) != 0;
  }

  // Number of argument registers required by the prototype.
  uint32_t NumArgRegisters();

 public:  // TODO: private
  // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
  Class* java_declaring_class_;
  ObjectArray<Class>* java_exception_types_;
  Object* java_formal_type_parameters_;
  Object* java_generic_exception_types_;
  Object* java_generic_parameter_types_;
  Object* java_generic_return_type_;
  Class* java_return_type_;
  String* java_name_;
  ObjectArray<Class>* java_parameter_types_;
  uint32_t java_generic_types_are_initialized_;
  uint32_t java_slot_;

  bool IsReturnAReference() const {
    return (shorty_[0] == 'L') || (shorty_[0] == '[');
  }

  bool IsReturnAFloatOrDouble() const {
    return (shorty_[0] == 'F') || (shorty_[0] == 'D');
  }

  bool IsReturnAFloat() const {
    return shorty_[0] == 'F';
  }

  bool IsReturnADouble() const {
    return shorty_[0] == 'D';
  }

  bool IsReturnALong() const {
    return shorty_[0] == 'J';
  }

  bool IsReturnVoid() const {
    return shorty_[0] == 'V';
  }

  // The number of arguments that should be supplied to this method
  size_t NumArgs() const {
    return (IsStatic() ? 0 : 1) + shorty_.length() - 1;
  }

  // The number of reference arguments to this method including implicit this
  // pointer
  size_t NumReferenceArgs() const;

  // The number of long or double arguments
  size_t NumLongOrDoubleArgs() const;

  // The number of reference arguments to this method before the given
  // parameter index
  size_t NumReferenceArgsBefore(unsigned int param) const;

  // Is the given method parameter a reference?
  bool IsParamAReference(unsigned int param) const;

  // Is the given method parameter a long or double?
  bool IsParamALongOrDouble(unsigned int param) const;

  // Size in bytes of the given parameter
  size_t ParamSize(unsigned int param) const;

  // Size in bytes of the return value
  size_t ReturnSize() const;

  void SetCode(const void* code) {
    code_ = code;
  }

  const void* GetCode() const {
    return code_;
  }

  void RegisterNative(const void* native_method) {
    native_method_ = native_method;
  }

  static MemberOffset NativeMethodOffset() {
    return MemberOffset(OFFSETOF_MEMBER(Method, native_method_));
  }

 public:  // TODO: private/const
  // the class we are a part of
  Class* declaring_class_;

  // access flags; low 16 bits are defined by spec (could be uint16_t?)
  uint32_t access_flags_;

  // For concrete virtual methods, this is the offset of the method
  // in "vtable".
  //
  // For abstract methods in an interface class, this is the offset
  // of the method in "iftable[n]->methodIndexArray".
  uint16_t method_index_;

  // Method bounds; not needed for an abstract method.
  //
  // For a native method, we compute the size of the argument list, and
  // set "insSize" and "registerSize" equal to it.
  uint16_t num_registers_;  // ins + locals
  uint16_t num_outs_;
  uint16_t num_ins_;

  // method name, e.g. "<init>" or "eatLunch"
  StringPiece name_;

  // Method prototype descriptor string (return and argument types).
  uint32_t proto_idx_;

  // The short-form method descriptor string.
  StringPiece shorty_;

  // A pointer to the memory-mapped DEX code.
  const uint16_t* insns_;

 private:
  Method();

  // Compiled code associated with this method
  const void* code_;

  // Any native method registered with this method
  const void* native_method_;
};

class Array : public Object {
 public:
  static Array* Alloc(Class* array_class,
                      size_t component_count,
                      size_t component_size) {
    size_t size = sizeof(Array) + component_count * component_size;
    Array* array = down_cast<Array*>(Heap::AllocObject(array_class, size));
    if (array != NULL) {
      array->SetLength(component_count);
    }
    return array;
  }
  uint32_t GetLength() const {
    return length_;
  }
  void SetLength(uint32_t length) {
    length_ = length;
  }
  const void* GetData() const {
    return &elements_;
  }
  void* GetData() {
    return &elements_;
  }

 private:
  // The number of array elements.
  uint32_t length_;
  // Location of first element.
  uint32_t elements_[0];
  Array();
};

template<class T>
class ObjectArray : public Array {
 public:
  static ObjectArray<T>* Alloc(Class* object_array_class,
                               size_t length) {
    return down_cast<ObjectArray<T>*>(Array::Alloc(object_array_class,
                                                   length,
                                                   sizeof(uint32_t)));
  }

  T* Get(uint32_t i) const {
    CHECK_LT(i, GetLength());
    Object* const * data = reinterpret_cast<Object* const *>(GetData());
    return down_cast<T*>(data[i]);
  }
  void Set(uint32_t i, T* object) {
    CHECK_LT(i, GetLength());
    T** data = reinterpret_cast<T**>(GetData());
    data[i] = object;
  }
  static void Copy(ObjectArray<T>* src, int src_pos, ObjectArray<T>* dst, int dst_pos, size_t length) {
    for (size_t i = 0; i < length; i++) {
      dst->Set(dst_pos + i, src->Get(src_pos + i));
    }
  }
  ObjectArray<T>* CopyOf(size_t new_length) {
    ObjectArray<T>* new_array = Alloc(klass_, new_length);
    Copy(this, 0, new_array, 0, std::min(GetLength(), new_length));
    return new_array;
  }

 private:
  ObjectArray();
};

// Class objects.
class Class : public Object {
 public:
  enum Status {
    kStatusError = -1,
    kStatusNotReady = 0,
    kStatusIdx = 1,  // loaded, DEX idx in super or ifaces
    kStatusLoaded = 2,  // DEX idx values resolved
    kStatusResolved = 3,  // part of linking
    kStatusVerifying = 4,  // in the process of being verified
    kStatusVerified = 5,  // logically part of linking; done pre-init
    kStatusInitializing = 6,  // class init in progress
    kStatusInitialized = 7,  // ready to go
  };

  enum PrimitiveType {
    kPrimNot = -1
  };

  Class* GetSuperClass() const {
    return super_class_;
  }

  uint32_t GetSuperClassIdx() const {
    return super_class_idx_;
  }

  bool HasSuperClass() const {
    return super_class_ != NULL;
  }

  Object* GetClassLoader() const {
    return class_loader_;
  }

  DexCache* GetDexCache() const {
    return dex_cache_;
  }

  Class* GetComponentType() const {
    DCHECK(IsArray());
    return component_type_;
  }

  const StringPiece& GetDescriptor() const {
    return descriptor_;
  }

  Status GetStatus() const {
    return status_;
  }

  void SetStatus(Status new_status) {
    // TODO: validate transition
    status_ = new_status;
  }

  // Returns true if the class has failed to link.
  bool IsErroneous() const {
    return GetStatus() == kStatusError;
  }

  // Returns true if the class has been verified.
  bool IsVerified() const {
    return GetStatus() >= kStatusVerified;
  }

  // Returns true if the class has been linked.
  bool IsLinked() const {
    return GetStatus() >= kStatusResolved;
  }

  bool IsLoaded() const {
    return GetStatus() >= kStatusLoaded;
  }

  // Returns true if this class is in the same packages as that class.
  bool IsInSamePackage(const Class* that) const;

  static bool IsInSamePackage(const StringPiece& descriptor1,
                              const StringPiece& descriptor2);

  // Returns true if this class represents an array class.
  bool IsArray() const {
    return descriptor_[0] == '[';  // TODO: avoid parsing the descriptor
  }

  // Returns true if the class is an interface.
  bool IsInterface() const {
    return (access_flags_ & kAccInterface) != 0;
  }

  // Returns true if the class is declared public.
  bool IsPublic() const {
    return (access_flags_ & kAccPublic) != 0;
  }

  // Returns true if the class is declared final.
  bool IsFinal() const {
    return (access_flags_ & kAccFinal) != 0;
  }

  // Returns true if the class is abstract.
  bool IsAbstract() const {
    return (access_flags_ & kAccAbstract) != 0;
  }

  // Returns true if the class is an annotation.
  bool IsAnnotation() const {
    return (access_flags_ & kAccAnnotation) != 0;
  }

  // Returns true if the class is a primitive type.
  bool IsPrimitive() const {
    return primitive_type_ != kPrimNot;
  }

  // Returns true if the class is synthetic.
  bool IsSynthetic() const {
    return (access_flags_ & kAccSynthetic) != 0;
  }

  // Returns true if this class can access that class.
  bool CanAccess(const Class* that) const {
    return that->IsPublic() || this->IsInSamePackage(that);
  }

  // Returns the size in bytes of a class object instance with the
  // given number of static fields.
  // static size_t Size(size_t num_sfields) {
  //   return OFFSETOF_MEMBER(Class, sfields_) +
  //          sizeof(StaticField) * num_sfields;
  // }

  // Returns the number of static, private, and constructor methods.
  size_t NumDirectMethods() const {
    return (direct_methods_ != NULL) ? direct_methods_->GetLength() : 0;
  }

  Method* GetDirectMethod(uint32_t i) const {
    DCHECK_NE(NumDirectMethods(), 0U);
    return direct_methods_->Get(i);
  }

  void SetDirectMethod(uint32_t i, Method* f) {  // TODO: uint16_t
    DCHECK_NE(NumDirectMethods(), 0U);
    direct_methods_->Set(i, f);
  }

  // Returns the number of non-inherited virtual methods.
  size_t NumVirtualMethods() const {
    return (virtual_methods_ != NULL) ? virtual_methods_->GetLength() : 0;
  }

  Method* GetVirtualMethod(uint32_t i) const {
    DCHECK_NE(NumVirtualMethods(), 0U);
    return virtual_methods_->Get(i);
  }

  void SetVirtualMethod(uint32_t i, Method* f) {  // TODO: uint16_t
    DCHECK_NE(NumVirtualMethods(), 0U);
    virtual_methods_->Set(i, f);
  }

  size_t NumInstanceFields() const {
    return (ifields_ != NULL) ? ifields_->GetLength() : 0;
  }

  // Returns the number of instance fields containing reference types.
  size_t NumReferenceInstanceFields() const {
    return num_reference_instance_fields_;
  }

  InstanceField* GetInstanceField(uint32_t i) {  // TODO: uint16_t
    DCHECK_NE(NumInstanceFields(), 0U);
    return ifields_->Get(i);
  }

  void SetInstanceField(uint32_t i, InstanceField* f) {  // TODO: uint16_t
    DCHECK_NE(NumInstanceFields(), 0U);
    ifields_->Set(i, f);
  }

  size_t NumStaticFields() const {
    return (sfields_ != NULL) ? sfields_->GetLength() : 0;
  }

  StaticField* GetStaticField(uint32_t i) const {  // TODO: uint16_t
    DCHECK_NE(NumStaticFields(), 0U);
    return sfields_->Get(i);
  }

  void SetStaticField(uint32_t i, StaticField* f) {  // TODO: uint16_t
    DCHECK_NE(NumStaticFields(), 0U);
    sfields_->Set(i, f);
  }

  uint32_t GetReferenceOffsets() const {
    return reference_offsets_;
  }

  void SetReferenceOffsets(uint32_t new_reference_offsets) {
    reference_offsets_ = new_reference_offsets;
  }

  Method* FindDirectMethod(const StringPiece& name) const;

  Method* FindVirtualMethod(const StringPiece& name) const;

  size_t NumInterfaces() const {
    return (interfaces_ != NULL) ? interfaces_->GetLength() : 0;
  }

  Class* GetInterface(uint32_t i) const {
    DCHECK_NE(NumInterfaces(), 0U);
    return interfaces_->Get(i);
  }

  void SetInterface(uint32_t i, Class* f) {  // TODO: uint16_t
    DCHECK_NE(NumInterfaces(), 0U);
    interfaces_->Set(i, f);
  }

  Method* FindDirectMethodLocally(const StringPiece& name,
                                  const StringPiece& descriptor) const;

 public:  // TODO: private
  // leave space for instance data; we could access fields directly if
  // we freeze the definition of java/lang/Class
#define CLASS_FIELD_SLOTS 1
  // Class.#0 name
  uint32_t instance_data_[CLASS_FIELD_SLOTS];
#undef CLASS_FIELD_SLOTS

  // UTF-8 descriptor for the class from constant pool
  // ("Ljava/lang/Class;"), or on heap if generated ("[C")
  StringPiece descriptor_;

  // Proxy classes have their descriptor allocated on the native heap.
  // When this field is non-NULL it must be explicitly freed.
  std::string* descriptor_alloc_;

  // access flags; low 16 bits are defined by VM spec
  uint32_t access_flags_;  // TODO: make an instance field?

  // DexCache of resolved constant pool entries
  // (will be NULL for VM-generated, e.g. arrays and primitive classes)
  DexCache* dex_cache_;

  // state of class initialization
  Status status_;

  // if class verify fails, we must return same error on subsequent tries
  Class* verify_error_class_;

  // threadId, used to check for recursive <clinit> invocation
  uint32_t clinit_thread_id_;

  // Total object size; used when allocating storage on gc heap.  (For
  // interfaces and abstract classes this will be zero.)
  size_t object_size_;

  // For array classes, the class object for base element, for
  // instanceof/checkcast (for String[][][], this will be String).
  // Otherwise, NULL.
  Class* component_type_;  // TODO: make an instance field

  // For array classes, the number of array dimensions, e.g. int[][]
  // is 2.  Otherwise 0.
  int32_t array_rank_;

  // primitive type index, or PRIM_NOT (-1); set for generated prim classes
  PrimitiveType primitive_type_;

  // The superclass, or NULL if this is java.lang.Object or a
  // primitive type.
  Class* super_class_;  // TODO: make an instance field
  uint32_t super_class_idx_;

  // defining class loader, or NULL for the "bootstrap" system loader
  Object* class_loader_;  // TODO: make an instance field

  // initiating class loader list
  // NOTE: for classes with low serialNumber, these are unused, and the
  // values are kept in a table in gDvm.
  // InitiatingLoaderList initiating_loader_list_;

  // array of interfaces this class implements directly
  ObjectArray<Class>* interfaces_;
  uint32_t* interfaces_idx_;

  // static, private, and <init> methods
  ObjectArray<Method>* direct_methods_;

  // virtual methods defined in this class; invoked through vtable
  ObjectArray<Method>* virtual_methods_;

  // Virtual method table (vtable), for use by "invoke-virtual".  The
  // vtable from the superclass is copied in, and virtual methods from
  // our class either replace those from the super or are appended.
  ObjectArray<Method>* vtable_;

  // Interface table (iftable), one entry per interface supported by
  // this class.  That means one entry for each interface we support
  // directly, indirectly via superclass, or indirectly via
  // superinterface.  This will be null if neither we nor our
  // superclass implement any interfaces.
  //
  // Why we need this: given "class Foo implements Face", declare
  // "Face faceObj = new Foo()".  Invoke faceObj.blah(), where "blah"
  // is part of the Face interface.  We can't easily use a single
  // vtable.
  //
  // For every interface a concrete class implements, we create a list
  // of virtualMethod indices for the methods in the interface.
  size_t iftable_count_;
  InterfaceEntry* iftable_;

  // The interface vtable indices for iftable get stored here.  By
  // placing them all in a single pool for each class that implements
  // interfaces, we decrease the number of allocations.
  size_t ifvi_pool_count_;
  uint32_t* ifvi_pool_;

  // instance fields
  //
  // These describe the layout of the contents of a
  // DataObject-compatible Object.  Note that only the fields directly
  // declared by this class are listed in ifields; fields declared by
  // a superclass are listed in the superclass's Class.ifields.
  //
  // All instance fields that refer to objects are guaranteed to be at
  // the beginning of the field list.  num_reference_instance_fields_
  // specifies the number of reference fields.
  ObjectArray<InstanceField>* ifields_;

  // number of fields that are object refs
  size_t num_reference_instance_fields_;

  // Bitmap of offsets of ifields.
  uint32_t reference_offsets_;

  // source file name, if known.  Otherwise, NULL.
  const char* source_file_;

  // Static fields
  ObjectArray<StaticField>* sfields_;

 private:
  Class();
};
std::ostream& operator<<(std::ostream& os, const Class::Status& rhs);

inline Object* Object::Alloc(Class* klass) {
  return Heap::AllocObject(klass, klass->object_size_);
}

class DataObject : public Object {
 public:
  uint32_t fields_[0];
 private:
  DataObject();
};

class CharArray : public Array {
 public:
  static CharArray* Alloc(Class* char_array_class, size_t length) {
    return down_cast<CharArray*>(Array::Alloc(char_array_class,
                                              length,
                                              sizeof(uint16_t)));
  }

  uint16_t* GetChars() {
    return reinterpret_cast<uint16_t*>(GetData());
  }

  const uint16_t* GetChars() const {
    return reinterpret_cast<const uint16_t*>(GetData());
  }

  uint16_t GetChar(uint32_t i) const {
    CHECK_LT(i, GetLength());
    return GetChars()[i];
  }

  void  SetChar(uint32_t i, uint16_t ch) {
    CHECK_LT(i, GetLength());
    GetChars()[i] = ch;
  }

 private:
  CharArray();
};

class String : public Object {
 public:
  static String* AllocFromUtf16(Class* java_lang_String,
                                Class* char_array,
                                int32_t utf16_length,
                                uint16_t* utf16_data_in) {
    String* string = Alloc(java_lang_String, char_array, utf16_length);
    uint16_t* utf16_data_out = string->array_->GetChars();
    // TODO use 16-bit wide memset variant
    for (int i = 0; i < utf16_length; i++ ) {
        utf16_data_out[i] = utf16_data_in[i];
    }
    string->hash_code_ = ComputeUtf16Hash(utf16_data_out, utf16_length);
    return string;
  }

  static String* AllocFromModifiedUtf8(Class* java_lang_String,
                                       Class* char_array,
                                       int32_t utf16_length,
                                       const char* utf8_data_in) {
    String* string = Alloc(java_lang_String, char_array, utf16_length);
    uint16_t* utf16_data_out = string->array_->GetChars();
    ConvertModifiedUtf8ToUtf16(utf16_data_out, utf8_data_in);
    string->hash_code_ = ComputeUtf16Hash(utf16_data_out, utf16_length);
    return string;
  }

 public: // TODO: private
  // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
  CharArray* array_;

  int32_t hash_code_;

  int32_t offset_;

  int32_t count_;

  static String* Alloc(Class* java_lang_String,
                       Class* char_array,
                       int32_t utf16_length) {
    String* string = down_cast<String*>(Object::Alloc(java_lang_String));
    CharArray* array = CharArray::Alloc(char_array, utf16_length);
    string->array_ = array;
    string->count_ = utf16_length;
    return string;
  }

  // Convert Modified UTF-8 to UTF-16
  // http://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8
  static void ConvertModifiedUtf8ToUtf16(uint16_t* utf16_data_out, const char* utf8_data_in) {
    while (*utf8_data_in != '\0') {
      *utf16_data_out++ = GetUtf16FromUtf8(&utf8_data_in);
    }
  }

  // Retrieve the next UTF-16 character from a UTF-8 string.
  //
  // Advances "*pUtf8Ptr" to the start of the next character.
  //
  // WARNING: If a string is corrupted by dropping a '\0' in the middle
  // of a 3-byte sequence, you can end up overrunning the buffer with
  // reads (and possibly with the writes if the length was computed and
  // cached before the damage). For performance reasons, this function
  // assumes that the string being parsed is known to be valid (e.g., by
  // already being verified). Most strings we process here are coming
  // out of dex files or other internal translations, so the only real
  // risk comes from the JNI NewStringUTF call.
  static uint16_t GetUtf16FromUtf8(const char** utf8_data_in) {
    uint8_t one = *(*utf8_data_in)++;
    if ((one & 0x80) == 0) {
      /* one-byte encoding */
      return one;
    }
    /* two- or three-byte encoding */
    uint8_t two = *(*utf8_data_in)++;
    if ((one & 0x20) == 0) {
      /* two-byte encoding */
      return ((one & 0x1f) << 6) |
              (two & 0x3f);
    }
    /* three-byte encoding */
    uint8_t three = *(*utf8_data_in)++;
    return ((one & 0x0f) << 12) |
            ((two & 0x3f) << 6) |
            (three & 0x3f);
  }

  // The java/lang/String.computeHashCode() algorithm
  static int32_t ComputeUtf16Hash(const uint16_t* string_data, size_t string_length) {
    int32_t hash = 0;
    while (string_length--) {
        hash = hash * 31 + *string_data++;
    }
    return hash;
  }

  static bool Equals(const String* string, const char* other) {
    uint16_t* chars = string->array_->GetChars();
    for (int32_t i = 0; i < string->count_; i++) {
      uint16_t c = GetUtf16FromUtf8(&other);
      if (c == '\0' || c != chars[string->offset_ + i]) {
        return false;
      }
    }
    return *other == '\0';
  }

 private:
  String();
};

class InterfaceEntry {
 public:
  Class* GetClass() const {
    return klass_;
  };

  void SetClass(Class* klass) {
    klass_ = klass;
  };

 private:
  // Points to the interface class.
  Class* klass_;

 public:  // TODO: private
  // Index into array of vtable offsets.  This points into the
  // ifviPool, which holds the vtables for all interfaces declared by
  // this class.
  uint32_t* method_index_array_;
};

}  // namespace art

#endif  // ART_SRC_OBJECT_H_
