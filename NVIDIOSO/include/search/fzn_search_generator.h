//
//  fzn_search_generator.h
//  NVIDIOSO
//
//  Created by Federico Campeotto on 10/08/14.
//  Copyright (c) 2014 ___UDNMSU___. All rights reserved.
//
//  Factory class for search method according to FlatZinc the specifications.
//

#ifndef __NVIDIOSO__fzn_search_generator__
#define __NVIDIOSO__fzn_search_generator__

#include "globals.h"
#include "depth_first_search.h"
#include "simple_heuristic.h"
#include "metric_inc.h"
#include "simple_solution_manager.h"
#include "simple_backtrack_manager.h"

class FZNSearchFactory {
public:
  
  /**
   * Get the right instance of FlatZinc search method
   * according to its type described by the input string.
   * @param variables a vector of pointers to all the variables in the model.
   * @param search_tkn reference to a search token in order to
   *        define the right instance of search engine.
   */
  static  SearchEnginePtr get_fzn_search_shr_ptr ( std::vector< Variable * > variables,
                                                   TokenSol * search_tkn ) {
      // Create a new search engine to set up
      SearchEnginePtr engine = std::make_shared<DepthFirstSearch> ();
    
      // Set heuristic, backtrack manager, and solution manager.
      HeuristicPtr heuristic;
      VariableChoiceMetric * var_choice_metric = nullptr;
      ValueChoiceMetric    * val_choice_metric = nullptr;
    
      // Var goal
      std::string var_goal = search_tkn->get_var_goal ();
      if ( var_goal != "" )
      {
          LogMsg << "Goal variable choice not yet implement: " + var_goal << std::endl;
          std::string str_err = "Error in var goal settings\n";
          throw NvdException ( str_err.c_str(), __FILE__, __LINE__ );
      }
      LogMsg << "Search Engine - Goal variable set: " + var_goal << std::endl;
    
      std::string solve_goal = search_tkn->get_solve_goal ();
      if ( solve_goal != "satisfy" )
      {
          LogMsg << "Solve goal choice not yet implement: " + solve_goal << std::endl;
      }
      LogMsg << "Search Engine - Solve goal set: satisfy" << std::endl;
      
      std::string search_choice = search_tkn->get_search_choice ();
      if ( ( search_choice != "int_search" ) &&
           ( search_choice != "") )
      {
          LogMsg << "Search choice not yet implement: " + search_choice << std::endl;
          std::string str_err = "Error in search choice settings\n";
          throw NvdException ( str_err.c_str(), __FILE__, __LINE__ );
      }
      LogMsg << "Search Engine - Search choice set: " + search_choice << std::endl;
      
      std::string label_choice = search_tkn->get_label_choice ();
      if ( var_goal != "" )
      {
          LogMsg << "Search on subset of variabls not yet implemented: " + label_choice << std::endl;
      }
      LogMsg << "Search Engine - Search labeling performed on all variables" << std::endl;

      // Set variable selection heuristic
      std::string variable_choice = search_tkn->get_variable_choice();
      if ( variable_choice != "" )
      {
          if ( variable_choice  == "input_order" )
          {
              var_choice_metric = new InputOrder ();
          }
          else if ( variable_choice == "first_fail" ) {
              var_choice_metric = new FirstFail ();
          }
          else if ( variable_choice == "anti_first_fail" ) {
              var_choice_metric = new AntiFirstFail ();
          }
          else if ( variable_choice == "smallest" ) {
              var_choice_metric = new Smallest ();
          }
          else if ( variable_choice == "largest" ) {
              var_choice_metric = new Largest ();
          }
          else if ( variable_choice == "occurence" ) {
              var_choice_metric = new Occurence ();
          }
          else if ( variable_choice == "most_constrained" ) {
              var_choice_metric = new MostConstrained ();
          }
          else if ( variable_choice == "max_regret" ) {
              var_choice_metric = new MaxRegret ();
          }
          else
          {
              LogMsg << "Search variable choice not yet implemented: " + variable_choice << std::endl;
              std::string str_err = "Search variable choice not yet implemented: " + variable_choice + "\n";
              throw NvdException ( str_err.c_str(), __FILE__, __LINE__ );
          }
          LogMsg << "Search Engine - Search variable choice set: " + variable_choice << std::endl;
      }

      // Set assignment heuristic for variables
      std::string assignment_choice = search_tkn->get_assignment_choice();
      if ( assignment_choice != "" ) {
          if ( assignment_choice == "indomain_min" )
          {
              val_choice_metric = new InDomainMin ();
          }
          else if ( assignment_choice == "indomain_max" )
          {
              val_choice_metric = new InDomainMax ();
          }
          else if ( assignment_choice == "indomain_median" )
          {
              val_choice_metric = new InDomainMedian ();
          }
          else if ( assignment_choice == "indomain" )
          {
              val_choice_metric = new InDomain ();
          }
          else if ( assignment_choice == "indomain_random" )
          {
              val_choice_metric = new InDomainRandom ();
          }
          /*
          else if ( assignment_choice == "indomain_split" )
          {
              val_choice_metric = new InDomainSplit ();
          }
          else if ( assignment_choice == "indomain_reverse_split" )
          {
              val_choice_metric = new InDomainReverseSplit ();
          }
          else if ( assignment_choice == "indomain_interval" )
          {
              val_choice_metric = new InDomainInterval ();
          }
          */
          else
          {
              LogMsg << "Assignment value choice not yet implemented: " + assignment_choice << std::endl;
              std::string str_err = "Assignment value choice not yet implemented: " + assignment_choice + "\n";
              throw NvdException ( str_err.c_str(), __FILE__, __LINE__ );              
          }
          LogMsg << "Search Engine - Assignment value choice set: " + assignment_choice << std::endl;
      }
    
      std::string strategy_choice = search_tkn->get_strategy_choice();
    
      // Default variable choice: InputOrder
      if ( var_choice_metric == nullptr )
      {
          var_choice_metric = new InputOrder ();
          LogMsg << "Search Engine - Search variable choice set: input_order" << std::endl;
      }
    
      // Default value choice: InDomainMin
      if ( val_choice_metric == nullptr )
      {
          val_choice_metric = new InDomainMin ();
          LogMsg << "Search Engine - Assignment value choice set: indomain_min" << std::endl;
      }
    
      // Set heuristics
      heuristic = std::make_shared<SimpleHeuristic>( variables, var_choice_metric, val_choice_metric );

      // Set Heuristic on the search engine
      engine->set_heuristic( heuristic );
    
      // Backtrack Manager
      BacktrackManagerPtr backtrack_manager = std::make_shared<SimpleBacktrackManager>();
      for ( auto var : variables )
      {
          (std::static_pointer_cast<SimpleBacktrackManager>( backtrack_manager ))->
              attach_backtracable( static_cast<IntVariable*>(var) );
          static_cast<IntVariable*>(var)->set_backtrack_manager( backtrack_manager );
      }
    
      // Set backtrack manager on search engine.
      engine->set_backtrack_manager( backtrack_manager );
    
      // Solution Manager
      SolutionManager * solution_manager = new SimpleSolutionManager ( variables );
    
      // Set solution manager on search engine.
      engine->set_solution_manager( solution_manager );

      return engine;
  }
};

#endif /* defined(__NVIDIOSO__fzn_search_generator__) */
