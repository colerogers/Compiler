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
  //node->visit_children(this);
  node->methodbody->accept(this); // idk
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

  int localOff, memOff;
  
  p "\t # Assignment Node" e;
  p "\tpop %eax" e;
  if (node->identifier_2 != NULL){
    // a.b = ...
    if (currentMethodInfo.variables->count(node->identifier_1->name) != 0){
      // declared in current method
      localOff = currentMethodInfo.variables->at(node->identifier_1->name).offset;
      memOff = (*classTable)[currentMethodInfo.variables->at(node->identifier_1->name).type.objectClassName].members->at(node->identifier_2->name).offset;
      p "mov " + std::to_string(localOff) + "(%ebp), %ebx" e;
      p "mov %eax, " + std::to_string(memOff) + "(%ebx)" e;
      
    }else if (currentClassInfo.members->count(node->identifier_1->name) != 0){
      // declared in current class
      localOff = currentClassInfo.members->at(node->identifier_1->name).offset;
      memOff = (*classTable)[currentClassInfo.members->at(node->identifier_1->name).type.objectClassName].members->at(node->identifier_2->name).offset;
      p "mov 8(%ebp), %ebx" e;
      p "mov " + std::to_string(localOff) + "(%ebx), %ebx" e;
      p "mov %eax, " + std::to_string(memOff) + "(%ebx)" e;
      
    }else {
      // TODO: implement inheritance
    }
  }else {
    // a = ...
    // check local var, global var, or inherited var
    if (currentMethodInfo.variables->count(node->identifier_1->name) != 0){
      p "\tmov %eax, " + std::to_string(currentMethodInfo.variables->at(node->identifier_1->name).offset) + "(%ebp)" e;
    }else if (currentClassInfo.members->count(node->identifier_1->name) != 0){
      // TODO: might have to change how offset works
      p "\tmov %eax, " + std::to_string(currentClassInfo.members->at(node->identifier_1->name).offset) + "(%ebp)" e;
    }else{
      // TODO: implement inheritance
      
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
  p " # If Else Node" e;
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
  //node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # While Node" e;
  p "while_" + num + ":" e;
  node->expression->accept(this); // need to keep updating variables
  p "\tpop %eax" e;
  p "\tmov $0, %ebx" e;
  p "\tcmp %eax, %ebx" e;
  p "\tje after_while_" + num e;
  // while expression is true
  if (!node->statement_list->empty()){
    for (auto i=node->statement_list->begin(); i!=node->statement_list->end(); ++i){
      (*i)->accept(this); // visit each statement node
    }
  }
  p "\tjmp while_" + num e;
  p "after_while_" + num + ":" e;
  
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
  node->visit_children(this);

  p "\tpush $printstr" e;
  p "\tcall printf" e;
  //p " add $8, %esp" e; // add 8 since we pushed a string 
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
  //node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Do While Node" e;
  p "do_while_" + num + ":" e;
  // while block
  if (!node->statement_list->empty()){
    for (auto i=node->statement_list->begin(); i!=node->statement_list->end(); ++i){
      (*i)->accept(this); // visit each statement node
    }
  }

  node->expression->accept(this); // need to keep updating variables
  p "\tpop %eax" e;
  p "\tmov $0, %ebx" e;
  p "\tcmp %eax, %ebx" e;
  p "\tje after_do_while_" + num e;
  p "\tjmp do_while_" + num e;  
  p "after_do_while_" + num + ":" e;
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
  p "\tcmp %ebx, %eax" e;
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
  p "\tcmp %ebx, %eax" e;
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
  p "\tje and_equal_" + num e;
  // not equal
  p "\tpush $0" e;
  p "\tjmp after_and_equal_" + num e;
  p "and_equal_" + num + ":" e;
  // equal
  p "\tpush $1" e;
  p "after_and_equal_" + num + ":" e;
}

void CodeGenerator::visitOrNode(OrNode* node) {
  node->visit_children(this);

  std::string num = std::to_string(nextLabel());

  p " # Equal" e;
  p "\tpop %ebx" e;
  p "\tpop %eax" e;
  p "\tmov $1, %ecx" e;
  p "\tcmp %ecx, %ebx" e;
  p "\tje or_equal_" + num e;
  // first wasn't true, try second
  p "\tcmp %ecx, %eax" e;
  p "\tje or_equal_" + num e;
  // both weren't true, set 0 and go past
  p "\tpush $0" e;
  p "\tjmp after_or_equal_" + num e;
  p "or_equal_" + num + ":" e;
  p "\tpush $1" e;
  p "after_or_equal_" + num + ":" e;
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

  std::string num = std::to_string(nextLabel());

  //TODO: might need to change
  p " # Negation" e;
  p "\tpop %ebx" e;
  p "\txor %eax, %eax" e;
  p "\tsub %eax, %ebx" e;
  p "\tpush %ebx" e;
  
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
  // saving the caller-save registers
  //p "push %eax" e;
  //p "push %ecx" e;
  //p "push %edx" e;
  

  
  std::string cName;

  int localOff, memOff;
  int args = 0;
  if (node->expression_list != NULL){
    for (auto i=node->expression_list->rbegin(); i!=node->expression_list->rend(); ++i){
      (*i)->accept(this);
      args++;
    }
  }

  if (node->identifier_2 != NULL){
    // a.b()
    // check a's methods
    
    if (currentMethodInfo.variables->count(node->identifier_1->name) != 0){
      // a is declared in the current method
      localOff = currentMethodInfo.variables->at(node->identifier_1->name).offset;
      p "push " + std::to_string(localOff) + "(%ebp)" e;

      cName = currentMethodInfo.variables->at(node->identifier_1->name).type.objectClassName;
      
    }else if (currentClassInfo.members->count(node->identifier_1->name) != 0){
      // a declared in current class
      localOff = currentClassInfo.members->at(node->identifier_1->name).offset;
      p "mov 8(%ebp), %ebx" e;
      p "push " + std::to_string(localOff) + "(%ebx)" e;

      cName = currentClassInfo.members->at(node->identifier_1->name).type.objectClassName;
      
    }else{
      // TODO: inheritance
    }

    p "call " + cName + "_" + node->identifier_2->name e;
    if (args > 0)
      p "add $" + std::to_string(args * 4) + ", %esp" e;
    p "push %eax" e;
    
  }else{
    // a()
    if (currentClassInfo.methods->count(node->identifier_1->name)){
      // in current class
      cName = currentClassName;
    }else{
      // not in current class
      std::string s = currentClassInfo.superClassName;
      while (!s.empty()){
	// check
	if ((*classTable)[s].methods->count(node->identifier_1->name)){
	  cName = s;
	  break;
	}

	s = (*classTable)[s].superClassName;
      }
      
    }
    p "push 8(%ebp)" e;

    p "call " + cName + "_" + node->identifier_1->name e;
    if (args > 0)
      p "add $" + std::to_string(args * 4 + 4) + ", %esp" e;
    p "push %eax" e; // return address
  }

  // remove args and return address from stack
  //p "pop %ebx" e;
  if (args > 0)
    p "sub $" + std::to_string(args * 4) + ", %esp" e;
  // return value
  //p "pop %eax" e;
  // restore caller-save registers
  //p "pop %edx" e;
  //p "pop %ecx" e;
  //p "pop %eax" e;
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
  node->visit_children(this);
  // in form of a or a.b
  int localOff, memOff;
  std::string cName, mName;
  
  if (node->identifier_2 != NULL){
    // find where node->identifier_1 is declared
    if (currentMethodInfo.variables->count(node->identifier_1->name) != 0){
      // declared in the current method
      localOff = currentMethodInfo.variables->at(node->identifier_1->name).offset;
      memOff = (*classTable)[currentMethodInfo.variables->at(node->identifier_1->name).type.objectClassName].members->at(node->identifier_2->name).offset;

      p "mov " + std::to_string(localOff) + "(%ebp), %ebx" e;
      p "mov " + std::to_string(memOff) + "(%ebx), %eax" e;
      p "push %eax" e;

    }else if (currentClassInfo.members->count(node->identifier_1->name) != 0){
      // declared in the current class
      localOff = currentClassInfo.members->at(node->identifier_1->name).offset;
      memOff = (*classTable)[currentClassInfo.members->at(node->identifier_1->name).type.objectClassName].members->at(node->identifier_2->name).offset;
      p "mov 8(%ebp), %ebx" e;
      p "mov " + std::to_string(localOff) + "(%ebx), %ebx" e;
      p "mov " + std::to_string(memOff) + "(%ebx), %eax" e;
      p "push %eax" e;
    }else {
    // TODO: add inheritance
    }
  }else {
    // a can be cur class or any superclass
    //cName = node->identifier_1->objectClassName;
    if (currentClassInfo.members->count(node->identifier_1->name) != 0){
      // global var
      cName = currentClassInfo.members->at(node->identifier_1->name).type.objectClassName;
      localOff = currentClassInfo.members->at(node->identifier_1->name).offset;
    }else{
      cName = currentClassName;
      while((*classTable)[cName].members->count(node->identifier_1->name) == 0)
	cName = (*classTable)[cName].superClassName;
      cName = (*classTable)[cName].members->at(node->identifier_1->name).type.objectClassName;
      localOff = (*classTable)[cName].members->at(node->identifier_1->name).offset;
    }

    if (currentMethodInfo.variables->count(node->identifier_1->name) != 0){
      // declared in current method
      localOff = currentMethodInfo.variables->at(node->identifier_1->name).offset;
      //memOff = (*classTable)[currentMethodInfo.variables->count(node->identifier_1->name).type.objectClassName].members->at(
      p "mov " + std::to_string(localOff) + "(%ebp), %ebx" e;
      //p "mov " + std::to_string(memOff) + "(%ebx), %eax" e;
      p "push %eax" e;
    }else if(currentClassInfo.members->count(node->identifier_1->name) != 0){
      // declared in current class
      localOff = currentClassInfo.members->at(node->identifier_1->name).offset;
      p "mov 8(%ebp), %ebx" e;
      p "mov " + std::to_string(localOff) + "(%ebx), %ebx" e;
      //p "mov " + std::to_string(memOff) + "(%ebx), %eax" e;
      p "push %eax" e;
    }else{
      // TODO: add inheritance
    }

  }

  
  
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
      // TODO: maybe add stuff?
      p "\tpush " + std::to_string(currentClassInfo.members->at(node->identifier->name).offset) + "(%ebp)" e;
    }else{
      // search for it in all super classes
      std::string sClassName = currentClassInfo.superClassName;
      int offset = 0;
      while (!sClassName.empty()){
	// TODO: MAKE OFFSETS BETTER?
	if ((*classTable)[sClassName].members->count(node->identifier->name)){
	  offset += (*classTable)[sClassName].members->at(node->identifier->name).offset;
	  p "\tpush" + std::to_string(offset) + "(%ebp)" e;
	  break;
	}
	sClassName = (*classTable)[sClassName].superClassName;
      }
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

  int sizeOfClass = (*classTable)[node->identifier->name].membersSize;

  // check for constructor
  if ((*classTable)[node->identifier->name].methods->count(node->identifier->name) != 0){
    // has a constructor
    int args = 0;
    if (node->expression_list != NULL){
      for (auto i=node->expression_list->rbegin(); i!=node->expression_list->rend(); ++i){
	(*i)->accept(this);
	args++;
      }
      p "\tpush $" + std::to_string(sizeOfClass) e;
      p "\tcall malloc" e;
      p "\tadd $4, %esp" e;
      p "\tpush %eax" e;
      p "\tcall " + node->identifier->name + "_" + node->identifier->name e;
      p "add $" + std::to_string(args * 4) + ", %esp" e;
      p "push %eax" e;
      
    }
  }else{
    // no constructor
    p "push $" + std::to_string(sizeOfClass) e;
    p "call malloc" e;
    p "add $4, %esp" e;
    p "push %eax" e;
  }
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
