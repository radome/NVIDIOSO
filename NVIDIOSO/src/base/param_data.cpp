/*
 *  param_data.cpp
 *  iNVIDIOSO
 *
 *  Created by Federico Campeotto on 07/15/15.
 *  Copyright (c) 2014-2015 Federico Campeotto. All rights reserved.
 */

#include "param_data.h"

ParamData* solver_params = nullptr;

using namespace std;

    // ======================= MACROS ======================
     constexpr std::string ParamData::SEPARATOR_KWD  = "_";
     constexpr std::string ParamData::PARAM_SEP_KWD  = "=";
     constexpr std::string ParamData::PARAM_YES_KWD  = "YES";
     constexpr std::string ParamData::PARAM_NO_KWD   = "NO";
     constexpr std::string ParamData::CUDA_KWD       = "CUDA";
     constexpr std::string ParamData::SEARCH_KWD     = "SEARCH";
     constexpr std::string ParamData::CONSTRAINT_KWD = "CONSTRAINT";
     constexpr std::string ParamData::CSTORE_KWD     = "CSTORE";
    // =====================================================

    // ======================= CUDA ======================
    constexpr std::string ParamData::CUDA_SHARED_SIZE_KWD = "SHARED_MEM_SIZE";
    constexpr std::string ParamData::CUDA_MAX_BLOCK_SIZE_KWD = "MAX_BLOCK_SIZE";
    // ===================================================

    // ======================== SEARCH PARAMETERS ========================
     constexpr std::string ParamData::SEARCH_DEBUG_KWD        = "TREE_DEBUG";
     constexpr std::string ParamData::SEARCH_TRAIL_DEBUG_KWD  = "TRAIL_DEBUG";
     constexpr std::string ParamData::SEARCH_TIME_WATCHER_KWD = "TIME_WATCHER";
     constexpr std::string ParamData::SEARCH_TIMEOUT_KWD      = "TIMEOUT";
     constexpr std::string ParamData::SEARCH_SOL_LIM_KWD      = "SOLUTION_LIMIT";
     constexpr std::string ParamData::SEARCH_BACKTRK_LIM_KWD  = "BACKTRACK_OUT";
     constexpr std::string ParamData::SEARCH_NODES_LIM_KWD    = "NODES_OUT";
     constexpr std::string ParamData::SEARCH_WRONG_DEC_KWD    = "WRONG_DECISIONS";
    // ====================================================================

    // ======================= CONSTRAINT STORE PARAMETERS =======================
     constexpr std::string ParamData::CSTORE_CONSISTENCY_KWD    = "CONSISTENCY";
     constexpr std::string ParamData::CSTORE_SATISFIABILITY_KWD = "SATISFIABILITY";

    // 		      ======== CONSTRAINT STORE CUDA ========
    
     constexpr std::string ParamData::CSTORE_CUDA_PROP_KWD          = "CUDA_PROP";
     constexpr std::string ParamData::CSTORE_CUDA_PROP_LOOP_OUT_KWD = "CUDA_PROP_LOOP_OUT";
     constexpr std::string ParamData::CSTORE_CUDA_SEQ_KWD           = "sequential";
     constexpr std::string ParamData::CSTORE_CUDA_BPC_KWD           = "block_per_constraint";
     constexpr std::string ParamData::CSTORE_CUDA_BPV_KWD           = "block_per_variable";
     constexpr std::string ParamData::CSTORE_CUDA_BKC_KWD           = "block_per_k_constraint";
     constexpr std::string ParamData::CSTORE_CUDA_BKV_KWD           = "block_per_k_variable";
    // ===========================================================================
    
    // ======================= CONSTRAINT PARAMETERS =======================
	constexpr std::string ParamData::CONSTRAINT_PROP_CLASS_KWD = "PROPAGATOR_CLASS";
	// =====================================================================


ParamData::ParamData () :
    _dbg     ( "ParamData - " ),
    _in_file ( "config/iNVIDIOSO.params" ) {
    set_default_parameters ();
}

ParamData::ParamData ( string path ) :
    _dbg     ( "ParamData::" ),
    _in_file ( path ) {
    set_default_parameters ();
}

ParamData::~ParamData () {}

void 
ParamData::set_param_path ( string path )
{
    _in_file = path;
}//set_param_path

string
ParamData::get_param_path () const
{
    return _in_file;
}//get_param_path

int  
ParamData::cstore_type_to_int ( CudaPropParam ctype ) const
{
	switch ( ctype )
    {
    	case CudaPropParam::SEQUENTIAL:
    		return 0;
    	case CudaPropParam::BLOCK_PER_CON:
    		return 1;
    	case CudaPropParam::BLOCK_PER_VAR:
    		return 2;
    	case CudaPropParam::BLOCK_K_CON:
    		return 3;
    	case CudaPropParam::BLOCK_K_VAR:
    		return 4;
    	default:
    		return 0;
    }
}//cstore_type_to_int

CudaPropParam 
ParamData::cstore_int_to_type ( int ctype ) const
{
	switch ( ctype )
    {
    	case 0:
    		return CudaPropParam::SEQUENTIAL;
    	case 1:
    		return CudaPropParam::BLOCK_PER_CON;
    	case 2:
    		return CudaPropParam::BLOCK_PER_VAR;
    	case 3:
    		return CudaPropParam::BLOCK_K_CON;
    	case 4:
    		return CudaPropParam::BLOCK_K_VAR;
    	default:
    		return CudaPropParam::SEQUENTIAL;
    }
}//cstore_int_to_type

void
ParamData::set_default_parameters ()
{
	// --- CUDA ---
	_cuda_shared_available = 46000;
	_cuda_max_block_size = 512;
	
    // --- Search ---
    _search_debug = false;
    _search_trail_debug = false;
    _search_time_watcher = false;
    _search_solution_limit = 1;
    _search_backtrack_limit = -1;
    _search_nodes_limit = -1;
    _search_wrong_decisions_limit = -1;
    _search_timeout = -1.0;
    
    // --- Constraint ---
    _constraint_propagator_class = "naive";

    // --- Constraint Store ---
    _cstore_consistency    = true;
    _cstore_satisfiability = true;
    _cstore_cuda_prop_loop_out = 1;
    _cstore_cuda_propagation_function = CudaPropParam::SEQUENTIAL;
    
}//set_default_parameters

void
ParamData::set_parameters ()
{
    open ();
    read_params ();
    close ();
}//set_parameters

void
ParamData::open ()
{
    _ifs_params.open ( _in_file );
    if ( !_ifs_params.is_open () )
    {
        LogMsg << _dbg + "open: unable to open parameters file " + _in_file << std::endl;
    }
}//open

void
ParamData::close ()
{
    _ifs_params.close ();
}

string
ParamData::get_param_value ( std::string line )
{
    std::size_t pos = line.find_first_of ( PARAM_SEP_KWD );
    if ( pos == string::npos )
    {
        return "";
    }
    return line.substr ( pos + 1 );
}//get_param_value

void
ParamData::read_params ()
{
    if ( !_ifs_params.is_open () )
    {
        LogMsg << _dbg + "read_params: unable to read parameters file, use default parameters" << std::endl;
        return;
    }

    string line;
    std::size_t pos;
    while ( getline ( _ifs_params, line ) )
    {
        pos = line.find_first_of ( "#" );
        if ( pos != string::npos )
        {
            line = line.substr ( 0, pos + 1 );
        }
		
        //Skip empty lines
        if ( line.size() == 0 || pos == 0 ) continue;
		
        // Skip lines with spaces
        bool useful_char = false;
        for ( auto& c : line )
        {
            if ( c != ' ' || c != '\t' )
            {
                useful_char = true;
                break;
            }
        }
        if ( !useful_char ) continue;
		
        // Parse line according to the solver's component
        // Search Engine
        pos = line.find ( SEARCH_KWD );
        if ( pos != string::npos )
        {
            set_search_parameters ( line );
        }

		// CONSTRAINT
        pos = line.find ( CONSTRAINT_KWD );
        if ( pos != string::npos )
        {
            set_constraint_parameters ( line );
        }
		
        // Constraint Store
        pos = line.find ( CSTORE_KWD );
        if ( pos != string::npos )
        {
            set_constraint_engine_parameters ( line );
        }
        
        // CUDA
        pos = line.find ( CUDA_KWD );
        if ( pos != string::npos )
        {
            set_cuda_parameters ( line );
        }
    }
}//read_params

void
ParamData::set_search_parameters ( std::string& line )
{
    std::size_t pos = line.find ( SEARCH_KWD );
    if ( pos == string::npos )
    {
        return;
    }

    pos = line.find_first_of ( SEPARATOR_KWD );
    if ( pos == string::npos )
    {
        return;
    }
	
    string line_aux = line.substr ( pos + 1 );
    string param    = get_param_value ( line_aux );
    pos = line_aux.find ( SEARCH_DEBUG_KWD );
    if ( pos != string::npos )
    {
        if ( param == PARAM_YES_KWD )
        {
            _search_debug = true;
        }
        else
        {
            _search_debug = false;
        }
        return;
    }

    pos = line_aux.find ( SEARCH_TRAIL_DEBUG_KWD );
    if ( pos != string::npos )
    {
        if ( param == PARAM_YES_KWD )
        {
            _search_trail_debug = true;
        }
        else
        {
            _search_trail_debug = false;
        }
        return;
    }

    pos = line_aux.find ( SEARCH_TIME_WATCHER_KWD );
    if ( pos != string::npos )
    {
        if ( param == PARAM_YES_KWD )
        {
            _search_time_watcher = true;
        }
        else
        {
            _search_time_watcher = false;
        }
        return;
    }

    pos = line_aux.find ( SEARCH_TIMEOUT_KWD );
    if ( pos != string::npos )
    {
        _search_timeout = atof ( param.c_str() );
        return;
    }

    pos = line_aux.find ( SEARCH_SOL_LIM_KWD );
    if ( pos != string::npos )
    {
        _search_solution_limit = atoi ( param.c_str() );
        return;
    }

    pos = line_aux.find ( SEARCH_BACKTRK_LIM_KWD );
    if ( pos != string::npos )
    {
        _search_backtrack_limit = atoi ( param.c_str() );
        return;
    }

    pos = line_aux.find ( SEARCH_NODES_LIM_KWD );
    if ( pos != string::npos )
    {
        _search_nodes_limit = atoi ( param.c_str() );
        return;
    }

    pos = line_aux.find ( SEARCH_WRONG_DEC_KWD );
    if ( pos != string::npos )
    {
        _search_wrong_decisions_limit = atoi ( param.c_str() );
        return;
    }
}//set_search_parameters

void
ParamData::set_constraint_parameters ( std::string& line )
{
	std::size_t pos = line.find ( CONSTRAINT_KWD );
    if ( pos == string::npos )
    {
        return;
    }
    
    pos = line.find_first_of ( SEPARATOR_KWD );
    if ( pos == string::npos )
    {
        return;
    }

    string line_aux = line.substr ( pos + 1 );
    string param    = get_param_value ( line_aux );
    pos = line_aux.find ( CONSTRAINT_PROP_CLASS_KWD );
    if ( pos != string::npos )
    {
    	_constraint_propagator_class = param;
    }
}//set_constraint_parameters

void
ParamData::set_constraint_engine_parameters ( std::string& line )
{
    std::size_t pos = line.find ( CSTORE_KWD );
    if ( pos == string::npos )
    {
        return;
    }

    pos = line.find_first_of ( SEPARATOR_KWD );
    if ( pos == string::npos )
    {
        return;
    }

    string line_aux = line.substr ( pos + 1 );
    string param = get_param_value ( line_aux );

    pos = line_aux.find ( CSTORE_CONSISTENCY_KWD );
    if ( pos != string::npos )
    {
        if ( param == PARAM_YES_KWD )
        {
            _cstore_consistency = true;
        }
        else
        {
            _cstore_consistency = false;
        }
        return;
    }

    pos = line_aux.find ( CSTORE_SATISFIABILITY_KWD );
    if ( pos != string::npos )
    {
        if ( param == PARAM_YES_KWD )
        {
            _cstore_satisfiability = true;
        }
        else
        {
            _cstore_satisfiability = false;
        }
        return;
    }
    
    pos = line_aux.find ( CSTORE_CUDA_PROP_LOOP_OUT_KWD );
    if ( pos != string::npos )
    {
        _cstore_cuda_prop_loop_out = atoi ( param.c_str() );
        return;
    }

    pos = line_aux.find ( CSTORE_CUDA_PROP_KWD );
    if ( pos != string::npos )
    {
        if ( param == CSTORE_CUDA_SEQ_KWD )
        {
            _cstore_cuda_propagation_function = CudaPropParam::SEQUENTIAL;
        }
        else if ( param == CSTORE_CUDA_BPC_KWD )
        {
            _cstore_cuda_propagation_function = CudaPropParam::BLOCK_PER_CON;
        }
        else if ( param == CSTORE_CUDA_BPV_KWD )
        {	
            _cstore_cuda_propagation_function = CudaPropParam::BLOCK_PER_VAR;
        }
        return;
    }
}//set_constraint_engine_parameters

void
ParamData::set_cuda_parameters ( std::string& line )
{
	std::size_t pos = line.find ( CUDA_KWD );
    if ( pos == string::npos )
    {
        return;
    }
    
    pos = line.find_first_of ( SEPARATOR_KWD );
    if ( pos == string::npos )
    {
        return;
    }

    string line_aux = line.substr ( pos + 1 );
    string param    = get_param_value ( line_aux );
    
    pos = line_aux.find ( CUDA_SHARED_SIZE_KWD );
    if ( pos != string::npos )
    {
        _cuda_shared_available = atoi ( param.c_str() );
        _cuda_shared_available *= 1000;
        return;
    }
    
    pos = line_aux.find ( CUDA_MAX_BLOCK_SIZE_KWD );
    if ( pos != string::npos )
    {
        _cuda_max_block_size = atoi ( param.c_str() );
        return;
    }
}//set_cuda_parameters

size_t 
ParamData::cuda_get_shared_mem_size () const
{
	return _cuda_shared_available;
}//cuda_get_shared_mem_size

size_t 
ParamData::cuda_get_max_block_size () const
{
	return _cuda_max_block_size;
}//cuda_get_max_block_size

bool
ParamData::search_get_debug () const
{
    return _search_debug;
}//search_get_debug

bool
ParamData::search_get_trail_debug () const
{
    return _search_trail_debug;
}//search_get_trail_debug

bool
ParamData::search_get_time_watcher () const
{
    return _search_time_watcher;
}//search_get_time_watcher

double
ParamData::search_get_timeout () const
{
    return _search_timeout;
}//search_get_timeout

int
ParamData::search_get_solution_limit () const
{
    return _search_solution_limit;
}//search_get_solution_limit

int
ParamData::search_get_backtrack_limit () const
{
    return _search_backtrack_limit;
}//search_get_backtrack_limit

int
ParamData::search_get_nodes_limit () const
{
    return _search_nodes_limit;
}//search_get_nodes_limit

int
ParamData::search_get_wrong_decision_limit () const
{
    return _search_wrong_decisions_limit;
}//search_get_wrong_decision_limit

std::string 
ParamData::constraint_get_propagator_class () const
{
	return _constraint_propagator_class;
}//constraint_get_propagator_class

bool
ParamData::cstore_get_consistency () const
{
    return _cstore_consistency;
}//cstore_get_consistency

bool
ParamData::cstore_get_satisfiability () const
{
    return _cstore_satisfiability;
}//cstore_get_satisfiability

int
ParamData::cstore_get_dev_loop_out () const
{
	return _cstore_cuda_prop_loop_out;
}//cstore_get_dev_loop_out

CudaPropParam
ParamData::cstore_get_dev_propagation () const
{
    return _cstore_cuda_propagation_function;
}//cstore_get_dev_propagation

void
ParamData::print_option ( bool b, bool new_line ) const
{
    if ( b ) cout << "TRUE";
    else     cout << "FALSE";
    if ( new_line ) cout << endl;
}//print_option

void
ParamData::print_option ( int n, bool new_line ) const
{
    cout << n;
    if ( new_line ) cout << endl;
}//print_option

void
ParamData::print_option ( double d, bool new_line ) const
{
    cout << d;
    if ( new_line ) cout << endl;
}//print_option

void
ParamData::print_option ( string s, bool new_line ) const
{
    cout << s;
    if ( new_line ) cout << endl;
}//print_option

void
ParamData::print_option ( std::size_t size, bool new_line ) const
{
    cout << size;
    if ( new_line ) cout << endl;
}//print_option

void
ParamData::print_option ( CudaPropParam p, bool new_line ) const
{
    switch ( p )
    {
    	case CudaPropParam::SEQUENTIAL:
    		cout << CSTORE_CUDA_SEQ_KWD;
    		break;
    	case CudaPropParam::BLOCK_PER_CON:
    		cout << CSTORE_CUDA_BPC_KWD;
    		break;
    	case CudaPropParam::BLOCK_PER_VAR:
    		cout << CSTORE_CUDA_BPV_KWD;
    		break;
    	default:
    		cout << CSTORE_CUDA_SEQ_KWD;
    		break;
    }
    if ( new_line ) cout << endl;
}//print_option

void
ParamData::print () const
{
    cout << "=========== Solver Parameters ===========\n";
    
    cout << "- Search:\n";
    cout << "\t+ Debug active: ";
    print_option ( _search_debug );
    cout << "\t+ Trail Debug active: ";
    print_option ( _search_trail_debug );
    cout << "\t+ Search Time Watcher active: ";
    print_option ( _search_time_watcher );
    cout << "\t+ Solution Limit: ";
    print_option ( _search_solution_limit );
    cout << "\t+ Backtrack Limit: ";
    print_option ( _search_backtrack_limit );
    cout << "\t+ Nodes Limit: ";
    print_option ( _search_nodes_limit );
    cout << "\t+ Wrong Decisions Limit: ";
    print_option ( _search_wrong_decisions_limit );
    cout << "\t+ Search Timeout: ";
    print_option ( _search_timeout );
    
    cout << "- Constraints:\n";
    cout << "\t+ Propagator class: "; 
    print_option ( _constraint_propagator_class );
    
    cout << "- Constraint Store:\n";
    cout << "\t+ Consistency active: ";
    print_option ( _cstore_consistency );
    cout << "\t+ Satisfiability active: ";
    print_option ( _cstore_satisfiability );
    
    cout << "- Device:\n";
    cout << "\t+ Device max shared memory: ";
    print_option ( _cuda_shared_available );
    cout << "\t+ Device max block size: ";
    print_option ( _cuda_max_block_size );
    
    cout << "- Device Constraint Store:\n";
    cout << "\t+ Device propagation loop Limit: ";
    print_option ( _cstore_cuda_prop_loop_out );
    cout << "\t+ Device Propagation type: ";
    print_option ( _cstore_cuda_propagation_function );

    cout << "=========================================\n";
}//print
