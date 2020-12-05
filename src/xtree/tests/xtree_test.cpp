#include "xtree/XTree.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Tests {

class XTreeTest : public ::testing::Test {
 protected:
  XTreeTest() {}
  virtual ~XTreeTest() {}

  void SetUp() override {}
  void TearDown() override {}

  /* Clase que almacena el numero de instancias activas (sólo es para las
   * pruebas). */
  // class Counter {
   // public:
    // Counter() : value_(0) { ++num_of_instances_; }
    // ~Counter() { --num_of_instances_; }
//
    // static int GetNumInstances() { return num_of_instances_; }
//
    // static void ResetNumInstances() { num_of_instances_ = 0; }
//
    // int GetValue() const { return value_; }
//
    // void SetValue(int value) { this->value_ = value; }
//
   // private:
    // static int num_of_instances_;
    // int value_;
//
    // Convirtiéndolo en un tipo "no copiable"
    // Counter(const Counter &other);
    // Counter &operator=(const Counter &other);
  // };
};

// int SmartPointerTest::Counter::num_of_instances_ = 0;

TEST_F(XTreeTest, ConstructorTest) {
  XTree<2, int, 6> xtree;
  EXPECT_EQ(xtree.size(), size_t(0))
    << "The size of a new xtree should equal zero.";
}

// TEST_F(SmartPointerTest, ConstructorFuncionality) {
  // SmartPointerTest::Counter::ResetNumInstances();
  // {
    // SmartPointer<Counter> basic_pointer(new Counter);
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del nuevo puntero debería ser uno.";
    // EXPECT_NE(basic_pointer.GetPointer(), nullptr)
        // << "El recurso almacenado de basic_pointer debería ser no-nullptr.";
    // EXPECT_EQ(Counter::GetNumInstances(), 1)
        // << "Debería haber exactamente una instancia activa de la clase "
           // "Counter.";
    // basic_pointer.GetPointer()->SetValue(125);
//
    // EXPECT_EQ(basic_pointer.GetPointer()->GetValue(), 125)
        // << "El valor almacenado debería ser 137";
  // }
  // EXPECT_EQ(SmartPointerTest::Counter::GetNumInstances(), 0)
      // << "No debería haber ninguna instancia en Counter.";
// }
//
// TEST_F(SmartPointerTest, SingleResource) {
  // SmartPointerTest::Counter::ResetNumInstances();
  // {
    // SmartPointer<Counter> basic_pointer(new Counter);
    // SmartPointer<Counter> copy_pointer = basic_pointer;
    // EXPECT_EQ(SmartPointerTest::Counter::GetNumInstances(), 1)
        // << "Solo debería haber una instancia de la clase Counter.";
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(2))
        // << "deberían existir 2 shared pointers.";
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(2))
        // << "copy_pointer debería tener un conteo de referencias de 2.";
    // EXPECT_EQ(basic_pointer.GetPointer(), copy_pointer.GetPointer())
        // << "Los dos punteros deberían almacenar el mismo recurso.";
  // }
  // EXPECT_EQ(SmartPointerTest::Counter::GetNumInstances(), 0)
      // << "No debería haber ninguna instancia en Counter.";
// }
//
// TEST_F(SmartPointerTest, DifferentResource) {
  // SmartPointerTest::Counter::ResetNumInstances();
  // {
    // SmartPointer<Counter> basic_pointer(new Counter);
    // SmartPointer<Counter> copy_pointer(new Counter);
    // EXPECT_EQ(SmartPointerTest::Counter::GetNumInstances(), 2)
        // << "Deberían existir solo 2 objetos Counter.";
    // copy_pointer = basic_pointer;
    // EXPECT_EQ(SmartPointerTest::Counter::GetNumInstances(), 1)
        // << "Debería existir solo un ojeto Counter.";
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(2))
        // << "Deberían existir 2 smart pointers.";
    // EXPECT_EQ(copy_pointer.GetReferenceCount(), size_t(2))
        // << "copy_pointer debería tener un conteo de referencias de 2.";
    // EXPECT_EQ(basic_pointer.GetPointer(), copy_pointer.GetPointer())
        // << "Los dos punteros deberían almacenar el mismo recurso.";
  // }
  // EXPECT_EQ(SmartPointerTest::Counter::GetNumInstances(), 0)
      // << "No debería haber ninguna instancia en Counter.";
// }
//
// TEST_F(SmartPointerTest, ChangeResource) {
  // SmartPointerTest::Counter::ResetNumInstances();
  // {
    // SmartPointer<Counter> basic_pointer(new Counter);
    // SmartPointer<Counter> copy_pointer = basic_pointer;
//
    // EXPECT_EQ(Counter::GetNumInstances(), 1)
        // << "Debería existir solo un ojeto Counter.";
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(2))
       // << "Deberían existir solo 2 objetos Counter.";
    // EXPECT_EQ(copy_pointer.GetReferenceCount(), size_t(2))
        // << "copy_pointer debería tener un conteo de referencias de 2.";
    // EXPECT_EQ(basic_pointer.GetPointer(), copy_pointer.GetPointer())
        // << "Los dos punteros deberían almacenar el mismo recurso.";
//
    // basic_pointer = new Counter();
//
    // EXPECT_EQ(Counter::GetNumInstances(), 2)
        // << "Deberían existir 2 objetos Counter";
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del puntero original debería ser 1.";
    // EXPECT_EQ(copy_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del puntero copia debería ser 1.";
    // EXPECT_NE(basic_pointer.GetPointer(), copy_pointer.GetPointer())
        // << "Los dos punteros no deberían almacenar el mismo recurso.";
  // }
  // EXPECT_EQ(Counter::GetNumInstances(), 0)
      // << "No deberían existir instancias de Counter.";
// }
//
// TEST_F(SmartPointerTest, SelfAssignment) {
  // SmartPointerTest::Counter::ResetNumInstances();
  // {
    // SmartPointer<Counter> basic_pointer(new Counter);
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias debería ser 1.";
    // EXPECT_NE(basic_pointer.GetPointer(), nullptr)
        // << "El recurso almacenado por basic_pointer debería ser no-NULL.";
    // EXPECT_EQ(Counter::GetNumInstances(), 1)
        // << "Debería existir exactamente una instancia activa de la clase "
           // "Counter.";
//
    // Counter *old_value = basic_pointer.GetPointer();
    // basic_pointer = basic_pointer;  // Self-assign.
//
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias debería ser exactamente 1.";
    // EXPECT_NE(basic_pointer.GetPointer(), nullptr)
        // << "El recurso almacenado por basic_pointer debería ser no-NULL.";
    // EXPECT_EQ(Counter::GetNumInstances(), 1)
        // << "Debería existir exactamente una instancia activa de la clase "
           // "Counter.";
    // EXPECT_EQ(basic_pointer.GetPointer(), old_value)
        // << "Después de la auto-asignación debería ser el mismo puntero.";
  // }
  // EXPECT_EQ(Counter::GetNumInstances(), 0)
      // << "No deberían existir instancias de Counter.";
// }
//
// TEST_F(SmartPointerTest, ChainedAssignment) {
  // SmartPointerTest::Counter::ResetNumInstances();
  // {
    // SmartPointer<Counter> basic_pointer(new Counter);
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del nuevo puntero debería ser uno.";
    // EXPECT_NE(basic_pointer.GetPointer(), nullptr)
        // << "El recurso almacenado por basic_pointer debería ser no-NULL.";
    // EXPECT_EQ(Counter::GetNumInstances(), 1)
        // << "Debería existir exactamente una instancia activa de la clase "
           // "Counter.";
//
    // Counter *old_value = basic_pointer.GetPointer();
    // SmartPointer<Counter> two(new Counter), three(new Counter);
    // EXPECT_EQ(Counter::GetNumInstances(), 3)
        // << "Deberían existir exactamente 3 instancias activas de la clase "
           // "Counter.";
//
//
    // three = two = basic_pointer;  // Chained Assign.
//
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(3))
        // << "El conteo de referencias debería ser 1.";
    // EXPECT_NE(basic_pointer.GetPointer(), nullptr)
        // << "El recurso almacenado por basic_pointer debería ser no-NULL.";
    // EXPECT_EQ(Counter::GetNumInstances(), 1)
        // << "Debería existir exactamente una instancia activa de la clase "
           // "Counter.";
    // EXPECT_EQ(basic_pointer.GetPointer(), three.GetPointer())
        // << "El primer puntero debería ser igual al tercer puntero.";
    // EXPECT_EQ(basic_pointer.GetPointer(), two.GetPointer())
        // << "El primer puntero debería ser igual al segundo puntero.";
//
    // old_value = basic_pointer.GetPointer();
//
    // basic_pointer = new Counter();
//
    // EXPECT_EQ(Counter::GetNumInstances(), 2)
        // << "Deberían existir exactamente 2 instancias activas de la clase "
           // "Counter.";
    // EXPECT_EQ(basic_pointer.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del puntero original debería ser uno.";
    // EXPECT_NE(basic_pointer.GetPointer(), old_value)
        // << "El puntero original no debería tener el valor original.";
    // EXPECT_EQ(two.GetPointer(), old_value)
        // << "El segundo puntero debería tener el mismo valor que el original.";
    // EXPECT_EQ(three.GetPointer(), old_value)
        // << "El tercer puntero debería tener el mismo valor que el original.";
//
    // three.Detach();
    // three = new Counter();
//
    // EXPECT_EQ(Counter::GetNumInstances(), 3)
        // << "Deberían existir exactamente 3 instancias activas de la clase "
           // "Counter.";
    // EXPECT_EQ(three.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del tercer puntero debería ser uno.";
    // EXPECT_EQ(two.GetReferenceCount(), size_t(1))
        // << "El conteo de referencias del segundo puntero debería ser uno.";
    // EXPECT_NE(three.GetPointer(), old_value)
        // << "El tercer puntero no debería tener el valor original.";
    // EXPECT_EQ(two.GetPointer(), old_value)
        // << "El segundo puntero debría tener el mismo valor que el original.";
  // }
  // EXPECT_EQ(Counter::GetNumInstances(), 0)
      // << "No debería existir ninguna instancia de la clase Counter.";
// }
}  // namespace Tests 
