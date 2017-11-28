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

/*
  Added helper methods for cleaner code
*/
bool IsAClass(std::string className, ClassTable *classTable){
  // checks to see if class exists in classTable
  if (className.compare(NULL)) return false;
  return (classTable)->count(className);
}
bool isVar(std::string var, VariableTable *currentVariableTable){
  // checks the current variableTable for the variable
  return (*currentVariableTable).count(var);
}
bool isVarOf(std::string var, std::string className, ClassTable *classTable){
  return (*classTable)[className].members->count(var);
}
bool isVarOfCurClass(std::string var, VariableTable *currentVariableTable, ClassTable *classTable, std::string currentClassName){
  // checks currentMethodTable and (*classTable)[currentClassName].members
  return (*currentVariableTable).count(var) || (*classTable)[currentClassName].members->count(var);
}
bool isMethod(std::string method, MethodTable *currentMethodTable){
  // checks the current class for method
  return (*currentMethodTable).count(method);
}
bool isMethodOf(std::string method, std::string className, ClassTable *classTable){
  // checks the class passed in if it contains the method
  return (*classTable)[className].methods->count(method);
}

bool IsASubClassOf(std::string className, std::string superClassName, ClassTable *classTable) {
  if (distance((*classTable).begin(), (*classTable).find(className)) >= distance((*classTable).begin(), (*classTable).find(superClassName)))
    return false;
  return true;
  
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

  currentClassName = node->identifier_1->name; // set the class name
  ClassInfo ci; 
  ci.methods = new MethodTable();
  ci.members = new VariableTable();

  // add superclass later

  currentVariableTable = ci.members;
  (*classTable).insert(std::pair<std::string, ClassInfo>(currentClassName, ci));

  node->visit_children(this);

  // add method size later
  (*classTable)[currentClassName].membersSize = 0;
  
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
  MethodInfo mi;
  //mi.returnType = node->type; // need to switch from basetype to compound type
  // add parameters from list<ParameterNode*> to list<CompoundType> 
  (*currentMethodTable).insert(std::pair<std::string, MethodInfo>(node->identifier->name, mi));
  //std::pair<std::string, ClassInfo>(currentClassName, ci)
  
  currentVariableTable = new VariableTable();
  (*currentMethodTable)[node->identifier->name].variables = currentVariableTable;

  node->visit_children(this);

  CompoundType ct;
  if(node->type->basetype == bt_object)
    ct.objectClassName = node->type->objectClassName;
  ct.baseType = node->type->basetype;
  (*currentMethodTable)[node->identifier->name].returnType = ct;
  
  if (node->type->basetype != node->methodbody->basetype){
    typeError(return_type_mismatch);
  }
  
  // check for null param's
  if (node->parameter_list != NULL){
    // we have param's
    std::list<CompoundType> *p = new std::list<CompoundType>();
    for (std::list<ParameterNode*>::iterator i=node->parameter_list->begin(); i != node->parameter_list->end(); i++){
      p->push_back(ConvertToCompoundType((*i)->type->basetype, (*i)->identifier->name));//create method??
    }
    (*currentMethodTable)[node->identifier->name].parameters = p;
  }

  // check if constructor returns None

  // update size
  (*currentMethodTable)[node->identifier->name].localsSize = 0;
  (*currentVariableTable);
}

void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {
  /*
    declaration_list
    statement_list
    returnstatement
   */
  node->visit_children(this);

  if (node->returnstatement == NULL)
    node->basetype = bt_none;
  else
    node->basetype = node->returnstatement->basetype;
  
}

void TypeCheck::visitParameterNode(ParameterNode* node) {
  /*
    type
    identifier
   */
  node->visit_children(this);

  node->basetype = node->type->basetype;
  if (node->basetype == bt_object) node->objectClassName = node->type->objectClassName;

  if (node->basetype == bt_object || node->basetype == bt_integer || node->basetype == bt_boolean){
    VariableInfo vi;
    //vi.type = ConvertToCompoundType(node->basetype, node->identifier->name);
    CompoundType ct;
    if(node->type->basetype == bt_object)
      ct.objectClassName = node->type->objectClassName;
    ct.baseType = node->type->basetype;
    vi.type = ct;
    vi.offset = currentParameterOffset;
    currentParameterOffset += 4;
    vi.size = 4;
    (*currentVariableTable).insert(std::pair<std::string, VariableInfo>(node->identifier->name, vi));
    //std::pair<std::string, ClassInfo>(currentClassName, ci)
  }else {
    typeError(undefined_class);
  }
}

void TypeCheck::visitDeclarationNode(DeclarationNode* node) {
  /*
    type
    identifier_list
   */
  node->visit_children(this);
  node->basetype = node->type->basetype;
  if (node->basetype == bt_object) 
    node->objectClassName = node->type->objectClassName;
  /*else 
    node->objectClassName = NULL;
  */
  if (IsAClass(node->objectClassName, classTable) || node->type->basetype==bt_integer || node->type->basetype==bt_boolean){
    
    for (std::list<IdentifierNode*>::iterator i = node->identifier_list->begin(); i != node->identifier_list->end(); i++ ){
      VariableInfo vi;
      //vi.type = ConvertToCompoundType(node->basetype, (*i)->name);
      CompoundType ct;
      if(node->type->basetype == bt_object)
	ct.objectClassName = node->type->objectClassName;
      ct.baseType = node->type->basetype;
      vi.type = ct;
      vi.offset = currentLocalOffset;
      currentLocalOffset -= 4;
      vi.size = 4;
      (*currentVariableTable).insert(std::pair<std::string, VariableInfo>((*i)->name, vi));
    }

  }else {
    typeError(undefined_class);
  }
  
}

void TypeCheck::visitReturnStatementNode(ReturnStatementNode* node) {
  /*
    expression
   */
  node->visit_children(this);
  
  node->basetype = node->expression->basetype;
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
    if (IsAClass(node->identifier_1->name, classTable) && IsAClass(node->identifier_2->name, classTable) &&  IsASubClassOf(node->identifier_1->name, node->identifier_2->name, classTable)){
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
    if (IsAClass(node->identifier_1->name, classTable)){
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
  if (node->expression->basetype != bt_boolean) typeError(if_predicate_type_mismatch);
}

void TypeCheck::visitWhileNode(WhileNode* node) {
  /*
    expression
    statement_list
   */
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean) typeError(if_predicate_type_mismatch);
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {
  /*
    statement_list
    expression
   */
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean) typeError(if_predicate_type_mismatch);
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

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  if (node->expression_1->basetype == node->expression_2->basetype){
    if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
      typeError(expression_type_mismatch);
  }else
    typeError(expression_type_mismatch);

  node->basetype = bt_integer;
}

void TypeCheck::visitMinusNode(MinusNode* node) {
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  if (node->expression_1->basetype == node->expression_2->basetype){
    if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
      typeError(expression_type_mismatch);
  }else
    typeError(expression_type_mismatch);

  node->basetype = bt_integer;
}

void TypeCheck::visitTimesNode(TimesNode* node) {
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  if (node->expression_1->basetype == node->expression_2->basetype){
    if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
      typeError(expression_type_mismatch);
  }else
    typeError(expression_type_mismatch);

  node->basetype = bt_integer;
}

void TypeCheck::visitDivideNode(DivideNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  if (node->expression_1->basetype == node->expression_2->basetype){
    if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
      typeError(expression_type_mismatch);
  }else
    typeError(expression_type_mismatch);

  node->basetype = bt_integer;
}

void TypeCheck::visitGreaterNode(GreaterNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  if (node->expression_1->basetype == node->expression_2->basetype){
    if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
      typeError(expression_type_mismatch);
  }else
    typeError(expression_type_mismatch);

  node->basetype = bt_boolean;
}

void TypeCheck::visitGreaterEqualNode(GreaterEqualNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  if (node->expression_1->basetype == node->expression_2->basetype)
    {
    if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
      typeError(expression_type_mismatch);
  }else
    typeError(expression_type_mismatch);

  node->basetype = bt_boolean;
}

void TypeCheck::visitEqualNode(EqualNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  BaseType e1=node->expression_1->basetype, e2=node->expression_2->basetype;
  if (e1 != e2)
    typeError(expression_type_mismatch);
  if (e1 != bt_integer || e1 != bt_boolean)
    typeError(expression_type_mismatch);
  
  node->basetype = bt_boolean;
}

void TypeCheck::visitAndNode(AndNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  BaseType e1=node->expression_1->basetype, e2=node->expression_2->basetype;
  if (e1 != e2)
    typeError(expression_type_mismatch);
  if (e1 != bt_boolean)
    typeError(expression_type_mismatch);
  
  node->basetype = bt_boolean;
}

void TypeCheck::visitOrNode(OrNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);

  //node->expression_1->basetype = node->expression_1->type->basetype;
  //node->expression_2->basetype = node->expression_2->type->basetype;

  BaseType e1=node->expression_1->basetype, e2=node->expression_2->basetype;
  if (e1 != e2)
    typeError(expression_type_mismatch);
  if (e1 != bt_boolean)
    typeError(expression_type_mismatch);
  
  node->basetype = bt_boolean;
}

void TypeCheck::visitNotNode(NotNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression
   */
  node->visit_children(this);

  //node->expression->basetype = node->expression->type->basetype;
  if (node->expression->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  
  node->basetype = bt_boolean;
}

void TypeCheck::visitNegationNode(NegationNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression
   */
  node->visit_children(this);

  //node->expression->basetype = node->expression->type->basetype;
  if (node->expression->basetype != bt_integer)
    typeError(expression_type_mismatch);
  
  node->basetype = bt_integer;
}

void TypeCheck::visitMethodCallNode(MethodCallNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    identifier_1
    identifier_2
    expression_list
   */
  node->visit_children(this);
  if (node->identifier_2 != NULL){
    // a.b(stuff)
    if (!isVar(node->identifier_1->name))
      typeError(undefined_variable);
    if (!isMethodOf(node->identifier_2->name, node->identifier_1->name))
      typeError(undefined_method);

    MethodTable *mt = (*classTable)[node->identifier_1->name].methods;
    node->basetype = (*mt)[node->identifier_2->name].returnType.baseType;
    if (node->basetype == bt_object)
      node->objectClassName = (*mt)[node->identifier_2->name].returnType.objectClassName;
    delete mt;

  }else{
    // a(stuff)
    if (!isMethod(node->identifier_1->name))
      typeError(undefined_method);

    node->basetype = (*currentMethodTable)[node->identifier_1->name].returnType.baseType;
    if (node->basetype == bt_object)
      node->objectClassName = (*currentMethodTable)[node->identifier_1->name].returnType.objectClassName;
  }
}

void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {
  /*
    identifier_1
    identifier_2
  */
  node->visit_children(this);
  if (IsAClass(node->identifier_1->name, classTable)){
    if (!isVarOf(node->identifier_2->name, node->identifier_1->name, currentVariableTable))
      typeError(undefined_member);
  }else
    typeError(undefined_class);

  VariableTable *vt = (*classTable)[node->identifier_1->name].members;
  node->basetype = (*vt)[node->identifier_2->name].type.baseType;

  delete vt;
}

void TypeCheck::visitVariableNode(VariableNode* node) {
  /*
    identifier_1
  */
  node->visit_children(this);
  if (!isVarOfCurClass(node->identifier->name, currentVariableTable, classTable, currentClassName))
    typeError(undefined_variable);
  // set type here?
}

void TypeCheck::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  node->basetype = bt_integer;
}

void TypeCheck::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  node->basetype = bt_boolean;
}

void TypeCheck::visitNewNode(NewNode* node) {
  /*
    identifier
    expression_list
   */
  node->visit_children(this);
  
  if (!IsAClass(node->identifier->name, classTable))
    typeError(undefined_class);
  
  MethodTable *mt = (*classTable)[node->identifier->name].methods;
  std::list<CompoundType> *params = (*mt)[node->identifier->name].parameters;
  if ((*params).size() != node->(*expression_list).size())
    typeError(argument_number_mismatch);
  
  std::list<ExpressionNode*>::iterator eIter = node->(*expression_list).begin();
  for (std::list<CompoundType>::iterator pIter = (*params).begin(); pIter != (*params).end(); pIter++){
    if ((*pIter).baseType != (*eIter)->basetype)
      typeError(argument_type_mismatch);
    eIter++;
  }

  node->basetype = bt_object;
  node->objectClassName = (*mt)[node->identifier->name].returnType.objectClassName;

  delete mt, params;
}

void TypeCheck::visitIntegerTypeNode(IntegerTypeNode* node) {
  node->basetype = bt_integer;
}

void TypeCheck::visitBooleanTypeNode(BooleanTypeNode* node) {
  node->basetype = bt_boolean;
}

void TypeCheck::visitObjectTypeNode(ObjectTypeNode* node) {
  node->basetype = bt_object;
  node->objectClassName = node->identifier->name;
}

void TypeCheck::visitNoneNode(NoneNode* node) {
  node->basetype = bt_none;
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
