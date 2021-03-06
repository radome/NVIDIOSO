//
//  cuda_simple_constraint_store_seq.cpp
//  iNVIDIOSO
//
//  Created by Federico Campeotto on 07/18/15.
//  Copyright (c) 2014-2015 Federico Campeotto. All rights reserved.
//


#include "cuda_simple_constraint_store_seq.h"
#include "cuda_propagation_utilities.h"
#include "cuda_synchronization_utilities.h"

using namespace std;

CudaSimpleConstraintStoreSeq::CudaSimpleConstraintStoreSeq () :
    CudaSimpleConstraintStore () {
    _dbg = "CudaSimpleConstraintStoreSeq - ";
}//CudaSimpleConstraintStore

CudaSimpleConstraintStoreSeq::~CudaSimpleConstraintStoreSeq () {
}//~CudaSimpleConstraintStoreSeq

void
CudaSimpleConstraintStoreSeq::dev_consistency ()
{

#if CUDAON

	CudaPropUtils::cuda_consistency_sequential <<< 1, 1, _scope_state_size >>> ( _d_constraint_queue, _constraint_queue.size() );
	
	CudaSynchUtils::cuda_set_domains_from_bit_1b1v <<< _cp_model_ptr->num_variables (), dev_block_size, STANDARD_DOM * sizeof ( uint ) >>>
        ( _cp_model_ptr->get_dev_domain_index_ptr (),  _cp_model_ptr->get_dev_domain_states_ptr () );
    
#endif

}//dev_consistency



