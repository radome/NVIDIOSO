//
//  cuda_variable.cpp
//  NVIDIOSO
//
//  Created by Federico Campeotto on 09/07/14.
//  Copyright (c) 2014 ___UDNMSU___. All rights reserved.
//

#include "cuda_variable.h"
#include "cuda_domain.h"

using namespace std;

CudaVariable::CudaVariable () {
  _dbg = "CudaVariable - ";
  
  //Instantiate CUDA-Specific domain
  _domain_ptr = make_shared<CudaDomain> ();
}//CudaVariable

CudaVariable::CudaVariable ( int id ) :
Variable ( id ) {
  _dbg = "CudaVariable - ";
  
  //Instantiate CUDA-Specific domain
  _domain_ptr = make_shared<CudaDomain> ();
}//CudaVariable

CudaVariable::~CudaVariable () {
}//~CudaVariable

void
CudaVariable::set_domain () {
  (std::static_pointer_cast<CudaDomain>( _domain_ptr ))->init_domain ( Domain::MIN_DOMAIN(), Domain::MAX_DOMAIN() );
}//set_domain

void
CudaVariable::set_domain ( int lower_bound, int upper_bound ) {
  (std::static_pointer_cast<CudaDomain>( _domain_ptr ))->init_domain( lower_bound, upper_bound);
}//set_domain

void
CudaVariable::set_domain ( vector < vector < int > > set_of_subsets ) {
  
  if ( set_of_subsets.size () > 1 ) {
    logger->error( _dbg + "Set of subsets not yet supported", __FILE__, __LINE__ );
    throw new std::string ( "Set of subsets not supported yet" );
  }
  
  
  int min_element = *std::min_element( set_of_subsets[ 0 ].begin(),
                                       set_of_subsets[ 0 ].end() );
  int max_element = *std::max_element( set_of_subsets[ 0 ].begin(),
                                       set_of_subsets[ 0 ].end() );
  set_domain ( min_element, max_element );
  
  // Clear values not belonging to the set of values
  for ( auto x : set_of_subsets[ 0 ] ) {
    (std::static_pointer_cast<CudaDomain>( _domain_ptr ))->subtract ( x );
  }
}//set_domain

void
CudaVariable::print () const {
  cout << "  - CUDA_Variable -  " << endl;
  Variable::print();
  _domain_ptr->print();
}//print