//
//  set_le.cpp
//  iNVIDIOSO
//
//  Created by Federico Campeotto on 07/07/15.
//  Copyright (c) 2014-2015 ___UDNMSU___. All rights reserved.
//

#include "set_le.h"

SetLe::SetLe () :
FZNConstraint ( SET_LE ) {
  /*
   * Set the event that trigger this constraint.
   * @note if no event is set, this constraint will never be re-evaluated.
   */
  //set_event( EventType::SINGLETON_EVT );
}//SetLe

SetLe::SetLe ( std::vector<VariablePtr> vars, std::vector<std::string> args ) :
SetLe () {
  setup ( vars, args );
}//SetLe

SetLe::~SetLe () {}

void
SetLe::setup ( std::vector<VariablePtr> vars, std::vector<std::string> args )
{
}//setup

const std::vector<VariablePtr>
SetLe::scope () const
{
  // Return the constraint's scope
  std::vector<VariablePtr> scope;
  return scope;
}//scope

void
SetLe::consistency ()
{
}//consistency

//! It checks if
bool
SetLe::satisfied ()
{
  return true;
}//satisfied

//! Prints the semantic of this constraint
void
SetLe::print_semantic () const
{
    FZNConstraint::print_semantic ();
}//print_semantic



