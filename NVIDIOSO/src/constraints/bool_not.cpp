//
//  bool_not.cpp
//  iNVIDIOSO
//
//  Created by Federico Campeotto on 07/07/15.
//  Copyright (c) 2014-2015 ___UDNMSU___. All rights reserved.
//

#include "bool_not.h"

BoolNot::BoolNot () :
FZNConstraint ( BOOL_NOT ) {
  /*
   * Set the event that trigger this constraint.
   * @note if no event is set, this constraint will never be re-evaluated.
   */
  //set_event( EventType::SINGLETON_EVT );
}//IntNe

BoolNot::BoolNot ( std::vector<VariablePtr> vars, std::vector<std::string> args ) :
BoolNot () {
  setup ( vars, args );
}//IntNe

BoolNot::~BoolNot () {}

void
BoolNot::setup ( std::vector<VariablePtr> vars, std::vector<std::string> args )
{
}//setup

const std::vector<VariablePtr>
BoolNot::scope () const
{
  // Return the constraint's scope
  std::vector<VariablePtr> scope;
  return scope;
}//scope

void
BoolNot::consistency ()
{
}//consistency

//! It checks if
bool
BoolNot::satisfied ()
{
  return true;
}//satisfied

//! Prints the semantic of this constraint
void
BoolNot::print_semantic () const
{
    FZNConstraint::print_semantic ();
}//print_semantic



