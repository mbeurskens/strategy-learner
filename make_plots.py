import os
import argparse
from evaluation import save_plots

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create experiment data plots.')
    parser.add_argument('-e', dest='exp_dirs', action='store', nargs="+", type=str,
                        help='List containing paths to all experiments to plot.', default = ["/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/results/10-nov-spg-test"])
    parser.add_argument('-t', dest="turtle_ids", action='store', nargs="+",
                        type=int, default=[2, 3], help='List containing ids of '
                                                       'turtles for which plots '
                                                       'are created.')
    parser.add_argument('-s', dest='save_dir', action='store', type=str,
                        help='Custom figure storage directory.')
    args = parser.parse_args()

    if not args.exp_dirs:
        raise ValueError("No experiment data directories given.")
    if not args.save_dir:
        args.save_dir = os.path.join(args.exp_dirs[0], "figures")
    save_plots(args.exp_dirs, args.turtle_ids, args.save_dir)
