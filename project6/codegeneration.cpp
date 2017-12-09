#include "codegeneration.hpp"

#define p std::cout<<
#define e <<std::endl
// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

void CodeGenerator::visitProgramNode(ProgramNode* node) {
  // set up all the assembly stuff from project description
  p " .data" e;
  p " printstr: .asciz \"%d\\n\"" e;
  p "" e;
  p " .text" e;
  p " .globl Main_main\n" e;
  
  node->visit_children(this);

  // exit the program
  // TODO: maybe through a syscall
}

void CodeGenerator::visitClassNode(ClassNode* node) {
  currentClassName = node->identifier_1->name;
  currentClassInfo = (*classTable)[currentClassName];
  
  node->visit_children(this);
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
  currentMethodName = node->identifier->name;
  currentMethodInfo = currentClassInfo.methods->at(currentMethodName);
  
  p currentClassName + "_" + currentMethodName + ":" e;

  // pre-call sequence
  // save caller-save registers, push args to stack, push ret addr

  // TODO: possibly change this to handle the method prolouge
  node->visit_children(this);

  // 


}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
  p "\n # Method Prologue" e;
  p "\tpush %ebp" e;         // save ebp
  p "\tmov %esp, %ebp" e;    // set ebp

  // set the esp ??
  //p ""e;
  /*  p " push $" + std::to_string(currentMethodInfo.localsSize) e;
  p " pop %eax" e;
  p " sub %eax, %esp" e; // moves the stack pointer down by local var size */

  p "\tsub $" + std::to_string(currentMethodInfo.localsSize) + ", %esp" e;
  // save callee-save registers
  p "\tpush %edi" e;
  p "\tpush %esi" e;
  p "\tpush %ebx" e;
  
  node->visit_children(this); // is the method body

  p "\n # Method Epilogue" e;
  p "\tpop %ebx" e;
  p "\tpop %esi" e;
  p "\tpop %edi" e;
  p "\tmov %ebp, %esp" e;
  p "\tpop %ebp" e;
  p "\tret" e;  // using what is in %eax
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
  node->visit_children(this);

  // adding the return value to the eax register
  p "\tpop %eax" e;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
  node->visit_children(this);
  
  p "\tpop %eax" e;
  if (node->identifier_2 != NULL){
    // a.b = ...
  }else {
    // a = ...
    // check local var, global var, or inherited var
    if (currentMethodInfo.variables->count(node->identifier_1->name) == 1){
      p "\tmov %eax, " + std::to_string(currentMethodInfo.variables->at(node->identifier_1->name).offset) + "(%ebp)" e;
    }else if (currentClassInfo.members->count(node->identifier_1->name) == 1){
      // TODO: might have to change how offset works
      p "\tmov %eax, " + std::to_string(currentClassInfo.members->at(node->identifier_1->name).offset) + "(%ebp)" e;
    }else{
      // TODO: implement inheritence
    }
  }
}

void CodeGenerator::visitCallNode(CallNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
  //node->visit_children(this);

  node->expression->accept(this); // get the expression value and push it to stack
  std::string num = std::to_string(nextLabel()); 
  p "\tpop %eax" e;
  p "\tmov $0, %ebx" e;
  p "\tcmp %eax, %ebx" e;
  p "\tje skip_if_" + num e;
  // if block
  if (!node->statement_list_1->empty()){
    for (auto i=node->statement_list_1->begin(); i!=node->statement_list_1->end(); ++i){
      (*i)->accept(this);
    }
  }
  p "\tjmp after_if_" + num e;
  p "skip_if_" + num + ":" e;
  // else
  if (!node->statement_list_2->empty()){
    // code in else
    for (auto i=node->statement_list_2->begin(); i!=node->statement_list_2->end(); ++i){
      (*i)->accept(this); // visit each statement node
    }
  }
  p "after_if_" + num + ":" e;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
  node->visit_children(this);

  p "\tpush $printstr" e;
  p "\tcall printf" e;
  //p " add $8, %esp" e; // add 8 since we pushed a string 
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
  node->visit_children(this);
  
  p " # Plus" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tadd %ebx, %eax" e;
  p "\tpush %eax" e;
  
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
  node->visit_children(this);

  p " # Minus" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tsub %ebx, %eax" e;
  p "\tpush %eax" e;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
  node->visit_children(this);

  p " # Times" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\timul %ebx, %eax" e;
  p "\tpush %eax" e;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
  // ( %edx : %eax ) / operand
  node->visit_children(this);

  // TODO: might have to change this ordering
  p " # Divide" e;
  p "\tpop %ebx" e; // denominator
  p "\tpop %eax" e; // numerator
  p "\tcdq" e; // extend eax to 64 bits
  p "\tidiv %ebx" e; // divide eax by this
  p "\tpush %eax" e; // place result onto stack
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Greater Than" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tcmp %eax, %ebx" e;
  p "\tjg greater_than_" + num e;
  // not greater than
  p "\tpush $0" e;
  p "\tjmp after_greater_than_" + num e;
  p "greater_than_" + num + ":" e;
  // greater than
  p "\tpush $1" e;
  p "after_greater_than_" + num + ":" e;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Greater Than Equal" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tcmp %eax, %ebx" e;
  p "\tjge greater_than_equal_" + num e;
  // not greater than or equal
  p "\tpush $0" e;
  p "\tjmp after_greater_than_equal_" + num e;
  p "greater_than_equal_" + num + ":" e;
  // greater than or equal
  p "\tpush $1" e;
  p "after_greater_than_equal_" + num + ":" e;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Equal" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tcmp %eax, %ebx" e;
  p "\tje equal_" + num e;
  // not equal
  p "\tpush $0" e;
  p "\tjmp after_equal_" + num e;
  p "equal_" + num + ":" e;
  // equal
  p "\tpush $1" e;
  p "after_equal_" + num + ":" e;
}

void CodeGenerator::visitAndNode(AndNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # And" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tcmp %eax, %ebx" e;
  p "\tje equal_" + num e;
  // not equal
  p "\tpush $0" e;
  p "\tjmp after_equal_" + num e;
  p "equal_" + num + ":" e;
  // equal
  p "\tpush $1" e;
  p "after_equal_" + num + ":" e;
}

void CodeGenerator::visitOrNode(OrNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Equal" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tmov $1, %ecx" e;
  p "\tcmp %ecx, %ebx" e;
  p "\tje equal_" + num e;
  // first wasn't true, try second
  p "\tcmp %ecx, %eax" e;
  p "\tje equal_" + num e;
  // both weren't true, set 0 and go past
  p "\tpush $0" e;
  p "\tjmp after_equal_" + num e;
  p "equal_" + num + ":" e;
  p "\tpush $1" e;
  p "after_equal_" + num + ":" e;
}

void CodeGenerator::visitNotNode(NotNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Equal" e;
  p "\tpop %ebx" e;
  p "\tmov $1, %eax" e;
  p "\tcmp %eax, %ebx" e;
  p "\tje not_a_true_" + num e;
  // change a 0 to a 1
  p "\tpush $1" e;
  p "\tjmp after_not_" + num e;
  p "not_a_true_" + num + ":" e;
  // change a 1 to a 0
  p "\tpush $0" e;
  p "after_not_" + num + ":" e;
  
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
  node->visit_children(this);

  // TODO: get correct variable info and allocate the correct space
  // check if param, local var, global var, or inherited var
  bool isParam = false;
  /*
  for (std::list<CompoundType>::iterator i=currentMethodInfo.parameters->begin(); i!=currentMethodInfo.parameters->end(); ++i){
    if ((*i).)
      // set var
      isParam = true;
  }
  */

  if (!isParam){
    if (currentMethodInfo.variables->count(node->identifier->name) == 1){
      // it's a local variable
      p "\tpush " + std::to_string(currentMethodInfo.variables->at(node->identifier->name).offset) + "(%ebp)" e;
    } else if (currentClassInfo.members->count(node->identifier->name) == 1){
      // it's a global variable
      p "\tpush " + std::to_string(currentClassInfo.members->at(node->identifier->name).offset) + "(%ebp)" e;
    }else{
      // search for it in all super classes
    }
  }
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  node->visit_children(this);

  p "\tpush $" + std::to_string(node->integer->value) e;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  node->visit_children(this);

  p "\tpush $" + std::to_string(node->integer->value) e;
}

void CodeGenerator::visitNewNode(NewNode* node) {
  node->visit_children(this);

  // TODO: add constructor checking and allocate correct space
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
  node->visit_children(this);
}
