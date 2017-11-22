#include "typecheck.hpp"

// Defines the function used to throw type errors. The possible
// type errors are defined as an enumeration in the header file.
void typeError(TypeErrorCode code) {
  switch (code) {
    case undefined_variable:
      std::cerr << "Undefined variable." << std::endl;
      break;
    case undefined_method:
      std::cerr << "Method does not exist." << std::endl;
      break;
    case undefined_class:
      std::cerr << "Class does not exist." << std::endl;
      break;
    case undefined_member:
      std::cerr << "Class member does not exist." << std::endl;
      break;
    case not_object:
      std::cerr << "Variable is not an object." << std::endl;
      break;
    case expression_type_mismatch:
      std::cerr << "Expression types do not match." << std::endl;
      break;
    case argument_number_mismatch:
      std::cerr << "Method called with incorrect number of arguments." << std::endl;
      break;
    case argument_type_mismatch:
      std::cerr << "Method called with argument of incorrect type." << std::endl;
      break;
    case while_predicate_type_mismatch:
      std::cerr << "Predicate of while loop is not boolean." << std::endl;
      break;
    case do_while_predicate_type_mismatch:
      std::cerr << "Predicate of do while loop is not boolean." << std::endl;
      break;
    case if_predicate_type_mismatch:
      std::cerr << "Predicate of if statement is not boolean." << std::endl;
      break;
    case assignment_type_mismatch:
      std::cerr << "Left and right hand sides of assignment types mismatch." << std::endl;
      break;
    case return_type_mismatch:
      std::cerr << "Return statement type does not match declared return type." << std::endl;
      break;
    case constructor_returns_type:
      std::cerr << "Class constructor returns a value." << std::endl;
      break;
    case no_main_class:
      std::cerr << "The \"Main\" class was not found." << std::endl;
      break;
    case main_class_members_present:
      std::cerr << "The \"Main\" class has members." << std::endl;
      break;
    case no_main_method:
      std::cerr << "The \"Main\" class does not have a \"main\" method." << std::endl;
      break;
    case main_method_incorrect_signature:
      std::cerr << "The \"main\" method of the \"Main\" class has an incorrect signature." << std::endl;
      break;
  }
  exit(1);
}

// TypeCheck Visitor Functions: These are the functions you will
// complete to build the symbol table and type check the program.
// Not all functions must have code, many may be left empty.

void TypeCheck::visitProgramNode(ProgramNode* node) {
  /*
    list<ClassNode*>* class_list
   */
  // create class table, 1 class table per program
  classTable = new ClassTable();
  // visit every children of this node to build the symbol table
  node->visit_children(this);
}

void TypeCheck::visitClassNode(ClassNode* node) {
  /*
    id_1=name
    id_2=superClassName
    list<DeclarationNode*>* declaration_list
    list<MethodNode*>* method_list
   */
  // clear all previously used references
  currentMethodTable = NULL;
  currentVariableTable = NULL;
  // this member will have the correct offset
  currentMemberOffset = 0;

  currentClassName = node->identifier_1; // set the class name
  ClassInfo ci = new ClassInfo; 
  ci.methods = new MethodTable();
  ci.members = new VariableTable();

  // add superclass later

  currentVariableTable = &ci.members;
  classTable.insert(currentClassName, ci);

  node->visit_children(this);

  // add method size later
  (*classTable)[currentClassName].memberSize = 0;
  
}

void TypeCheck::visitMethodNode(MethodNode* node) {
  /*
    identifier=name
    parameter_list
    type=return type
    methodbody
   */
  currentLocalOffset = -4;
  currentParameterOffset = 12;

  currentMethodTable = classTable->at(currentClassName).methods;
  MethodInfo mi = new MethodInfo();
  mi.returnType = node->type; // need to switch from basetype to compound type
  // add parameters from list<ParameterNode*> to list<CompoundType> 
  currentMethodTable.insert(node->identifier->name, mi);
  
  currentVariableTable = new VariableTable();
  (*currentMethodTable)[node->identifier->name]->variables = currentVariableTable;

  node->visit_children(this);
  
  // check for null param's
  if (node->parameter_list != NULL){
    // we have param's
    std::list<CompoundType> p = new list<CompoundType>();
    for (std::iterator<ParameterNode*> *i=node->parameter_list.begin(); i!=node->parameter_list.end(); i++){
      p->push_back(ConvetToCompoundType((*i)->type->basetype, (*i)->identifier->name));
    }
    (*currentMethodTable)[node->identifier->name].parameters = &p;
  }

  // check if constructor returns None

  // update size
  (*currentMethodTable)[node->identifier->name].localsSize = 0;
  (*currentVariableTable)
}

void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {
  /*
    declaration_list
    statement_list
    returnstatement
   */
  node->visit_children(this);

  // check if return type is same as when method was defined
  // throw error if not
}

void TypeCheck::visitParameterNode(ParameterNode* node) {
  /*
    type
    identifier
   */
  // check if param is a class, integer, or boolean
  if (node->type == IntegerTypeNode) node->basetype = bt_integer;
  else if (node->type == BooleanTypeNode) node->basetype=bt_boolean;
  else node->basetype=bt_object;
    
  VariableInfo vi;
  vi.type = ConvetToCompoundType(node->basetype, node->identifier->name);
  vi.offset = currentParameterOffset;
  currentParameterOffset += 4;
  vi.size = 4;
  (*currentVariableTable).insert(node->identifier->name, vi);
  // throw error if not
}

void TypeCheck::visitDeclarationNode(DeclarationNode* node) {
  /*
    type
    identifier_list
   */
  if (node->type == IntegerTypeNode) node->basetype = bt_integer;
  else if (node->type == BooleanTypeNode) node->basetype=bt_boolean;
  else node->basetype=bt_object;
  //  if (IsAClass() || node->type==IntegerType || node->type==BooleanType){
    
    for (std::iterator<IdentifierNode*> i = node->identifier_list.begin();
	 i != node->identifier_list.end();
	 i++ ){
      
      VariableInfo vi;
      vi.type = ConvetToCompoundType(node->basetype, i->name);
      vi.offset = currentLocalOffset;
      currentLocalOffset -= 4;
      vi.size = 4;
      (*currentVariableTable).insert(i->name, vi);
    }
    //  }else {
    // throw exception
    //  }
  
}

void TypeCheck::visitReturnStatementNode(ReturnStatementNode* node) {
  /*
    expression
   */
  node->visit_children(this);
  // set expression type??
}

void TypeCheck::visitAssignmentNode(AssignmentNode* node) {
  /*
    identifier_1
    identifier_2
    expression
   */
  node->visit_children(this);
  
  if (node->identifier_2 != NULL){
    // a.b = (expr)
    // check if type of a.b is equal to type of expr
    // check if a is vaild class or int or bool
    // check if b is valid subclass of a
    if (IsAClass(node->identifier_1) && IsASubClassOf(node->identifier_1, node->identifier_2)){
      if (node->identifier_2->basetype != node->expression->basetype)
	// throw error
	typeError(errorCode);
    }else{
      // var not defined
      // throw error
      typeError(undefined_variable);
    } 
  }else {
    // a = (expr)
    // check if type of a is equal to type of expr
    // check if a is vaild class or int or bool
    if (IsAClass(node->identifier)){
      if (node->identifier_1->basetype != node->expression->basetype)
	typeError(errorCode);
    }else
      typeError(undefined_variable);
  }
}

void TypeCheck::visitCallNode(CallNode* node) {
  /*
    methodcall
  */
  
  node->visit_children(this);
}

void TypeCheck::visitIfElseNode(IfElseNode* node) {
  /*
    expression
    statement_list_1
    statement_list_2
   */
  node->visit_children(this);
}

void TypeCheck::visitWhileNode(WhileNode* node) {
  /*
    expression
    statement_list
   */
  node->visit_children(this);
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {
  /*
    statement_list
    expression
   */
  node->visit_children(this);
}

void TypeCheck::visitPrintNode(PrintNode* node) {
  /*
    expression
   */
  node->visit_children(this);
}

void TypeCheck::visitPlusNode(PlusNode* node) {
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitMinusNode(MinusNode* node) {
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitTimesNode(TimesNode* node) {
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitDivideNode(DivideNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitGreaterNode(GreaterNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitGreaterEqualNode(GreaterEqualNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitEqualNode(EqualNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitAndNode(AndNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitOrNode(OrNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
}

void TypeCheck::visitNotNode(NotNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression
   */
  node->visit_children(this);
}

void TypeCheck::visitNegationNode(NegationNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression
   */
  node->visit_children(this);
}

void TypeCheck::visitMethodCallNode(MethodCallNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    identifier_1
    identifier_2
    expression_list
   */
  node->visit_children(this);
}

void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitVariableNode(VariableNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitIntegerLiteralNode(IntegerLiteralNode* node) {

}

void TypeCheck::visitBooleanLiteralNode(BooleanLiteralNode* node) {

}

void TypeCheck::visitNewNode(NewNode* node) {
  /*
    identifier
    expression_list
   */
  node->visit_children(this);
}

void TypeCheck::visitIntegerTypeNode(IntegerTypeNode* node) {

}

void TypeCheck::visitBooleanTypeNode(BooleanTypeNode* node) {

}

void TypeCheck::visitObjectTypeNode(ObjectTypeNode* node) {

}

void TypeCheck::visitNoneNode(NoneNode* node) {

}

void TypeCheck::visitIdentifierNode(IdentifierNode* node) {

}

void TypeCheck::visitIntegerNode(IntegerNode* node) {

}


// The following functions are used to print the Symbol Table.
// They do not need to be modified at all.

std::string genIndent(int indent) {
  std::string string = std::string("");
  for (int i = 0; i < indent; i++)
    string += std::string(" ");
  return string;
}

std::string string(CompoundType type) {
  switch (type.baseType) {
    case bt_integer:
      return std::string("Integer");
    case bt_boolean:
      return std::string("Boolean");
    case bt_none:
      return std::string("None");
    case bt_object:
      return std::string("Object(") + type.objectClassName + std::string(")");
    default:
      return std::string("");
  }
}


void print(VariableTable variableTable, int indent) {
  std::cout << genIndent(indent) << "VariableTable {";
  if (variableTable.size() == 0) {
    std::cout << "}";
    return;
  }
  std::cout << std::endl;
  for (VariableTable::iterator it = variableTable.begin(); it != variableTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << string(it->second.type);
    std::cout << ", " << it->second.offset << ", " << it->second.size << "}";
    if (it != --variableTable.end())
      std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}";
}

void print(MethodTable methodTable, int indent) {
  std::cout << genIndent(indent) << "MethodTable {";
  if (methodTable.size() == 0) {
    std::cout << "}";
    return;
  }
  std::cout << std::endl;
  for (MethodTable::iterator it = methodTable.begin(); it != methodTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
    std::cout << genIndent(indent + 4) << string(it->second.returnType) << "," << std::endl;
    std::cout << genIndent(indent + 4) << it->second.localsSize << "," << std::endl;
    print(*it->second.variables, indent + 4);
    std::cout <<std::endl;
    std::cout << genIndent(indent + 2) << "}";
    if (it != --methodTable.end())
      std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}";
}

void print(ClassTable classTable, int indent) {
  std::cout << genIndent(indent) << "ClassTable {" << std::endl;
  for (ClassTable::iterator it = classTable.begin(); it != classTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
    if (it->second.superClassName != "")
      std::cout << genIndent(indent + 4) << it->second.superClassName << "," << std::endl;
    print(*it->second.members, indent + 4);
    std::cout << "," << std::endl;
    print(*it->second.methods, indent + 4);
    std::cout <<std::endl;
    std::cout << genIndent(indent + 2) << "}";
    if (it != --classTable.end())
      std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}" << std::endl;
}

void print(ClassTable classTable) {
  print(classTable, 0);
}
