//
//  array_int_element.cpp
//  iNVIDIOSO
//
//  Created by Federico Campeotto on 07/07/15.
//  Copyright (c) 2014-2015 ___UDNMSU___. All rights reserved.
//

#include "array_int_element.h"

ArrayIntElement::ArrayIntElement () :
FZNConstraint ( INT_NE ) {
  /*
   * Set the event that trigger this constraint.
   * @note if no event is set, this constraint will never be re-evaluated.
   */
  //set_event( EventType::SINGLETON_EVT );
}//IntNe

ArrayIntElement::ArrayIntElement ( std::vector<VariablePtr> vars, std::vector<std::string> args ) :
ArrayIntElement () {
  setup ( vars, args );
}//IntNe

ArrayIntElement::~ArrayIntElement () {}

void
ArrayIntElement::setup ( std::vector<VariablePtr> vars, std::vector<std::string> args )
{
}//setup

const std::vector<VariablePtr>
ArrayIntElement::scope () const
{
  // Return the constraint's scope
  std::vector<VariablePtr> scope;
  return scope;
}//scope

void
ArrayIntElement::consistency ()
{
}//consistency

//! It checks if
bool
ArrayIntElement::satisfied ()
{
  return true;
}//satisfied

//! Prints the semantic of this constraint
void
ArrayIntElement::print_semantic () const
{
    FZNConstraint::print_semantic ();
}//print_semantic



