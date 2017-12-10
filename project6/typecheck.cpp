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
  //std::cout<<"ISACLASS"<<std::endl;
  // checks to see if class exists in classTable
  if (className.empty()) return false;

  if (classTable->count(className) == 1){
    //std::cout<<"true"<<std::endl;
    return true;
  }else{
    //std::cout<<"false"<<std::endl;
    return false;
  }
}
bool isVar(std::string var, VariableTable *currentVariableTable){
  // checks the current variableTable for the variable
  return ((*currentVariableTable).count(var) == 1);
}

bool isVarOf(std::string var, std::string className, ClassTable *classTable){
  if ((*classTable)[className].members->count(var))
    return true;
  else{
    if ((*classTable)[className].superClassName.empty())
      return false;
    return isVarOf(var, (*classTable)[className].superClassName, classTable);
  }
  //  return ((*classTable)[className].members->count(var) == 1);
}
VariableInfo getVIForMember(std::string var, std::string className, ClassTable *classTable){
  //std::cout<<"in vi"<<std::endl;
  if ((*classTable)[className].members->count(var)==1){
    //std::cout<<"found"<<std::endl;
    return (*classTable)[className].members->at(var);
  }
  else{
    //std::cout<<"rec"<<std::endl;
    return getVIForMember(var, (*classTable)[className].superClassName, classTable);
  }
}

bool isVarOfCurClass(std::string var, VariableTable *currentVariableTable, ClassTable *classTable, std::string currentClassName){
  // checks currentMethodTable and (*classTable)[currentClassName].members
  return (((*currentVariableTable).count(var)==1) || ((*classTable)[currentClassName].members->count(var)==1));
}
bool isMethod(std::string method, MethodTable *currentMethodTable){
  // checks the current class for method
  return ((*currentMethodTable).count(method)==1);
}

bool isMethodOf(std::string method, std::string className, ClassTable *classTable){
  // checks the class passed in if it contains the method
  if ((*classTable)[className].methods->count(method) == 1){
    return true;
  }else{
    if ((*classTable)[className].superClassName.empty()){
      return false;
    }else
      return isMethodOf(method, (*classTable)[className].superClassName, classTable);
  }
  //  return ((*classTable)[className].methods->count(method)==1);
}
MethodInfo getMIForMethod(std::string method, std::string className, ClassTable *classTable){
  //std::cout<<"in getMI"<<std::endl;
  if ((*classTable)[className].methods->count(method)==1){
    //std::cout<<"found method"<<std::endl;
    return (*classTable)[className].methods->at(method);
  }else{
    //std::cout<<"rec method"<<std::endl;
    return getMIForMethod(method, (*classTable)[className].superClassName, classTable);
  }
}

bool IsASubClassOf(std::string className, std::string superClassName, ClassTable *classTable) {
  if (distance((*classTable).begin(), (*classTable).find(className)) >= distance((*classTable).begin(), (*classTable).find(superClassName)))
    return false;
  return true;
  
}
CompoundType ConvertToCompoundType(BaseType bt, std::string name){
  CompoundType ct;
  if(bt == bt_object)
    ct.objectClassName = name;
  ct.baseType = bt;

  return ct;
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

  if ((*classTable).count("Main") == 1){
    // there's a Main class
    if ((*classTable)["Main"].members->size() != 0)
      typeError(main_class_members_present);
    if ((*classTable)["Main"].methods->count("main") != 1)
      typeError(no_main_method);
    if ((*classTable)["Main"].methods->at("main").returnType.baseType != bt_none)
      typeError(main_method_incorrect_signature);
  }else
    typeError(no_main_class);
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

  std::string superClassName = "";
  // superclass check
  if (node->identifier_2 != NULL){
    if (IsAClass(node->identifier_2->name, classTable)){
      superClassName = node->identifier_2->name;
      // add all members of super class to our class
      /*
      for (auto i = (*classTable)[superClassName].members->begin(); i!=(*classTable)[superClassName].members->end(); ++i){
	ci.members->insert(std::pair<std::string, VariableInfo>((*i).first, { (*i).second.type, currentMemberOffset+=4, 4 }));
	//ci.members[(*i).first] = { (*i).second.type, currentMemberOffset+=4, 4 };
      }
      */
    }else{
      typeError(undefined_class);
    }
  }
  ci.superClassName = superClassName;

  //currentVariableTable = ci.members;
  (*classTable).insert(std::pair<std::string, ClassInfo>(currentClassName, ci));
  
  //print(*classTable);
  node->visit_children(this);
  
 
  (*classTable)[currentClassName].membersSize = (*classTable)[currentClassName].members->size() * 4;
  
}

void TypeCheck::visitMethodNode(MethodNode* node) {
  /*
    identifier=name
    parameter_list
    type=return type
    methodbody
   */
  //print(*classTable);
  currentLocalOffset = -4;
  currentParameterOffset = 12;

  currentMethodTable = classTable->at(currentClassName).methods;
  MethodInfo mi;
  mi.returnType = ConvertToCompoundType(bt_none, "");
  mi.variables = NULL;
  mi.parameters = new std::list<CompoundType>();
  mi.localsSize = 0;
  
  (*currentMethodTable).insert(std::pair<std::string, MethodInfo>(node->identifier->name, mi));
    
  currentVariableTable = new VariableTable();
  (*currentMethodTable)[node->identifier->name].variables = currentVariableTable;

  //std::cout<<currentClassName<<std::endl;
  node->visit_children(this);
  //std::cout<<currentClassName<<std::endl;
  
  (*currentMethodTable)[node->identifier->name].returnType = ConvertToCompoundType(node->type->basetype, node->type->objectClassName);
  
  if (node->type->basetype != node->methodbody->basetype){
    typeError(return_type_mismatch);
  }
  
  // check for null param's
  if (!node->parameter_list->empty()){
    // we have param's
    //std::list<CompoundType> *p = new std::list<CompoundType>();
    for (std::list<ParameterNode*>::iterator i=node->parameter_list->begin(); i != node->parameter_list->end(); i++){
      (*classTable)[currentClassName].methods->at(node->identifier->name).parameters->push_back(ConvertToCompoundType((*i)->basetype, (*i)->objectClassName));
      //p->push_back(ConvertToCompoundType((*i)->basetype, (*i)->objectClassName));//create method??
    }
    //(*currentMethodTable)[node->identifier->name].parameters = p;
    //delete p;
  }


  // check if constructor returns None
  if (currentClassName.compare(node->identifier->name) == 0){
    // its a constructor
    if (node->methodbody->basetype != bt_none)
      typeError(constructor_returns_type);
  }

  // update size
  (*currentMethodTable)[node->identifier->name].localsSize = -4 - currentLocalOffset;
  
}

void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {
  /*
    declaration_list
    statement_list
    returnstatement
   */
  //print(*classTable);
  //std::cout<<"before visit children in MethodBodyNode"<<std::endl;
  node->visit_children(this);
  //std::cout<<"after visit children in MethodBodyNode"<<std::endl;
  //print(*classTable);
  
  if (node->returnstatement == NULL){
    node->basetype = bt_none;
  }else
    node->basetype = node->returnstatement->basetype;
  
}

void TypeCheck::visitParameterNode(ParameterNode* node) {
  /*
    type
    identifier
   */
  //print(*classTable);
  //std::cout<<currentClassName<<std::endl;
  node->visit_children(this);
  //std::cout<<currentClassName<<std::endl;
  //print(*classTable);

  //std::cout<<node->identifier->name<<std::endl;
  //node->identifier->basetype = node->type->basetype;
  
  node->basetype = node->type->basetype;
  if (node->basetype == bt_object) node->objectClassName = node->type->objectClassName;

  if (node->basetype == bt_object || node->basetype == bt_integer || node->basetype == bt_boolean){
    VariableInfo vi;
    //vi.type = ConvertToCompoundType(node->basetype, node->identifier->name);
    /*
    CompoundType ct;
    if(node->type->basetype == bt_object)
      ct.objectClassName = node->type->objectClassName;
    ct.baseType = node->type->basetype;
    vi.type = ct;
    */
    vi.type = ConvertToCompoundType(node->basetype, node->objectClassName);
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
  //std::cout<<currentClassName<<std::endl;
  
  node->visit_children(this);
  //std::cout<<"after visit"<<std::endl;
  //std::cout<<currentClassName<<std::endl;
   
  node->basetype = node->type->basetype;
  if (node->basetype == bt_object)
    node->objectClassName = node->type->objectClassName;

  
  bool isMember = false;
  if (currentVariableTable == NULL)
    isMember = true;

  //std::cout<<"before if"<<std::endl;
  if (IsAClass(node->type->objectClassName, classTable) || node->type->basetype==bt_integer || node->type->basetype==bt_boolean){
    //std::cout<<"IN IF"<<std::endl;
    for (std::list<IdentifierNode*>::iterator i = node->identifier_list->begin(); i != node->identifier_list->end(); i++ ){
      VariableInfo vi;
      //std::cout<<"before convert"<<std::endl;
      vi.type = ConvertToCompoundType(node->basetype, node->objectClassName);
      //std::cout<<"after convert"<<std::endl;
      vi.size = 4;
      if (isMember){
	vi.offset = currentMemberOffset;
	currentMemberOffset += 4;
	(*classTable)[currentClassName].members->insert(std::pair<std::string, VariableInfo>((*i)->name, vi));
      }else{
	vi.offset = currentLocalOffset;
	currentLocalOffset -= 4;
	(*currentVariableTable).insert(std::pair<std::string, VariableInfo>((*i)->name, vi));
      }
      (*i)->basetype = node->type->basetype;
      if ((*i)->basetype == bt_object)
	(*i)->objectClassName = node->type->objectClassName;
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
  //std::cout<<"In assignment Node"<<std::endl;
  node->visit_children(this);
  //std::cout<<"after visit children"<<std::endl;

  if (node->identifier_2 != NULL){
    // a.b = (expr)
    // check if type of a.b is equal to type of expr
    // check if a is vaild class or int or bool
    // check if b is valid subclass of a
    //std::cout<<"assignment node id2 not null"<<std::endl;

    if (isVarOfCurClass(node->identifier_1->name, currentVariableTable, classTable, currentClassName)){
      VariableInfo vi;
      if ((*currentVariableTable).count(node->identifier_1->name)==1)
	vi = (*currentVariableTable)[node->identifier_1->name];
      else
	vi = (*classTable)[currentClassName].members->at(node->identifier_1->name);
      //VariableInfo vi = (*currentVariableTable)[node->identifier_1->name];
      if (IsAClass(vi.type.objectClassName, classTable)){

	if (isVarOf(node->identifier_2->name, vi.type.objectClassName, classTable)){
	  //if (node->identifier_2->basetype != node->expression->basetype)
	  VariableInfo vi_2 = getVIForMember(node->identifier_2->name, vi.type.objectClassName, classTable);
	//if ((*classTable)[vi.type.objectClassName].members->at(node->identifier_2->name).type.baseType != node->expression->basetype)
	  if (vi_2.type.baseType != node->expression->basetype)
	    typeError(assignment_type_mismatch);
	}else
	  typeError(undefined_member);
	
	
      }else{
	typeError(not_object);
      }
    }else{
      
      typeError(undefined_variable);
    }


  }else {
    // a = (expr)
    // check if type of a is equal to type of expr
    // check if a is vaild class or int or bool
    if (!isVarOfCurClass(node->identifier_1->name, currentVariableTable, classTable, currentClassName)){
      
      typeError(undefined_variable);
    }
    //if (node->identifier_1->basetype != node->expression->basetype)
    //    typeError(assignment_type_mismatch);
    VariableInfo vi;
    if ((*currentVariableTable).count(node->identifier_1->name)==1)
      vi = (*currentVariableTable)[node->identifier_1->name];
    else
      vi = (*classTable)[currentClassName].members->at(node->identifier_1->name);
    if (vi.type.baseType != node->expression->basetype)
      typeError(assignment_type_mismatch);
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
  //std::cout<<currentClassName<<std::endl;
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean) typeError(while_predicate_type_mismatch);
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {
  /*
    statement_list
    expression
   */
  //std::cout<<currentClassName<<std::endl;
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean) typeError(do_while_predicate_type_mismatch);
}

void TypeCheck::visitPrintNode(PrintNode* node) {
  /*
    expression
   */
  //std::cout<<"In PrintNode"<<std::endl;
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
    if (node->expression_1->basetype != bt_integer && node->expression_2->basetype != bt_integer)
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
    if (node->expression_1->basetype != bt_integer && node->expression_2->basetype != bt_integer)
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
    if (node->expression_1->basetype != bt_integer && node->expression_2->basetype != bt_integer)
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
  //std::cout<<"Divide Node"<<std::endl;
  node->visit_children(this);

  
  if (node->expression_1->basetype == node->expression_2->basetype){
    if (node->expression_1->basetype != bt_integer && node->expression_2->basetype != bt_integer)
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
    if (node->expression_1->basetype != bt_integer && node->expression_2->basetype != bt_integer)
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
    if (node->expression_1->basetype != bt_integer && node->expression_2->basetype != bt_integer)
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
  if (e1 != bt_integer && e1 != bt_boolean){
    typeError(expression_type_mismatch);
  }
  
  node->basetype = bt_boolean;
}

void TypeCheck::visitAndNode(AndNode* node) {
  // WRITEME: Replace with code if necessary
  /*
    expression_1
    expression_2
   */
  node->visit_children(this);
  
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
  //std::cout<<"MethodCallNode"<<std::endl;
  node->visit_children(this);
  //std::cout<<"MethodCallNode"<<std::endl;
  if (node->identifier_2 != NULL){
    // a.b(stuff)
    if (!isVarOfCurClass(node->identifier_1->name, currentVariableTable, classTable, currentClassName)){
      typeError(undefined_variable);
    }

    VariableInfo vi;
    if ((*currentVariableTable).count(node->identifier_1->name)==1)
      vi = (*currentVariableTable)[node->identifier_1->name];
    else
      vi = (*classTable)[currentClassName].members->at(node->identifier_1->name);
    
    if (IsAClass(vi.type.objectClassName, classTable)){

      if (!isMethodOf(node->identifier_2->name, vi.type.objectClassName, classTable)){

	typeError(undefined_method);
      }

      MethodInfo mi = getMIForMethod(node->identifier_2->name, vi.type.objectClassName, classTable);

      // check sizes of expression_list
      if (mi.parameters->size() != node->expression_list->size()){
	typeError(argument_number_mismatch);
      }

      std::list<ExpressionNode*>::iterator eIter = node->expression_list->begin();
      for (std::list<CompoundType>::iterator pIter = mi.parameters->begin(); pIter != mi.parameters->end(); pIter++){
	if ((*pIter).baseType != (*eIter)->basetype)
	  typeError(argument_type_mismatch);
	eIter++;
      }
      
      node->basetype = mi.returnType.baseType;
      if (node->basetype == bt_object)
	node->objectClassName = mi.returnType.objectClassName;
	
    } else{
      //std::cout<<"HERE"<<std::endl;
      typeError(not_object);
    }
  }else{
    // a(stuff)
    if (!isMethod(node->identifier_1->name, currentMethodTable)){

      typeError(undefined_method);
    }

    //check sizes of lists
    if ((*currentMethodTable)[node->identifier_1->name].parameters->size() != node->expression_list->size()){
      typeError(argument_number_mismatch);
    }

    std::list<ExpressionNode*>::iterator eIter = node->expression_list->begin();
      for (std::list<CompoundType>::iterator pIter = (*currentMethodTable)[node->identifier_1->name].parameters->begin(); pIter != (*currentMethodTable)[node->identifier_1->name].parameters->end(); pIter++){
	if ((*pIter).baseType != (*eIter)->basetype)
	  typeError(argument_type_mismatch);
	eIter++;
      }

    node->basetype = (*currentMethodTable)[node->identifier_1->name].returnType.baseType;
    if (node->basetype == bt_object)
      node->objectClassName = (*currentMethodTable)[node->identifier_1->name].returnType.objectClassName;
  }
  //std::cout<<"At END"<<std::endl;
}

void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {
  /*
    identifier_1
    identifier_2
  */
  
  node->visit_children(this);
  
  //std::cout<<"In member access"<<std::endl;
  if (isVarOfCurClass(node->identifier_1->name, currentVariableTable, classTable, currentClassName)||isVarOf(node->identifier_1->name, currentClassName, classTable)){
    //std::cout<<"In if"<<std::endl;
    VariableInfo vi;
    if (isVarOf(node->identifier_1->name, currentClassName, classTable)){
      vi = getVIForMember(node->identifier_1->name, currentClassName, classTable);
    }else if ((*currentVariableTable).count(node->identifier_1->name)==1)
      vi = (*currentVariableTable)[node->identifier_1->name];
    else
      vi = (*classTable)[currentClassName].members->at(node->identifier_1->name);


    //std::cout<<"before IsAClass"<<std::endl;
    if (IsAClass(vi.type.objectClassName, classTable)){
      if (!isVarOf(node->identifier_2->name, vi.type.objectClassName, classTable))
	      typeError(undefined_member);
      VariableInfo vi_2 = getVIForMember(node->identifier_2->name, vi.type.objectClassName, classTable);
      node->basetype = vi_2.type.baseType;
      if (node->basetype==bt_object)
	node->objectClassName = vi_2.type.objectClassName;
      //std::cout<<"after if"<<std::endl;


      
    }else{
      typeError(undefined_class);
    }
  }else{
    typeError(undefined_variable);
  }
}

void TypeCheck::visitVariableNode(VariableNode* node) {
  /*
    identifier_1
  */
  node->visit_children(this);
  node->basetype = node->identifier->basetype;
  if (node->basetype==bt_object)
    node->objectClassName = node->identifier->objectClassName;
  
  if (!isVarOfCurClass(node->identifier->name, currentVariableTable, classTable, currentClassName)){
    if (!isVarOf(node->identifier->name, currentClassName, classTable)){
      typeError(undefined_variable);
    }
    //std::cout<<"here"<<std::endl;
    //std::cout<<node->identifier->name<<std::endl;
    
  }
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

  
  if ((*classTable)[node->identifier->name].methods->count(node->identifier->name) != 0){
      
    if ((*classTable)[node->identifier->name].methods->at(node->identifier->name).parameters->size() != node->expression_list->size()){
  
      typeError(argument_number_mismatch);
    }
  
  
  /*  
  if (!isMethodOf(node->identifier->name, node->identifier->name, classTable))
    typeError(undefined_method);

  MethodInfo mi = getMIForMethod(node->identifier->name, node->identifier->name, classTable);
  /*
  MethodTable *mt = (*classTable)[node->identifier->name].methods;
  std::list<CompoundType> *params = (*mt)[node->identifier->name].parameters;
  if ((*params).size() != node->expression_list->size())
    typeError(argument_number_mismatch);
  */
  /*
  if ((*classTable)[node->identifier->name].methods->at(node->identifier->name).parameters->size() != node->expression_list->size()){
    std::cout<<(*classTable)[node->identifier->name].methods->at(node->identifier->name).parameters->size()<<std::endl;
    std::cout<<node->expression_list->size()<<std::endl;
    typeError(argument_number_mismatch);
  }
  
 
  if (mi.parameters->size() != node->expression_list->size()){
    typeError(argument_number_mismatch);
  }
  */
  
  std::list<ExpressionNode*>::iterator eIter = node->expression_list->begin();
  for (std::list<CompoundType>::iterator pIter = (*classTable)[node->identifier->name].methods->at(node->identifier->name).parameters->begin(); pIter != (*classTable)[node->identifier->name].methods->at(node->identifier->name).parameters->end(); pIter++){
    if ((*pIter).baseType != (*eIter)->basetype)
      typeError(argument_type_mismatch);
    eIter++;
  }

  node->basetype = bt_object;
  node->objectClassName = (*classTable)[node->identifier->name].methods->at(node->identifier->name).returnType.objectClassName;
  }else{
    node->basetype = bt_object;
    node->objectClassName = node->identifier->name;
  }
  
  //node->objectClassName = mi.returnType.objectClassName;
  //delete mt, params;
}

void TypeCheck::visitIntegerTypeNode(IntegerTypeNode* node) {
  node->basetype = bt_integer;
  //std::cout<<currentClassName+"setting int"<<std::endl;
}

void TypeCheck::visitBooleanTypeNode(BooleanTypeNode* node) {
  node->basetype = bt_boolean;
  //std::cout<<currentClassName + "setting boolean"<<std::endl;
}

void TypeCheck::visitObjectTypeNode(ObjectTypeNode* node) {
  node->basetype = bt_object;
  node->objectClassName = node->identifier->name;
  //std::cout<<currentClassName+"setting obj"<<std::endl;
}

void TypeCheck::visitNoneNode(NoneNode* node) {
  node->basetype = bt_none;
  //std::cout<<currentClassName + "setting none"<<std::endl;
}

void TypeCheck::visitIdentifierNode(IdentifierNode* node) {
  if (isVarOf(node->name, currentClassName, classTable)){
    VariableInfo vi = getVIForMember(node->name, currentClassName, classTable);
    node->basetype = vi.type.baseType;
    if (node->basetype == bt_object)
      node->objectClassName = vi.type.objectClassName;
  }else if (currentVariableTable!=NULL && isVarOfCurClass(node->name, currentVariableTable, classTable, currentClassName)){
    node->basetype = (*currentVariableTable)[node->name].type.baseType;
    if (node->basetype == bt_object)
      node->objectClassName = (*currentVariableTable)[node->name].type.objectClassName;
  }
  
  //print(*classTable);
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
