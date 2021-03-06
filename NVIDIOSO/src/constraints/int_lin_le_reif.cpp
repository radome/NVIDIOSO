//
//  int_lin_le_reif.cpp
//  iNVIDIOSO
//
//  Created by Federico Campeotto on 07/07/15.
//  Copyright (c) 2014-2015 ___UDNMSU___. All rights reserved.
//

#include "int_lin_le_reif.h"

IntLinLeReif::IntLinLeReif () :
FZNConstraint ( INT_LIN_LE_REIF ) {
  /*
   * Set the event that trigger this constraint.
   * @note if no event is set, this constraint will never be re-evaluated.
   */
  //set_event( EventType::SINGLETON_EVT );
}//IntLinLeReif

IntLinLeReif::IntLinLeReif ( std::vector<VariablePtr> vars, std::vector<std::string> args ) :
IntLinLeReif () {
  setup ( vars, args );
}//IntLinLeReif

IntLinLeReif::~IntLinLeReif () {}

void
IntLinLeReif::setup ( std::vector<VariablePtr> vars, std::vector<std::string> args )
{
}//setup

const std::vector<VariablePtr>
IntLinLeReif::scope () const
{
  // Return the constraint's scope
  std::vector<VariablePtr> scope;
  return scope;
}//scope

void
IntLinLeReif::consistency ()
{
}//consistency

//! It checks if
bool
IntLinLeReif::satisfied ()
{
  return true;
}//satisfied

//! Prints the semantic of this constraint
void
IntLinLeReif::print_semantic () const
{
    FZNConstraint::print_semantic ();
}//print_semantic



