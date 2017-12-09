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
  p " \n#Method Prolouge" e;
  p " push %ebp" e;         // save ebp
  p " mov %esp, %ebp" e;    // set ebp
  // set the esp ??
  //p ""e;
  /*  p " push $" + std::to_string(currentMethodInfo.localsSize) e;
  p " pop %eax" e;
  p " sub %eax, %esp" e; // moves the stack pointer down by local var size */
  //p " sub " + std::to_string(currentMethodInfo.localsSize) + ", %esp" e;
  // save callee-save registers
  p " push %edi" e;
  p " push %esi" e;
  p " push %ebx" e;
  
  node->visit_children(this); // is the method body

  p " \n#Method Epilogue" e;
  p " pop %ebx" e;
  p " pop %esi" e;
  p " pop %edi" e;
  p " mov %ebp, %esp" e;
  p " pop %ebp" e;
  p " ret" e;  // using what is in %eax
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
  p " pop %eax" e;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitCallNode(CallNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
  node->visit_children(this);

  p " push $printstr" e;
  p " call printf" e;
  //p " add $8, %esp" e; // add 8 since we pushed a string 
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
  node->visit_children(this);
  
  p " # Plus" e;
  p " pop %ebx" e;
  p " pop %eax" e;
  p " add %ebx, %eax" e;
  p " push %eax" e;
  
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
  node->visit_children(this);

  p " # Minus" e;
  p " pop %ebx" e;
  p " pop %eax" e;
  p " sub %ebx, %eax" e;
  p " push %eax" e;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
  node->visit_children(this);

  p " # Times" e;
  p " pop %ebx" e;
  p " pop %eax" e;
  p " sub %ebx, %eax" e;
  p " push %eax" e;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
  // ( %edx : %eax ) / operand
  node->visit_children(this);

  // TODO: might have to change this ordering
  p " # Divide" e;
  p " pop %eax" e; // numerator
  p " cdq" e;
  p " pop %ebx" e; // denominator
  p " idiv %ebx" e;
  p " push %eax" e;
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitAndNode(AndNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitOrNode(OrNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitNotNode(NotNode* node) {
  node->visit_children(this);
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
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  node->visit_children(this);

  p " push $" + std::to_string(node->integer->value) e;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  node->visit_children(this);

  p " push $" + std::to_string(node->integer->value) e;
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