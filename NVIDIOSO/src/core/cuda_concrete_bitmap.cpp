//
//  cuda_concrete_bitmap.cpp
//  NVIDIOSO
//
//  Created by Federico Campeotto on 15/07/14.
//  Copyright (c) 2014 ___UDNMSU___. All rights reserved.
//

#include "cuda_concrete_bitmap.h"
#include "cuda_utilities.h"

CudaConcreteDomainBitmap::CudaConcreteDomainBitmap ( size_t size ) :
CudaConcreteDomain ( size ) {
  _dbg = "CudaConcreteDomainBitmap - ";
  
  // Initialize bitmap
  for ( int i = 0; i < _num_chunks; i++ ) {
    _concrete_domain[ i ] = 0xffffffff;
  }
  // All valid bits
  _num_valid_bits = _num_chunks * BITS_IN_CHUNK;
  
  // Set current lower/upper bounds
  _lower_bound = 0;
  _upper_bound = _num_valid_bits - 1;
  
  // Set initial lower/upper bounds
  _init_lower_bound = _lower_bound;
  _init_upper_bound = _upper_bound;
}//CudaConcreteDomainBitmap

CudaConcreteDomainBitmap::CudaConcreteDomainBitmap ( size_t size,
                                                     int min, int max ) :
CudaConcreteDomainBitmap ( size ) {
  
  // Empty domain if not consistent
  if ( max < min ) {
    flush_domain ();
    return;
  }
  
  // Consistency checks
  if ( _init_lower_bound >= min &&
       _init_upper_bound <= max ) return;
  
  // Only max is lower
  if ( _init_lower_bound >= min &&
       _init_upper_bound > max ) {
    _init_upper_bound = max;
    in_max( max );
    return;
  }
  
  // Only min is bigger
  if ( _init_lower_bound <= min &&
       _init_upper_bound <= max ) {
     _init_lower_bound = min;
    in_min( min );
    return;
  }
  
  // Both are strict than the allowed bounds
  _init_lower_bound = min;
  _init_upper_bound = max;
  shrink ( min, max );
}//CudaConcreteDomainBitmap

unsigned int
CudaConcreteDomainBitmap::size () const {
  return _num_valid_bits;
}//size

void
CudaConcreteDomainBitmap::shrink ( int min, int max ) {
  
  // Empty domain if not consistent
  if ( max < min ) {
    flush_domain ();
    return;
  }
  
  // Check min/max value consistency
  if ( min == _lower_bound &&
       max == _upper_bound ) return;
  
  // Return if no chages in the domain
  if ( (min <= _lower_bound) &&
       (max >= _upper_bound) ) {
    return;
  }
  
  // Set min/max w.r.t. the current bounds
  if ( (min < _lower_bound) &&
       (max < _upper_bound) ) {
    min = _lower_bound;
  }
  
  if ( (min > _lower_bound) &&
       (max > _upper_bound) ) {
    max = _upper_bound;
  }
  
  int lower_idx = _num_chunks - 1 - IDX_CHUNK ( min );
  int upper_idx = _num_chunks - 1 - IDX_CHUNK ( max );
  int old_lower_idx = _num_chunks - 1 - IDX_CHUNK ( _lower_bound );
  int old_upper_idx = _num_chunks - 1 - IDX_CHUNK ( _upper_bound );
  
  // Clear chunks out of the current bounds
  if ( old_lower_idx > lower_idx )
    for ( ; old_lower_idx > lower_idx; old_lower_idx-- )
      _concrete_domain[ old_lower_idx ] = 0;
  
  if ( old_upper_idx < upper_idx )
    for ( ; old_upper_idx < upper_idx; old_upper_idx++ )
      _concrete_domain[ old_upper_idx ] = 0;
  
  /*
   * Set to zero all the bits < lower_bound and > upper_bound
   * in the chunks identified by lower_bound and upper_bound respectively.
   */
  _concrete_domain [ lower_idx ] =
  CudaBitUtils::clear_bits_i_through_0 ( _concrete_domain [ lower_idx ],
                                         IDX_BIT ( min ) - 1
                                        );
  
  _concrete_domain [ upper_idx ] =
  CudaBitUtils::clear_bits_MSB_through_i (
                                          _concrete_domain [ upper_idx ],
                                          IDX_BIT ( max ) + 1
                                          );
  
  // Calculate the number of bits set to 1
  int num_bits = 0;
  for ( ; lower_idx <= upper_idx; lower_idx++ ) {
    num_bits += CudaBitUtils::num_1bit( (uint) _concrete_domain[ lower_idx ] );
  }
  
  _lower_bound    = min;
  _upper_bound    = max;
  _num_valid_bits = num_bits;
  
  // Check whether the domain is empty after shrinking it
  if ( _num_valid_bits == 0 ) set_empty ();
}//shrink

void
CudaConcreteDomainBitmap::in_min ( int min ) {
  if ( min <= _lower_bound ) return;
  shrink ( min, _upper_bound );
}//in_min

void
CudaConcreteDomainBitmap::in_max ( int max ) {
  if ( max >= _upper_bound ) return;
  shrink ( _lower_bound, max );
}//in_max

void
CudaConcreteDomainBitmap::add ( int value ) {
  
  // Consistency check
  if ( value < _init_lower_bound ) value = _init_lower_bound;
  if ( value > _init_upper_bound ) value = _init_upper_bound;

  // Return if the value is already set to 1
  if ( contains( value ) ) return;
  
  // Get the right chunk
  int chunk = IDX_CHUNK ( value );
  chunk = _num_chunks - 1 - chunk;
  
  // Otherwise set the corresponding bit to 1 and increment size
  _concrete_domain[ chunk ] = CudaBitUtils::set_bit( _concrete_domain[ chunk ],
                                                     IDX_BIT( value ) );
  
  // Increase number of valid bits
  _num_valid_bits += 1;
  
  // Check domain's bounds
  if ( value < _lower_bound ) _lower_bound = value;
  if ( value > _upper_bound ) _upper_bound = value;
}//add

void
CudaConcreteDomainBitmap::add ( int min, int max ) {
  // Consistency check
  if ( min < _init_lower_bound ) min = _init_lower_bound;
  if ( max > _init_upper_bound ) max = _init_upper_bound;
  
  // Add values from min to max
  for ( int i = min; i <= max; i++ ) add ( i );
}//add

const void *
CudaConcreteDomainBitmap::get_representation () const {
  return (void *) _concrete_domain;
}//get_representation

bool
CudaConcreteDomainBitmap::contains ( int value ) const {
  int chunk = IDX_CHUNK ( value );
  chunk = _num_chunks - 1 - chunk;
  
  return CudaBitUtils::get_bit( _concrete_domain[ chunk ],
                                IDX_BIT( value ) );
}//contains

bool
CudaConcreteDomainBitmap::is_singleton () const {
  return _lower_bound == _upper_bound;
}//is_singleton

int
CudaConcreteDomainBitmap::get_singleton () const {
  if ( !is_singleton() ) {
    throw  NvdException ( (_dbg + "Domain not singleton").c_str() );
  }
  
  return _lower_bound;
}//get_singleton

void
CudaConcreteDomainBitmap::print () const {
  for ( int i = 0; i <  _num_chunks; i++ ) {
    if ( _concrete_domain [ i ] ) {
      CudaBitUtils::print_bit_rep ( _concrete_domain [ i ] );
      std::cout << " ";
    }
  }//i
}//print

