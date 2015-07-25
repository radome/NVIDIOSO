//
//  cuda_simple_constraint_store.h
//  NVIDIOSO
//
//  Created by Federico Campeotto on 04/12/14.
//  Copyright (c) 2014-2015 ___UDNMSU___. All rights reserved.
//
//  This class implements a (simple) constraint store.
//

#ifndef __NVIDIOSO__cuda_simple_constraint_store__
#define __NVIDIOSO__cuda_simple_constraint_store__

#include "simple_constraint_store.h"
#include "cuda_cp_model.h"

class CudaSimpleConstraintStore : public SimpleConstraintStore {
protected:
    //! Constraint queue: constraints to be propagated on device
    size_t * _d_constraint_queue;

    //! Number of bytes needed to store the state of the variables in the scope of a constraint
    size_t _scope_state_size;

	//! Propagation loop out parameter
	int _loop_out;
	
    //! Constraint queue on device
    std::vector<size_t> _h_constraint_queue;

    /**
     * Copy the current states (domains) on device
     * @return true if the copy has been completed successfully. False otherwise.
     * @note copy is performed synchronously.
     */
    bool move_states_to_device ();

    /**
     * Copy the current states (domains) from device to host
     * @return true if the copy has been completed successfully
     *         AND no domain is empty. False otherwise.
     * @note copy is performed synchronously.
     */
    bool move_states_from_device ();

    //! Copy the current queue of constraints on device
    void move_queue_to_device ();

    /**
     * Pointer to the current CP_model.
     * @note this is used to move data from-to device
     */
    CudaCPModel * _cp_model_ptr;

    //! Invoke the kernel which performs consistency on device
    virtual void dev_consistency ();

public:
    /**
     * Default constructor. It initializes the
     * internal data structures of this constraint store.
     */
    CudaSimpleConstraintStore ();

    ~CudaSimpleConstraintStore ();

	/**
	 * Sets the number of iterations to perform to
	 * propagate constrains on device.
	 * This represents the number of time the propagation
	 * kernel is invoked (it may not reach the fix point if
	 * the number of propagations is small).
	 * @param loop_out integer value representing the number 
	 *        of times the propagation kernel is invoked.
	 * @note default is 1.
	 */
	 void set_prop_loop_out ( int loop_out = 1);
	
	//! Get the number of times the propagation loop is invoked
	int get_prop_loop_out () const;
	
    /**
     * Verify and propagate consistency of
     * constraints in the constraint queue.
     * @note consistency if performed in parallel on device.
     */
    bool consistency () override;

    /**
     * Allocate memory on device.
     * @param num_cons total number of constraints.
     * @note if num_cons == 0, this function will use the
     *       parameter "_number_of_constraints" of SimpleConstraintStore.
     */
    virtual void finalize ( CudaCPModel* ptr );
};

#endif /* defined(__NVIDIOSO__cuda_simple_constraint_store__) */