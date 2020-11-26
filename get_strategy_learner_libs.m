function [ libs ] = get_strategy_learner_libs()
%GET_STRATEGY_LEARNER_LIB Get strategy_learner libs from the strategy
%learner directory and lib directory link.
libs =[' -L/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner', ...
       ' -lstrategy_learner'
      ];
end

