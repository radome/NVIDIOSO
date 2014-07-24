//
//  cuda_domain.cpp
//  NVIDIOSO
//
//  Created by Federico Campeotto on 09/07/14.
//  Copyright (c) 2014 ___UDNMSU___. All rights reserved.
//

#include "cuda_domain.h"
#include "cuda_utilities.h"
#include "cuda_concrete_bitmap.h"
#include "cuda_concrete_list.h"
#include "cuda_concrete_bitmaplist.h"

using namespace std;

CudaDomain::CudaDomain () :
_num_allocated_bytes ( 0 ),
_domain          ( nullptr ),
_concrete_domain ( nullptr ) {
}//CudaDomain

CudaDomain::~CudaDomain () {
  delete [] _domain;
}//~CudaDomain

size_t
CudaDomain::get_allocated_bytes () const {
  return _num_allocated_bytes;
}//get_allocated_bytes

void
CudaDomain::init_domain ( int min, int max ) {
  
  // Return if domain has been already initialized
  if ( _domain != nullptr ) return;
  
  // Consistency check
  assert ( min <= max );
  
  // Get proper size of domain
  int size;
  if ( ((min <= Domain::MIN_DOMAIN() + 1) && max > 0) ||
       ((max == Domain::MAX_DOMAIN()    ) && min < 0) ||
       ((max == Domain::MAX_DOMAIN() / 2) && (max == Domain::MIN_DOMAIN() / 2))
      ) {
    size = Domain::MAX_DOMAIN();
  }
  else {
    size = abs( max - min + 1 );
  }
  
  // Allocate domain according to its size
  if ( min >= 0 &&  min <= VECTOR_MAX && size <= VECTOR_MAX ) {
    _num_int_chunks = num_chunks ( VECTOR_MAX );
    _num_allocated_bytes = MAX_STATUS_SIZE + (VECTOR_MAX / BITS_IN_BYTE);
    
    // Create domains representations
    _domain          = new int [ _num_allocated_bytes / sizeof( int ) ];
    _concrete_domain = make_shared<CudaConcreteDomainBitmap>( _num_allocated_bytes );
    
    set_bit_representation ();
  }
  else {
    _num_allocated_bytes = MAX_BYTES_SIZE;
    _num_int_chunks      = MAX_DOMAIN_VALUES;
    
    // Create domains representations
    _domain          = new int [ _num_allocated_bytes / sizeof( int ) ];
    
    if ( size <= VECTOR_MAX ) {
      vector < pair < int, int > > bounds_list;
      bounds_list.push_back( make_pair( min, max ) );
      _concrete_domain = make_shared<CudaConcreteBitmapList>( _num_allocated_bytes, bounds_list);
      set_bitlist_representation ();
    }
    else {
      _concrete_domain = make_shared<CudaConcreteDomainList>( _num_allocated_bytes, min, max );
      set_list_representation ();
    }
  }
  
  // Set bounds
  _domain[ LB_IDX() ] = min;
  _domain[ UB_IDX() ] = max;

  // Set initial events
  if ( _domain[ LB_IDX() ] == _domain[ UB_IDX() ] ) {
    event_to_int ( EventType::SINGLETON_EVT );
  }
  else {
    event_to_int ( EventType::NO_EVT );
  }
}//init_domain

//! Get the domain's lower bound
int
CudaDomain::get_lower_bound () const {
  return _domain[ LB_IDX() ];
}//get_lower_bound

//! Get the domain's upper bound
int
CudaDomain::get_upper_bound () const {
  return _domain[ UB_IDX() ];
}//get_upper_bound

DomainPtr
CudaDomain::clone_impl () const {
  //@todo
  return ( shared_ptr<CudaDomain> ( new CudaDomain ( *this ) ) );
}//clone_impl

EventType
CudaDomain::int_to_event () const {
  switch ( _domain[ EVT_IDX() ] ) {
    case INT_NO_EVT:
      return EventType::NO_EVT;
    case INT_SINGLETON_EVT:
      return EventType::SINGLETON_EVT;
    case INT_BOUNDS_EVT:
      return EventType::BOUNDS_EVT;
    case INT_CHANGE_EVT:
      return EventType::CHANGE_EVT;
    case INT_FAIL_EVT:
      return EventType::FAIL_EVT;
    default:
      return EventType::OTHER_EVT;
  }
}//int_to_event

void
CudaDomain::event_to_int ( EventType evt ) const {
  switch ( evt ) {
    case EventType::NO_EVT:
      _domain[ EVT_IDX() ] = INT_NO_EVT;
      break;
    case EventType::SINGLETON_EVT:
      _domain[ EVT_IDX() ] = INT_SINGLETON_EVT;
      break;
    case EventType::BOUNDS_EVT:
      _domain[ EVT_IDX() ] = INT_BOUNDS_EVT;
      break;
    case EventType::CHANGE_EVT:
      _domain[ EVT_IDX() ] = INT_CHANGE_EVT;
      break;
    case EventType::FAIL_EVT:
      _domain[ EVT_IDX() ] = INT_FAIL_EVT;
      break;
    default:
      _domain[ EVT_IDX() ] = INT_OTHER_EVT;
      break;
  }
}//int_to_event

void
CudaDomain::set_bit_representation () {
  _domain[ REP_IDX() ] = INT_BITMAP;
}//set_bit_representation

void
CudaDomain::set_bitlist_representation ( int num_list ) {
  assert( num_list < 0 );
  _domain[ REP_IDX() ] = num_list;
}//set_bit_representation

void
CudaDomain::set_list_representation ( int num_list ) {
  assert( num_list > 0 );
  _domain[ REP_IDX() ] = num_list;
}//set_bit_representation

CudaDomainRepresenation
CudaDomain::get_representation () const {
  if ( _domain[ REP_IDX() ] < 0 ) {
    return CudaDomainRepresenation::BITMAP_LIST;
  }
  else if ( _domain[ REP_IDX() ] == 0 ) {
    return CudaDomainRepresenation::BITMAP;
  }
  else {
    return CudaDomainRepresenation::LIST;
  }
}//get_representation

DomainPtr
CudaDomain::clone () const {
  return clone_impl ();
}//clone 

EventType
CudaDomain::get_event () const {
  return int_to_event ();
}//get_event

size_t
CudaDomain::get_size () const {
  return ( (int) _domain[ DSZ_IDX() ] );
}//get_size

void
CudaDomain::shrink ( int min, int max ) {
  set_bounds ( min, max );
}//shrink

void
CudaDomain::set_bounds ( int min, int max ) {
  
  // Domain failure: non valid min/max
  if ( max < min ) {
    event_to_int ( EventType::FAIL_EVT );
    return;
  }
  
  // Domain failure: out or range
  if ( _domain[ UB_IDX() ] < min || _domain[ LB_IDX() ] > max ) {
    event_to_int ( EventType::FAIL_EVT );
    return;
  }
  
  // Trying to enlarge domain has no effect
  if ( (min <= _domain[ LB_IDX() ]) && (max >= _domain[ UB_IDX() ]) ) {
    return;
  }
  
  // Set bounds on domain's internal representation
  try {
    _concrete_domain->shrink( min, max );
  } catch ( NvdException & e ) {
    e.what();
    return;
  }
  
  // Check events on current domain
  _domain[ LB_IDX() ] = min;
  _domain[ UB_IDX() ] = max;
  
  int new_size = _concrete_domain->size ();
  
  if ( _concrete_domain->is_empty() ) {
    _domain [ EVT_IDX() ] = INT_FAIL_EVT;
    return;
  }
  
  if ( _concrete_domain->is_singleton () ) {
    _domain [ EVT_IDX() ] = INT_SINGLETON_EVT;
    return;
  }
  
  if ( new_size < _domain [ DSZ_IDX() ] ) {
    _domain [ DSZ_IDX() ] = new_size;
    _domain [ EVT_IDX() ] = INT_BOUNDS_EVT;
  }
  
  /*
   * Check new domain size:
   * if the sum of elements is <= VECTOR_MAX ->
   * switch representation to bitmap list.
   */
  if ( _domain [ DSZ_IDX() ] <= VECTOR_MAX ) {
    switch_list_to_bitmaplist ();
  }
}//set_bounds

void
CudaDomain::switch_list_to_bitmaplist () {
  
  // Consistency check on current representation
  if ( get_representation() != CudaDomainRepresenation::LIST ) return;
  
  // Consistency check on bounds
  assert ( _domain[ LB_IDX() ] == _concrete_domain->lower_bound () );
  assert ( _domain[ UB_IDX() ] == _concrete_domain->upper_bound () );
  
  // Prepare list of bounds
  int * list_representation = (int *) _concrete_domain->get_representation ();
  vector< pair <int, int> > pairs;
  
  /*
   * Check all the pairs and exit as soon as a pair is 
   * greater than the upper bound.
   */
  for ( int i = 0; i < _concrete_domain->get_num_chunks(); i = i + 2 ) {
    if ( (list_representation[ i ]     >= _domain[ LB_IDX() ]) &&
         (list_representation[ i + 1 ] <= _domain[ UB_IDX() ])) {
      pairs.push_back( make_pair(list_representation[ i     ],
                                 list_representation[ i + 1 ]) );
    }
    if ( list_representation[ i ] > _upper_bound ) break;
  }
  
  _concrete_domain = make_shared<CudaConcreteBitmapList>( _num_allocated_bytes, pairs );
  
  set_bitlist_representation();
}//switch_list_to_bitmap

bool
CudaDomain::set_singleton ( int value ) {
  if ( _concrete_domain->contains( value ) ) {
    _concrete_domain->shrink( value, value );
    
    // Update domain's information
    _domain[ LB_IDX() ] = value;
    _domain[ UB_IDX() ] = value;
    _domain [ DSZ_IDX() ] = 1;
    _domain [ EVT_IDX() ] = INT_SINGLETON_EVT;
    return true;
  }
  
  return false;
}//set_singleton

bool
CudaDomain::subtract ( int value ) {
  if ( _concrete_domain->contains( value ) ) {
    //_concrete_domain->subtract ( value );
    
    // Update domain's information
    if ( _concrete_domain->is_singleton () ) {
      _domain [ LB_IDX() ]  = _concrete_domain->get_singleton ();
      _domain [ UB_IDX() ]  = _concrete_domain->get_singleton ();
      _domain [ DSZ_IDX() ] = 1;
      _domain [ EVT_IDX() ] = INT_SINGLETON_EVT;
    }
    else {
      _domain [ LB_IDX() ]  = _concrete_domain->lower_bound ();
      _domain [ UB_IDX() ]  = _concrete_domain->upper_bound ();
      _domain [ DSZ_IDX() ] -= 1;
      _domain [ EVT_IDX() ] = INT_CHANGE_EVT;
    }
    return true;
  }
  
  return false;
}//subtract

void
CudaDomain::add_element ( int n ) {
  _concrete_domain->add( n );
  int new_size = _concrete_domain->size ();
  if ( new_size > _domain [ DSZ_IDX() ] ) {
    _domain [ DSZ_IDX() ] = new_size;
    _domain [ EVT_IDX() ] = INT_CHANGE_EVT;
  }
}//add_element

void
CudaDomain::in_min ( int min ) {
  _concrete_domain->in_min ( min );
  _domain [ DSZ_IDX() ] = _concrete_domain->size ();
  _domain [ LB_IDX()  ] = _concrete_domain->lower_bound ();
  _domain [ UB_IDX()  ] = _concrete_domain->upper_bound ();
  
  if ( _domain [ DSZ_IDX() ] == 1 ) {
    _domain [ EVT_IDX() ] = INT_SINGLETON_EVT;
  }
  else {
    _domain [ EVT_IDX() ] = INT_BOUNDS_EVT;
  }
}//in_min

void
CudaDomain::in_max ( int max ) {
  _concrete_domain->in_max ( max );
  _domain [ DSZ_IDX() ] = _concrete_domain->size ();
  _domain [ LB_IDX()  ] = _concrete_domain->lower_bound ();
  _domain [ UB_IDX()  ] = _concrete_domain->upper_bound ();
  
  if ( _domain [ DSZ_IDX() ] == 1 ) {
    _domain [ EVT_IDX() ] = INT_SINGLETON_EVT;
  }
  else {
    _domain [ EVT_IDX() ] = INT_BOUNDS_EVT;
  }
}//in_max

void
CudaDomain::print () const {
  cout << "- CudaDomain:\n";
  cout << "EVT:\t";
  switch ( _domain[ EVT_IDX() ] ) {
    case INT_NO_EVT:
      cout << "NO Event\n";
      break;
    case INT_SINGLETON_EVT:
      cout << "Singleton Event\n";
      break;
    case INT_BOUNDS_EVT:
      cout << "Bounds Event\n";
      break;
    case INT_CHANGE_EVT:
      cout << "Change Event\n";
      break;
    case INT_FAIL_EVT:
      cout << "Fail Event\n";
      break;
    default:
      cout << "Other Event\n";
      break;
  }
  cout << "REP:\t";
  if ( get_representation()  == CudaDomainRepresenation::BITMAP ) {
    cout << "Bitmap\n";
  }
  else if ( get_representation()  == CudaDomainRepresenation::BITMAP_LIST ){
    cout << "Bitmap lists\n";
  }
  else {
    cout << "List\n";
  }
  cout << "LB-UB:\t";
  cout << "[" << _domain [ LB_IDX() ] << ".." << _domain [ UB_IDX() ] << "]\n";
  cout << "DSZ:\t";
  cout << _domain [ DSZ_IDX() ] << "\n";
  cout << "Bytes:\t";
  if ( get_allocated_bytes () < 1024 )
    cout << get_allocated_bytes () << "Bytes\n";
  else
    cout << (get_allocated_bytes () / 1024) << "kB\n";
  print_domain ();
}//print

void
CudaDomain::print_domain () const {
  cout << "|| ";
  for ( int i = 0; i < BIT_IDX(); i++ ) {
    cout << _domain[ i ];
    if ( i < BIT_IDX() - 1 ) cout << " | ";
    else cout << " || ";
  }
  // Print according to the internal representatation of domain
  _concrete_domain->print ();
  cout << "||\n";
}//print_domain





